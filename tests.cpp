#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "types.hpp"

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
    gap_manager.place_job_at(0, job);
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
    gap_manager.place_job_at(0, job);
    EXPECT_EQ(gap_manager.current_time, 0);
    EXPECT_EQ(gap_manager.available_machines_in_gap, 500);
  }
  {
    Job job(15, 499);
    uint time = gap_manager.update_earliest_time_to_place(job);
    EXPECT_EQ(time, 0);
    gap_manager.place_job_at(0, job);
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














