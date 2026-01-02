#include <iostream>
#include <vector>
#include <cstdint>
#include <optional>
#include <list>

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
};


struct Gap {
  uint time;                // time where the gap occurs
  uint additional_machines; // machines more available at that time than in the previous gap;
};

class Gap_List {
public:
  uint m;

  Gap_List(uint m) 
    : m(m) 
  {
    Gap gap;
    gap.time = 0;
    gap.additional_machines = m;

    // at time 0 there are m available machines in an empty schedule
    gaps[0] = m;
  }

  void insert_gap(uint time, uint available_machines) {
    if (gaps.key_exists(time))
      gaps[time] -= available_machines;
    else 
      gaps[time] = m - available_machines;
  }
  
  uint get_earliest_time_to_place(Job job) 
  {}

private:
  /* list<uint> gaps;  // gaps sorted by start_time */
  
  indexed_tree gaps;

  // next index in gap_start to consider
  uint next_gap = 0;                  
  uint available_machine_at_next_gap = m;
};

// --- SCHEDULE --- {{{
class Schedule {
public:
  uint m;
  uint n;
  Job_List jobs;

  /* Gap_List gap_list; */

  Schedule(uint m, uint n, Job_List jobs) 
    : m(m), n(n), jobs(jobs)
  {
    
  }

  void list_schedule(Job job) {}

  void list_schedule(Job_List jobs) {}

  void on_two_stacks(Job job) {}

  void on_two_stacks(Job_List job) {}

};
// }}}

// --- TOWER_SCHEDULE --- {{{
class Tower_Schedule {
};
// }}}




int main() {
  uint m=10;
  uint n =10;
  vector<Job> jobs = {Job(2,3), Job(4,3), Job(2,5), Job(6,9), Job(1,1), Job(2,1), Job(1,2), Job(8,1), Job(1,8), Job(5,5)};

  Job job(2, 3);
  cout << job.processing_time << endl;
  cout << job.required_machines << endl;

  cout << job.starting_time.has_value() << endl;
  job.starting_time = 0;
  cout << job.starting_time.has_value() << endl;
}
