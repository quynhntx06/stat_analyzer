#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;
using db = double;

struct student{
    string id, name;
    db sc;
};

using st = student;

int inp(const string& p, vector<st>& sts, vector<db>& scs, int& invalid, int& miss){
    ifstream in(p);
    if(!in.is_open()){
        cerr << "Error : Cannot open file: " << p << "\n";
        return 0;
    }
    string l; getline(in, l);
    while(getline(in, l)){
        if(l.empty()){
            continue;
        }
        stringstream ss(l);
        string id, name, s;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, s, ',');
        if(id.empty() || name.empty()){
            invalid++;
            continue;
        }
        if(s.empty() || s == "NaN" || s == "NA" || s == "NULL" || s == "null"){
            invalid++;
            miss++;
            continue;
        }
        db score;
        try{
            size_t pos;
            score = stod(s, &pos);
            if(pos != s.size()){
                invalid++;
                continue;
            }
        }catch(...){
            invalid++;
            continue;
        }
        if(!isfinite(score)){
            invalid++;
            continue;
        }
        if(score < 0 || score > 10){
            invalid++;
            continue;
        }
        sts.push_back({id, name, score});
        scs.push_back(score);
    }
    sts.shrink_to_fit();
    scs.shrink_to_fit();
    in.close();
    return 1;
}

db Mean(const vector<db>& scs){
    if(scs.empty()) return 0.0;
    db sum = 0.0;
    for(db x : scs){
        sum += x;
    }
    return sum / scs.size();
}

db Median(const vector<db>& scs){
    if(scs.empty()) return 0.0;
    vector<db> tmp = scs;
    int n = tmp.size();
    int mid = n / 2;
    nth_element(tmp.begin(), tmp.begin() + mid, tmp.end());
    if(n % 2 == 1){
        return tmp[mid];
    }
    else{
        auto x = max_element(tmp.begin(), tmp.begin() + mid);
        return (*x + tmp[mid]) / 2.0;
    }
}

pair<db, int> Mode(const vector<db>& scs){
    if(scs.empty()) return {0.0, 0};
    map<db, int> fre;
    for(db x : scs){
        fre[x]++;
    }
    db mode_val = scs[0];
    int max_fre = 0;
    for(const auto& [val, count] : fre){
        if(count > max_fre){
            max_fre = count;
            mode_val = val;
        }
    }
    if(max_fre == 1) return {0.0, 0};
    return {mode_val, max_fre};
}

db Var(const vector<db>& scs, db mean){
    if(scs.size() <= 1) return 0.0;
    db sum = 0.0;
    for(db x : scs){
        sum += (x - mean) * (x - mean);
    }
    return sum / (scs.size() - 1);
}

db stddev(double var){
    return sqrt(var);
}

pair<db, db> estimate(db mean, db std_dev, int n){
    if(n == 0) return {0.0, 0.0};
    db margin = 1.96 * (std_dev / sqrt(db(n)));
    return {mean - margin, mean + margin};
}

void Outliers(const vector<st>& sts, db mean, db std_dev, db t = 2.0){
    cout << "\n---------------- OUTLIER DETECTION (|z| >= " << t << ") ----------------\n";
    if(std_dev == 0.0){
        cout << "Standard deviation is zero, cannot compute z-scores!\n";
        return;
    }
    bool check = false;
    for(const auto& st : sts){
        db z = (st.sc - mean) / std_dev;
        if(fabs(z) >= t){
            check = true;
            cout << "[WARNING] ID: " << setw(10) << left << st.id;
            cout << " | Name: " << setw(25) << left << st.name;
            cout << " | Score: " << setw(5) << fixed << setprecision(2) << st.sc;
            cout << " | z-score: " << fixed << setprecision(2) << z << "\n";
        }
    }
    if(!check){
        cout << "No outliers detected!\n";
    }
}

void outp(const vector<st>& sts, const vector<db>& scs, int invalid, int miss){
    if(scs.empty()){
        cout << "\nNo valid data available for report generation!\n";
        cout << "Invalid records: " << invalid << "\n";
        return;
    }
    int n = scs.size();
    db mean = Mean(scs);
    db median = Median(scs);
    auto [mode_val, mode_fre] = Mode(scs);
    db var = Var(scs, mean);
    db std_dev = stddev(var);
    auto [x, y] = estimate(mean, std_dev, n);
    auto min_it = min_element(scs.begin(), scs.end());
    auto max_it = max_element(scs.begin(), scs.end());
    cout << "\n=======================================================\n";
    cout << "              STATISTICAL SUMMARY REPORT               \n";
    cout << "=======================================================\n";
    cout << "Total records (n): " << n + invalid << "\n";
    cout << "Valid records          : " << n << "\n";
    cout << "Invalid records        : " << invalid << "\n";
    cout << "Missing score records  : " << miss << "\n";
    cout << "\n";
    cout << fixed << setprecision(4);
    cout << "Minimum score (min)    : " << *min_it << "\n";
    cout << "Maximum score (max)    : " << *max_it << "\n";
    cout << "Mean score (mean)      : " << mean << "\n";
    cout << "Median score (median)  : " << median << "\n";
    if(mode_fre == 0){
    cout << "Mode score             : " << "no mode\n";
    }
    else{
    cout << "Mode score (mode)      : " << mode_val << " (repeated " << mode_fre << " times)\n";
    }
    cout << "Sample variance (s^2)  : " << var << "\n";
    cout << "Sample std dev (s)     : " << std_dev << "\n";
    cout << "95% Confidence interval: [" << x << ", " << y << "]\n";
    cout << "=======================================================\n";
    Outliers(sts, mean, std_dev, 2.0);
}

int main(int argc, char* argv[]){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    string p = (argc > 1) ? argv[1] : "data.csv";
    vector<st> sts;
    vector<db> scs;
    int invalid = 0, miss = 0;
    cout << "Loading dataset from: " << p << "...\n";
    if (!inp(p, sts, scs, invalid, miss)) return 1;
    outp(sts, scs, invalid, miss);
    return 0;
}