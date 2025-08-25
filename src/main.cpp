#include "adapter/config_manager.hpp"
#include "adapter/csv_parser.hpp"
#include "adapter/data_cleaner.hpp"
#include "adapter/time_aligner.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace adapter {

class AdapterApplication {
private:
  ConfigManager config;
  std::string input_file;
  std::string output_file;
  std::string time_column;
  std::vector<std::string> dependent_variables;
  std::vector<std::string> independent_variables;

  void print_usage() const;
  bool parse_arguments(int argc, char *argv[]);
  bool
  write_output_csv(const std::vector<std::string> &headers,
                   const std::vector<std::vector<std::string>> &data) const;

public:
  AdapterApplication();
  int run(int argc, char *argv[]);
};

AdapterApplication::AdapterApplication() {}

void AdapterApplication::print_usage() const {
  std::cout << "Usage: adapter [options] <input_file>" << std::endl;
  std::cout << std::endl;
  std::cout << "Required arguments:" << std::endl;
  std::cout << "  input_file              Path to input CSV file" << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -o, --output <file>     Output file path (default: "
               "input_cleaned.csv)"
            << std::endl;
  std::cout << "  -t, --time <column>     Time column name for alignment"
            << std::endl;
  std::cout
      << "  -d, --dependent <vars>  Comma-separated dependent variable names"
      << std::endl;
  std::cout
      << "  -i, --independent <vars> Comma-separated independent variable names"
      << std::endl;
  std::cout << "  -c, --config <file>     Configuration file path" << std::endl;
  std::cout
      << "  --delimiter <char>      CSV delimiter character (default: comma)"
      << std::endl;
  std::cout << "  -h, --help              Show this help message" << std::endl;
  std::cout << std::endl;
  std::cout << "Examples:" << std::endl;
  std::cout << "  adapter data.csv" << std::endl;
  std::cout << "  adapter -t time -d temperature,pressure data.csv"
            << std::endl;
  std::cout << "  adapter -c config.txt -o cleaned_data.csv data.csv"
            << std::endl;
}

bool AdapterApplication::parse_arguments(int argc, char *argv[]) {
  if (argc < 2) {
    print_usage();
    return false;
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      print_usage();
      return false;
    } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
      output_file = argv[++i];
    } else if ((arg == "-t" || arg == "--time") && i + 1 < argc) {
      time_column = argv[++i];
    } else if ((arg == "-d" || arg == "--dependent") && i + 1 < argc) {
      std::string vars = argv[++i];
      std::stringstream ss(vars);
      std::string var;
      while (std::getline(ss, var, ',')) {
        // Trim whitespace
        var.erase(0, var.find_first_not_of(" \t"));
        var.erase(var.find_last_not_of(" \t") + 1);
        dependent_variables.push_back(var);
      }
    } else if ((arg == "-i" || arg == "--independent") && i + 1 < argc) {
      std::string vars = argv[++i];
      std::stringstream ss(vars);
      std::string var;
      while (std::getline(ss, var, ',')) {
        // Trim whitespace
        var.erase(0, var.find_first_not_of(" \t"));
        var.erase(var.find_last_not_of(" \t") + 1);
        independent_variables.push_back(var);
      }
    } else if ((arg == "-c" || arg == "--config") && i + 1 < argc) {
      config.load_from_file(argv[++i]);
    } else if (arg == "--delimiter" && i + 1 < argc) {
      std::string delim_str = argv[++i];
      if (!delim_str.empty()) {
        config.set_delimiter(delim_str[0]);
      }
    } else if (arg[0] != '-') {
      input_file = arg;
    } else {
      std::cerr << "Unknown option: " << arg << std::endl;
      return false;
    }
  }

  if (input_file.empty()) {
    std::cerr << "Error: No input file specified" << std::endl;
    return false;
  }

  // Set input file in config
  config.set_input_file(input_file);

  if (output_file.empty()) {
    size_t dot_pos = input_file.find_last_of('.');
    if (dot_pos != std::string::npos) {
      output_file = input_file.substr(0, dot_pos) + "_cleaned.csv";
    } else {
      output_file = input_file + "_cleaned.csv";
    }
  }

  config.set_output_file(output_file);

  if (!time_column.empty()) {
    config.set_time_column(time_column);
  }

  if (!dependent_variables.empty()) {
    config.set_dependent_variables(dependent_variables);
  }

  if (!independent_variables.empty()) {
    config.set_independent_variables(independent_variables);
  }

  return true;
}

bool AdapterApplication::write_output_csv(
    const std::vector<std::string> &headers,
    const std::vector<std::vector<std::string>> &data) const {
  std::ofstream file(output_file);
  if (!file.is_open()) {
    std::cerr << "Error: Could not create output file '" << output_file << "'"
              << std::endl;
    return false;
  }

  char delimiter = config.get_delimiter();

  // Write headers
  for (size_t i = 0; i < headers.size(); ++i) {
    file << headers[i];
    if (i < headers.size() - 1)
      file << delimiter;
  }
  file << std::endl;

  // Write data
  for (const auto &row : data) {
    for (size_t i = 0; i < row.size(); ++i) {
      file << row[i];
      if (i < row.size() - 1)
        file << delimiter;
    }
    file << std::endl;
  }

  file.close();
  return true;
}

int AdapterApplication::run(int argc, char *argv[]) {
  std::cout << "Adapter - High-Performance Data Cleaning and Preparation Tool"
            << std::endl;
  std::cout << "============================================================="
            << std::endl;
  std::cout << std::endl;

  if (!parse_arguments(argc, argv)) {
    return 1;
  }

  config.print_configuration();
  std::cout << std::endl;

  // Step 1: Parse CSV
  std::cout << "Step 1: Parsing CSV file..." << std::endl;
  CsvParser parser;
  parser.set_delimiter(config.get_delimiter());

  if (!parser.load_file(input_file)) {
    std::cerr << "Error: Failed to load CSV file" << std::endl;
    return 1;
  }

  std::cout << "Successfully loaded " << parser.get_row_count() << " rows with "
            << parser.get_column_count() << " columns" << std::endl;
  std::cout << std::endl;

  // Step 2: Data Cleaning
  std::cout << "Step 2: Cleaning data..." << std::endl;
  DataCleaner cleaner;

  auto data = parser.get_data();
  cleaner.clean_data(data);
  std::cout << std::endl;

  // Step 3: Time Series Alignment (if time column specified)
  std::vector<std::string> final_headers = parser.get_headers();
  std::vector<std::vector<std::string>> final_data = data;

  if (!config.get_time_column().empty()) {
    std::cout << "Step 3: Aligning time series data..." << std::endl;
    TimeAligner aligner;
    aligner.set_target_time_interval(config.get_target_time_interval());

    // Prepare data with headers as first row
    std::vector<std::vector<std::string>> data_with_headers;
    data_with_headers.push_back(final_headers);
    data_with_headers.insert(data_with_headers.end(), final_data.begin(),
                             final_data.end());

    aligner.align_time_series_data(data_with_headers, config.get_time_column(),
                                   config.get_dependent_variables(),
                                   config.get_independent_variables());

    // Extract headers and data
    if (!data_with_headers.empty()) {
      final_headers = data_with_headers[0];
      final_data.clear();
      for (size_t i = 1; i < data_with_headers.size(); ++i) {
        final_data.push_back(data_with_headers[i]);
      }
    }
    std::cout << std::endl;
  }

  // Step 4: Write Output
  std::cout << "Step 4: Writing output..." << std::endl;
  if (!write_output_csv(final_headers, final_data)) {
    std::cerr << "Error: Failed to write output file" << std::endl;
    return 1;
  }

  std::cout << "Successfully processed " << final_data.size() << " rows"
            << std::endl;
  std::cout << "Output written to: " << output_file << std::endl;
  std::cout << "Processing complete!" << std::endl;

  return 0;
}

} // namespace adapter

int main(int argc, char *argv[]) {
  adapter::AdapterApplication app;
  return app.run(argc, argv);
}
