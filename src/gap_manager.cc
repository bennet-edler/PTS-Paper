#include "gap_manager.hpp"

Gap_Manager::Gap_Manager(uint m) 
    : m(m) 
  {
    // at time 0 there are m available machines in an empty schedule
    gaps[0] = m;
    reset_structure();
    makespan = 0;
  }

void Gap_Manager::reset_structure() {
  current_time = 0;
  available_machines_in_gap = gaps[0];
}

void Gap_Manager::set_structure_to_top() {
  current_time = makespan;
  available_machines_in_gap = m;
}

void Gap_Manager::place_job_at(Job job, uint time) {
  // available machines at starttime reduced
  add_additional_machines_at(time,                    -job.required_machines); 
  // available machines at endtime increased
  add_additional_machines_at(time+job.processing_time, job.required_machines);

  if(makespan < time+job.processing_time)
    makespan = time+job.processing_time;
}

void Gap_Manager::add_additional_machines_at(uint time, uint additional_machines) { 
  if (gaps.key_exists(time))
    gaps[time] += additional_machines;
  else 
    gaps[time] = additional_machines;

  if(current_time >= time)
    available_machines_in_gap += additional_machines;
}

uint Gap_Manager::update_earliest_time_to_place(Job job) {
  while(available_machines_in_gap < job.required_machines) {
    optional<Gap> opt_gap = gaps.get_next_gap(current_time+1);
    if(opt_gap.has_value()) {
      Gap gap = opt_gap.value();
      current_time = gap.time;   
      available_machines_in_gap += gap.additional_machines;
    }
    else
      throw std::runtime_error("should never happen");
  }
  return current_time;
}

// transform relative gap structure to structure which absolute values
// which means that in the new structure at time t the value of inv_absolute_gaps[t]
// represents how many machines are available at that time 
// (instead of how many available machines change at time t)
// note that we will inverse the time since we move down
// (so t=makespan is now 0 and t=0 is now makespan) 
map<uint,uint> Gap_Manager::build_inverse_absolute_gaps() {
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

uint Gap_Manager::get_makespan() {
  return makespan;
}

