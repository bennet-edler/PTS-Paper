
#include "types.hpp"



// --- TOWER_SCHEDULE --- {{{
class Tower_Schedule {
};
// }}}




int main() {
  uint m=100;
  uint n =10;
  /*
  vector<Job> jobs = {Job(2,3), Job(4,3), Job(2,5), Job(6,9), Job(1,1), Job(2,1), Job(1,2), Job(8,1), Job(1,8), Job(5,5)};

  cout << job.processing_time << endl;
  cout << job.required_machines << endl;

  cout << job.starting_time.has_value() << endl;
  job.starting_time = 0;
  cout << job.starting_time.has_value() << endl;
  */

  Job job(20, 30);
  Gap_Manager gap_manager(100);
  cout << " test " << endl;
  /* return 1; */
  /* uint time = gap_manager.get_earliest_time_to_place(job); */
  /* gap_manager.add_machines_at(0, -job.procesing_time); */
  /* gap_manager.add_machines_at(0, -job.procesing_time); */
  /* gap_manager.current_time = -1; */
  gap_manager.place_job_at(0, job);

  cout << "width at 0: " << gap_manager.gaps[0] << endl;

  optional<Gap> opt_gap = gap_manager.gaps.get_next_gap(gap_manager.current_time);
  if(opt_gap.has_value()){
    Gap gap = opt_gap.value();
    cout << gap.time << " " << gap.additional_machines << endl;
  } else {cout << "opt value error" << endl;}


  gap_manager.current_time = 20;

  optional<Gap> opt_gap2 = gap_manager.gaps.get_next_gap(gap_manager.current_time);
  if(opt_gap2.has_value()){
    Gap gap2 = opt_gap2.value();
    cout << gap2.time << " " << gap2.additional_machines << endl;
  } else {cout << "opt value error" << endl;}

  /* cout << gap */

  cout << gap_manager.current_time << endl;
  /* cout << time << endl; */
  /* gap_manager.insert_gap( */
}
