#ifndef TASKFLOW_FWK_GRAPH_REPRESENTATION_H_
#define TASKFLOW_FWK_GRAPH_REPRESENTATION_H_

#include <boost/graph/adjacency_list.hpp>
#include <string>

namespace mockup {

  constexpr auto DataObjectKey  = "DataObject";
  constexpr auto AlgorithmKey   = "Algorithm";
  constexpr auto DecisionHubKey = "DecisionHub";

  namespace df {
    struct VertexProperties {
      std::string name;
      std::string type;
      std::string klass;
      double      memory_footprint_B;
      double      runtime_s;
    };

    struct VertexPropertiesKeys {
      std::string name               = "node_id";
      std::string type               = "type";
      std::string klass              = "class";
      std::string memory_footprint_B = "size_average_B";
      std::string runtime_s          = "runtime_average_s";
    };

    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperties>;

  } // namespace df

  namespace cf {
    struct VertexProperties {
      std::string name;
      std::string type;
      std::string klass;
      bool        blocking;
      bool        modeOR;
      bool        sequential;
      bool        invert;
      bool        shortCircuit;
      bool        ignoreFilterPassed;
      std::string requireObjects;
      std::string vetoObjects;
    };

    struct VertexPropertiesKeys {
      std::string name               = "node_id";
      std::string type               = "type";
      std::string klass              = "class";
      std::string blocking           = "blocking";
      std::string modeOR             = "modeOR";
      std::string sequential         = "sequential";
      std::string invert             = "invert";
      std::string shortCircuit       = "shortCircuit";
      std::string ignoreFilterPassed = "ignoreFilterPassed";
      std::string requireObjects     = "requireObjects";
      std::string vetoObjects        = "vetoObjects";
    };

    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperties>;

  } // namespace cf

} // namespace mockup

#endif // TASKFLOW_FWK_GRAPH_REPRESENTATION_H_
