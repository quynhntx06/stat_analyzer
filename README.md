# Statistical Data Analyzer (C++ CLI)

A command-line tool written in C++17 that parses a CSV file of student scores, cleans irregular data, computes descriptive statistics, and flags anomalous records using z-scores. Implemented with the standard library only, with no external dependencies.

## Features

- **Data cleaning:** skips corrupted rows, missing markers (`NaN`, `NA`, `NULL`, `null`), non-numeric values, and scores outside the valid range [0, 10].
- **Descriptive statistics:**
  - Mean, sample variance, sample standard deviation
  - 95% confidence interval for the mean
  - Median (computed with `std::nth_element`, average O(n))
  - Mode and frequency count
  - Minimum and maximum
- **Outlier detection:** flags records with |z| >= 2.0.
- **Report counters:** valid, invalid, and missing-score records.

## Build and Run

```bash
g++ -std=c++17 -O2 main.cpp -o analyzer
./analyzer                 # reads data.csv by default
./analyzer path/to/file.csv
```

On Windows PowerShell, run `.\analyzer.exe` instead of `./analyzer`.

## Input Format

The bundled `data.csv` is a synthetic sample of 35 rows covering missing markers, malformed values, out-of-range scores, and one true outlier.

A CSV file with three columns: `id`, `name`, `score`. The first line is treated as a header and skipped.

```csv
id,name,score
SV001,Student A,7.5
SV002,Student B,8.2
SV003,Student C,NaN
SV004,Student D,NA
SV005,Student E,11.5
SV006,Student F,abc
```

A row is counted as invalid and excluded from the statistics if:

- `id` or `name` is empty;
- the score is empty or one of `NaN`, `NA`, `NULL`, `null` (these are also counted as missing);
- the score is not fully numeric, or not finite;
- the score is outside [0, 10].

A decimal comma is accepted only when the field is quoted (e.g. `"8,5"`) and is read as 8.5. A row with the wrong number of fields is counted as invalid.

## Example Output

```text
Loading dataset from: data.csv...

=======================================================
              STATISTICAL SUMMARY REPORT               
=======================================================
Total rows: 35
Valid records          : 26
Invalid records        : 9
Missing score records  : 5

Minimum score (min)    : 0.5000
Maximum score (max)    : 10.0000
Mean score (mean)      : 7.2038
Median score (median)  : 7.4000
Mode score (mode)      : 7.5000 (repeated 5 times)
Sample variance (s^2)  : 2.4772
Sample std dev (s)     : 1.5739
95% Confidence interval: [6.5989, 7.8088]
=======================================================

---------------- OUTLIER DETECTION (|z| >= 2.0000) ----------------
[WARNING] ID: SV012      | Name: Ly Tuan Kiet              | Score: 0.50  | z-score: -4.26
```

## Methodology

| Quantity | Definition |
|---|---|
| Mean | x̄ = (1/n) Σ xᵢ |
| Variance | s² = Σ (xᵢ − x̄)² / (n − 1) (sample variance) |
| Standard deviation | s = √s² |
| 95% CI of the mean | x̄ ± 1.96 · s / √n (normal approximation) |
| Median | Middle value for odd n; for even n, the average of the two middle values |
| Mode | Most frequent value; the smallest one if several tie; "no mode" if every value is unique |
| Outlier rule | zᵢ = (xᵢ − x̄) / s; flagged if abs(zᵢ) >= 2.0 |

**Median with `nth_element`:** sorting the whole array costs O(n log n), while `std::nth_element` only places the middle element in its correct position, which takes O(n) on average. For even n, the lower middle value is the maximum of the left partition (found with `std::max_element`), which is the correct value because `nth_element` guarantees every element in the left partition is less than or equal to the middle element.

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

## Limitations

- The z-score rule assumes the data is roughly normally distributed. Outliers also inflate the mean and standard deviation, which can hide other outliers.
- The confidence interval uses the normal approximation (z = 1.96), which is less accurate for small samples than the t-distribution.
- The CSV parser is minimal: quoted fields are supported but escaped quotes are not, and a header row is assumed.
- Invalid rows are skipped rather than repaired, so heavy corruption reduces the effective sample size.
- The whole file is loaded into memory and processed on a single thread.

## Future Work

- Add robust outlier detection (IQR, MAD) alongside z-score.
- Validate the outputs against Python (pandas / SciPy) on the same dataset.
- Add unit tests for the parser and the statistics functions.
- Use the t-distribution for the confidence interval when n is small.

## Project Structure

```text
.
├── main.cpp      # analyzer
├── bench.cpp     # nth_element vs sort benchmark
├── data.csv      # sample dataset
└── README.md
```
