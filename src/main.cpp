
#include "types.hpp"
#include "schedule.hpp"
#include "tower_schedule.hpp"
#include "mcs.hpp"

#include <random>
#include <chrono>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void save_instance(const Job_List& jobs, const string& filename) {
  ofstream out(filename);
  for (const auto& job : jobs) {
    out << job.processing_time << " " << job.required_machines << "\n"; 
  }
}

Job_List load_instance(const string& filename) {
  Job_List jobs;
  ifstream in(filename);
  uint processing_time, required_machines;
  while (in >> processing_time >> required_machines) {
    jobs.push_back(Job(processing_time, required_machines));
  }
  return jobs;
}

Job_List generate_random_jobs(uint n, uint m, uint p_min = 1, uint p_max = 100) {
  Job_List jobs;
  jobs.reserve(n);

  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<uint> dist_time(p_min, p_max);
  uniform_int_distribution<uint> dist_machines(1, m);

  for(uint i = 0; i < n; ++i) {
    jobs.emplace_back(dist_time(gen), dist_machines(gen));
  }
  return jobs;
}

int main() {
  uint m = 100000;
  uint p_max = 100;

  std::ofstream data_file("benchmark/benchmark_results.csv");
  data_file << "n,time_ms,makespan\n"; 

  for (uint n = 100; n <= 100000; n += 100) {
    // load/create jobs
    string instance_path = "benchmark/instances/inst_n" + to_string(n) + "_m" + to_string(m) + ".txt";
    Job_List jobs;
    if (fs::exists(instance_path)) {
      cout << "Load existing file: n=" << n << "..." << flush;
      jobs = load_instance(instance_path);
    } else {
      cout << "Generate new instance: n=" << n << "..." << flush;
      jobs = generate_random_jobs(n, m, 1, p_max);
      save_instance(jobs, instance_path);
    }

    Tower_Schedule tower_schedule(m, n);

    // measure time of the function
    auto start = std::chrono::high_resolution_clock::now();
    tower_schedule.schedule_jobs(jobs);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    double makespan = tower_schedule.sigma.get_makespan();

    // write to file
    data_file << n << "," << duration.count() << "," << makespan << "\n";

    // log
    cout << "took " << duration.count() << " ms" << endl;
    cout << "makespan: " << tower_schedule.sigma.get_makespan() << endl;
    cout << "ratio is at least " << tower_schedule.sigma.get_makespan()/tower_schedule.sigma.calculate_makespan_lower_bound(p_max) << endl << endl;
  }

  data_file.close();
  return 0;
}
