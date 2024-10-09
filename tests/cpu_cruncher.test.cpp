#include "mockup/cpu_cruncher.h"
#include "chrono"
#include <catch2/catch_test_macros.hpp>
using namespace mockup;

TEST_CASE( "CPUCruncher", "[timing]" ) {
  auto average  = std::chrono::milliseconds( 5 );
  auto cruncher = CPUCruncher().average( average ).sleep_fraction( .5 ).variance( std::chrono::milliseconds( 0 ) );
  auto start    = std::chrono::steady_clock::now();
  cruncher();
  auto stop    = std::chrono::steady_clock::now();
  auto runtime = stop - start;
  REQUIRE( average * 1.1 > runtime );
  REQUIRE( runtime > average * 0.9 );
}