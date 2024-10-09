#ifndef TASKFLOW_FWK_CPU_CRUNCHER_H_
#define TASKFLOW_FWK_CPU_CRUNCHER_H_

#include <chrono>
#include <memory>
#include <random>
#include <vector>
namespace mockup {
  using runtime_duration = std::chrono::duration<double>;

  namespace detail {

    class CPUCruncher {
    public:
      void calibrate( double correction_factor, runtime_duration min_time, unsigned int min_runs, bool fast_calibrate );
      void crunch( runtime_duration duration ) const;

    private:
      unsigned int get_iterations( runtime_duration duration ) const;

      std::vector<unsigned int> m_times_vect;
      std::vector<unsigned int> m_niters_vect;
    };

  } // namespace detail

  class CPUCruncherBuilder;

  class CPUCruncher {
  public:
    void operator()();

    CPUCruncher& average( runtime_duration average );
    CPUCruncher& stddev( runtime_duration stddev );
    CPUCruncher& sleep_fraction( double sleep_fraction );

  private:
    runtime_duration                           m_duration_average;
    runtime_duration                           m_duration_stddev;
    double                                     m_sleep_fraction = 0;
    std::shared_ptr<const detail::CPUCruncher> m_cruncher;
    std::mt19937                               m_random;

  private:
    CPUCruncher( std::shared_ptr<detail::CPUCruncher> cruncher, std::seed_seq seeds );
    friend CPUCruncherBuilder;
  };

  class CPUCruncherBuilder {
  public:
    CPUCruncherBuilder() : m_cruncher( std::make_shared<detail::CPUCruncher>() ), m_random{} {}
    CPUCruncherBuilder& calibrate( double correction_factor = 1, runtime_duration min_time = runtime_duration( 0 ),
                                   unsigned int min_runs = 1, bool fast_calibrate=false ) {
      m_cruncher->calibrate( correction_factor, min_time, min_runs, fast_calibrate );
      return *this;
    }
    CPUCruncher make() {
      return CPUCruncher{ m_cruncher, { m_random(), m_random(), m_random(), m_random(), m_random(), m_random() } };
    }

  private:
    std::shared_ptr<detail::CPUCruncher> m_cruncher;
    std::random_device                   m_random;
  };
} // namespace mockup
#endif // TASKFLOW_FWK_CPU_CRUNCHER_H_