#ifndef ADAPTER_CONFIG_MANAGER_HPP
#define ADAPTER_CONFIG_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace adapter {

class ConfigManager {
public:
  ConfigManager();
  ~ConfigManager();

  bool load_from_file(const std::string &config_file);
  bool save_to_file(const std::string &config_file) const;

  void set_input_file(const std::string &filename);
  void set_output_file(const std::string &filename);
  void set_dependent_variables(const std::vector<std::string> &variables);
  void set_independent_variables(const std::vector<std::string> &variables);
  void set_time_column(const std::string &column_name);
  void set_delimiter(char delimiter);
  void set_target_time_interval(double interval);

  std::string get_input_file() const;
  std::string get_output_file() const;
  std::vector<std::string> get_dependent_variables() const;
  std::vector<std::string> get_independent_variables() const;
  std::string get_time_column() const;
  char get_delimiter() const;
  double get_target_time_interval() const;

  void print_configuration() const;

private:
  std::unordered_map<std::string, std::string> settings;

  void set_default_values();
  std::vector<std::string> parse_string_list(const std::string &value) const;
  std::string join_string_list(const std::vector<std::string> &values) const;
};

} // namespace adapter

#endif // ADAPTER_CONFIG_MANAGER_HPP
