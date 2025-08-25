# Adapter - High-Performance Data Cleaning and Preparation Tool

A robust, high-performance C++ application for cleaning, processing, and aligning CSV data with a focus on time series data preparation.

## Quick Start

### Prerequisites

- C++17 compatible compiler (g++ 7.0+ or clang++ 5.0+)
- Make

### Building

```bash
# Clone the repository
git clone https://github.com/davidrpapp1/adapter.git
cd adapter

# Build the main executable
make

# Build and run tests
make test

# Build optimized release version
make release
```

### Basic Usage

```bash
# Process a CSV file with default settings
./build/adapter data.csv

# Time series alignment with 5-second intervals
./build/adapter -t timestamp -d temperature,pressure,flow sensor_data.csv

# Custom delimiter and configuration
./build/adapter -c config.txt --delimiter ";" data.csv
```

## Examples

The `sample_data/` directory contains example files demonstrating various use cases:

### 1. Basic Data Cleaning
```bash
./build/adapter sample_data/data.csv
```
- Removes duplicate rows
- Fills missing values with calculated means
- Normalizes numeric precision
- Creates time-aligned interpolated series

### 2. Time Series Processing
```bash
./build/adapter -t timestamp -d temperature,pressure,flow sample_data/sensor_data.csv
```
- Processes high-frequency sensor data
- Creates uniform 1-second intervals
- Interpolates missing time points

### 3. Production Data Processing
```bash
./build/adapter -c sample_data/production_config.txt sample_data/large_dataset.csv
```
- Uses configuration file for complex setups
- Processes daily time series data
- Handles system monitoring data with missing values

### 4. Custom Delimiters
```bash
./build/adapter --delimiter ";" sample_data/semicolon_data.csv
```
- Processes semicolon-delimited files
- Handles mixed data types
- Preserves non-numeric fields

## Command Line Options

| Option | Description |
|--------|-------------|
| `-o, --output <file>` | Output file path |
| `-t, --time <column>` | Time column name for alignment |
| `-d, --dependent <vars>` | Comma-separated dependent variable names |
| `-i, --independent <vars>` | Comma-separated independent variable names |
| `-c, --config <file>` | Configuration file path |
| `--delimiter <char>` | CSV delimiter character |
| `-h, --help` | Show help message |

## Configuration File

Create a configuration file to specify processing parameters:

```ini
# Input/Output Settings
input_file=data.csv
output_file=cleaned_data.csv

# CSV Format Settings
delimiter=,

# Time Series Settings
time_column=timestamp
target_time_interval=1.0

# Variable Classification
dependent_variables=temperature,pressure,flow_rate
independent_variables=humidity,wind_speed

# Data Processing Settings
numeric_precision=2
date_format=%Y-%m-%d

# Solver Settings
solver_method=linear
```

## Development

### Building for Development
```bash
# Debug build with symbols
make debug

# Static analysis
make analyze

# Code formatting
make format

# View project structure
make structure
```

### Running Tests
```bash
# Run all test suites
make test

# Run specific test suite
./build/test_csv_parser
./build/test_data_cleaner
./build/test_integration
```
