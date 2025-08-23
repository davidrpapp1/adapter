#include "adapter/config_manager.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace adapter {

ConfigManager::ConfigManager() { set_default_values(); }

ConfigManager::~ConfigManager() {}

bool ConfigManager::load_from_file(const std::string &config_file) {
  std::ifstream file(config_file);
  if (!file.is_open()) {
    std::cerr << "Warning: Could not open config file " << config_file
              << std::endl;
    std::cerr << "Using default settings" << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Parse key=value pairs
    size_t equals_pos = line.find('=');
    if (equals_pos != std::string::npos) {
      std::string key = line.substr(0, equals_pos);
      std::string value = line.substr(equals_pos + 1);

      // Trim whitespace
      key.erase(key.begin(),
                std::find_if(key.begin(), key.end(), [](unsigned char ch) {
                  return !std::isspace(ch);
                }));
      key.erase(std::find_if(key.rbegin(), key.rend(),
                             [](unsigned char ch) { return !std::isspace(ch); })
                    .base(),
                key.end());

      value.erase(value.begin(), std::find_if(value.begin(), value.end(),
                                              [](unsigned char ch) {
                                                return !std::isspace(ch);
                                              }));
      value.erase(
          std::find_if(value.rbegin(), value.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          value.end());

      settings[key] = value;
    }
  }

  file.close();
  std::cout << "Configuration loaded from " << config_file << std::endl;
  return true;
}

bool ConfigManager::save_to_file(const std::string &config_file) const {
  std::ofstream file(config_file);
  if (!file.is_open()) {
    std::cerr << "Error: Could not create config file " << config_file
              << std::endl;
    return false;
  }

  file << "# Adapter Configuration File" << std::endl;
  file << "# Generated automatically" << std::endl;
  file << std::endl;

  for (const auto &setting : settings) {
    file << setting.first << "=" << setting.second << std::endl;
  }

  file.close();
  std::cout << "Configuration saved to " << config_file << std::endl;
  return true;
}

void ConfigManager::set_input_file(const std::string &filename) {
  settings["input_file"] = filename;
}

void ConfigManager::set_output_file(const std::string &filename) {
  settings["output_file"] = filename;
}

void ConfigManager::set_dependent_variables(
    const std::vector<std::string> &variables) {
  settings["dependent_variables"] = join_string_list(variables);
}

void ConfigManager::set_independent_variables(
    const std::vector<std::string> &variables) {
  settings["independent_variables"] = join_string_list(variables);
}

void ConfigManager::set_time_column(const std::string &column_name) {
  settings["time_column"] = column_name;
}

void ConfigManager::set_delimiter(char delimiter) {
  settings["delimiter"] = std::string(1, delimiter);
}

std::string ConfigManager::get_input_file() const {
  auto it = settings.find("input_file");
  return (it != settings.end()) ? it->second : "";
}

std::string ConfigManager::get_output_file() const {
  auto it = settings.find("output_file");
  return (it != settings.end()) ? it->second : "output.csv";
}

std::vector<std::string> ConfigManager::get_dependent_variables() const {
  auto it = settings.find("dependent_variables");
  return (it != settings.end()) ? parse_string_list(it->second)
                                : std::vector<std::string>();
}

std::vector<std::string> ConfigManager::get_independent_variables() const {
  auto it = settings.find("independent_variables");
  return (it != settings.end()) ? parse_string_list(it->second)
                                : std::vector<std::string>();
}

std::string ConfigManager::get_time_column() const {
  auto it = settings.find("time_column");
  return (it != settings.end()) ? it->second : "time";
}

char ConfigManager::get_delimiter() const {
  auto it = settings.find("delimiter");
  return (it != settings.end() && !it->second.empty()) ? it->second[0] : ',';
}

void ConfigManager::print_configuration() const {
  std::cout << "=== Current Configuration ===" << std::endl;
  std::cout << "Input File: " << get_input_file() << std::endl;
  std::cout << "Output File: " << get_output_file() << std::endl;
  std::cout << "Time Column: " << get_time_column() << std::endl;
  std::cout << "Delimiter: '" << get_delimiter() << "'" << std::endl;

  auto dep_vars = get_dependent_variables();
  std::cout << "Dependent Variables: ";
  for (size_t i = 0; i < dep_vars.size(); ++i) {
    std::cout << dep_vars[i];
    if (i < dep_vars.size() - 1)
      std::cout << ", ";
  }
  std::cout << std::endl;

  auto indep_vars = get_independent_variables();
  std::cout << "Independent Variables: ";
  for (size_t i = 0; i < indep_vars.size(); ++i) {
    std::cout << indep_vars[i];
    if (i < indep_vars.size() - 1)
      std::cout << ", ";
  }
  std::cout << std::endl;
  std::cout << "=============================" << std::endl;
}

void ConfigManager::set_default_values() {
  settings["input_file"] = "";
  settings["output_file"] = "output.csv";
  settings["dependent_variables"] = "";
  settings["independent_variables"] = "";
  settings["time_column"] = "time";
  settings["delimiter"] = ",";
  settings["target_time_interval"] = "1.0";
  settings["solver_method"] = "linear";
  settings["numeric_precision"] = "2";
  settings["date_format"] = "%Y-%m-%d";
}

std::vector<std::string>
ConfigManager::parse_string_list(const std::string &value) const {
  std::vector<std::string> result;
  if (value.empty()) {
    return result;
  }

  std::stringstream ss(value);
  std::string item;

  while (std::getline(ss, item, ',')) {
    // Trim whitespace
    item.erase(item.begin(),
               std::find_if(item.begin(), item.end(), [](unsigned char ch) {
                 return !std::isspace(ch);
               }));
    item.erase(std::find_if(item.rbegin(), item.rend(),
                            [](unsigned char ch) { return !std::isspace(ch); })
                   .base(),
               item.end());

    if (!item.empty()) {
      result.push_back(item);
    }
  }

  return result;
}

std::string
ConfigManager::join_string_list(const std::vector<std::string> &values) const {
  if (values.empty()) {
    return "";
  }

  std::string result = values[0];
  for (size_t i = 1; i < values.size(); ++i) {
    result += "," + values[i];
  }

  return result;
}

} // namespace adapter
