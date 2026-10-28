# Statistical Data Analyzer (C++ CLI)

A command-line tool written in C++17 that parses a CSV file of student scores, cleans irregular data, computes descriptive statistics, and flags anomalous records using z-scores.

## Features

- **Data cleaning:** Skips corrupted rows, missing markers (`NaN`, `NA`, `NULL`, `null`), non-numeric values, and scores outside [0, 10]. Handles quoted decimal commas (e.g. `"8,5"`).
- **Descriptive statistics:**
  - Mean, sample variance, sample standard deviation
  - 95% confidence interval for the mean
  - Median (computed with `std::nth_element`, average O(n))
  - Mode and frequency count
  - Minimum and maximum
- **Outlier detection:** Flags records with |z| >= 2.0.
- **Report counters:** Valid, invalid, and missing-score records.

## Build and Run

```bash
g++ -std=c++17 -O2 main.cpp -o analyzer
./analyzer                 # reads data.csv by default
./analyzer path/to/file.csv
On Windows PowerShell, run `.\analyzer.exe` instead of `./analyzer`.

## Benchmark

Median of N random doubles in [0, 10]: `std::sort` versus `std::nth_element`, averaged over 5 runs per size.

| N | std::sort | nth_element | Speedup |
|---|---|---|---|
| 100,000 | 40.6 ms | 4.8 ms | ~8.5x |
| 1,000,000 | 283.3 ms | 29.4 ms | ~9.6x |
| 5,000,000 | 1407.2 ms | 146.8 ms | ~9.6x |

Environment: g++ -O2 -std=c++17, Windows 11. Absolute timings vary between runs; the speedup ratio is stable.

To reproduce:

```bash
g++ -O2 -std=c++17 bench.cpp -o bench
./bench
```

## Future Work

- Add robust outlier detection (IQR, MAD) alongside z-score.
- Validate the outputs against Python (pandas / SciPy) on the same dataset.
- Add unit tests for the parser and the statistics functions.
- Use the t-distribution for the confidence interval when n is small.

## Project Structure

.
├── main.cpp      # CLI analytics engine
├── bench.cpp     # Benchmark for nth_element vs sort
├── data.csv      # Sample test dataset
└── README.md