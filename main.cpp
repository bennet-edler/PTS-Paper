
#include "types.hpp"



// --- TOWER_SCHEDULE --- {{{
/*
class Tower_Schedule {
  Schedule sigma1;
  Schedule sigma2;

  Job_List big_jobs;
  Job_List medium_jobs;
  Job_List small_jobs;
  Job_List tiny_jobs;

  Tower_Schedule(m, n) 
    : sigma1(m,n), sigma2(m,n)
  {}

  void schedule_jobs(Job_List jobs) {
    // TODO: make global function sort_jobs_by_machine_requirement_decreasing(jobs);
    
    // TODO: calculate p_max

    partition_jobs(jobs); 

    sigma1.list_schedule(big_jobs);
    jobs = sigma1.schedule_down(medium_jobs + small_jobs);
    uint separation time = get_separation_time_from_sigma1(sigma1);
    sigma1.list_schedule(tiny_jobs, until_t=separation_time);


    sigma2.schedule_on_two_stacks(small_jobs+medium_jobs);

    uint sigma2_makespan;
    sigma2.makespan = 0;
    sigma2.list_schedule(tiny_jobs, until_t=sigma2_makespan);
    highest_tiny_job_completion_time = sigma2.makespan();
    sigma2.makespan = max(sigma2_makespan, highest_tiny_job_completion_time); 
    
    if(tiny_jobs.size() != 0) { // many tiny jobs
      Job_List small_and_medium_jobs = 
        remove_small_and_medium_jobs_from_schedule(sigma1);

      sigma2_makespan = sigma2.get_makespan();
      remove_tiny_jobs(sigma2); 
      sigma2.sort_in_higher_stack(small_and_medium_jobs);

      // TODO: list_scheduling_in_higher_gap(sigma1, sigma2);
      
    } else { // few or several tiny jobs
      // TODO (during p_max down. where the last job is \leq 4/3)
      sigma1.split_at(separation_time, sigma1T, sigma1B);       

      sigma2.remove_jobs_above(highest_tiny_job_completion_time);
      list_schedule(medium_jobs);
      list_schedule(small_jobs);

      sigma1B.place_schedule_on_top(sigma2);
      sigma1B.place_schedule_on_top(sigma1T);
    }
  }

  // TODO: schedule.unschedule_jobs();

  void partition_jobs(Job_List jobs) {

  }

  Job_List remove_small_and_medium_jobs_from_schedule(Schedule schedule) {
  }

};
*/
// }}}




int main() {
}
