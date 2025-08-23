#include "adapter/csv_parser.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace adapter {

CsvParser::CsvParser() : delimiter(',') {}

CsvParser::~CsvParser() {}

bool CsvParser::load_file(const std::string &filename) {
  this->filename = filename;
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << std::endl;
    return false;
  }

  data.clear();
  headers.clear();
  column_indices.clear();

  std::string line;
  bool is_first_line = true;

  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    if (is_first_line) {
      parse_headers(line);
      is_first_line = false;
    } else {
      std::vector<std::string> row = split_line(line);
      if (row.size() == headers.size()) {
        data.push_back(row);
      } else {
        std::cerr << "Warning: Skipping malformed row with " << row.size()
                  << " columns (expected " << headers.size() << ")"
                  << std::endl;
      }
    }
  }

  file.close();
  build_column_indices();
  return true;
}

bool CsvParser::parse_data() {
  if (filename.empty()) {
    std::cerr << "Error: No file loaded" << std::endl;
    return false;
  }
  return load_file(filename);
}

std::vector<std::string> CsvParser::get_headers() const { return headers; }

std::vector<std::vector<std::string>> CsvParser::get_data() const {
  return data;
}

std::vector<std::string>
CsvParser::get_column(const std::string &column_name) const {
  std::vector<std::string> column;

  auto it = column_indices.find(column_name);
  if (it == column_indices.end()) {
    std::cerr << "Error: Column '" << column_name << "' not found" << std::endl;
    return column;
  }

  size_t column_index = it->second;
  for (const auto &row : data) {
    if (column_index < row.size()) {
      column.push_back(row[column_index]);
    }
  }

  return column;
}

size_t CsvParser::get_row_count() const { return data.size(); }

size_t CsvParser::get_column_count() const { return headers.size(); }

void CsvParser::set_delimiter(char delimiter) { this->delimiter = delimiter; }

void CsvParser::parse_headers(const std::string &line) {
  headers = split_line(line);

  // Trim whitespace from headers
  for (auto &header : headers) {
    header.erase(header.begin(), std::find_if(header.begin(), header.end(),
                                              [](unsigned char ch) {
                                                return !std::isspace(ch);
                                              }));
    header.erase(
        std::find_if(header.rbegin(), header.rend(),
                     [](unsigned char ch) { return !std::isspace(ch); })
            .base(),
        header.end());
  }
}

std::vector<std::string> CsvParser::split_line(const std::string &line) const {
  std::vector<std::string> result;
  std::stringstream ss(line);
  std::string cell;

  bool in_quotes = false;
  std::string current_cell;

  for (size_t i = 0; i < line.length(); ++i) {
    char c = line[i];

    if (c == '"') {
      in_quotes = !in_quotes;
    } else if (c == delimiter && !in_quotes) {
      result.push_back(current_cell);
      current_cell.clear();
    } else {
      current_cell += c;
    }
  }

  // Add the last cell
  result.push_back(current_cell);

  // Clean up quotes from cells
  for (auto &cell : result) {
    if (cell.length() >= 2 && cell.front() == '"' && cell.back() == '"') {
      cell = cell.substr(1, cell.length() - 2);
    }

    // Trim whitespace
    cell.erase(cell.begin(),
               std::find_if(cell.begin(), cell.end(), [](unsigned char ch) {
                 return !std::isspace(ch);
               }));
    cell.erase(std::find_if(cell.rbegin(), cell.rend(),
                            [](unsigned char ch) { return !std::isspace(ch); })
                   .base(),
               cell.end());
  }

  return result;
}

void CsvParser::build_column_indices() {
  column_indices.clear();
  for (size_t i = 0; i < headers.size(); ++i) {
    column_indices[headers[i]] = i;
  }
}

} // namespace adapter
