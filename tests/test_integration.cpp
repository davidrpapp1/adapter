#include "adapter/config_manager.hpp"
#include "adapter/csv_parser.hpp"
#include "adapter/data_cleaner.hpp"
#include "adapter/time_aligner.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

using namespace adapter;

void test_assert(bool condition, const std::string &test_name) {
  if (condition) {
    std::cout << "[PASS] " << test_name << std::endl;
  } else {
    std::cout << "[FAIL] " << test_name << std::endl;
    throw std::runtime_error("Test failed: " + test_name);
  }
}

void test_full_pipeline() {
  std::cout << "Testing full data processing pipeline..." << std::endl;

  // Create a test CSV file with time series data
  std::ofstream test_file("integration_test_data.csv");
  test_file << "time,temperature,pressure,humidity\n";
  test_file << "0,20.5,1013.25,45.0\n";
  test_file << "1,21.0,1012.80,46.5\n";
  test_file << "2,,1011.50,\n"; // Missing values
  test_file << "3,22.5,1010.25,48.0\n";
  test_file << "1,21.0,1012.80,46.5\n"; // Duplicate row
  test_file << "4,23.0,1009.75,49.5\n";
  test_file.close();

  // Step 1: Parse CSV
  CsvParser parser;
  bool loaded = parser.load_file("integration_test_data.csv");
  test_assert(loaded, "CSV file should load successfully");
  test_assert(parser.get_row_count() == 6, "should have 6 data rows initially");
  test_assert(parser.get_column_count() == 4, "should have 4 columns");

  // Step 2: Clean data
  DataCleaner cleaner;
  auto data = parser.get_data();
  size_t original_size = data.size();
  cleaner.clean_data(data);
  test_assert(data.size() < original_size, "cleaning should remove some rows");

  // Step 3: Time alignment
  TimeAligner aligner;
  std::vector<std::vector<std::string>> data_with_headers;
  data_with_headers.push_back(parser.get_headers());
  data_with_headers.insert(data_with_headers.end(), data.begin(), data.end());

  size_t size_before_alignment = data_with_headers.size();
  aligner.align_time_series_data(
      data_with_headers, "time",
      std::vector<std::string>{"temperature", "pressure"},
      std::vector<std::string>{"humidity"});

  test_assert(!data_with_headers.empty(), "alignment should produce data");

  // Step 4: Configuration management
  ConfigManager config;
  config.set_input_file("integration_test_data.csv");
  config.set_output_file("integration_test_output.csv");
  config.set_time_column("time");
  config.set_dependent_variables({"temperature", "pressure"});
  config.set_independent_variables({"humidity"});

  test_assert(config.get_input_file() == "integration_test_data.csv",
              "input file should be set correctly");
  test_assert(config.get_time_column() == "time",
              "time column should be set correctly");

  // Step 5: Write output and verify
  std::ofstream output_file("integration_test_output.csv");
  char delimiter = config.get_delimiter();

  for (const auto &row : data_with_headers) {
    for (size_t i = 0; i < row.size(); ++i) {
      output_file << row[i];
      if (i < row.size() - 1)
        output_file << delimiter;
    }
    output_file << std::endl;
  }
  output_file.close();

  // Verify output file exists and is readable
  std::ifstream verify_file("integration_test_output.csv");
  test_assert(verify_file.is_open(),
              "output file should be created and readable");
  verify_file.close();

  // Clean up test files
  std::remove("integration_test_data.csv");
  std::remove("integration_test_output.csv");

  std::cout << "Full pipeline integration test passed!" << std::endl;
}

void test_config_file_operations() {
  std::cout << "Testing configuration file operations..." << std::endl;

  ConfigManager config;

  // Set some configuration values
  config.set_input_file("test_input.csv");
  config.set_output_file("test_output.csv");
  config.set_time_column("timestamp");
  config.set_delimiter(';');
  config.set_dependent_variables({"var1", "var2"});
  config.set_independent_variables({"var3"});

  // Save configuration
  bool saved = config.save_to_file("test_config.txt");
  test_assert(saved, "configuration should save successfully");

  // Create a new config manager and load the file
  ConfigManager loaded_config;
  bool loaded = loaded_config.load_from_file("test_config.txt");
  test_assert(loaded, "configuration should load successfully");

  // Verify loaded values
  test_assert(loaded_config.get_input_file() == "test_input.csv",
              "input file should match");
  test_assert(loaded_config.get_output_file() == "test_output.csv",
              "output file should match");
  test_assert(loaded_config.get_time_column() == "timestamp",
              "time column should match");
  test_assert(loaded_config.get_delimiter() == ';', "delimiter should match");

  auto dep_vars = loaded_config.get_dependent_variables();
  test_assert(dep_vars.size() == 2, "should have 2 dependent variables");
  test_assert(dep_vars[0] == "var1", "first dependent variable should match");
  test_assert(dep_vars[1] == "var2", "second dependent variable should match");

  auto indep_vars = loaded_config.get_independent_variables();
  test_assert(indep_vars.size() == 1, "should have 1 independent variable");
  test_assert(indep_vars[0] == "var3", "independent variable should match");

  // Clean up
  std::remove("test_config.txt");

  std::cout << "Configuration file operations test passed!" << std::endl;
}

void test_error_handling() {
  std::cout << "Testing error handling..." << std::endl;

  // Test loading non-existent file
  CsvParser parser;
  bool loaded = parser.load_file("non_existent_file.csv");
  test_assert(!loaded, "loading non-existent file should fail");

  // Test parsing invalid time column
  TimeAligner aligner;
  std::vector<std::vector<std::string>> empty_data;
  aligner.align_time_series_data(empty_data, "non_existent_column", {}, {});
  // Should not crash (error handling is internal)
  test_assert(true, "time alignment with invalid column should not crash");

  // Test configuration with invalid file
  ConfigManager config;
  bool config_loaded = config.load_from_file("non_existent_config.txt");
  test_assert(!config_loaded,
              "loading non-existent config should fail gracefully");

  std::cout << "Error handling test passed!" << std::endl;
}

int main() {
  try {
    test_full_pipeline();
    test_config_file_operations();
    test_error_handling();

    std::cout << std::endl
              << "All integration tests passed successfully!" << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Integration test failed with exception: " << e.what()
              << std::endl;
    return 1;
  }
}
