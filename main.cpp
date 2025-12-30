#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;
#define uint uint32_t

class Job {
public:
  Job(uint processing_time, uint required_machines) 
    : processing_time(processing_time), required_machines(required_machines) {}

  uint processing_time;
  uint required_machines;
};


class Schedule {

   

};



int main() {

  
  uint m=10;
  uint n =10;
  vector<Job> jobs = {Job(2,3), Job(4,3), Job(2,5), Job(6,9), Job(1,1), Job(2,1), Job(1,2), Job(8,1), Job(1,8), Job(5,5)};

  Job job(2, 3);
  cout << job.processing_time << endl;
  cout << job.required_machines << endl;
}
