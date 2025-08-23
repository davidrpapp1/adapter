#ifndef ADAPTER_DATA_CLEANER_HPP
#define ADAPTER_DATA_CLEANER_HPP

#include <set>
#include <string>
#include <vector>

namespace adapter {

class DataCleaner {
public:
  DataCleaner();
  ~DataCleaner();

  void clean_data(std::vector<std::vector<std::string>> &data);
  void remove_duplicate_rows(std::vector<std::vector<std::string>> &data);
  void handle_missing_values(std::vector<std::vector<std::string>> &data);
  void normalize_formats(std::vector<std::vector<std::string>> &data);

  void set_missing_value_strategies(const std::vector<std::string> &strategies);
  void set_date_format(const std::string &format);
  void set_numeric_precision(int precision);

private:
  std::vector<std::string> missing_value_strategies;
  std::string date_format;
  int numeric_precision;

  bool is_numeric(const std::string &value) const;
  bool is_date(const std::string &value) const;
  std::string normalize_date_format(const std::string &value) const;
  std::string normalize_numeric_format(const std::string &value) const;
  std::string calculate_mean(const std::vector<std::string> &column) const;
  std::string calculate_median(const std::vector<std::string> &column) const;
};

} // namespace adapter

#endif // ADAPTER_DATA_CLEANER_HPP
