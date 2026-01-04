#pragma once

#include <optional>
#include <iostream>
#include <vector>
#include <cstdint>
#include <list>
#include <set>
#include <algorithm>

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

struct Gap {
  uint time;                // time where the gap occurs
  uint additional_machines; // machines more available at that time than in the previous gap;
};

typedef uint32_t uint;
typedef vector<Job> Job_List;

// order statistic tree 
// has O(log n) for indexing, searching and insertion
typedef tree<uint,                                  // key type
             uint,                                  // value type
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
    
};


class Gap_Manager {
public:
  uint m;

  Gap_Manager(uint m) 
    : m(m) 
  {
    Gap gap;
    gap.time = 0;
    gap.additional_machines = m;

    // at time 0 there are m available machines in an empty schedule
    gaps[0] = m;
  }

  void place_job_at(uint time, Job job) {
    // available machines at starttime reduced
    add_additional_machines_at(time,                    -job.required_machines); 
    // available machines at endtime increased
    add_additional_machines_at(time+job.processing_time, job.required_machines);
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

/* private: */
  
  indexed_tree gaps;

  // next index in gap_start to consider
  uint current_time = 0;                  
  uint available_machines_in_gap = m;
};



const uint INVALID_TIME = std::numeric_limits<uint>::max();

class Schedule {
public:
  uint m;
  uint n;
  Job_List placed_jobs;
  Gap_Manager gap_manager;

  /* Gap_List gap_list; */

  Schedule(uint m, uint n) 
    : m(m), gap_manager(m)
  {
    if(m<2)
      throw runtime_error("need to have at least 2 machines");

    /* jobs.capacity(n); */
    // sort jobs
    // ...
  }

  void schedule_job(Job& job, uint time = INVALID_TIME) {
    if(time == INVALID_TIME)
      time = gap_manager.update_earliest_time_to_place(job);

    gap_manager.place_job_at(time, job);

    job.starting_time = time;
    placed_jobs.push_back(job);
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

      uint time = gap_manager.update_earliest_time_to_place(min_job);
      uint available_machines = gap_manager.available_machines_in_gap;

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
      uint time = gap_manager.update_earliest_time_to_place(dummy_job);

      if(gap_manager.available_machines_in_gap == m-1)
        gap_manager.place_job_at(time, Job(job.processing_time, m-1));
      else  // == 1
        gap_manager.place_job_at(time, Job(job.processing_time, 1));

      job.starting_time = time;
      placed_jobs.push_back(job);
      
    }
  }
};


