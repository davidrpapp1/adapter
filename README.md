# Adapter - High-Performance Data Cleaning and Preparation Tool

A robust C++ application for cleaning, processing, and aligning CSV data with focus on time series data preparation.

## Features

- **CSV Parsing**: Robust parsing with support for various delimiters and quoted fields
- **Data Cleaning**: Automatic removal of duplicates, handling of missing values, format normalization
- **Time Series Alignment**: Sophisticated time alignment with multiple interpolation methods
- **Configuration Management**: Flexible configuration system with file-based settings
- **High Performance**: Optimized C++ implementation for processing large datasets

## Build Requirements

- C++17 compatible compiler (g++ 7.0+ or clang++ 5.0+)

## Building

```bash
# Build the main executable
make

# Build and run tests
make test

# Build optimized release version
make release

# Build debug version
make debug

# Install system-wide
make install
```

## Usage

### Basic Usage

```bash
# Process a CSV file with default settings
./build/adapter data.csv

# Specify output file
./build/adapter -o cleaned_data.csv data.csv

# Use configuration file
./build/adapter -c config.txt data.csv
```

### Advanced Usage

```bash
# Time series alignment
./build/adapter -t timestamp -d temperature,pressure data.csv

# Custom delimiter
./build/adapter --delimiter ";" data.csv

# Specify variable types
./build/adapter -t time -d dependent_vars -i independent_vars data.csv
```

### Command Line Options

- `-o, --output <file>`: Output file path
- `-t, --time <column>`: Time column name for alignment
- `-d, --dependent <vars>`: Comma-separated dependent variable names
- `-i, --independent <vars>`: Comma-separated independent variable names
- `-c, --config <file>`: Configuration file path
- `--delimiter <char>`: CSV delimiter character
- `-h, --help`: Show help message

## Configuration File

Create a configuration file to specify processing parameters:

```ini
# Input/Output Settings
input_file=data.csv
output_file=cleaned_data.csv

# CSV Format Settings
delimiter=,

# Time Series Settings
time_column=time
target_time_interval=1.0

# Variable Classification
dependent_variables=temperature,pressure
independent_variables=humidity,wind_speed

# Data Processing Settings
numeric_precision=2
date_format=%Y-%m-%d
```

## Architecture

```
include/adapter/
├── csv_parser.hpp      # CSV file parsing
├── data_cleaner.hpp    # Data cleaning operations
├── time_aligner.hpp    # Time series alignment
└── config_manager.hpp  # Configuration management

src/
├── csv_parser.cpp      # CSV parser implementation
├── data_cleaner.cpp    # Data cleaning implementation
├── time_aligner.cpp    # Time alignment implementation
├── config_manager.cpp  # Configuration implementation
└── main.cpp           # Main application

tests/
├── test_csv_parser.cpp     # CSV parser unit tests
├── test_data_cleaner.cpp   # Data cleaner unit tests
└── test_integration.cpp    # Integration tests
```

## Data Processing Pipeline

1. **CSV Parsing**: Load and parse input CSV file with configurable delimiters
2. **Data Cleaning**:
   - Remove duplicate rows
   - Handle missing values (replacement with mean/median/zero)
   - Normalize numeric and date formats
3. **Time Series Alignment**:
   - Align data to uniform time intervals
   - Interpolate missing time points
   - Support for multiple solver methods
4. **Output Generation**: Write processed data to output CSV file

## Development

### Code Formatting
```bash
make format
```

### Static Analysis
```bash
make analyze
```

### Project Structure
```bash
make structure
```

## Examples

### Basic Data Cleaning
```bash
# Clean a CSV file with automatic duplicate removal and missing value handling
./build/adapter sensor_data.csv
```

### Time Series Processing
```bash
# Align time series data with 5-second intervals
./build/adapter -t timestamp -d temperature,pressure,flow sensor_data.csv
```

### Configuration-Based Processing
```bash
# Use predefined configuration
./build/adapter -c production_config.txt large_dataset.csv
```
