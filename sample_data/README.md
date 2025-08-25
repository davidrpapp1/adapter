# Sample Data Files for Adapter Testing

This directory contains sample CSV files that demonstrate various use cases for the adapter:

## Files

### 1. `data.csv`
- **Purpose**: Default example matching the main config.txt file
- **Features**:
  - Irregular time intervals (demonstrates time alignment)
  - Missing values (empty pressure value)
  - Duplicate rows (line 8 and 9 are identical)
  - Variables match config: temperature, pressure, flow_rate (dependent) and humidity, wind_speed (independent)

### 2. `sensor_data.csv`
- **Purpose**: Demonstrates basic data cleaning and time series processing examples from README
- **Features**:
  - 5-second intervals (matches README example)
  - Variables: temperature, pressure, flow

### 3. `large_dataset.csv`
- **Purpose**: Configuration-based processing example
- **Features**:
  - System monitoring data (CPU, memory, disk, network)
  - Contains NaN values and missing data
  - Duplicate rows
  - Works with `production_config.txt`

### 4. `semicolon_data.csv`
- **Purpose**: Custom delimiter demonstration
- **Features**:
  - Uses semicolon (;) as delimiter
  - Mixed data types (numeric and status strings)
  - Missing values

### 5. `production_config.txt`
- **Purpose**: Example configuration file for large_dataset.csv
- **Features**:
  - Daily interval time alignment
  - Specific variable classification
  - Lower precision settings for large datasets

## Usage Examples

```bash
# Process default data with main config
./build/adapter sample_data/data.csv

# Time series with 5-second intervals
./build/adapter -t timestamp -d temperature,pressure,flow sample_data/sensor_data.csv

# Large dataset with production config
./build/adapter -c sample_data/production_config.txt sample_data/large_dataset.csv

# Custom delimiter processing
./build/adapter --delimiter ";" sample_data/semicolon_data.csv
```
