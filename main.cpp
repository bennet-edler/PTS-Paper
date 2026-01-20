
#include "types.hpp"
#include "schedule.hpp"
#include "tower_schedule.hpp"
#include "mcs.hpp"

#include <random>
#include <chrono>

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
  uint n = 10000;
  uint p_max = 100;

  auto jobs = generate_random_jobs(n,m,1,p_max);
  Tower_Schedule tower_schedule(m,n);
  
  cout << "start benchmarking for " << n << " job on " << m << " machines..." << endl;

  auto start = chrono::high_resolution_clock::now();
  tower_schedule.schedule_jobs(jobs);
  auto end   = chrono::high_resolution_clock::now(); 

  chrono::duration<double, milli> duration = end - start;

  cout << "took " << duration.count() << " ms" << endl;

  cout << "makespan: " << tower_schedule.sigma.get_makespan() << endl;

  cout << "ratio is at least " << tower_schedule.sigma.get_makespan()/tower_schedule.sigma.calculate_makespan_lower_bound(p_max) << endl;
}
