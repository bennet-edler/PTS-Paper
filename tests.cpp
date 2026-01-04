#include <gtest/gtest.h>
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


TEST(Schedule_Tests, OnTwoStackWorksCorrectly) {
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














