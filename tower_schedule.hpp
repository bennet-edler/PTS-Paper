#pragma once

#include "types.hpp"
#include "gap_manager.hpp"
#include "schedule.hpp"


class Tower_Schedule {
public:
  Schedule sigma1;
  Schedule sigma2;

  Schedule sigma;

  Job_List tiny_jobs;
  Job_List small_jobs;
  Job_List medium_jobs;
  Job_List big_jobs;

  uint m;
  uint n;

  Tower_Schedule(uint m, uint n);

  bool is_tiny_job(Job job);

  bool is_small_job(Job job);

  bool is_medium_job(Job job);

  bool is_big_job(Job job);

  void schedule_jobs(Job_List jobs);

  void partition_jobs(Job_List jobs);

  uint height(Job_List jobs);

  Job_List remove_small_and_medium_jobs(Schedule& schedule);

  Job_List remove_tiny_jobs(Schedule& schedule);

  // TODO: make helper function in schedule to avoid acessing gap_manager
  // find earliest time tau where machine usage is <= 2/3 m
  // if there is no such time tau=makespan
  uint get_separation_time_from_sigma1(Schedule sigma1, uint p_max, bool& skip_to_many_jobs);

};
