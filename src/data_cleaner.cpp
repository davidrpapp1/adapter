#include "adapter/data_cleaner.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <regex>
#include <sstream>

namespace adapter {

DataCleaner::DataCleaner() : date_format("%Y-%m-%d"), numeric_precision(2) {
  missing_value_strategies = {"mean"};
}

DataCleaner::~DataCleaner() {}

void DataCleaner::clean_data(std::vector<std::vector<std::string>> &data) {
  if (data.empty()) {
    return;
  }

  remove_duplicate_rows(data);
  handle_missing_values(data);
  normalize_formats(data);
}

void DataCleaner::remove_duplicate_rows(
    std::vector<std::vector<std::string>> &data) {
  if (data.size() <= 1) {
    return;
  }

  std::set<std::vector<std::string>> unique_rows;
  std::vector<std::vector<std::string>> cleaned_data;

  // Keep header row
  cleaned_data.push_back(data[0]);

  // Process data rows
  for (size_t i = 1; i < data.size(); ++i) {
    if (unique_rows.find(data[i]) == unique_rows.end()) {
      unique_rows.insert(data[i]);
      cleaned_data.push_back(data[i]);
    }
  }

  data = std::move(cleaned_data);
}

void DataCleaner::handle_missing_values(
    std::vector<std::vector<std::string>> &data) {
  if (data.size() <= 1) {
    return;
  }

  const size_t num_columns = data[0].size();

  for (size_t col = 0; col < num_columns; ++col) {
    std::vector<std::string> column_values;
    std::vector<size_t> missing_indices;

    // Collect column values and find missing ones
    for (size_t row = 1; row < data.size(); ++row) {
      if (col < data[row].size()) {
        const std::string &value = data[row][col];
        if (value.empty() || value == "NaN" || value == "nan" || 
            value == "NA" || value == "NULL") {
          missing_indices.push_back(row);
        } else {
          column_values.push_back(value);
        }
      }
    }

    if (missing_indices.empty() || column_values.empty()) {
      continue;
    }

    // Determine replacement value based on strategy
    std::string replacement_value = "0";
    if (!missing_value_strategies.empty()) {
      const std::string &strategy = missing_value_strategies[0];

      if (strategy == "mean" && is_numeric_column(column_values)) {
        replacement_value = calculate_mean(column_values);
      } else if (strategy == "median" && is_numeric_column(column_values)) {
        replacement_value = calculate_median(column_values);
      } else if (strategy == "zero") {
        replacement_value = "0";
      }
    }

    // Replace missing values
    for (size_t missing_row : missing_indices) {
      if (col < data[missing_row].size()) {
        data[missing_row][col] = replacement_value;
      }
    }
  }
}

void DataCleaner::normalize_formats(
    std::vector<std::vector<std::string>> &data) {
  if (data.size() <= 1) {
    return;
  }

  for (size_t row = 1; row < data.size(); ++row) {
    for (size_t col = 0; col < data[row].size(); ++col) {
      std::string &value = data[row][col];

      if (is_numeric(value)) {
        value = normalize_numeric_format(value);
      } else if (is_date(value)) {
        value = normalize_date_format(value);
      }
    }
  }
}

void DataCleaner::set_missing_value_strategies(
    const std::vector<std::string> &strategies) {
  missing_value_strategies = strategies;
}

void DataCleaner::set_date_format(const std::string &format) {
  date_format = format;
}

void DataCleaner::set_numeric_precision(int precision) {
  numeric_precision = precision;
}

bool DataCleaner::is_numeric(const std::string &value) const {
  if (value.empty()) {
    return false;
  }

  std::regex numeric_pattern(R"(^-?\d*\.?\d+$)");
  return std::regex_match(value, numeric_pattern);
}

bool DataCleaner::is_date(const std::string &value) const {
  if (value.empty()) {
    return false;
  }

  // Basic date pattern matching
  std::regex date_pattern(R"(\d{4}-\d{2}-\d{2})");
  std::regex datetime_pattern(R"(\d{4}-\d{2}-\d{2}[\sT]\d{2}:\d{2}:\d{2})");

  return std::regex_search(value, date_pattern) ||
         std::regex_search(value, datetime_pattern);
}

std::string DataCleaner::normalize_date_format(const std::string &value) const {
  // For simplicity, return the value as-is for now
  // In a full implementation, this would parse and reformat dates
  return value;
}

std::string
DataCleaner::normalize_numeric_format(const std::string &value) const {
  try {
    double numeric_value = std::stod(value);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(numeric_precision) << numeric_value;
    return oss.str();
  } catch (const std::exception &) {
    return value;
  }
}

std::string
DataCleaner::calculate_mean(const std::vector<std::string> &column) const {
  if (column.empty()) {
    return "0";
  }

  double sum = 0.0;
  size_t count = 0;

  for (const std::string &value : column) {
    if (is_numeric(value)) {
      try {
        sum += std::stod(value);
        count++;
      } catch (const std::exception &) {
        // Skip invalid values
      }
    }
  }

  if (count == 0) {
    return "0";
  }

  double mean = sum / count;
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(numeric_precision) << mean;
  return oss.str();
}

std::string
DataCleaner::calculate_median(const std::vector<std::string> &column) const {
  if (column.empty()) {
    return "0";
  }

  std::vector<double> numeric_values;

  for (const std::string &value : column) {
    if (is_numeric(value)) {
      try {
        numeric_values.push_back(std::stod(value));
      } catch (const std::exception &) {
        // Skip invalid values
      }
    }
  }

  if (numeric_values.empty()) {
    return "0";
  }

  std::sort(numeric_values.begin(), numeric_values.end());

  double median;
  size_t size = numeric_values.size();
  if (size % 2 == 0) {
    median = (numeric_values[size / 2 - 1] + numeric_values[size / 2]) / 2.0;
  } else {
    median = numeric_values[size / 2];
  }

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(numeric_precision) << median;
  return oss.str();
}

bool DataCleaner::is_numeric_column(
    const std::vector<std::string> &column) const {
  if (column.empty()) {
    return false;
  }

  size_t numeric_count = 0;
  for (const std::string &value : column) {
    if (is_numeric(value)) {
      numeric_count++;
    }
  }

  // Consider it numeric if at least 80% of values are numeric
  return static_cast<double>(numeric_count) / column.size() >= 0.8;
}

} // namespace adapter
