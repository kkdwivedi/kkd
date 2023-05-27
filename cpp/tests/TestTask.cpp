// SPDX-License-Identifier: MIT
// Copyright (c) 2023 Kumar Kartikeya Dwivedi <memxor@gmail.com>
#include <gtest/gtest.h>

#include <Task.hpp>
#include <coroutine>
#include <stdexcept>
#include <type_traits>

template <typename T>
static void TestTaskCopy() {
  EXPECT_FALSE(std::is_copy_assignable_v<kkd::Task<T>>);
  EXPECT_FALSE(std::is_copy_constructible_v<kkd::Task<T>>);
}

template <typename T>
static void TestTaskMove() {
  EXPECT_TRUE(std::is_move_assignable_v<kkd::Task<T>>);
  EXPECT_TRUE(std::is_move_constructible_v<kkd::Task<T>>);

  auto task = []() -> kkd::Task<int> { co_return 0; }();
  kkd::Task<int> new_task = std::move(task);
  EXPECT_TRUE(task.Empty());
  EXPECT_FALSE(new_task.Empty());
}

TEST(Task, TaskCopy) {
  TestTaskCopy<void>();
  TestTaskCopy<int>();
  TestTaskCopy<long>();
  TestTaskCopy<unsigned int>();
  TestTaskCopy<unsigned long>();
}

TEST(Task, TaskMove) {
  TestTaskMove<void>();
  TestTaskMove<int>();
  TestTaskMove<long>();
  TestTaskMove<unsigned int>();
  TestTaskMove<unsigned long>();
}

TEST(Task, TaskResult) {
  auto gen_task = []() -> kkd::Task<int> { co_return 69; };
  auto gen_susp_task = []() -> kkd::Task<int> {
    co_await std::suspend_always{};
    co_return 69;
  };
  auto gen_ex_task = []() -> kkd::Task<int> {
    throw 0;
    co_return 0;
  };
  {
    auto task = gen_task();
    EXPECT_EQ(task.State(), kkd::TaskState::Pending);
    EXPECT_EQ(task.Poll(), kkd::TaskState::Done);
    EXPECT_EQ(task.Result(), 69);
  }
  {
    auto task = gen_task();
    EXPECT_EQ(task.State(), kkd::TaskState::Pending);
    EXPECT_THROW((void)task.Result(), std::runtime_error);
  }
  {
    auto task = gen_susp_task();
    EXPECT_EQ(task.State(), kkd::TaskState::Pending);
    EXPECT_EQ(task.Poll(), kkd::TaskState::Pending);
    EXPECT_EQ(task.Poll(), kkd::TaskState::Done);
    EXPECT_EQ(task.Result(), 69);
  }
  {
    auto task = gen_susp_task();
    EXPECT_EQ(task.State(), kkd::TaskState::Pending);
    EXPECT_EQ(task.Poll(), kkd::TaskState::Pending);
    EXPECT_THROW((void)task.Result(), std::runtime_error);
  }
  {
    auto task = gen_ex_task();
    EXPECT_EQ(task.State(), kkd::TaskState::Pending);
    EXPECT_EQ(task.Poll(), kkd::TaskState::Done);
    EXPECT_THROW((void)task.Result(), int);
  }
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
