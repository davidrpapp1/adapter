#include "adapter/time_aligner.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <regex>
#include <sstream>

namespace adapter {

TimeAligner::TimeAligner()
    : target_time_interval(1.0),
      solver_method(SolverMethod::LINEAR_INTERPOLATION),
      time_format("%Y-%m-%d %H:%M:%S") {}

TimeAligner::~TimeAligner() {}

void TimeAligner::align_time_series_data(
    std::vector<std::vector<std::string>> &data,
    const std::string &time_column_name,
    const std::vector<std::string> &dependent_columns,
    const std::vector<std::string> &independent_columns) {
  if (data.empty()) {
    std::cerr << "Error: No data to align" << std::endl;
    return;
  }

  std::cout << "Starting time series alignment..." << std::endl;
  std::cout << "Target time interval: " << target_time_interval << " seconds"
            << std::endl;

  // Find the time column index
  size_t time_column_index = 0;
  bool found_time_column = false;

  // Assume first row contains headers
  if (!data.empty()) {
    for (size_t i = 0; i < data[0].size(); ++i) {
      if (data[0][i] == time_column_name) {
        time_column_index = i;
        found_time_column = true;
        break;
      }
    }
  }

  if (!found_time_column) {
    std::cerr << "Error: Time column '" << time_column_name << "' not found"
              << std::endl;
    return;
  }

  // Extract time column (skip header)
  std::vector<std::string> time_column;
  for (size_t i = 1; i < data.size(); ++i) {
    if (time_column_index < data[i].size()) {
      time_column.push_back(data[i][time_column_index]);
    }
  }

  // Parse time values
  std::vector<double> original_times = parse_time_column(time_column);
  if (original_times.empty()) {
    std::cerr << "Error: Could not parse time column" << std::endl;
    return;
  }

  // Create uniform time grid
  double start_time =
      *std::min_element(original_times.begin(), original_times.end());
  double end_time =
      *std::max_element(original_times.begin(), original_times.end());
  std::vector<double> target_times =
      create_uniform_time_grid(start_time, end_time);

  // Create new aligned data structure
  std::vector<std::vector<std::string>> aligned_data;

  // Keep the header row
  if (!data.empty()) {
    aligned_data.push_back(data[0]);
  }

  // Interpolate data for each target time
  for (size_t time_idx = 0; time_idx < target_times.size(); ++time_idx) {
    std::vector<std::string> new_row(data[0].size());

    // Set the time value
    new_row[time_column_index] = std::to_string(target_times[time_idx]);

    // Interpolate each column
    for (size_t col = 0; col < data[0].size(); ++col) {
      if (col == time_column_index) {
        continue; // Already set above
      }

      // Extract original values for this column
      std::vector<std::string> original_values;
      for (size_t i = 1; i < data.size(); ++i) {
        if (col < data[i].size()) {
          original_values.push_back(data[i][col]);
        }
      }

      // Interpolate values
      std::vector<std::string> interpolated_values = interpolate_values(
          original_times, original_values, {target_times[time_idx]});

      if (!interpolated_values.empty()) {
        new_row[col] = interpolated_values[0];
      } else {
        new_row[col] = "0"; // Default value
      }
    }

    aligned_data.push_back(new_row);
  }

  // Replace original data with aligned data
  data = aligned_data;

  std::cout << "Time series alignment complete. Generated "
            << target_times.size() << " aligned data points" << std::endl;
}

void TimeAligner::set_target_time_interval(double interval_seconds) {
  target_time_interval = interval_seconds;
}

void TimeAligner::set_solver_method(SolverMethod method) {
  solver_method = method;
}

void TimeAligner::set_time_format(const std::string &format) {
  time_format = format;
}

std::vector<double> TimeAligner::parse_time_column(
    const std::vector<std::string> &time_column) const {
  std::vector<double> parsed_times;

  for (const auto &time_str : time_column) {
    // Simple numeric timestamp parsing (assume Unix timestamp or seconds)
    try {
      double time_value = std::stod(time_str);
      parsed_times.push_back(time_value);
    } catch (const std::exception &) {
      // If not a simple numeric value, try to parse as a date/time
      // For now, just skip invalid entries
      std::cerr << "Warning: Could not parse time value: " << time_str
                << std::endl;
    }
  }

  return parsed_times;
}

std::vector<double>
TimeAligner::create_uniform_time_grid(double start_time,
                                      double end_time) const {
  std::vector<double> time_grid;

  if (end_time <= start_time || target_time_interval <= 0) {
    return time_grid;
  }

  for (double t = start_time; t <= end_time; t += target_time_interval) {
    time_grid.push_back(t);
  }

  return time_grid;
}

std::vector<std::string>
TimeAligner::interpolate_values(const std::vector<double> &original_times,
                                const std::vector<std::string> &original_values,
                                const std::vector<double> &target_times) const {
  std::vector<std::string> interpolated_values;

  if (original_times.size() != original_values.size() ||
      original_times.empty()) {
    return interpolated_values;
  }

  for (double target_time : target_times) {
    std::string interpolated_value = "0";

    // Find the two closest time points
    size_t lower_idx = 0;
    size_t upper_idx = original_times.size() - 1;

    for (size_t i = 0; i < original_times.size() - 1; ++i) {
      if (original_times[i] <= target_time &&
          target_time <= original_times[i + 1]) {
        lower_idx = i;
        upper_idx = i + 1;
        break;
      }
    }

    // Check if we can interpolate numerically
    try {
      double lower_value = std::stod(original_values[lower_idx]);
      double upper_value = std::stod(original_values[upper_idx]);

      double interpolated_numeric = linear_interpolation(
          target_time, original_times[lower_idx], lower_value,
          original_times[upper_idx], upper_value);

      interpolated_value = std::to_string(interpolated_numeric);
    } catch (const std::exception &) {
      // If not numeric, use nearest neighbor
      double dist_lower = std::abs(target_time - original_times[lower_idx]);
      double dist_upper = std::abs(target_time - original_times[upper_idx]);

      if (dist_lower <= dist_upper) {
        interpolated_value = original_values[lower_idx];
      } else {
        interpolated_value = original_values[upper_idx];
      }
    }

    interpolated_values.push_back(interpolated_value);
  }

  return interpolated_values;
}

double TimeAligner::linear_interpolation(double x, double x1, double y1,
                                         double x2, double y2) const {
  if (std::abs(x2 - x1) < 1e-10) {
    return y1; // Avoid division by zero
  }

  return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
}

double TimeAligner::runge_kutta_step(double x, double y, double h) const {
  // Simplified RK4 implementation (would need a proper differential equation)
  // For now, just return a linear step
  return y + h;
}

double TimeAligner::heun_step(double x, double y, double h) const {
  // Simplified Heun method implementation
  // For now, just return a linear step
  return y + h;
}

std::vector<double>
TimeAligner::cubic_spline_interpolation(const std::vector<double> &x,
                                        const std::vector<double> &y,
                                        const std::vector<double> &xi) const {
  std::vector<double> result;

  // Simplified cubic spline - for full implementation would need spline
  // coefficients For now, fall back to linear interpolation
  for (double xi_val : xi) {
    // Find surrounding points
    size_t idx = 0;
    for (size_t i = 0; i < x.size() - 1; ++i) {
      if (x[i] <= xi_val && xi_val <= x[i + 1]) {
        idx = i;
        break;
      }
    }

    if (idx < x.size() - 1) {
      double interpolated =
          linear_interpolation(xi_val, x[idx], y[idx], x[idx + 1], y[idx + 1]);
      result.push_back(interpolated);
    } else {
      result.push_back(y.back());
    }
  }

  return result;
}

} // namespace adapter
