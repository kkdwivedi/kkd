// SPDX-License-Identifier: MIT
// Copyright (c) 2023 Kumar Kartikeya Dwivedi <memxor@gmail.com>
#ifndef KKD_CPP_TASK_HPP
#define KKD_CPP_TASK_HPP

#include <coroutine>
#include <exception>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace kkd {
namespace detail {

template <typename T>
constexpr inline bool is_valid_promise_type_v =
    std::is_default_constructible_v<T> && (std::is_copy_assignable_v<T> || std::is_move_assignable_v<T>);

template <>
constexpr inline bool is_valid_promise_type_v<void> = true;

}  // namespace detail

template <typename>
class Task;

template <typename>
class Awaitable;

template <typename Derived, typename Future>
class PromiseBase {
  friend class Awaitable<Future>;

  struct Awaitable {
    bool await_ready() noexcept { return false; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<>) noexcept { return continuation_coro_; }
    void await_resume() noexcept {}

    std::coroutine_handle<> continuation_coro_;
  };

 public:
  constexpr PromiseBase() : continuation_coro_(std::noop_coroutine()) {}

  PromiseBase(const PromiseBase&) = delete;
  PromiseBase& operator=(const PromiseBase&) = delete;
  PromiseBase(PromiseBase&&) = delete;
  PromiseBase& operator=(PromiseBase&&) = delete;
  ~PromiseBase() = default;

  Future get_return_object() noexcept {
    return Future{std::coroutine_handle<Derived>::from_promise(*static_cast<Derived *>(this))};
  }

  std::suspend_always initial_suspend() noexcept { return {}; }

  Awaitable final_suspend() noexcept { return {continuation_coro_}; }

  void unhandled_exception() {
    if (exception_) {
      throw std::runtime_error("Exception already set for promise!");
    }
    exception_ = std::current_exception();
  }

  [[nodiscard]] std::exception_ptr Exception() { return exception_; }

 protected:
  void SetContinuationHandle(std::coroutine_handle<> coro) { this->continuation_coro_ = coro; }

  std::coroutine_handle<> continuation_coro_;
  std::exception_ptr exception_;
};

template <typename T, typename Future, typename = std::enable_if_t<detail::is_valid_promise_type_v<T>, void>>
class Promise : public PromiseBase<Promise<T, Future>, Future> {
 public:
  template <typename U>  //, std::enable_if_t<std::is_convertible_v<U, T>>>
  void return_value(U&& value) {
    value_ = std::forward<U>(value);
  }

  T& Value() { return value_; }

 private:
  T value_;
};

template <typename Future>
class Promise<void, Future, void> : public PromiseBase<Promise<void, Future, void>, Future> {
 public:
  void Value() {}
  void return_void() {}
};

template <typename Derived>
class Awaitable {
 public:
  bool await_ready() noexcept { return false; }

  std::coroutine_handle<> await_suspend(std::coroutine_handle<> caller_coro) noexcept {
    auto coro = static_cast<Derived *>(this)->Handle();
    coro.promise().SetContinuationHandle(caller_coro);
    return coro;
  }

  [[nodiscard]] auto await_resume() { return static_cast<Derived *>(this)->Result(); }
};

enum class TaskState {
  Pending,
  Done,
};

template <typename T = void>
class Task : public Awaitable<Task<T>> {
  friend class Awaitable<Task<T>>;

 public:
  using promise_type = Promise<T, Task<T>>;
  using PromiseType = promise_type;

  Task(std::coroutine_handle<PromiseType> coro) : coro_(coro) {}

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  Task(Task&& t) noexcept : coro_(t.coro_) { t.coro_ = nullptr; }

  Task& operator=(Task&& t) noexcept {
    if (std::addressof(t) != this) {
      if (coro_) {
        coro_.destroy();
      }
      coro_ = std::exchange(t.coro_, nullptr);
    }
    return *this;
  }

  ~Task() {
    if (coro_) {
      coro_.destroy();
    }
  }

  bool Empty() const noexcept { return !coro_; }

  TaskState State() const noexcept {
    if (coro_.done()) {
      return TaskState::Done;
    }
    return TaskState::Pending;
  }

  [[nodiscard]] TaskState Poll() const {
    if (Empty()) {
      throw std::runtime_error("Poll() called on empty task!");
    }
    if (State() != TaskState::Done) {
      coro_.resume();
    }
    return State();
  }

  [[nodiscard]] std::conditional_t<std::is_same_v<T, void>, void, std::add_lvalue_reference_t<T>> Result() const {
    if (Empty() || State() != TaskState::Done) {
      throw std::runtime_error("Result() called on incomplete or empty task!");
    }
    auto exception = coro_.promise().Exception();
    if (exception) {
      std::rethrow_exception(exception);
    }
    return coro_.promise().Value();
  }

  [[nodiscard]] std::coroutine_handle<PromiseType> Release() {
    return std::exchange(coro_, std::coroutine_handle<PromiseType>{});
  }

 private:
  std::coroutine_handle<PromiseType> Handle() { return coro_; }

  std::coroutine_handle<PromiseType> coro_;
};

}  // namespace kkd

#endif
