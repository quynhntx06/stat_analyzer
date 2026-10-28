#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

int main(){
    const int RUNS = 5;
    mt19937 rng(42);
    uniform_real_distribution<double> dist(0, 10);
    for(size_t N : {100000u, 1000000u, 5000000u}){
        vector<double> base(N);
        for(auto& x : base) x = dist(rng);
        double t_sort = 0, t_nth = 0;
        volatile double sink = 0;
        for(int r = 0; r < RUNS; r++){
            auto a = base;
            auto s0 = steady_clock::now();
            sort(a.begin(), a.end());
            double med_sort = (N % 2 == 1) ? a[N / 2] : (a[N / 2 - 1] + a[N / 2]) / 2.0;
            sink += med_sort;
            auto s1 = steady_clock::now();
            t_sort += duration<double, milli>(s1 - s0).count();
            auto b = base;
            auto n0 = steady_clock::now();
            size_t mid = N / 2;
            nth_element(b.begin(), b.begin() + mid, b.end());
            double med_nth = 0;
            if (N % 2 == 1) {
                med_nth = b[mid];
            } else {
                auto x = max_element(b.begin(), b.begin() + mid);
                med_nth = (*x + b[mid]) / 2.0;
            }
            sink += med_nth;
            auto n1 = steady_clock::now();
            t_nth += duration<double, milli>(n1 - n0).count();
        }
        printf("N=%-7zu | sort: %6.1f ms | nth_element: %5.1f ms | speedup: %4.1fx\n",
               N, t_sort / RUNS, t_nth / RUNS, t_sort / t_nth);
    }
    return 0;
}