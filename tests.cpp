#include <gtest/gtest.h>
#include "types.hpp"

// INDEX TREE
TEST(Index_Tree_Tests, GetNextGap_GetsCorrectGap) {

}

// GAP MANAGER
TEST(Gap_Manager_Tests, GetEarliestTimeToPlace_ReturnsCorrectTime) {
  Gap_Manager gap_manager(100);
  Job job(10,10);
  uint time = gap_manager.get_earliest_time_to_place(job);
  EXPECT_EQ(time, 0);
}

TEST(Gap_Manager_Tests, GetEarliestTimeToPlace_ReturnsZeroInitially) {
  Gap_Manager gap_manager(100);

  {
    Job job(10,60); // proccesing_time, required_machines
    gap_manager.place_job_at(0, job);
  }

  Job job(10,60);
  uint time = gap_manager.get_earliest_time_to_place(job);
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
    uint time = gap_manager.get_earliest_time_to_place(job);
    EXPECT_EQ(time, 0);
    gap_manager.place_job_at(0, job);
    EXPECT_EQ(gap_manager.current_time, 0);
    EXPECT_EQ(gap_manager.available_machines_in_gap, 500);
  }
  {
    Job job(15, 499);
    uint time = gap_manager.get_earliest_time_to_place(job);
    EXPECT_EQ(time, 0);
    gap_manager.place_job_at(0, job);
    EXPECT_EQ(gap_manager.current_time, 0);
    EXPECT_EQ(gap_manager.available_machines_in_gap, 1);
  }

  {
    Job job(10, 2);
    uint time = gap_manager.get_earliest_time_to_place(job);
    EXPECT_EQ(time, 10);
    EXPECT_EQ(gap_manager.current_time, 10);
    EXPECT_EQ(gap_manager.available_machines_in_gap, 501);
  }
}
