#include "adapter/csv_parser.hpp"
#include <cassert>
#include <fstream>
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

void test_csv_parser_basic_functionality() {
  std::cout << "Testing CSV Parser basic functionality..." << std::endl;

  // Create a test CSV file
  std::ofstream test_file("test_data.csv");
  test_file << "name,age,city\n";
  test_file << "John,25,New York\n";
  test_file << "Jane,30,Los Angeles\n";
  test_file << "Bob,35,Chicago\n";
  test_file.close();

  CsvParser parser;
  bool loaded = parser.load_file("test_data.csv");
  test_assert(loaded, true, "load_file should succeed");

  auto headers = parser.get_headers();
  test_assert(headers.size(), static_cast<size_t>(3), "should have 3 headers");
  test_assert(headers[0], std::string("name"), "first header should be 'name'");
  test_assert(headers[1], std::string("age"), "second header should be 'age'");
  test_assert(headers[2], std::string("city"), "third header should be 'city'");

  test_assert(parser.get_row_count(), static_cast<size_t>(3),
              "should have 3 data rows");
  test_assert(parser.get_column_count(), static_cast<size_t>(3),
              "should have 3 columns");

  auto name_column = parser.get_column("name");
  test_assert(name_column.size(), static_cast<size_t>(3),
              "name column should have 3 values");
  test_assert(name_column[0], std::string("John"),
              "first name should be 'John'");
  test_assert(name_column[1], std::string("Jane"),
              "second name should be 'Jane'");
  test_assert(name_column[2], std::string("Bob"), "third name should be 'Bob'");

  // Clean up
  std::remove("test_data.csv");

  std::cout << "CSV Parser tests passed!" << std::endl;
}

void test_csv_parser_different_delimiters() {
  std::cout << "Testing CSV Parser with different delimiters..." << std::endl;

  // Create a test CSV file with semicolon delimiter
  std::ofstream test_file("test_semicolon.csv");
  test_file << "product;price;category\n";
  test_file << "Widget A;10.99;Electronics\n";
  test_file << "Widget B;15.50;Home\n";
  test_file.close();

  CsvParser parser;
  parser.set_delimiter(';');
  bool loaded = parser.load_file("test_semicolon.csv");
  test_assert(loaded, true,
              "load_file with semicolon delimiter should succeed");

  auto headers = parser.get_headers();
  test_assert(headers.size(), static_cast<size_t>(3), "should have 3 headers");
  test_assert(headers[0], std::string("product"),
              "first header should be 'product'");

  auto data = parser.get_data();
  test_assert(data.size(), static_cast<size_t>(2), "should have 2 data rows");
  test_assert(data[0][1], std::string("10.99"),
              "first price should be '10.99'");

  // Clean up
  std::remove("test_semicolon.csv");

  std::cout << "CSV Parser delimiter tests passed!" << std::endl;
}

void test_csv_parser_quoted_fields() {
  std::cout << "Testing CSV Parser with quoted fields..." << std::endl;

  // Create a test CSV file with quoted fields containing commas
  std::ofstream test_file("test_quoted.csv");
  test_file << "name,description,value\n";
  test_file << "\"Product A\",\"High quality, durable item\",100\n";
  test_file << "\"Product B\",\"Lightweight, portable\",75\n";
  test_file.close();

  CsvParser parser;
  bool loaded = parser.load_file("test_quoted.csv");
  test_assert(loaded, true, "load_file with quoted fields should succeed");

  auto data = parser.get_data();
  test_assert(data[0][1], std::string("High quality, durable item"),
              "quoted field should preserve commas");
  test_assert(data[1][1], std::string("Lightweight, portable"),
              "second quoted field should be correct");

  // Clean up
  std::remove("test_quoted.csv");

  std::cout << "CSV Parser quoted fields tests passed!" << std::endl;
}

int main() {
  try {
    test_csv_parser_basic_functionality();
    test_csv_parser_different_delimiters();
    test_csv_parser_quoted_fields();

    std::cout << std::endl
              << "All CSV Parser tests passed successfully!" << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
}
