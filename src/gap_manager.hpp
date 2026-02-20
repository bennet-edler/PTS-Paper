#pragma once
#include "types.hpp"


class Gap_Manager {
public:
  uint m;

  Gap_Manager(uint m);

  void reset_structure();

  void set_structure_to_top();

  void place_job_at(Job job, uint time);

  void add_additional_machines_at(uint time, uint additional_machines);
  
  uint update_earliest_time_to_place(Job job);

  // transform relative gap structure to structure which absolute values
  // which means that in the new structure at time t the value of inv_absolute_gaps[t]
  // represents how many machines are available at that time 
  // (instead of how many available machines change at time t)
  // note that we will inverse the time since we move down
  // (so t=makespan is now 0 and t=0 is now makespan) 
  virtual map<uint,uint> build_inverse_absolute_gaps();

  uint get_makespan();

/* private: */
  indexed_tree gaps;

  // next index in gap_start to consider
  uint current_time;                  
  uint available_machines_in_gap;

  uint makespan;
};
