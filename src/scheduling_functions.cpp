#include "scheduling/scheduling.hpp"
#include "scheduling_tasks.hpp"
#include <iostream>

using namespace scheduling;

// Fibonacci implementation
int Fibonacci_Scheduling(ThreadPool &thread_pool, const int n) {
  if (n < 2) {
    return 1;
  }
  int a, b;
  std::atomic counter{0};
  thread_pool.Submit([&, n] {
    a = Fibonacci_Scheduling(thread_pool, n - 1);
    counter.fetch_add(1);
  });
  thread_pool.Submit([&, n] {
    b = Fibonacci_Scheduling(thread_pool, n - 2);
    counter.fetch_add(1);
  });
  thread_pool.Wait([&] { return counter.load() == 2; });
  return a + b;
}

// Linear Chain implementation
void Linear_Chain_Scheduling(const int length) {
  int counter = 0;
  std::vector<Task> v(length);
  v[0] = Task([&] { ++counter; });
  for (auto i = v.begin(), j = std::next(v.begin()); j != v.end(); ++i, ++j) {
    *j = Task([&] { ++counter; });
    j->Succeed(&*i);
  }
  ThreadPool thread_pool;
  thread_pool.Submit(&v[0]);
}

// Matrix Multiplication implementation
void Matrix_Mul_Scheduling(const int n, std::vector<std::vector<int>> &a,
                           std::vector<std::vector<int>> &b,
                           std::vector<std::vector<int>> &c) {
  std::vector<Task> tasks;
  tasks.reserve(4 * n + 1);

  tasks.emplace_back();

  for (int i = 0; i < n; ++i) {
    tasks
        .emplace_back([&, i, n] {
          for (int j = 0; j < n; ++j) {
            a[i][j] = i + j;
          }
        })
        .Precede(&tasks[0]);
  }

  for (int i = 0; i < n; ++i) {
    tasks
        .emplace_back([&, i, n] {
          for (int j = 0; j < n; ++j) {
            b[i][j] = i * j;
          }
        })
        .Precede(&tasks[0]);
  }

  for (int i = 0; i < n; ++i) {
    tasks
        .emplace_back([&, i, n] {
          for (int j = 0; j < n; ++j) {
            c[i][j] = 0;
          }
        })
        .Precede(&tasks[0]);
  }

  for (int i = 0; i < n; ++i) {
    tasks
        .emplace_back([&, i, n] {
          for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
              c[i][j] += a[i][k] * b[k][j];
            }
          }
        })
        .Succeed(&tasks[0]);
  }

  ThreadPool thread_pool;
  thread_pool.Submit(tasks);
}