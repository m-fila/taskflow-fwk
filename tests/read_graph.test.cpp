#include "mockup/read_graph.h"
#include <boost/graph/graphml.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <istream>
#include <sstream>
using namespace mockup;

constexpr auto df_grapmhl = R"(
<?xml version='1.0' encoding='utf-8'?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
<key id="d5" for="edge" attr.name="id" attr.type="string"/>
<key id="d4" for="node" attr.name="size_average_B" attr.type="double"/>
<key id="d3" for="node" attr.name="runtime_average_s" attr.type="double"/>
<key id="d2" for="node" attr.name="type" attr.type="string"/>
<key id="d1" for="node" attr.name="node_id" attr.type="string"/>
<key id="d0" for="node" attr.name="class" attr.type="string"/>
<graph edgedefault="directed" id="G"><node id="n0">
  <data key="d0">MicroProducer</data>
  <data key="d1">ProducerA</data>
  <data key="d2">Algorithm</data>
  <data key="d3">0.5</data>
</node>
<node id="n1">
  <data key="d1">A</data>
  <data key="d2">DataObject</data>
  <data key="d0">AnyDataWrapper&lt;int&gt;</data>
  <data key="d4">8.0</data>
</node>
<edge source="n0" target="n1">
  <data key="d5">e0</data>
</edge>
</graph></graphml>
)";

TEST_CASE( "Read DFG", "[DFG][graphml]" ) {
  auto input = std::stringstream( df_grapmhl );
  auto graph = read_df( input );
  SECTION( "Algorithm node" ) {
    auto& node = graph[0];
    REQUIRE( node.type == "Algorithm" );
    REQUIRE( node.klass == "MicroProducer" );
    REQUIRE( node.name == "ProducerA" );
    REQUIRE( node.runtime_s == .5 );
  }
  SECTION( "DataObject node" ) {
    auto& node = graph[1];
    REQUIRE( node.type == "DataObject" );
    REQUIRE( node.klass == "AnyDataWrapper<int>" );
    REQUIRE( node.name == "A" );
    REQUIRE( node.memory_footprint_B == 8.0 );
  }
}

constexpr auto cf_grapmhl = R"(
<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
  <key id="key0" for="node" attr.name="blocking" attr.type="boolean" />
  <key id="key1" for="node" attr.name="class" attr.type="string" />
  <key id="key2" for="node" attr.name="ignoreFilterPassed" attr.type="boolean" />
  <key id="key3" for="node" attr.name="invert" attr.type="boolean" />
  <key id="key4" for="node" attr.name="modeOR" attr.type="boolean" />
  <key id="key5" for="node" attr.name="node_id" attr.type="string" />
  <key id="key6" for="node" attr.name="requireObjects" attr.type="string" />
  <key id="key7" for="node" attr.name="sequential" attr.type="boolean" />
  <key id="key8" for="node" attr.name="shortCircuit" attr.type="boolean" />
  <key id="key9" for="node" attr.name="type" attr.type="string" />
  <key id="key10" for="node" attr.name="vetoObjects" attr.type="string" />
  <graph id="G" edgedefault="directed" parse.nodeids="free" parse.edgeids="canonical" parse.order="nodesfirst">
    <node id="n0">
      <data key="key0">0</data>
      <data key="key1"></data>
      <data key="key2">0</data>
      <data key="key3">0</data>
      <data key="key4">0</data>
      <data key="key5">RootDecisionHub</data>
      <data key="key6"></data>
      <data key="key7">0</data>
      <data key="key8">0</data>
      <data key="key9">DecisionHub</data>
      <data key="key10"></data>
    </node>
    <node id="n1">
      <data key="key0">0</data>
      <data key="key1">GaudiSequencer</data>
      <data key="key2">0</data>
      <data key="key3">0</data>
      <data key="key4">0</data>
      <data key="key5">Sequencer</data>
      <data key="key6">[  ]</data>
      <data key="key7">0</data>
      <data key="key8">1</data>
      <data key="key9">DecisionHub</data>
      <data key="key10">[  ]</data>
    </node>
    <node id="n2">
      <data key="key0">0</data>
      <data key="key1">MicroProducer</data>
      <data key="key2">0</data>
      <data key="key3">0</data>
      <data key="key4">0</data>
      <data key="key5">ProducerA</data>
      <data key="key6"></data>
      <data key="key7">0</data>
      <data key="key8">0</data>
      <data key="key9">Algorithm</data>
      <data key="key10"></data>
    </node>
    <edge id="e0" source="n0" target="n1">
    </edge>
    <edge id="e1" source="n0" target="n2">
    </edge>
  </graph>
</graphml>

)";

TEST_CASE( "Read CFG", "[CFG][graphml]" ) {
  auto input = std::stringstream( cf_grapmhl );
  auto graph = read_cf( input );
  SECTION( "RootDecisionHub" ) {
    auto& node = graph[0];
    REQUIRE( node.type == "DecisionHub" );
    REQUIRE( node.name == "RootDecisionHub" );
  }
  SECTION( "DecisionHub node" ) {
    auto& node = graph[1];
    REQUIRE( node.type == "DecisionHub" );
    REQUIRE( node.name == "Sequencer" );
    REQUIRE( node.klass == "GaudiSequencer" );
    REQUIRE( node.invert == false );
    REQUIRE( node.modeOR == false );
    REQUIRE( node.ignoreFilterPassed == false );
    REQUIRE( node.shortCircuit == true );
    REQUIRE( node.sequential == false );
    REQUIRE( node.requireObjects == "[ ]" );
    REQUIRE( node.vetoObjects == "[ ]" );
  }
  SECTION( "Algorithm node" ) {
    auto& node = graph[2];
    REQUIRE( node.type == "Algorithm" );
    REQUIRE( node.name == "ProducerA" );
    REQUIRE( node.klass == "MicroProducer" );
    REQUIRE( node.blocking == false );
  }
}