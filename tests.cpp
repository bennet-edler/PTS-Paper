#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "types.hpp"
#include "gap_manager.hpp"
#include "schedule.hpp"
#include "tower_schedule.hpp"

// INDEX TREE
TEST(Index_Tree_Tests, GetNextGap_GetsCorrectGap) {

}

// GAP MANAGER
TEST(Gap_Manager_Tests, GetEarliestTimeToPlace_ReturnsCorrectTime) {
  Gap_Manager gap_manager(100);
  Job job(10,10);
  uint time = gap_manager.update_earliest_time_to_place(job);
  EXPECT_EQ(time, 0);
}

TEST(Gap_Manager_Tests, GetEarliestTimeToPlace_ReturnsZeroInitially) {
  Gap_Manager gap_manager(100);

  {
    Job job(10,60); // proccesing_time, required_machines
    gap_manager.place_job_at(job, 0);
  }

  Job job(10,60);
  uint time = gap_manager.update_earliest_time_to_place(job);
  EXPECT_EQ(time, 10);
}

TEST(Gap_Manager_Tests, AddAdditionalMachinesAt_AddtionWorks) {
  Gap_Manager gap_manager(50);
  gap_manager.add_additional_machines_at(0, -20);
  {
    Gap gap = gap_manager.gaps.get_next_gap(0).value();
    EXPECT_EQ(gap.additional_machines, 30);
  }
  gap_manager.add_additional_machines_at(10,10);
  {
    Gap gap = gap_manager.gaps.get_next_gap(10).value();
    EXPECT_EQ(gap.additional_machines, 10);
  }
  gap_manager.add_additional_machines_at(10,10);
  {
    Gap gap = gap_manager.gaps.get_next_gap(10).value();
    EXPECT_EQ(gap.additional_machines, 20);
  }
}

TEST(Gap_Manager_Tests, GetEarliestTimeToPlace_UpdatesAvailableMachinesInGapCorrectly) {
  Gap_Manager gap_manager(1000);
  EXPECT_EQ(gap_manager.available_machines_in_gap, 1000);
  EXPECT_EQ(gap_manager.current_time, 0);
  {
    Job job(10, 500);
    uint time = gap_manager.update_earliest_time_to_place(job);
    EXPECT_EQ(time, 0);
    gap_manager.place_job_at(job, 0);
    EXPECT_EQ(gap_manager.current_time, 0);
    EXPECT_EQ(gap_manager.available_machines_in_gap, 500);
  }
  {
    Job job(15, 499);
    uint time = gap_manager.update_earliest_time_to_place(job);
    EXPECT_EQ(time, 0);
    gap_manager.place_job_at(job, 0);
    EXPECT_EQ(gap_manager.current_time, 0);
    EXPECT_EQ(gap_manager.available_machines_in_gap, 1);
  }
  {
    Job job(10, 2);
    uint time = gap_manager.update_earliest_time_to_place(job);
    EXPECT_EQ(time, 10);
    EXPECT_EQ(gap_manager.current_time, 10);
    EXPECT_EQ(gap_manager.available_machines_in_gap, 501);
  }
}

TEST(Gap_Manager_Tests, BuildInverseAbsoluteGapsWorks) {
  Gap_Manager gap_manager(10);
  gap_manager.gaps[0] =  4;
  gap_manager.gaps[1] =  1;
  gap_manager.gaps[2] =  2;
  gap_manager.gaps[3] = -3;
  gap_manager.gaps[4] =  2;
  gap_manager.gaps[6] =  4;
  gap_manager.gaps[7] = -2;
  gap_manager.gaps[8] =  2;

  gap_manager.makespan = 8;

  map<uint,uint> inverse_absolute_gaps = 
    gap_manager.build_inverse_absolute_gaps();

  EXPECT_EQ(inverse_absolute_gaps[0], 10);
  EXPECT_EQ(inverse_absolute_gaps[1],  8);
  EXPECT_EQ(inverse_absolute_gaps[2], 10);
  EXPECT_EQ(inverse_absolute_gaps[4],  6);
  EXPECT_EQ(inverse_absolute_gaps[5],  4);
  EXPECT_EQ(inverse_absolute_gaps[6],  7);
  EXPECT_EQ(inverse_absolute_gaps[7],  5);
  EXPECT_EQ(inverse_absolute_gaps[8],  4);

}

// SCHEDULE
TEST(Schedule_Tests, ListScheduleWorksCorrect) {
  // Job(processing_time, required_machines)
  Job_List jobs = {
    {10, 80}, // J1
    {15, 70}, // J2
    {30, 60}, // J3
    {20, 50}, // J4
    {35, 50}, // J5
    {10, 40}, // J6
    {10, 25}, // J7
    {10, 20}, // J8
    {10, 10}, // J9
    {10, 9},  // J10
    {10, 8}   // J11
  };

  Schedule schedule(100, jobs.size());
  schedule.list_schedule(jobs);
  jobs = schedule.placed_jobs;

  // I expect them here in the order in which they will be placed
  // at the correct time
  EXPECT_EQ(jobs[0].starting_time.value(), 0);  // J1
  EXPECT_EQ(jobs[1].starting_time.value(), 0);  // J8
  EXPECT_EQ(jobs[2].starting_time.value(), 10); // J2
  EXPECT_EQ(jobs[3].starting_time.value(), 10); // J7
  EXPECT_EQ(jobs[4].starting_time.value(), 20); // J9
  EXPECT_EQ(jobs[5].starting_time.value(), 20); // J10
  EXPECT_EQ(jobs[6].starting_time.value(), 20); // J11
  EXPECT_EQ(jobs[7].starting_time.value(), 25); // J3
  EXPECT_EQ(jobs[8].starting_time.value(), 30); // J6
  EXPECT_EQ(jobs[9].starting_time.value(), 55); // J4 || J5
  EXPECT_EQ(jobs[10].starting_time.value(),55); // J4 || J5
}


TEST(Schedule_Tests, OnTwoStackWorksCorrect) {
  // Job(processing_time, required_machines)
  Job_List jobs = {
    {20, 50}, // J1
    {35, 50}, // J2
    {10, 40}, // J3
    {10, 25}, // J4
    {10, 20}, // J5
    {10, 10}, // J6
    {10, 9},  // J7
    {10, 8}   // J8
  };

  Schedule schedule(100, jobs.size());
  schedule.on_two_stacks(jobs);
  jobs = schedule.placed_jobs;

  // I expect them here in the order in which they will be placed
  // at the correct time
  EXPECT_EQ(jobs[0].starting_time.value(), 0);  // J1 || J2
  EXPECT_EQ(jobs[1].starting_time.value(), 0);  // J1 || J2
  EXPECT_EQ(jobs[2].starting_time.value(), 20); // J3
  EXPECT_EQ(jobs[3].starting_time.value(), 30); // J4
  EXPECT_EQ(jobs[4].starting_time.value(), 35); // J5
  EXPECT_EQ(jobs[5].starting_time.value(), 40); // J6
  EXPECT_EQ(jobs[6].starting_time.value(), 45); // J7
  EXPECT_EQ(jobs[7].starting_time.value(), 50); // J8
}


class Mock_Gap_Manager : public Gap_Manager {
public:
    Mock_Gap_Manager(uint m) : Gap_Manager(m) {}
    MOCK_METHOD((std::map<uint, uint>), build_inverse_absolute_gaps, (), (override));
};

TEST(Schedule_Tests, ScheduleDownWorksCorrect) {
  Job_List jobs = {
    Job(2,10),  // J1  (not scheduled)
    Job(2, 8),  // J2
    Job(1, 6),  // J3
    Job(5, 4),  // J4  (not scheduled)
    Job(2, 3),  // J5
    Job(3, 2),  // J6  (not scheduled)
    Job(1, 1)   // J7  
  };

  map<uint,uint> inverse_absolute_gaps;
  inverse_absolute_gaps[0] = 11;
  inverse_absolute_gaps[1] = 10;
  inverse_absolute_gaps[3] =  8;
  inverse_absolute_gaps[5] =  4;
  inverse_absolute_gaps[7] =  2;
  inverse_absolute_gaps[9] =  0;

  auto gap_manager = make_shared<Mock_Gap_Manager>(11);
  EXPECT_CALL(*gap_manager, build_inverse_absolute_gaps())
    .WillOnce(testing::Return(inverse_absolute_gaps));

  gap_manager->makespan = 9;

  Schedule schedule(11, 6);
  schedule.gap_manager = gap_manager;
  Job_List unscheduled_jobs = schedule.schedule_down(jobs);


  Job J1 = unscheduled_jobs[0];
  Job J4 = unscheduled_jobs[1];
  Job J6 = unscheduled_jobs[2];
  
  Job J2 = schedule.placed_jobs[3];
  Job J3 = schedule.placed_jobs[2];
  Job J5 = schedule.placed_jobs[1];
  Job J7 = schedule.placed_jobs[0];

  EXPECT_EQ(J1.processing_time, 2);
  EXPECT_EQ(J1.required_machines, 10);

  EXPECT_EQ(J2.processing_time, 2);
  EXPECT_EQ(J2.required_machines, 8);
  EXPECT_EQ(J2.starting_time, 7);

  EXPECT_EQ(J3.processing_time, 1);
  EXPECT_EQ(J3.required_machines, 6);
  EXPECT_EQ(J3.starting_time, 6);

  EXPECT_EQ(J4.processing_time, 5);
  EXPECT_EQ(J4.required_machines, 4);

  EXPECT_EQ(J5.processing_time, 2);
  EXPECT_EQ(J5.required_machines, 3);
  EXPECT_EQ(J5.starting_time, 4);

  EXPECT_EQ(J6.processing_time, 3);
  EXPECT_EQ(J6.required_machines, 2);

  EXPECT_EQ(J7.processing_time, 1);
  EXPECT_EQ(J7.required_machines, 1);
  EXPECT_EQ(J7.starting_time, 3);
}




TEST(Schedule_Tests, ScheduleAndUnschedule) {
  Job_List jobs = {
    Job(2,10),  // J1  
    Job(2, 8),  // J2
    Job(1, 6),  // J3
    Job(5, 4),  // J4  
    Job(2, 3),  // J5
    Job(3, 2),  // J6  
    Job(1, 1)   // J7  
  };
  uint m = 10;
  uint n = 7;

  Schedule schedule(m, n);
  schedule.schedule_job(jobs[0],0);
  EXPECT_EQ(schedule.gap_manager->gaps[0], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[1], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[2], m);
  EXPECT_EQ(schedule.placed_jobs.size(), 1);
  
  schedule.unschedule_jobs({0}); // remove J1

  EXPECT_EQ(schedule.gap_manager->gaps[0], m);
  EXPECT_EQ(schedule.gap_manager->gaps[1], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[2], 0);
  EXPECT_EQ(schedule.placed_jobs.size(), 0);

  schedule.schedule_job(jobs[0], 0);
  schedule.schedule_job(jobs[1], 2);
  schedule.schedule_job(jobs[2], 4);
  schedule.schedule_job(jobs[3], 4);
  schedule.schedule_job(jobs[4], 6);
  schedule.schedule_job(jobs[5], 5);
  schedule.schedule_job(jobs[6], 6);

  EXPECT_EQ(schedule.placed_jobs.size(), 7);

  EXPECT_EQ(schedule.gap_manager->gaps[0],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[1],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[2],  2);
  EXPECT_EQ(schedule.gap_manager->gaps[3],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[4], -2);
  EXPECT_EQ(schedule.gap_manager->gaps[5],  4);
  EXPECT_EQ(schedule.gap_manager->gaps[6], -4);
  EXPECT_EQ(schedule.gap_manager->gaps[7],  1);
  EXPECT_EQ(schedule.gap_manager->gaps[8],  5);
  EXPECT_EQ(schedule.gap_manager->gaps[9],  4);

  schedule.unschedule_jobs({1,4,5,6}); // remove J2, J5, J6, J7
  EXPECT_EQ(schedule.placed_jobs.size(), 3);
  EXPECT_EQ(schedule.gap_manager->gaps[0],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[1],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[2],  m);
  EXPECT_EQ(schedule.gap_manager->gaps[3],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[4], -m);
  EXPECT_EQ(schedule.gap_manager->gaps[5],  6);
  EXPECT_EQ(schedule.gap_manager->gaps[6],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[7],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[8],  0);
  EXPECT_EQ(schedule.gap_manager->gaps[9],  4);

  Job J1 = schedule.placed_jobs[0];
  Job J3 = schedule.placed_jobs[1];
  Job J4 = schedule.placed_jobs[2];

  EXPECT_EQ(J1.required_machines,    10);
  EXPECT_EQ(J1.processing_time,       2);
  EXPECT_EQ(J1.starting_time.value(), 0);

  EXPECT_EQ(J3.required_machines,     6);
  EXPECT_EQ(J3.processing_time,       1);
  EXPECT_EQ(J3.starting_time.value(), 4);

  EXPECT_EQ(J4.required_machines,     4);
  EXPECT_EQ(J4.processing_time,       5);
  EXPECT_EQ(J4.starting_time.value(), 4);
}

TEST(Schedule_Tests, SplitAt) {
  uint m = 10;
  uint n = 9;
  
  Job_List jobs = {
    Job(3, 2),
    Job(2, 2),
    Job(2, 3),
    Job(3, 3),
    Job(3, 5),
    Job(4, 2),
    Job(2, 3),
    Job(2, 2),
    Job(2, 6)
  };

  Schedule schedule(m,n);
  schedule.schedule_job(jobs[0], 0);
  schedule.schedule_job(jobs[1], 0);
  schedule.schedule_job(jobs[2], 0);
  schedule.schedule_job(jobs[3], 0);
  schedule.schedule_job(jobs[4], 2);
  schedule.schedule_job(jobs[5], 3);
  schedule.schedule_job(jobs[6], 3);
  schedule.schedule_job(jobs[7], 5);
  schedule.schedule_job(jobs[8], 5);

  EXPECT_EQ(schedule.gap_manager->gaps[0], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[1], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[2], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[3], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[4], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[5], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[6], 0);
  EXPECT_EQ(schedule.gap_manager->gaps[7], m);

  Schedule s1(m,n), s2(m,n);
  schedule.split_at(3, s1, s2);

  EXPECT_EQ(s1.placed_jobs.size(), 5);
  EXPECT_EQ(s2.placed_jobs.size(), 4);

  EXPECT_EQ(s1.gap_manager->gaps[0], 0);
  EXPECT_EQ(s1.gap_manager->gaps[1], 0);
  EXPECT_EQ(s1.gap_manager->gaps[2], 0);
  EXPECT_EQ(s1.gap_manager->gaps[3], 5);
  EXPECT_EQ(s1.gap_manager->gaps[4], 0);
  EXPECT_EQ(s1.gap_manager->gaps[5], 5);
  EXPECT_EQ(s1.gap_manager->gaps[6], 0);
  EXPECT_EQ(s1.gap_manager->gaps[7], 0);

  EXPECT_EQ(s2.gap_manager->gaps[0], 5);
  EXPECT_EQ(s2.gap_manager->gaps[1], 0);
  EXPECT_EQ(s2.gap_manager->gaps[2], -5);
  EXPECT_EQ(s2.gap_manager->gaps[3], 0);
  EXPECT_EQ(s2.gap_manager->gaps[4], m);
  EXPECT_EQ(s2.gap_manager->gaps[5], 0);
  EXPECT_EQ(s2.gap_manager->gaps[6], 0);
  EXPECT_EQ(s2.gap_manager->gaps[7], 0);

}


TEST(Schedule_Tests, BalancedListSchedule) {
  uint m = 10;
  uint n = 100;
  Schedule sigma1(m,n);
  Schedule sigma2(m,n);

  sigma1.gap_manager->gaps[0]=2;
  sigma1.gap_manager->gaps[2]=1;
  sigma1.gap_manager->gaps[5]=1;
  sigma1.gap_manager->gaps[8]=6;

  sigma1.gap_manager->available_machines_in_gap=2;
  sigma1.gap_manager->makespan=8;

  sigma2.gap_manager->gaps[0]=1;
  sigma2.gap_manager->gaps[3]=1;
  sigma2.gap_manager->gaps[4]=4;
  sigma2.gap_manager->gaps[6]=1;
  sigma2.gap_manager->gaps[8]=1;
  sigma2.gap_manager->gaps[10]=2;

  sigma2.gap_manager->available_machines_in_gap=1;
  sigma2.gap_manager->makespan=10;

  sint h = 6;

  {
    Job_List jobs = {
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1)
    };
  
    Schedule::balanced_list_schedule(jobs, sigma1, sigma2, /*balance_time=*/h);

    EXPECT_EQ(h, 4);
    EXPECT_EQ(sigma1.placed_jobs.size(), 2);
    EXPECT_EQ(sigma2.placed_jobs.size(), 3);

    EXPECT_EQ(sigma1.placed_jobs[0].starting_time.value(), 0);
    EXPECT_EQ(sigma1.placed_jobs[1].starting_time.value(), 0);

    EXPECT_EQ(sigma2.placed_jobs[0].starting_time.value(), 0);
    EXPECT_EQ(sigma2.placed_jobs[1].starting_time.value(), 3);
    EXPECT_EQ(sigma2.placed_jobs[2].starting_time.value(), 3);
  }
}


TEST(Schedule_Tests, BalancedListSchedule2BalanceTime) {
  uint m = 10;
  uint n = 100;
  Schedule sigma1(m,n);
  Schedule sigma2(m,n);

  sigma1.gap_manager->gaps[0]=2;
  sigma1.gap_manager->gaps[2]=1;
  sigma1.gap_manager->gaps[5]=1;
  sigma1.gap_manager->gaps[8]=6;

  sigma1.gap_manager->available_machines_in_gap=2;
  sigma1.gap_manager->makespan=8;

  sigma2.gap_manager->gaps[0]=1;
  sigma2.gap_manager->gaps[3]=1;
  sigma2.gap_manager->gaps[4]=4;
  sigma2.gap_manager->gaps[6]=1;
  sigma2.gap_manager->gaps[8]=1;
  sigma2.gap_manager->gaps[10]=2;

  sigma2.gap_manager->available_machines_in_gap=1;
  sigma2.gap_manager->makespan=10;

  sint h = 6;

  {
    Job_List jobs = {
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1),
      Job(/*processing_time=*/3, /*required_machines=*/1)
    };
  
    Schedule::balanced_list_schedule(jobs, sigma1, sigma2, /*balance_time=*/h);

    EXPECT_EQ(h, 2);

    EXPECT_EQ(sigma1.placed_jobs.size(), 5);
    EXPECT_EQ(sigma2.placed_jobs.size(), 7);
  }
}


TEST(Schedule_Tests, BalancedListScheduleZeroBalanceTime) {
  uint m = 4;
  uint n = 100;
  Schedule sigma1(m,n);
  Schedule sigma2(m,n);

  sigma1.gap_manager->gaps[0]=2;
  sigma1.gap_manager->gaps[5]=2;

  sigma1.gap_manager->available_machines_in_gap=2;
  sigma1.gap_manager->makespan=5;

  sigma2.gap_manager->gaps[0]=2;
  sigma2.gap_manager->gaps[8]=2;

  sigma2.gap_manager->available_machines_in_gap=2;
  sigma2.gap_manager->makespan=8;

  sint h = 2;

  {
    Job_List jobs = {
      Job(/*processing_time=*/3, /*required_machines=*/2),
      Job(/*processing_time=*/3, /*required_machines=*/2),
      Job(/*processing_time=*/3, /*required_machines=*/2),
      Job(/*processing_time=*/3, /*required_machines=*/2),
      Job(/*processing_time=*/3, /*required_machines=*/2),
      Job(/*processing_time=*/3, /*required_machines=*/2),
      Job(/*processing_time=*/3, /*required_machines=*/2)
    };
  
    Schedule::balanced_list_schedule(jobs, sigma1, sigma2, /*balance_time=*/h);

    EXPECT_EQ(h, -3);
    EXPECT_EQ(sigma1.placed_jobs.size(), 3);
    EXPECT_EQ(sigma2.placed_jobs.size(), 4);
  }
}

TEST(Schedule_Tests, SortInHigherStack) {
  uint m = 13;
  uint n = 9;
  Schedule schedule(m,n);
  
  Job J1 = Job(/*processing_time=*/2, /*required_machines=*/ 6);
  J1.starting_time = 0;
  Job J2 = Job(/*processing_time=*/2, /*required_machines=*/ 6);
  J2.starting_time = 0;
  Job J3 = Job(/*processing_time=*/3, /*required_machines=*/ 5);
  J3.starting_time = 2;
  Job J4 = Job(/*processing_time=*/2, /*required_machines=*/ 5);
  J4.starting_time = 2;
  Job J5 = Job(/*processing_time=*/3, /*required_machines=*/ 4);
  J5.starting_time = 4;
  Job J6 = Job(/*processing_time=*/3, /*required_machines=*/ 3);
  J6.starting_time = 5;
  Job J7 = Job(/*processing_time=*/2, /*required_machines=*/ 3);
  J7.starting_time = 7;
  Job J8 = Job(/*processing_time=*/3, /*required_machines=*/ 1);
  J8.starting_time = 8;

  schedule.placed_jobs = {
    J1, J2, J3, J4, J5, J6, J7, J8
  };

  schedule.gap_manager->makespan = 11;
  schedule.gap_manager->available_machines_in_gap = 1;

  schedule.gap_manager->gaps[0] = 1;
  schedule.gap_manager->gaps[2] = 2;
  schedule.gap_manager->gaps[4] = 1;
  schedule.gap_manager->gaps[5] = 2;
  schedule.gap_manager->gaps[7] = 1;
  schedule.gap_manager->gaps[8] = 2;
  schedule.gap_manager->gaps[9] = 2;
  schedule.gap_manager->gaps[11] = 1;

  Job_List jobs_to_sort = {
    Job(2,7),
    Job(2,4),
    Job(1,2)
  };

  schedule.sort_in_higher_stack(jobs_to_sort);

  EXPECT_EQ(schedule.get_makespan(), 16);

  EXPECT_EQ(schedule.placed_jobs[0].starting_time.value(), 0);
  EXPECT_EQ(schedule.placed_jobs[0].processing_time, 2);
  EXPECT_EQ(schedule.placed_jobs[0].required_machines, 7);

  EXPECT_EQ(schedule.placed_jobs[6].starting_time.value(), 7);
  EXPECT_EQ(schedule.placed_jobs[6].processing_time, 2);
  EXPECT_EQ(schedule.placed_jobs[6].required_machines, 4);

  EXPECT_EQ(schedule.placed_jobs[9].starting_time.value(), 12);
  EXPECT_EQ(schedule.placed_jobs[9].processing_time, 1);
  EXPECT_EQ(schedule.placed_jobs[9].required_machines, 2);
}


TEST(Schedule_Tests, RemoveJobsAbove) {
  uint m = 13;
  uint n = 8;
  Schedule schedule(m,n);

  Job J1 = Job(/*processing_time=*/2, /*required_machines=*/ 6);
  J1.starting_time = 0;
  Job J2 = Job(/*processing_time=*/2, /*required_machines=*/ 6);
  J2.starting_time = 0;
  Job J3 = Job(/*processing_time=*/3, /*required_machines=*/ 5);
  J3.starting_time = 2;
  Job J4 = Job(/*processing_time=*/2, /*required_machines=*/ 5);
  J4.starting_time = 2;
  Job J5 = Job(/*processing_time=*/3, /*required_machines=*/ 4);
  J5.starting_time = 4;
  Job J6 = Job(/*processing_time=*/3, /*required_machines=*/ 3);
  J6.starting_time = 5;
  Job J7 = Job(/*processing_time=*/2, /*required_machines=*/ 3);
  J7.starting_time = 7;
  Job J8 = Job(/*processing_time=*/3, /*required_machines=*/ 1);
  J8.starting_time = 8;

  schedule.placed_jobs = {
    J1, J2, J3, J4, J5, J6, J7, J8
  };

  schedule.gap_manager->makespan = 11;
  schedule.gap_manager->available_machines_in_gap = 1;

  schedule.gap_manager->gaps[0] = 1;
  schedule.gap_manager->gaps[2] = 2;
  schedule.gap_manager->gaps[4] = 1;
  schedule.gap_manager->gaps[5] = 2;
  schedule.gap_manager->gaps[7] = 1;
  schedule.gap_manager->gaps[8] = 2;
  schedule.gap_manager->gaps[9] = 2;
  schedule.gap_manager->gaps[11] = 1;

  Job_List removed_jobs = schedule.remove_jobs_above(5);
  
  EXPECT_EQ(removed_jobs.size(), 4);
  EXPECT_EQ(schedule.placed_jobs.size(), 4);

  EXPECT_EQ(removed_jobs[0].starting_time.has_value(), 0);
  EXPECT_EQ(removed_jobs[0].processing_time, 3);
  EXPECT_EQ(removed_jobs[0].required_machines, 4);

  EXPECT_EQ(removed_jobs[1].starting_time.has_value(), 0);
  EXPECT_EQ(removed_jobs[1].processing_time, 3);
  EXPECT_EQ(removed_jobs[1].required_machines, 3);

  EXPECT_EQ(removed_jobs[2].starting_time.has_value(), 0);
  EXPECT_EQ(removed_jobs[2].processing_time, 2);
  EXPECT_EQ(removed_jobs[2].required_machines, 3);

  EXPECT_EQ(removed_jobs[3].starting_time.has_value(), 0);
  EXPECT_EQ(removed_jobs[3].processing_time, 3);
  EXPECT_EQ(removed_jobs[3].required_machines, 1);


  EXPECT_EQ(schedule.placed_jobs[0].starting_time.has_value(), 1);
  EXPECT_EQ(schedule.placed_jobs[0].processing_time, 2);
  EXPECT_EQ(schedule.placed_jobs[0].required_machines, 6);
}

TEST(Tower_Schedule_Tests, Sigma1Example) {
  uint m = 100;
  uint n = 20;

  // big
  Job J1 = Job(/*processing_time=*/30, /*required_machines=*/ 100);
  Job J2 = Job(/*processing_time=*/10, /*required_machines=*/ 90);
  Job J3 = Job(/*processing_time=*/10, /*required_machines=*/ 80);
  Job J4 = Job(/*processing_time=*/50, /*required_machines=*/ 60);

  // medium
  Job J5 = Job(/*processing_time=*/10, /*required_machines=*/ 40);
  Job J6 = Job(/*processing_time=*/20, /*required_machines=*/ 35);

  // small
  Job J7 = Job(/*processing_time=*/10, /*required_machines=*/ 30);
  Job J8 = Job(/*processing_time=*/10, /*required_machines=*/ 26);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);

  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 100);

  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(), 30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(), 50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(), 50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(), 60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(), 70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(), 90);
}

TEST(Tower_Schedule_Tests, Sigma2Example) {
  uint m = 100;
  uint n = 20;

  // medium
  Job J1  = Job(/*processing_time=*/20, /*required_machines=*/ 50);
  Job J2  = Job(/*processing_time=*/10, /*required_machines=*/ 50);
  Job J3  = Job(/*processing_time=*/30, /*required_machines=*/ 40);
  Job J4  = Job(/*processing_time=*/20, /*required_machines=*/ 40);
  Job J5  = Job(/*processing_time=*/20, /*required_machines=*/ 35);
  Job J6  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J7  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J8  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J9  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J10 = Job(/*processing_time=*/20, /*required_machines=*/ 30);

  // tiny
  Job J11 = Job(/*processing_time=*/20, /*required_machines=*/ 10);
  Job J12 = Job(/*processing_time=*/10, /*required_machines=*/ 20);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);
  
  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 80);

  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(), 20);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(), 30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[8].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9].starting_time.value(), 60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11].starting_time.value(),60);
}

TEST(Tower_Schedule_Tests, Sigma2Example2) {
  uint m = 100;
  uint n = 20;

  // medium
  Job J1  = Job(/*processing_time=*/20, /*required_machines=*/ 50);
  Job J2  = Job(/*processing_time=*/10, /*required_machines=*/ 50);
  Job J3  = Job(/*processing_time=*/30, /*required_machines=*/ 40);
  Job J4  = Job(/*processing_time=*/20, /*required_machines=*/ 40);
  Job J5  = Job(/*processing_time=*/20, /*required_machines=*/ 35);
  Job J6  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J7  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J8  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J9  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J10 = Job(/*processing_time=*/20, /*required_machines=*/ 30);

  // tiny
  Job J11 = Job(/*processing_time=*/20, /*required_machines=*/ 10);
  Job J12 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J13 = Job(/*processing_time=*/10, /*required_machines=*/ 20);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);
  
  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 90);

  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(), 20);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(), 30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[8].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9].starting_time.value(), 50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[12].starting_time.value(),70);

}


TEST(Tower_Schedule_Tests, Sigma2Example3) {
  uint m = 100;
  uint n = 20;

  // medium
  Job J1  = Job(/*processing_time=*/20, /*required_machines=*/ 50);
  Job J2  = Job(/*processing_time=*/10, /*required_machines=*/ 50);
  Job J3  = Job(/*processing_time=*/30, /*required_machines=*/ 40);
  Job J4  = Job(/*processing_time=*/20, /*required_machines=*/ 40);
  Job J5  = Job(/*processing_time=*/20, /*required_machines=*/ 35);
  Job J6  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J7  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J8  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J9  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J10 = Job(/*processing_time=*/20, /*required_machines=*/ 30);

  // tiny
  Job J11 = Job(/*processing_time=*/20, /*required_machines=*/ 10);
  Job J12 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J13 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J14 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J15 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J16 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J17 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J18 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J19 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J20 = Job(/*processing_time=*/10, /*required_machines=*/ 20);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,J15,J16,J17,J18,J19,J20};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);
  
  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 100);

  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(), 20);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(), 30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[8].starting_time.value(), 40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9].starting_time.value(), 50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[12].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[13].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[14].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[15].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[16].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[17].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[18].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[19].starting_time.value(),80);
}


TEST(Tower_Schedule_Tests, Sigma2Example4) {
  uint m = 100;
  uint n = 20;

  // medium
  Job J1  = Job(/*processing_time=*/20, /*required_machines=*/ 50);
  Job J2  = Job(/*processing_time=*/10, /*required_machines=*/ 50);
  Job J3  = Job(/*processing_time=*/30, /*required_machines=*/ 40);
  Job J4  = Job(/*processing_time=*/20, /*required_machines=*/ 40);
  Job J5  = Job(/*processing_time=*/20, /*required_machines=*/ 35);
  Job J6  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J7  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J8  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J9  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J10 = Job(/*processing_time=*/20, /*required_machines=*/ 30);

  // tiny
  Job J11 = Job(/*processing_time=*/20, /*required_machines=*/ 10);
  Job J12 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J13 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J14 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J15 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J16 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J17 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J18 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J19 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J20 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J21 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J22 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J23 = Job(/*processing_time=*/10, /*required_machines=*/ 20);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,J15,J16,J17,J18,J19,J20,J21,J22,J23};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);
  
  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 110);

  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(), 0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(), 10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(), 20);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[8].starting_time.value(), 20);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9].starting_time.value(), 20);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[12].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[13].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[14].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[15].starting_time.value(),50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[16].starting_time.value(),50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[17].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[18].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[19].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[20].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[21].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[22].starting_time.value(),100);
}

TEST(Tower_Schedule_Tests, Example4) {
  uint m = 100;
  uint n = 31;

  // sigma1
  // big
  Job J24 = Job(/*processing_time=*/30, /*required_machines=*/ 100);
  Job J25 = Job(/*processing_time=*/10, /*required_machines=*/ 90);
  Job J26 = Job(/*processing_time=*/10, /*required_machines=*/ 80);
  Job J27 = Job(/*processing_time=*/50, /*required_machines=*/ 61);

  // medium
  Job J28 = Job(/*processing_time=*/10, /*required_machines=*/ 39);
  Job J29 = Job(/*processing_time=*/20, /*required_machines=*/ 35);

  // small
  Job J30 = Job(/*processing_time=*/10, /*required_machines=*/ 30);
  Job J31 = Job(/*processing_time=*/10, /*required_machines=*/ 26);

  // sigma2
  // medium
  Job J1  = Job(/*processing_time=*/20, /*required_machines=*/ 50);
  Job J2  = Job(/*processing_time=*/10, /*required_machines=*/ 50);
  Job J3  = Job(/*processing_time=*/30, /*required_machines=*/ 40);
  Job J4  = Job(/*processing_time=*/20, /*required_machines=*/ 40);
  Job J5  = Job(/*processing_time=*/20, /*required_machines=*/ 40);

  // small
  Job J6  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J7  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J8  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J9  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J10 = Job(/*processing_time=*/20, /*required_machines=*/ 30);

  // tiny
  Job J11 = Job(/*processing_time=*/20, /*required_machines=*/ 10);
  Job J12 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J13 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J14 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J15 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J16 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J17 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J18 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J19 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J20 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J21 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J22 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J23 = Job(/*processing_time=*/10, /*required_machines=*/ 20);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,J15,J16,J17,J18,J19,J20,J21,J22,J23,J24,J25,J26,J27,J28,J29,J30,J31};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);

  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 200);

  // sigma1
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[23].starting_time.value(),100);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[24].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[25].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[26].starting_time.value(),140);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[27].starting_time.value(),150);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[28].starting_time.value(),150);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[29].starting_time.value(),170);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[30].starting_time.value(),190);

  // sigma2
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(),0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(),0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(),10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(),20);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(),20);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[8].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9].starting_time.value(),50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[12].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[13].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[14].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[15].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[16].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[17].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[18].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[19].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[20].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[21].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[22].starting_time.value(),90);

  
}

TEST(Tower_Schedule_Tests, ManyTinyBalancePositive) {
  uint m = 100;
  uint n = 30;

  // sigma1
  // big
  Job J25 = Job(/*processing_time=*/30, /*required_machines=*/ 100);
  Job J27 = Job(/*processing_time=*/10, /*required_machines=*/ 80);
  Job J28 = Job(/*processing_time=*/50, /*required_machines=*/ 61);

  // medium
  Job J29 = Job(/*processing_time=*/10, /*required_machines=*/ 39);
  Job J30 = Job(/*processing_time=*/20, /*required_machines=*/ 35);

  // small
  Job J31 = Job(/*processing_time=*/10, /*required_machines=*/ 30);

  // sigma2
  // medium
  Job J1  = Job(/*processing_time=*/21, /*required_machines=*/ 50);
  Job J2  = Job(/*processing_time=*/10, /*required_machines=*/ 50);
  Job J3  = Job(/*processing_time=*/30, /*required_machines=*/ 40);
  Job J4  = Job(/*processing_time=*/20, /*required_machines=*/ 40);
  Job J5  = Job(/*processing_time=*/20, /*required_machines=*/ 40);

  // small
  Job J6  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J7  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J8  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J9  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J10 = Job(/*processing_time=*/20, /*required_machines=*/ 30);

  // tiny
  Job J11 = Job(/*processing_time=*/20, /*required_machines=*/ 10);
  Job J12 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J13 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J14 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J15 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J16 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J17 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J18 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J19 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J20 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J21 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J22 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J23 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J24 = Job(/*processing_time=*/10, /*required_machines=*/ 20);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,J15,J16,J17,J18,J19,J20,J21,J22,J23,J24,J25/*,J26*/,J27,J28,J29,J30,J31/*,J32*/};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);

  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 200);

  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(),0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(),50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[8].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11].starting_time.value(),100);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[12].starting_time.value(),100);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[13].starting_time.value(),100);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[14].starting_time.value(),110);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[15].starting_time.value(),110);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[16].starting_time.value(),110);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[17].starting_time.value(),120);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[18].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[19].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[20].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[21].starting_time.value(),140);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[22].starting_time.value(),140);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[23].starting_time.value(),150);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[24].starting_time.value(),150);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[25].starting_time.value(),160);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[26].starting_time.value(),160);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[27].starting_time.value(),170);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[28].starting_time.value(),170);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[29].starting_time.value(),190);
}

TEST(Tower_Schedule_Tests, ManyTinyBalanceZero) {
  uint m = 100;
  uint n = 46;

  // sigma1
  // big
  Job J25 = Job(/*processing_time=*/30, /*required_machines=*/ 100);
  Job J27 = Job(/*processing_time=*/10, /*required_machines=*/ 80);
  Job J28 = Job(/*processing_time=*/50, /*required_machines=*/ 61);

  // medium
  Job J29 = Job(/*processing_time=*/10, /*required_machines=*/ 39);
  Job J30 = Job(/*processing_time=*/20, /*required_machines=*/ 35);

  // small
  Job J31 = Job(/*processing_time=*/10, /*required_machines=*/ 30);

  // sigma2
  // medium
  Job J1  = Job(/*processing_time=*/21, /*required_machines=*/ 50);
  Job J2  = Job(/*processing_time=*/10, /*required_machines=*/ 50);
  Job J3  = Job(/*processing_time=*/30, /*required_machines=*/ 40);
  Job J4  = Job(/*processing_time=*/20, /*required_machines=*/ 40);
  Job J5  = Job(/*processing_time=*/20, /*required_machines=*/ 40);

  // small
  Job J6  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J7  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J8  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J9  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J10 = Job(/*processing_time=*/20, /*required_machines=*/ 30);

  // tiny
  Job J11 = Job(/*processing_time=*/20, /*required_machines=*/ 10);
  Job J12 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J13 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J14 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J15 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J16 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J17 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J18 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J19 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J20 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J21 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J22 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J23 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J24 = Job(/*processing_time=*/10, /*required_machines=*/ 20);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,J15,J16,J17,J18,J19,J20,J21,J22,J23,J24,J25/*,J26*/,J27,J28,J29,J30,J31/*,J32*/,
    J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);

  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 240);

  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(),0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(),50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[8].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[12].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[13].starting_time.value(),100);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[14].starting_time.value(),100);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[15].starting_time.value(),100);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[16].starting_time.value(),110);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[17].starting_time.value(),110);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[18].starting_time.value(),110);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[19].starting_time.value(),110);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[20].starting_time.value(),120);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[21].starting_time.value(),120);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[22].starting_time.value(),120);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[23].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[24].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[25].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[26].starting_time.value(),130);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[27].starting_time.value(),140);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[28].starting_time.value(),140);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[29].starting_time.value(),140);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[30].starting_time.value(),150);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[31].starting_time.value(),150);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[32].starting_time.value(),150);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[33].starting_time.value(),160);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[34].starting_time.value(),170);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[35].starting_time.value(),170);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[36].starting_time.value(),170);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[37].starting_time.value(),180);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[38].starting_time.value(),180);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[39].starting_time.value(),190);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[40].starting_time.value(),190);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[41].starting_time.value(),200);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[42].starting_time.value(),200);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[43].starting_time.value(),210);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[44].starting_time.value(),210);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[45].starting_time.value(),230);
}

TEST(Tower_Schedule_Tests, ManyTinyBalanceNegative) {
  uint m = 100;
  uint n = 30;

  // sigma1
  // big
  Job J25 = Job(/*processing_time=*/30, /*required_machines=*/ 100);
  Job J27 = Job(/*processing_time=*/10, /*required_machines=*/ 80);
  Job J28 = Job(/*processing_time=*/50, /*required_machines=*/ 61);

  // medium
  Job J29 = Job(/*processing_time=*/10, /*required_machines=*/ 39);
  Job J30 = Job(/*processing_time=*/20, /*required_machines=*/ 35);

  // small
  Job J31 = Job(/*processing_time=*/10, /*required_machines=*/ 30);

  // sigma2
  // medium
  Job J1  = Job(/*processing_time=*/21, /*required_machines=*/ 50);
  Job J2  = Job(/*processing_time=*/10, /*required_machines=*/ 50);
  Job J3  = Job(/*processing_time=*/30, /*required_machines=*/ 40);
  Job J4  = Job(/*processing_time=*/20, /*required_machines=*/ 40);
  Job J5  = Job(/*processing_time=*/20, /*required_machines=*/ 40);

  // small
  Job J6  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J7  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J8  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J9  = Job(/*processing_time=*/20, /*required_machines=*/ 30);
  Job J10 = Job(/*processing_time=*/20, /*required_machines=*/ 30);

  // tiny
  Job J11 = Job(/*processing_time=*/20, /*required_machines=*/ 10);
  Job J12 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J13 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J14 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J15 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J16 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J17 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J18 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J19 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J20 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J21 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J22 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J23 = Job(/*processing_time=*/10, /*required_machines=*/ 20);
  Job J24 = Job(/*processing_time=*/10, /*required_machines=*/ 20);

  Job_List jobs = {J1,J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,J15,J16,J17,J18,J19,J20,J21,J22,J23,J24,J25/*,J26*/,J27,J28,J29,J30,J31/*,J32*/,
    J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12,J12};

  Tower_Schedule tower_schedule(m,n);
  tower_schedule.schedule_jobs(jobs);

  EXPECT_EQ(tower_schedule.sigma.get_makespan(), 250);

  EXPECT_EQ(tower_schedule.sigma.placed_jobs[0].starting_time.value(),0);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[1].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[2].starting_time.value(),30);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[3].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[4].starting_time.value(),40);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[5].starting_time.value(),50);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[6].starting_time.value(),60);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[7].starting_time.value(),70);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[8].starting_time.value(),80);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[12].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[13].starting_time.value(),90);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[9+5].starting_time.value(),90+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[10+5].starting_time.value(),90+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[11+5].starting_time.value(),90+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[12+5].starting_time.value(),90+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[13+5].starting_time.value(),100+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[14+5].starting_time.value(),100+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[15+5].starting_time.value(),100+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[16+5].starting_time.value(),110+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[17+5].starting_time.value(),110+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[18+5].starting_time.value(),110+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[19+5].starting_time.value(),110+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[20+5].starting_time.value(),120+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[21+5].starting_time.value(),120+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[22+5].starting_time.value(),120+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[23+5].starting_time.value(),130+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[24+5].starting_time.value(),130+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[25+5].starting_time.value(),130+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[26+5].starting_time.value(),130+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[27+5].starting_time.value(),140+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[28+5].starting_time.value(),140+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[29+5].starting_time.value(),140+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[30+5].starting_time.value(),150+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[31+5].starting_time.value(),150+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[32+5].starting_time.value(),150+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[33+5].starting_time.value(),160+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[34+5].starting_time.value(),170+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[35+5].starting_time.value(),170+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[36+5].starting_time.value(),170+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[37+5].starting_time.value(),180+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[38+5].starting_time.value(),180+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[39+5].starting_time.value(),190+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[40+5].starting_time.value(),190+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[41+5].starting_time.value(),200+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[42+5].starting_time.value(),200+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[43+5].starting_time.value(),210+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[44+5].starting_time.value(),210+10);
  EXPECT_EQ(tower_schedule.sigma.placed_jobs[45+5].starting_time.value(),230+10);
}
