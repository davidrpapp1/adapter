#ifndef ADAPTER_TIME_ALIGNER_HPP
#define ADAPTER_TIME_ALIGNER_HPP

#include <chrono>
#include <string>
#include <vector>

namespace adapter {

enum class SolverMethod { LINEAR_INTERPOLATION, RK4, HEUN, CUBIC_SPLINE };

class TimeAligner {
public:
  TimeAligner();
  ~TimeAligner();

  void
  align_time_series_data(std::vector<std::vector<std::string>> &data,
                         const std::string &time_column_name,
                         const std::vector<std::string> &dependent_columns,
                         const std::vector<std::string> &independent_columns);

  void set_target_time_interval(double interval_seconds);
  void set_solver_method(SolverMethod method);
  void set_time_format(const std::string &format);

private:
  double target_time_interval;
  SolverMethod solver_method;
  std::string time_format;

  std::vector<double>
  parse_time_column(const std::vector<std::string> &time_column) const;
  std::string format_time_value(double time_value) const;
  std::vector<double> create_uniform_time_grid(double start_time,
                                               double end_time) const;
  std::vector<std::string>
  interpolate_values(const std::vector<double> &original_times,
                     const std::vector<std::string> &original_values,
                     const std::vector<double> &target_times) const;

  double linear_interpolation(double x, double x1, double y1, double x2,
                              double y2) const;
  double runge_kutta_step(double x, double y, double h) const;
  double heun_step(double x, double y, double h) const;
  std::vector<double>
  cubic_spline_interpolation(const std::vector<double> &x,
                             const std::vector<double> &y,
                             const std::vector<double> &xi) const;
};

} // namespace adapter

#endif // ADAPTER_TIME_ALIGNER_HPP
