#pragma once

#include <optional>
#include <iostream>
#include <vector>
#include <cstdint>
#include <list>
#include <set>
#include <algorithm>
#include <memory>

// order statistic tree
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds; // order statistic tree

typedef uint32_t uint;
typedef int32_t sint;

class Job {
public:
  Job(uint processing_time, uint required_machines) 
    : processing_time(processing_time), required_machines(required_machines) {}

  uint processing_time;
  uint required_machines;

  optional<uint> starting_time;
};

std::vector<Job> operator+(std::vector<Job>& lhs, std::vector<Job>& rhs) {
    std::vector<Job> result;
    result.reserve(lhs.size() + rhs.size());

    result.insert(result.end(), 
                  std::make_move_iterator(lhs.begin()), 
                  std::make_move_iterator(lhs.end()));
                  
    result.insert(result.end(), 
                  std::make_move_iterator(rhs.begin()), 
                  std::make_move_iterator(rhs.end()));

    lhs.clear();
    rhs.clear();

    return result;
}


typedef vector<Job> Job_List;

// DEBUG
void print_jobs(Job_List jobs) {
  if(jobs.size() == 0) {
    cout << "EMPTY LIST OF JOBS" << endl;
  }
  for(auto job : jobs) {
    cout << "processing_time: " << job.processing_time << ", required_machines: " << job.required_machines;
    if(job.starting_time.has_value())
      cout << ", starting_time: " << job.starting_time.value();
    cout << endl;
  } cout << endl;
}

struct Gap {
  uint time;                // time where the gap occurs
  sint additional_machines; // machines more available at that time than in the previous gap;
};

// order statistic tree // TODO: replace with std::map
// has O(log n) for indexing, searching and insertion
typedef tree<uint,                                  // key type
             sint,                                  // value type
             std::less<uint>,                       // sorting increasing
             rb_tree_tag,                           // tree type (red black tree)
             tree_order_statistics_node_update>     // log indexing
             indexed_tree_base;

// extend structure 
struct indexed_tree : public indexed_tree_base {

    bool key_exists(uint key) const {
        return this->find(key) != this->end();
    }

    optional<Gap> get_next_gap(uint current_time) {
      // lower_bound returns the current key (if exists) or the next larger
      auto it = this->lower_bound(current_time);   
      if (it == this->end())
        return {};        // return nothing
      else
        return Gap{it->first, it->second}; // time, additional_machines
    }
    
    optional<Gap> get_previous_gap(uint current_time) {
      auto it = this->lower_bound(current_time); 
      if(it != this->begin()) {
        auto smaller_it = std::prev(it);
        return Gap{smaller_it->first, smaller_it->second};
      }
      else 
        return {};
    }
};


class Gap_Manager {
public:
  uint m;

  Gap_Manager(uint m) 
    : m(m) 
  {
    // at time 0 there are m available machines in an empty schedule
    gaps[0] = m;
    reset_structure();
    makespan = 0;
  }

  void reset_structure() {
    current_time = 0;
    available_machines_in_gap = gaps[0];
  }

  void set_structure_to_top() {
    current_time = makespan;
    available_machines_in_gap = m;
  }

  void place_job_at(Job job, uint time) {
    // available machines at starttime reduced
    add_additional_machines_at(time,                    -job.required_machines); 
    // available machines at endtime increased
    add_additional_machines_at(time+job.processing_time, job.required_machines);

    if(makespan < time+job.processing_time)
      makespan = time+job.processing_time;
  }

  void add_additional_machines_at(uint time, uint additional_machines) { 
    if (gaps.key_exists(time))
      gaps[time] += additional_machines;
    else 
      gaps[time] = additional_machines;

    if(current_time >= time)
      available_machines_in_gap += additional_machines;
  }
  
  uint update_earliest_time_to_place(Job job) {
    while(available_machines_in_gap < job.required_machines) {
      optional<Gap> opt_gap = gaps.get_next_gap(current_time+1);
      if(opt_gap.has_value()) {
        Gap gap = opt_gap.value();
        current_time = gap.time;   
        available_machines_in_gap += gap.additional_machines;
      }
      else
        break; // TODO: should never happen right?
    }
    return current_time;
  }

  // transform relative gap structure to structure which absolute values
  // which means that in the new structure at time t the value of inv_absolute_gaps[t]
  // represents how many machines are available at that time 
  // (instead of how many available machines change at time t)
  // note that we will inverse the time since we move down
  // (so t=makespan is now 0 and t=0 is now makespan) 
  virtual map<uint,uint> build_inverse_absolute_gaps() {
    set_structure_to_top();

    map<uint, uint> inverse_absolute_gaps;
    inverse_absolute_gaps[0] = 0;
    uint available_time = 0;
    Gap previous_gap = Gap{makespan,0};
    optional<Gap> opt_gap = Gap{makespan, gaps[makespan]};
    while(opt_gap.has_value()) {
      Gap gap = opt_gap.value();
      available_time += current_time - gap.time;
      current_time = gap.time;
      available_machines_in_gap -= previous_gap.additional_machines;

      inverse_absolute_gaps[available_time] = available_machines_in_gap;

      opt_gap = gaps.get_previous_gap(current_time);
      previous_gap = gap;
    }

    reset_structure();

    return inverse_absolute_gaps;
  }

  uint get_makespan() {
    return makespan;
  }

/* private: */
  indexed_tree gaps;

  // next index in gap_start to consider
  uint current_time;                  
  uint available_machines_in_gap;

  uint makespan;
};



const uint INVALID_TIME = std::numeric_limits<uint>::max();

class Schedule {
public:
  uint m;
  uint n;
  Job_List placed_jobs;
  shared_ptr<Gap_Manager> gap_manager;

  /* Gap_List gap_list; */

  Schedule(uint m, uint n) 
    : m(m), n(n)
  {
    if(m<2)
      throw runtime_error("need to have at least 2 machines");

    gap_manager = make_shared<Gap_Manager>(m);

    /* jobs.capacity(n); */
    // sort jobs
    // ...
  }

  void schedule_job(Job& job, uint time = INVALID_TIME) {
    if(time == INVALID_TIME)
      time = gap_manager->update_earliest_time_to_place(job);

    gap_manager->place_job_at(job, time); 

    job.starting_time = time;
    placed_jobs.push_back(job);
  }

  // Expects sorted (by starting_time) list of indices
  // returns unscheduled jobs
  // TODO: does not affect the makespan. 
  Job_List unschedule_jobs(vector<uint> placed_jobs_indices) {
    sort(placed_jobs_indices.begin(), placed_jobs_indices.end()); // redundant

    Job_List new_jobs;
    Job_List removed_jobs;

    int current_index = 0;
    for(int i = 0; i < placed_jobs.size(); i++) {
      Job job = placed_jobs[i];

      if(current_index < placed_jobs_indices.size() && i == placed_jobs_indices[current_index]) {
        uint time = job.starting_time.value();
        gap_manager->add_additional_machines_at(time,                    +job.required_machines); 
        gap_manager->add_additional_machines_at(time+job.processing_time,-job.required_machines);
        current_index += 1;
        job.starting_time.reset();
        removed_jobs.push_back(job);
      }
      else 
        new_jobs.push_back(job);
    }
    print_jobs(new_jobs);

    placed_jobs = new_jobs;
    return removed_jobs;
  }

  // list schedules jobs without letting the differences of jobs placed be more than p_max
  // makespan - balance_time is the initial upper_bound to not place jobs above
  static void balanced_list_schedule(Job_List jobs, Schedule& sigma1, Schedule& sigma2, sint& balance_time, uint p_max) {
    uint sigma1_old_makespan = sigma1.get_makespan();
    uint sigma2_old_makespan = sigma2.get_makespan();

    multiset<pair<uint, size_t>> job_pool = create_job_pool(jobs);

    while(!job_pool.empty()) {
      uint gap_end1 = sigma1_old_makespan - balance_time;
      uint gap_end2 = sigma2_old_makespan - balance_time;

      sigma1.list_schedule_single(/*jobs=*/jobs, /*job_pool=*/job_pool, /*until_t=*/gap_end1);
      if(job_pool.empty())
        return;

      sigma2.list_schedule_single(/*jobs=*/jobs, /*job_pool=*/job_pool, /*until_t=*/gap_end2);
      if(job_pool.empty())
        return;

      // get larger gap
      auto min_job_iterator = job_pool.begin(); 
      uint min_job_index = min_job_iterator->second;
      Job min_job = jobs[min_job_index];

      uint g1 = gap_end1-sigma1.gap_manager->update_earliest_time_to_place(min_job);
      uint g2 = gap_end2-sigma2.gap_manager->update_earliest_time_to_place(min_job);

      // update balance_time so that we have afterwards p_max time available in the larger gap 
      // where the actual gap is the time between g1 and balance_time
      balance_time -= p_max - max(g1,g2);
      

    }

  }

  static multiset<pair<uint, size_t>> create_job_pool(Job_List jobs) {
    multiset<pair<uint, size_t>> job_pool;

    for(size_t i = 0; i < jobs.size(); i++) 
      job_pool.insert({jobs[i].required_machines, i});

    return job_pool;
  }

  bool list_schedule(Job_List& jobs, uint until_t=INVALID_TIME) {
    multiset<pair<uint, size_t>> job_pool = create_job_pool(jobs);

    bool stop = false;
    while(!job_pool.empty() && !stop)
      stop = list_schedule_single(/*jobs=*/jobs, /*job_pool=*/job_pool,until_t);

    jobs = update_remaining_jobs_with_job_pool(jobs, job_pool);
    
    sort_jobs_increasingly_by_starting_time(placed_jobs);

    return stop;
  }

  Job_List update_remaining_jobs_with_job_pool(Job_List jobs, multiset<pair<uint, size_t>> &job_pool) {
    Job_List new_jobs;
    while(!job_pool.empty()){ 
      auto next_job_iterator = job_pool.begin(); 
      uint next_job_index = next_job_iterator->second;
      Job next_job = jobs[next_job_index];
      new_jobs.push_back(next_job);
      job_pool.erase(next_job_iterator);
    }
    return new_jobs;
  }

  // jobs need to have machine requirement at most m/2
  void on_two_stacks(Job_List jobs) {
    sort_jobs_decreasingly_by_required_machines(jobs);

    Job dummy_job(1, 1);    // required_machines = 1
    Gap_Manager dummy_gap_manager = *gap_manager;
    for(Job& job : jobs) {
      uint time = dummy_gap_manager.update_earliest_time_to_place(dummy_job);

      if(dummy_gap_manager.available_machines_in_gap == m-1)
        dummy_gap_manager.place_job_at(Job(job.processing_time, m-1), time);
      else  // == 1
        dummy_gap_manager.place_job_at(Job(job.processing_time, 1), time);
      
      schedule_job(job, time);
    }
  }

  void sort_jobs_decreasingly_by_required_machines(Job_List& jobs) {
    sort(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) {
        return j1.required_machines > j2.required_machines;
    });
  }

  void sort_jobs_increasingly_by_starting_time(Job_List& jobs) {
    sort(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) {
        return j1.starting_time < j2.starting_time || (j1.starting_time == j2.starting_time && j1.required_machines > j2.required_machines);
    });
  }

  // returns a list of jobs which were not schedule during this step
  // starts at the top and places repeatedly the widest job which fits directly below the last one
  // assumes the existing jobs are decreasingly placed in machine_requirement
  Job_List schedule_down(Job_List jobs) {
    sort_jobs_decreasingly_by_required_machines(jobs);

    map<uint,uint> inverse_absolute_gaps = gap_manager->build_inverse_absolute_gaps();
    uint makespan = gap_manager->get_makespan();
    
    uint used_time = 0;
    Job_List jobs_to_schedule;
    Job_List jobs_unused;

    for(Job& job : jobs) {
      uint available_machines_during_job_end = 
        inverse_absolute_gaps.lower_bound(used_time + job.processing_time)->second;
      
      if(available_machines_during_job_end >= job.required_machines) {
        used_time += job.processing_time;
        job.starting_time = makespan - used_time;
        jobs_to_schedule.push_back(job);
      } 
      else
        jobs_unused.push_back(job);
    }

    // schedule jobs
    for(int i = jobs_to_schedule.size()-1; i >= 0; i--) {
      Job job = jobs_to_schedule[i];
      schedule_job(job, job.starting_time.value());
    }

    return jobs_unused;
  }

  // jobs which start at or below separation_time will be scheduled in s1
  // the remaining jobs will be scheduled in s2
  // TODO: ordered list of separation_times (for mcs)
  void split_at(uint separation_time, Schedule& lower_schedule, Schedule& upper_schedule) {
    for(auto& job : placed_jobs) {
      if(job.starting_time.value() < separation_time)
        lower_schedule.schedule_job(job, job.starting_time.value());
      else 
        upper_schedule.schedule_job(job, job.starting_time.value() - separation_time);
    }
  }

  uint get_makespan() {
    return gap_manager->get_makespan();
  }

  void set_makespan(uint new_makespan) {
    gap_manager->makespan = new_makespan;
  }

  // assumes that the current schedule is valid for this operation
  // TODO: no jobs in stacks
  void sort_in_higher_stack(Job_List jobs) {
    Job_List jobs_on_higher_stack = jobs;
    Job_List jobs_on_lower_stack;

    if(placed_jobs.size() == 0) {
      jobs_on_higher_stack = jobs;
    } 
    else {
      // the last placed job is on the higher stack 
      Job last_job = placed_jobs[placed_jobs.size()-1];
      uint current_time = last_job.starting_time.value()+last_job.processing_time;
      
      for(int i=placed_jobs.size()-1; i>=0; i--) {
        Job job = placed_jobs[i];
        // from there go down and get all jobs which are placed directly below
        if(current_time == job.processing_time + job.starting_time.value()){
          jobs_on_higher_stack.push_back(job);
          current_time = job.starting_time.value();
        }
        else {
          jobs_on_lower_stack.push_back(job);
        }
      }
    }
    sort_jobs_decreasingly_by_required_machines(jobs_on_higher_stack);
    sort_jobs_decreasingly_by_required_machines(jobs_on_lower_stack);

    // create new gap manager and iterate through jobs in stacks and place them
    gap_manager = make_shared<Gap_Manager>(m);
    placed_jobs = {};
    
    schedule_jobs_on_top_of_each_other(jobs_on_higher_stack);
    schedule_jobs_on_top_of_each_other(jobs_on_lower_stack);

    sort_jobs_increasingly_by_starting_time(placed_jobs);
  }

  void schedule_jobs_on_top_of_each_other(Job_List jobs, uint start_time=0) {
    for(auto job : jobs) {
      schedule_job(job, start_time);
      start_time += job.processing_time;
    }
  }


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
  Job_List remove_jobs_above(uint time) {
    return remove_jobs_if([time](const Job& job) {
      return job.starting_time.value() + job.processing_time > time;
    });
  }

  void place_schedule_on_top(Schedule& schedule) {
    for(auto job : schedule.placed_jobs) 
      schedule_job(job);

    schedule = Schedule(schedule.m, schedule.n);
  }

  Schedule get_rotated_schedule() {
    Schedule rotated_schedule(m,n);
    uint makespan = get_makespan();
    for(auto job : placed_jobs)
      schedule_job(job,/*time=*/makespan-job.starting_time.value()-job.processing_time);
    return rotated_schedule;
  }

private:

  // until_t ensures that no job will be executed after until_t
  // assumes job_pool.empty()==false
  // returns if procedure should end
  bool list_schedule_single(Job_List jobs, multiset<pair<uint, size_t>> &job_pool, uint until_t=INVALID_TIME) {
      auto min_job_iterator = job_pool.begin(); 
      // index i with jobs[i] has lowest required machines 
      uint min_job_index = min_job_iterator->second;
      Job min_job = jobs[min_job_index];

      uint time = gap_manager->update_earliest_time_to_place(min_job);
      uint available_machines = gap_manager->available_machines_in_gap;

      // find index i where jobs[i].required_machines is the smallest value ..
      // such that jobs[i].required_machines > available_machines
      // upper_bound returns the first key which is larger available machines ..
      // and with value larger than <size_t>::max() ..
      // (this ensures that the key is larger available_machines)
      auto large_job_iterator = 
        job_pool.upper_bound({available_machines, numeric_limits<size_t>::max()});

      // take the previous key
      // large_jobs_iterator must be larger than job_pool.begin() at this time
      --large_job_iterator; 
      uint large_job_index = large_job_iterator->second;
      Job large_job = jobs[large_job_index];
      
      if(until_t != INVALID_TIME && until_t < time+large_job.processing_time)
        return true;

      schedule_job(large_job, time);
      job_pool.erase(large_job_iterator);
      return false;
  }



};

