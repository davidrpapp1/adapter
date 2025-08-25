#include "adapter/data_cleaner.hpp"
#include <cassert>
#include <iostream>

using namespace adapter;

// Simple test framework
template <typename T>
void test_assert(const T &actual, const T &expected,
                 const std::string &test_name) {
  if (actual == expected) {
    std::cout << "[PASS] " << test_name << std::endl;
  } else {
    std::cout << "[FAIL] " << test_name << " - Expected: " << expected
              << ", Got: " << actual << std::endl;
    throw std::runtime_error("Test failed: " + test_name);
  }
}

void test_data_cleaner_missing_values() {
  std::cout << "Testing Data Cleaner missing value handling..." << std::endl;

  DataCleaner cleaner;

  // Test data with missing values
  std::vector<std::vector<std::string>> data = {
      {"10", "20", "30"},
      {"", "25", "35"},    // Empty value
      {"15", "NA", "40"},  // NA value
      {"20", "30", "NULL"} // NULL value
  };

  cleaner.handle_missing_values(data);

  // Check that missing values were replaced
  test_assert(data[1][0] != "", true, "empty value should be replaced");
  test_assert(data[2][1] != "NA", true, "NA value should be replaced");
  test_assert(data[3][2] != "NULL", true, "NULL value should be replaced");

  std::cout << "Data Cleaner missing values tests passed!" << std::endl;
}

void test_data_cleaner_duplicate_removal() {
  std::cout << "Testing Data Cleaner duplicate row removal..." << std::endl;

  DataCleaner cleaner;

  // Test data with duplicates (first row treated as header)
  std::vector<std::vector<std::string>> data = {
      {"header1", "header2", "header3"}, // Header row
      {"10", "20", "30"},
      {"15", "25", "35"},
      {"10", "20", "30"}, // Duplicate of first data row
      {"20", "30", "40"},
      {"15", "25", "35"} // Duplicate of second data row
  };

  size_t original_size = data.size();
  cleaner.remove_duplicate_rows(data);

  test_assert(data.size() < original_size, true,
              "duplicate rows should be removed");
  test_assert(data.size(), static_cast<size_t>(4), "should have 4 rows (1 header + 3 unique data)");

  std::cout << "Data Cleaner duplicate removal tests passed!" << std::endl;
}

void test_data_cleaner_format_normalization() {
  std::cout << "Testing Data Cleaner format normalization..." << std::endl;

  DataCleaner cleaner;
  cleaner.set_numeric_precision(2);

  // Test data with various numeric formats (first row treated as header)
  std::vector<std::vector<std::string>> data = {
      {"header1", "header2", "header3"}, // Header row
      {"10.123456", "2021-01-01", "text"},
      {"15.789", "01/15/2021", "more text"},
      {"20.0", "2021/03/01", "another"}};

  cleaner.normalize_formats(data);

  // Check numeric precision (row 1 since row 0 is header)
  test_assert(data[1][0].length() <= 5, true,
              "numeric value should have limited precision"); // e.g., "10.12"

  std::cout << "Data Cleaner format normalization tests passed!" << std::endl;
}

void test_data_cleaner_complete_workflow() {
  std::cout << "Testing Data Cleaner complete workflow..." << std::endl;

  DataCleaner cleaner;

  // Test data with various issues (first row is header)
  std::vector<std::vector<std::string>> data = {
      {"col1", "col2", "col3"}, // Header row
      {"10.123456", "2021-01-01", "text"},
      {"", "25.789", "more text"},         // Missing value
      {"15.000", "NA", "another"},         // NA value
      {"10.123456", "2021-01-01", "text"}, // Duplicate of row 1
      {"20.5", "01/15/2021", "final"}};

  size_t original_size = data.size();
  cleaner.clean_data(data);

  // Should have removed duplicates and handled missing values
  test_assert(data.size() < original_size, true,
              "should have fewer rows after cleaning");

  std::cout << "Data Cleaner complete workflow tests passed!" << std::endl;
}

int main() {
  try {
    test_data_cleaner_missing_values();
    test_data_cleaner_duplicate_removal();
    test_data_cleaner_format_normalization();
    test_data_cleaner_complete_workflow();

    std::cout << std::endl
              << "All Data Cleaner tests passed successfully!" << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
}
