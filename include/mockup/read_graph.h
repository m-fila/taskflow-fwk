#ifndef TASKFLOW_FWK_READ_GRAPH_H_
#define TASKFLOW_FWK_READ_GRAPH_H_

#include "mockup/graph_representation.h"
#include <boost/graph/graphml.hpp>
#include <istream>
#include <string>
namespace mockup {

  df::Graph read_df( std::istream& input, const df::VertexPropertiesKeys& keys = {} );
  df::Graph read_df( const std::string& filename, const df::VertexPropertiesKeys& keys = {} );

  cf::Graph read_cf( std::istream& input, const cf::VertexPropertiesKeys& keys = {} );
  cf::Graph read_cf( const std::string& filename, const cf::VertexPropertiesKeys& keys = {} );

} // namespace mockup
#endif // TASKFLOW_FWK_READ_GRAPH_H_