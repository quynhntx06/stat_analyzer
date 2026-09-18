1. Statistical data analyzer (C++ CLI)
A command-line tool that parses a CSV file containing student records, sanitizes irregular data, calculates descriptive statistics, and detects anomalies using Z-scores.

2. Features
- Data cleaning: Skips corrupted rows, missing markers (NaN, NA, NULL), and invalid scores outside [0, 10].
- Statistical calculations:
  + Mean, variance, and standard deviation
  + 95% Confidence interval for the mean
  + Median (optimized using std::nth_element)
  + Mode and frequency count
  + Minimum and maximum scores
- Outlier detection: flags records with |z| >= 2.0.

3. Build and run
g++ -std=c++17 main.cpp -o analyzer
./analyzer