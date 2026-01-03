#pragma once

#include <optional>

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
  
  uint get_earliest_time_to_place(Job job) {
    while(available_machines_in_gap <= job.required_machines) {
      cout << available_machines_in_gap << endl;
      optional<Gap> opt_gap = gaps.get_next_gap(current_time+1);
      if(opt_gap.has_value()) {
        Gap gap = opt_gap.value();
        current_time = gap.time;   
        available_machines_in_gap += gap.additional_machines;
      }
      else
        break;
    }
    return current_time;
  }

/* private: */
  /* list<uint> gaps;  // gaps sorted by start_time */
  
  indexed_tree gaps;

  // next index in gap_start to consider
  uint current_time = 0;                  
  uint available_machines_in_gap = m;
};
