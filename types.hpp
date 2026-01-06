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

class Job {
public:
  Job(uint processing_time, uint required_machines) 
    : processing_time(processing_time), required_machines(required_machines) {}

  uint processing_time;
  uint required_machines;

  optional<uint> starting_time;
};

typedef uint32_t uint;
typedef int32_t sint;
typedef vector<Job> Job_List;

struct Gap {
  uint time;                // time where the gap occurs
  sint additional_machines; // machines more available at that time than in the previous gap;
};

// order statistic tree 
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

  void place_job_at(uint time, Job job) {
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
    : m(m)
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

    gap_manager->place_job_at(time, job);

    job.starting_time = time;
    placed_jobs.push_back(job);
  }

  void place_job(Job& job, uint time = INVALID_TIME) {

  }

  void list_schedule(Job_List jobs) {
    multiset<pair<uint, size_t>> job_pool;

    for(size_t i = 0; i < jobs.size(); i++) 
      job_pool.insert({jobs[i].required_machines, i});

    while(!job_pool.empty()) {
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

      schedule_job(jobs[large_job_index], time);
      job_pool.erase(large_job_iterator);
    }
  }

  // jobs need to have machine requirement at most m/2
  void on_two_stacks(Job_List jobs) {
    // sort decreasingly by required_machines
    sort(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) {
        return j1.required_machines > j2.required_machines;
    });

    Job dummy_job(1, 1);    // required_machines = 1

    for(Job& job : jobs) {
      uint time = gap_manager->update_earliest_time_to_place(dummy_job);

      if(gap_manager->available_machines_in_gap == m-1)
        gap_manager->place_job_at(time, Job(job.processing_time, m-1));
      else  // == 1
        gap_manager->place_job_at(time, Job(job.processing_time, 1));
      
      // TODO: place jobs correctly: use a copy of the gap_manager for the logic and 
      // then use schedule_job to actually place it
      job.starting_time = time;
      placed_jobs.push_back(job);
      
    }
  }

  // returns a list of jobs which were not schedule during this step
  // starts at the top and places repeatedly the widest job which fits directly below the last one
  // assumes the existing jobs are decreasingly placed in machine_requirement
  Job_List schedule_down(Job_List jobs) {
    // sort decreasingly by required_machines
    sort(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) {
        return j1.required_machines > j2.required_machines;
    });

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
};

