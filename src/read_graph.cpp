#include "mockup/read_graph.h"
#include "mockup/graph_representation.h"
#include <boost/property_map/dynamic_property_map.hpp>
#include <fstream>
#include <string>
namespace mockup {
  df::Graph read_df( const std::string& filename, const df::VertexPropertiesKeys& keys ) {
    std::ifstream ifs( filename.c_str() );
    ifs.exceptions( std::ifstream::failbit );
    return read_df( ifs, keys );
  }

  df::Graph read_df( std::istream& input, const df::VertexPropertiesKeys& keys ) {
    auto graph = df::Graph{};
    auto dp    = boost::dynamic_properties( boost::ignore_other_properties );

    dp.property( keys.klass, boost::get( &df::Graph::vertex_property_type::klass, graph ) );
    dp.property( keys.type, boost::get( &df::Graph::vertex_property_type::type, graph ) );
    dp.property( keys.name, boost::get( &df::Graph::vertex_property_type::name, graph ) );
    dp.property( keys.runtime_s, boost::get( &df::Graph::vertex_property_type::runtime_s, graph ) );
    dp.property( keys.memory_footprint_B, boost::get( &df::Graph::vertex_property_type::memory_footprint_B, graph ) );
    boost::read_graphml( input, graph, dp );
    return graph;
  }

  cf::Graph read_cf( const std::string& filename, const cf::VertexPropertiesKeys& keys ) {
    std::ifstream ifs( filename.c_str() );
    ifs.exceptions( std::ifstream::failbit );
    return read_cf( ifs, keys );
  }

  cf::Graph read_cf( std::istream& input, const cf::VertexPropertiesKeys& keys ) {
    auto graph = cf::Graph{};
    auto dp    = boost::dynamic_properties( boost::ignore_other_properties );

    auto to_bool = []( const std::string& value ) -> bool { return value == "True"; };

    dp.property( keys.klass, boost::get( &cf::Graph::vertex_property_type::klass, graph ) );
    dp.property( keys.type, boost::get( &cf::Graph::vertex_property_type::type, graph ) );
    dp.property( keys.name, boost::get( &cf::Graph::vertex_property_type::name, graph ) );
    dp.property( keys.blocking, boost::get( &cf::Graph::vertex_property_type::blocking, graph ) );
    dp.property( keys.ignoreFilterPassed, boost::get( &cf::Graph::vertex_property_type::ignoreFilterPassed, graph ) );
    dp.property( keys.invert, boost::get( &cf::Graph::vertex_property_type::invert, graph ) );
    dp.property( keys.modeOR, boost::get( &cf::Graph::vertex_property_type::modeOR, graph ) );
    dp.property( keys.sequential, boost::get( &cf::Graph::vertex_property_type::sequential, graph ) );
    dp.property( keys.shortCircuit, boost::get( &cf::Graph::vertex_property_type::shortCircuit, graph ) );
    dp.property( keys.requireObjects, boost::get( &cf::Graph::vertex_property_type::requireObjects, graph ) );
    dp.property( keys.vetoObjects, boost::get( &cf::Graph::vertex_property_type::vetoObjects, graph ) );

    boost::read_graphml( input, graph, dp );
    return graph;
  }

} // namespace mockup