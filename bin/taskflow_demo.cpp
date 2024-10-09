#include "mockup/cpu_cruncher.h"
#include "mockup/graph_representation.h"
#include "mockup/read_graph.h"
#include "taskflow/algorithm/pipeline.hpp"
#include "taskflow/core/taskflow.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/program_options.hpp>
#include <boost/range/iterator_range.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <thread>

auto make_dummy_task( const mockup::df::VertexProperties& properties ) {
  return [name = properties.name]() { std::cout << name << std::endl; };
}

mockup::CPUCruncher configure_cruncher( mockup::CPUCruncher&&               cruncher,
                                        const mockup::df::VertexProperties& properties ) {
  cruncher.sleep_fraction( 0 )
      .average( mockup::runtime_duration( properties.runtime_s ) )
      .stddev( mockup::runtime_duration( std::min( 0.01 * properties.runtime_s, 0.001 ) ) );
  return cruncher;
}

auto make_cruncher_task( mockup::CPUCruncher&& cruncher ) {
  return [cruncher = std::move( cruncher )]() mutable { return cruncher(); };
}

tf::Taskflow make_flow( mockup::CPUCruncherBuilder& task_builder, const mockup::df::Graph& dag ) {
  auto flow            = tf::Taskflow{};
  auto algorithm_tasks = std::map<typename std::decay_t<decltype( dag )>::vertex_descriptor, tf::Task>();
  for ( auto node_id : boost::make_iterator_range( boost::vertices( dag ) ) ) {
    const auto& node = dag[node_id];
    if ( node.type == mockup::AlgorithmKey ) {
      algorithm_tasks[node_id] =
          flow.emplace( [cruncher = configure_cruncher( task_builder.make(), node )]() mutable { return cruncher(); } )
              .name( node.name );
    }
  }
  for ( auto node_id : boost::make_iterator_range( boost::vertices( dag ) ) ) {
    if ( dag[node_id].type == mockup::DataObjectKey ) {
      for ( auto in_edge : boost::make_iterator_range( boost::in_edges( node_id, dag ) ) ) {
        auto parent_id = in_edge.m_source;
        for ( auto out_edge : boost::make_iterator_range( boost::out_edges( node_id, dag ) ) ) {
          auto child_id = out_edge.m_target;
          algorithm_tasks[parent_id].precede( algorithm_tasks[child_id] );
        }
      }
    }
  }
  return flow;
};

boost::program_options::variables_map parse_arguments( int argc, char** argv ) {
  auto desc = boost::program_options::options_description( "General" );
  desc.add_options()( "help,h", "Print help message." )(
      "dfg", boost::program_options::value<std::string>()->required(),
      "Data flow graphml file." )( "cfg", boost::program_options::value<std::string>(), "Control flow graphml file." )(
      "name", boost::program_options::value<std::string>()->default_value( "Demonstrator", "Name of the workflow." ) )(
      "dry-run", boost::program_options::bool_switch(), "Dry run. Build but don't run the execution graph." );

  auto desc_runtime = boost::program_options::options_description( "Runtime" );
  desc_runtime.add_options()(
      "threads,t", boost::program_options::value<unsigned int>()->default_value( std::thread::hardware_concurrency() ),
      "Number of threads to use." )( "event-count", boost::program_options::value<unsigned int>()->default_value( 1 ),
                                     "Number of events to be processed." )(
      "slots", boost::program_options::value<unsigned int>()->default_value( 1 ), "Number of concurrent event slots." );

  auto desc_trace = boost::program_options::options_description( "Trace" );
  desc_trace.add_options()( "logs-tfp", boost::program_options::value<std::string>(),
                            "Output the execution logs as a TFProf trace. Must be a json or tfp file." )(
      "logs-trace", boost::program_options::value<std::string>(), "Output the execution logs as a chrome trace. Must be a json file." )(
      "dump-plan", boost::program_options::bool_switch(), "Write execution plan to files named {name}.dot." )(
      "fast-calibrate", boost::program_options::bool_switch(), "Calibrate CPUCrunching on smaller sample." );

  boost::program_options::options_description cmdline_options{ "Options" };
  cmdline_options.add( desc ).add( desc_runtime ).add( desc_trace );

  auto vm = boost::program_options::variables_map{};

  try {
    boost::program_options::store(
        boost::program_options::command_line_parser( argc, argv ).options( cmdline_options ).run(), vm );
    if ( vm.count( "help" ) ) {
      std::cout << cmdline_options << std::endl;
      std::exit( 0 );
    }

    boost::program_options::notify( vm );
  } catch ( const boost::program_options::error& ex ) {
    std::cerr << ex.what() << "\n\n"
              << "Try '--help' for more information" << std::endl;
    std::exit( 1 );
  }
  return vm;
}

int main( int argc, char** argv ) {
  auto vm = parse_arguments( argc, argv );

  auto fast_calibrate = vm["fast-calibrate"].as<bool>();
  auto slots          = vm["slots"].as<unsigned int>();
  auto threads        = vm["threads"].as<unsigned int>();
  auto max_events     = vm["event-count"].as<unsigned int>();
  auto workload_name  = vm["name"].as<std::string>();
  auto dag            = mockup::read_df( vm["dfg"].as<std::string>() );

  auto executor        = tf::Executor{ threads };
  auto chrome_observer = vm.count( "logs-trace" ) ? executor.make_observer<tf::ChromeObserver>() : nullptr;
  auto tfp_observer = vm.count( "logs-tfp" ) ? executor.make_observer<tf::TFProfObserver>() : nullptr;

  auto task_builder = mockup::CPUCruncherBuilder{};
  std::cout << "Calibrating CPUCrunching" << std::endl;
  task_builder.calibrate( 1, mockup::runtime_duration( 0 ), 1, fast_calibrate );
  std::cout << "Calibrating CPUCrunching done" << std::endl;
  auto core_flows = std::vector<tf::Taskflow>{};
  core_flows.reserve( slots );
  for ( int i = 0; i < slots; ++i ) {
    core_flows.emplace_back( make_flow( task_builder, dag ) );
    core_flows[i].name( workload_name + "-core-" + std::to_string( i ) );
  }
  auto pipeline = tf::Pipeline{
      slots,
      tf::Pipe{ tf::PipeType::SERIAL,
                [max_events]( tf::Pipeflow& pf ) {
                  if ( pf.token() >= max_events ) {
                    pf.stop();
                  } else {
                    std::cout << "Begin event: " << pf.token() << std::endl;
                  }
                } },
      tf::Pipe{ tf::PipeType::PARALLEL,
                [&executor, &core_flows]( tf::Pipeflow& pf ) { executor.corun( core_flows[pf.line()] ); } },
      tf::Pipe{ tf::PipeType::PARALLEL,
                []( tf::Pipeflow& pf ) { std::cout << "End event: " << pf.token() << std::endl; } } };

  auto master_flow = tf::Taskflow{ workload_name };

  auto core_task = master_flow.composed_of( pipeline ).name( workload_name + "-pipeline" );
  auto init_task =
      master_flow.emplace( []() { std::cout << "Begin processing" << std::endl; } ).name( "Begin processing" );
  auto final_task =
      master_flow.emplace( []() { std::cout << "End processing" << std::endl; } ).name( "End processing" );
  init_task.precede( core_task );
  core_task.precede( final_task );

  if ( !vm["dry-run"].as<bool>() ) {
    executor.run( master_flow ).wait();

    if ( chrome_observer ) {
      auto trace_file_name = vm["logs-trace"].as<std::string>();
      auto traceFile       = std::ofstream( trace_file_name );
      chrome_observer->dump( traceFile );
      std::cout << "Perfetto trace written to file: \"" << trace_file_name << '\"' << std::endl;
    }
    if ( tfp_observer ) {
      auto trace_file_name = vm["logs-tfp"].as<std::string>();
      auto traceFile       = std::ofstream( trace_file_name );
      tfp_observer->dump( traceFile );
      std::cout << "TFProf trace written to file: \"" << trace_file_name << '\"' << std::endl;
    }
  }
  if ( vm["dump-plan"].as<bool>() ) {
    auto plan_file_name = workload_name + ".dot";
    auto plan_file      = std::ofstream{ plan_file_name };
    master_flow.dump( plan_file );

    auto core_plan_file_name = workload_name + "-core.dot";
    auto core_plan_file      = std::ofstream{ core_plan_file_name };
    core_flows.front().dump( core_plan_file );

    std::cout << "Execution plan saved to files: \"" << plan_file_name << "\" and \"" << core_plan_file_name << '\"'
              << std::endl;
  }

  return 0;
}