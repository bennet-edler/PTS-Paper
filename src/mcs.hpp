#pragma once

#include "types.hpp"
#include "gap_manager.hpp"
#include "schedule.hpp"
#include "tower_schedule.hpp"


class MCS_Scheduler {
public:
  uint m;
  uint n;
  uint N;

  MCS_Scheduler(uint m, uint n, uint N)
    : m(m), n(n), N(N)
  {}


  void schedule_jobs(Job_List jobs) {
    Tower_Schedule tower_schedule(m,n);
    tower_schedule.schedule_jobs(jobs);
    
    uint i = N/3;
    uint partition_height = tower_schedule.sigma.get_makespan() / (2*i+1);

    uint current_cut = partition_height;
    uint current_cluster = 1;

    for(auto job : tower_schedule.sigma.placed_jobs) {
      uint starting_time = job.starting_time.value();
      uint completion_time = starting_time + job.processing_time;
      if(completion_time <= current_cut) {
        if(starting_time > current_cut) {
          // use next cluster 
          current_cluster += 1;
          current_cut += partition_height;
        }
        // put job in current_cluster
        cout << "cluster of job is " << current_cluster << endl;

      }
      else {
        // put job in cut
        cout << "cluster of job is " << current_cluster + ((current_cut/partition_height)%2) << endl;
      } 
    }
  }
};
