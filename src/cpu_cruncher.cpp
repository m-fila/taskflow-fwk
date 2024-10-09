#include "mockup/cpu_cruncher.h"
#include <chrono>
#include <random>
#include <ratio>
#include <stdexcept>
#include <thread>
namespace mockup {
  volatile int fool;

  // taken from GaudiHive/CPUCrunchSvc
  void find_primes( unsigned int n_iterations ) {
    // Flag to trigger the allocation
    bool is_prime;

    // Let's prepare the material for the allocations
    unsigned int   primes_size = 1;
    unsigned long* primes      = new unsigned long[primes_size];
    primes[0]                  = 2;

    unsigned long i = 2;

    // Loop on numbers
    for ( unsigned long int iiter = 0; iiter < n_iterations; iiter++ ) {
      // Once at max, it returns to 0
      i += 1;

      // Check if it can be divided by the smaller ones
      is_prime = true;
      for ( unsigned long j = 2; j < i && is_prime; ++j ) {
        if ( i % j == 0 ) is_prime = false;
      } // end loop on numbers < than tested one

      if ( is_prime ) {
        // copy the array of primes (INEFFICIENT ON PURPOSE!)
        unsigned int   new_primes_size = 1 + primes_size;
        unsigned long* new_primes      = new unsigned long[new_primes_size];

        for ( unsigned int prime_index = 0; prime_index < primes_size; prime_index++ ) {
          new_primes[prime_index] = primes[prime_index];
        }
        // attach the last prime
        new_primes[primes_size] = i;

        // Update primes array
        delete[] primes;
        primes      = new_primes;
        primes_size = new_primes_size;
      } // end is prime

    } // end of while loop

    // Fool Compiler optimisations:
    for ( unsigned int prime_index = 0; prime_index < primes_size; prime_index++ )
      if ( primes[prime_index] == 4 ) ++fool;

    delete[] primes;
  }

  void detail::CPUCruncher::crunch( runtime_duration duration ) const {
    auto iterations = get_iterations( duration );
    find_primes( iterations );
  }

  // taken from GaudiHive/CPUCrunchSvc
  void detail::CPUCruncher::calibrate( double correction_factor, runtime_duration min_time, unsigned int min_runs,
                                       bool fast_calibrate ) {
    if ( m_niters_vect.size() == 0 ) {
      m_niters_vect = { 0,     500,   600,   700,   800,   1000,  1300,  1600,  2000,  2300,
                        2600,  3000,  3300,  3500,  3900,  4200,  5000,  6000,  8000,  10000,
                        12000, 15000, 17000, 20000, 25000, 30000, 35000, 40000, 50000, 60000 };
      if ( !fast_calibrate ) {
        m_niters_vect.push_back( 100000 );
        m_niters_vect.push_back( 150000 );
        m_niters_vect.push_back( 200000 );
        m_niters_vect.push_back( 300000 );
        m_niters_vect.push_back( 400000 );
      }
    }

    if ( m_niters_vect.at( 0 ) != 0 ) { m_niters_vect.at( 0 ) = 0; }

    m_times_vect.resize( m_niters_vect.size() );
    m_times_vect.at( 0 ) = 0;

    // warm it up by doing 20k iterations
    find_primes( 20000 );

    for ( int irun = 0; irun < min_runs; ++irun ) {

      // debug() << "Starting calibration run " << irun + 1 << " ..." << endmsg;
      for ( unsigned int i = 1; i < m_niters_vect.size(); ++i ) {
        unsigned int niters = m_niters_vect.at( i );
        unsigned int trials = 30;
        do {
          auto start_cali = std::chrono::steady_clock::now();
          find_primes( niters );
          auto stop_cali       = std::chrono::steady_clock::now();
          auto deltat          = std::chrono::duration_cast<std::chrono::microseconds>( stop_cali - start_cali );
          m_times_vect.at( i ) = deltat.count(); // in microseconds
          // debug() << " Calibration: # iters = " << niters << " => " << m_times_vect.at( i ) << " us" << endmsg;
          trials--;
        } while ( trials > 0 && m_times_vect.at( i ) < m_times_vect.at( i - 1 ) ); // make sure that they are monotonic

        if ( i == m_niters_vect.size() - 1 && min_time.count() > 0 ) {
          if ( m_times_vect.at( i ) < std::chrono::duration_cast<std::chrono::microseconds>( min_time ).count() ) {
            // debug() << "  increasing calib vect with " << int( m_niters_vect.value().back() * 1.2 )
            //         << " iterations to reach min calib time of " << m_minCalibTime.value() << " ms " << endmsg;
            m_niters_vect.push_back( int( m_niters_vect.back() * 1.2 ) );
            m_times_vect.push_back( 0. );
          }
        }
      }
    }
    for ( auto& t : m_times_vect ) { t = t * correction_factor; }
  }

  // taken from GaudiHive/CPUCrunchSvc
  unsigned int detail::CPUCruncher::get_iterations( runtime_duration duration ) const {
    unsigned int smaller_i = 0;
    double       time      = 0.;
    bool         found     = false;
    double       corrRuntime =
        std::chrono::duration_cast<std::chrono::duration<double, std::micro>>( duration ).count(); // * m_corrFact;
    // We know that the first entry is 0, so we start to iterate from 1
    for ( unsigned int i = 1; i < m_times_vect.size(); i++ ) {
      time = m_times_vect.at( i );
      if ( time > corrRuntime ) {
        smaller_i = i - 1;
        found     = true;
        break;
      }
    }

    // Case 1: we are outside the interpolation range, we take the last 2 points
    if ( not found ) smaller_i = m_times_vect.size() - 2;

    // Case 2: we make a linear interpolation
    // y=mx+q
    const auto   x0 = m_times_vect.at( smaller_i );
    const auto   x1 = m_times_vect.at( smaller_i + 1 );
    const auto   y0 = m_niters_vect.at( smaller_i );
    const auto   y1 = m_niters_vect.at( smaller_i + 1 );
    const double m  = (double)( y1 - y0 ) / (double)( x1 - x0 );
    const double q  = y0 - m * x0;

    const unsigned int nCaliIters = m * corrRuntime + q;

    return nCaliIters;
  }

  CPUCruncher::CPUCruncher( std::shared_ptr<detail::CPUCruncher> cruncher, std::seed_seq seeds )
      : m_cruncher( cruncher ), m_random( seeds ) {}

  CPUCruncher& CPUCruncher::average( runtime_duration average ) {
    m_duration_average = average;
    return *this;
  }
  CPUCruncher& CPUCruncher::stddev( runtime_duration stddev ) {
    m_duration_stddev = stddev;
    return *this;
  }
  CPUCruncher& CPUCruncher::sleep_fraction( double sleep_fraction ) {
    if ( sleep_fraction > 1 or sleep_fraction < 0 ) {
      throw std::domain_error( "Sleep fraction received value that isn't a fraction" );
    }
    m_sleep_fraction = sleep_fraction;
    return *this;
  }

  void CPUCruncher::operator()() {

    auto distribution =
        std::normal_distribution<runtime_duration::rep>{ m_duration_average.count(), m_duration_stddev.count() };
    auto       random_duration = runtime_duration( std::abs( distribution( m_random ) ) );
    const auto sleep_duration  = m_sleep_fraction * random_duration;
    const auto work_duration   = ( 1 - m_sleep_fraction ) * random_duration;
    if ( m_sleep_fraction > 0 ) { std::this_thread::sleep_for( sleep_duration ); }
    if ( m_sleep_fraction < 1 ) { m_cruncher->crunch( work_duration ); }
  }

} // namespace mockup