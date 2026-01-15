
#include "types.hpp"



// --- TOWER_SCHEDULE --- {{{
class Tower_Schedule {
  Schedule sigma1;
  Schedule sigma2;

  Job_List tiny_jobs;
  Job_List small_jobs;
  Job_List medium_jobs;
  Job_List big_jobs;

  uint m;
  uint n;

  Tower_Schedule(uint m, uint n) 
    : m(m), n(n), sigma1(m,n), sigma2(m,n)
  {}

  bool is_tiny_job(Job job) {
    return 4*job.processing_time <= m;
  }

  bool is_small_job(Job job) {
    return m < 4*job.processing_time && 3*job.processing_time <= m;
  }

  bool is_medium_job(Job job) {
    return m < 3*job.processing_time && 2*job.processing_time <= m;
  }

  bool is_big_job(Job job) {
    return m < 2*job.processing_time;
  }



  void schedule_jobs(Job_List jobs) {
    uint p_max = 0;
    for(const auto& job : jobs)
      p_max = max(p_max, job.processing_time);


    partition_jobs(jobs); 

    sigma1.list_schedule(big_jobs);
    Job_List remaining_medium_and_small_jobs 
      = sigma1.schedule_down(medium_jobs + small_jobs);
    uint separation_time = get_separation_time_from_sigma1(sigma1); 
    sigma1.list_schedule(tiny_jobs, /*until_t=*/separation_time); 

    sigma2.on_two_stacks(small_jobs+medium_jobs);

    uint sigma2_makespan = sigma2.get_makespan();
    sigma2.set_makespan(0);
    sigma2.list_schedule(tiny_jobs, /*until_t=*/sigma2_makespan);
    uint highest_tiny_job_completion_time = sigma2.get_makespan();
    sigma2.set_makespan(max(sigma2_makespan, highest_tiny_job_completion_time));
    
    if(tiny_jobs.size() != 0) { // many tiny jobs
      Job_List small_and_medium_jobs = 
        remove_small_and_medium_jobs(sigma1); 
      
      sint height_of_removed_jobs = static_cast<sint>(height(small_and_medium_jobs));

      Job_List additional_tiny_jobs = remove_tiny_jobs(sigma2); 
      tiny_jobs = tiny_jobs + additional_tiny_jobs;
      sigma2.sort_in_higher_stack(small_and_medium_jobs);

      Schedule::balanced_list_schedule(tiny_jobs, sigma1, sigma2, /*balance_height=*/height_of_removed_jobs, p_max);
    } else { // few or several tiny jobs
      Schedule sigma1T(m,n), sigma1B(m,n);
      sigma1.split_at(separation_time, sigma1T, sigma1B);       

      Job_List removed_jobs = sigma2.remove_jobs_above(highest_tiny_job_completion_time);
      sigma2.list_schedule(removed_jobs);

      sigma1B.place_schedule_on_top(sigma2); 
      sigma1B.place_schedule_on_top(sigma1T);
    }
  }

  void partition_jobs(Job_List jobs) {
    tiny_jobs = {};
    small_jobs = {};
    medium_jobs = {};
    big_jobs = {};

    for(const auto& job : jobs) {
      if(is_tiny_job(job))
        tiny_jobs.push_back(job);

      else if(is_small_job(job))
        small_jobs.push_back(job);

      else if(is_medium_job(job))
        medium_jobs.push_back(job);

      else if(is_big_job(job))
        big_jobs.push_back(job);
    }
    jobs = {};

  }

  uint height(Job_List jobs) {
    uint sum = 0;
    for(auto job : jobs) 
      sum += job.processing_time;
    return sum;
  }

  Job_List remove_small_and_medium_jobs(Schedule& schedule) {
      return schedule.remove_jobs_if([this](const Job& j) {
          return is_small_job(j) || is_medium_job(j);
      });
  }

  Job_List remove_tiny_jobs(Schedule& schedule) {
      return schedule.remove_jobs_if([this](const Job& j) {
          return is_tiny_job(j);
      });
  }

  // TODO: make helper function in schedule to avoid acessing gap_manager
  // find earliest time tau where machine usage is <= 2/3 m
  // if there is no such time tau=makespan
  uint get_separation_time_from_sigma1(Schedule sigma1) {
    sigma1.gap_manager->reset_structure();
    Job biggest_small_job(/*processing_time=*/1, /*required_machines=*/m/3); 
    uint tau = sigma1.gap_manager->update_earliest_time_to_place(biggest_small_job);
    sigma1.gap_manager->reset_structure();

    return tau;
  }

};
// }}}




int main() {
}
