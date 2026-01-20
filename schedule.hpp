#pragma once

#include "types.hpp"
#include "gap_manager.hpp"

class Schedule {
public:
  uint m;
  uint n;
  Job_List placed_jobs;
  shared_ptr<Gap_Manager> gap_manager;

  /* Gap_List gap_list; */

  Schedule(uint m, uint n);

  void schedule_job(Job& job, uint time = INVALID_TIME);

  // Expects sorted (by starting_time) list of indices
  // returns unscheduled jobs
  // TODO: does not affect the makespan. 
  Job_List unschedule_jobs(vector<uint> placed_jobs_indices);

  // list schedules jobs without letting the differences of jobs placed be more than p_max
  // makespan - balance_time is the initial upper_bound to not place jobs above
  static void balanced_list_schedule(Job_List jobs, Schedule& sigma1, Schedule& sigma2, sint& balance_time);

  static multiset<pair<uint, size_t>> create_job_pool(Job_List jobs);

  bool list_schedule(Job_List& jobs, uint until_t=INVALID_TIME);

  Job_List update_remaining_jobs_with_job_pool(Job_List jobs, multiset<pair<uint, size_t>> &job_pool);

  // jobs need to have machine requirement at most m/2
  void on_two_stacks(Job_List jobs);

  // returns a list of jobs which were not schedule during this step
  // starts at the top and places repeatedly the widest job which fits directly below the last one
  // assumes the existing jobs are decreasingly placed in machine_requirement
  Job_List schedule_down(Job_List jobs);

  // jobs which start at or below separation_time will be scheduled in s1
  // the remaining jobs will be scheduled in s2
  // TODO: ordered list of separation_times (for mcs)
  void split_at(uint separation_time, Schedule& lower_schedule, Schedule& upper_schedule);

  uint get_makespan();

  void set_makespan(uint new_makespan);

  // assumes that the current schedule is valid for this operation
  void sort_in_higher_stack(Job_List jobs);

  void schedule_jobs_on_top_of_each_other(Job_List jobs, uint start_time=0);

  template<typename Predicate>
  Job_List remove_jobs_if(Predicate should_remove) {
    std::vector<uint> remove_indices;
    
    for (uint i = 0; i < placed_jobs.size(); ++i) {
      if (should_remove(placed_jobs[i])) {
        remove_indices.push_back(i);
      }
    }
    
    return unschedule_jobs(remove_indices);
  }

  // returns the removed jobs
  Job_List remove_jobs_above(uint time);

  void place_schedule_on_top(Schedule& schedule);

  Schedule get_rotated_schedule();

  double calculate_makespan_lower_bound(uint p_max) const {
    if(p_max*n*m > numeric_limits<unsigned long long>::max())
      throw runtime_error("possible area overflow");

    if (m == 0) return 0.0;

    unsigned long long total_area = 0;
    for (const auto& job : placed_jobs) {
        total_area += static_cast<unsigned long long>(job.processing_time) * job.required_machines;
    }

    return static_cast<double>(total_area) / m;
  }

private:

  // until_t ensures that no job will be executed after until_t
  // assumes job_pool.empty()==false
  // returns if procedure should end
  bool list_schedule_single(Job_List jobs, multiset<pair<uint, size_t>> &job_pool, uint until_t=INVALID_TIME);


};

