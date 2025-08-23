#ifndef ADAPTER_CSV_PARSER_HPP
#define ADAPTER_CSV_PARSER_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace adapter {

class CsvParser {
public:
  CsvParser();
  ~CsvParser();

  bool load_file(const std::string &filename);
  bool parse_data();

  std::vector<std::string> get_headers() const;
  std::vector<std::vector<std::string>> get_data() const;
  std::vector<std::string> get_column(const std::string &column_name) const;

  size_t get_row_count() const;
  size_t get_column_count() const;

  void set_delimiter(char delimiter);

private:
  std::string filename;
  char delimiter;
  std::vector<std::string> headers;
  std::vector<std::vector<std::string>> data;
  std::unordered_map<std::string, size_t> column_indices;

  void parse_headers(const std::string &line);
  std::vector<std::string> split_line(const std::string &line) const;
  void build_column_indices();
};

} // namespace adapter

#endif // ADAPTER_CSV_PARSER_HPP
