#pragma once

#include <optional>
#include <iostream>
#include <vector>
#include <cstdint>
#include <list>
#include <set>
#include <algorithm>
#include <memory>
#include <ranges>

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

inline std::vector<Job> operator+(std::vector<Job>& lhs, std::vector<Job>& rhs) {
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

inline void sort_jobs_increasingly_by_starting_time(Job_List& jobs) {
  sort(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) {
      return j1.starting_time < j2.starting_time || (j1.starting_time == j2.starting_time && j1.required_machines > j2.required_machines);
  });
}

inline void sort_jobs_increasingly_by_starting_time_and_second_by_required_machines(Job_List& jobs) {
  sort(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) {
      return j1.starting_time < j2.starting_time || (j1.starting_time == j2.starting_time && j1.required_machines < j2.required_machines);
  });
}

inline void sort_jobs_decreasingly_by_required_machines(Job_List& jobs) {
  sort(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) {
      return j1.required_machines > j2.required_machines || (j1.required_machines == j2.required_machines && j1.processing_time > j2.processing_time);
  });
}

// DEBUG
inline void print_jobs(Job_List jobs) {
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

    inline bool key_exists(uint key) const {
        return this->find(key) != this->end();
    }

    inline optional<Gap> get_next_gap(uint current_time) {
      // lower_bound returns the current key (if exists) or the next larger
      auto it = this->lower_bound(current_time);   
      if (it == this->end())
        return {};        // return nothing
      else
        return Gap{it->first, it->second}; // time, additional_machines
    }
    
    inline optional<Gap> get_previous_gap(uint current_time) {
      auto it = this->lower_bound(current_time); 
      if(it != this->begin()) {
        auto smaller_it = std::prev(it);
        return Gap{smaller_it->first, smaller_it->second};
      }
      else 
        return {};
    }
};

const uint INVALID_TIME = std::numeric_limits<uint>::max();

