#include <benchmark/benchmark.h>
#include <tbb/tbb.h>

namespace tbb_benchmarks::fibonacci {
int Fibonacci(int n) {
  if (n < 2)
    return 1;

  int a, b;
  tbb::task_group g;

  g.run([&] { a = Fibonacci(n - 1); });
  g.run([&] { b = Fibonacci(n - 2); });
  g.wait();

  return a + b;
}

void Benchmark(benchmark::State &state) {
  const auto n = static_cast<int>(state.range(0));
  for (auto _ : state) {
    benchmark::DoNotOptimize(Fibonacci(n));
  }
}
} // namespace tbb_benchmarks::fibonacci

namespace tbb_benchmarks::linear_chain {
void LinearChain(int length) {
  std::atomic<int> counter{0};

  // Create a series of tasks where each depends on the previous
  tbb::task_group root_group;
  tbb::task_group *prev_group = &root_group;

  for (int i = 0; i < length; ++i) {
    auto *new_group = new tbb::task_group();

    prev_group->run([=, &counter] {
      counter.fetch_add(1, std::memory_order_relaxed);
      new_group->wait(); // Next task waits for this one
    });

    prev_group = new_group;
  }

  // Final task
  prev_group->run(
      [&counter] { counter.fetch_add(1, std::memory_order_relaxed); });

  root_group.wait(); // Wait for entire chain to complete
}

void Benchmark(benchmark::State &state) {
  const auto length = static_cast<int>(state.range(0));

  for (auto _ : state) {
    LinearChain(length);
    benchmark::ClobberMemory();
  }
}
} // namespace tbb_benchmarks::linear_chain

namespace tbb_benchmarks::matrix_multiplication {
void MatrixMultiply(int size, const std::vector<std::vector<double>> &a,
                    const std::vector<std::vector<double>> &b,
                    std::vector<std::vector<double>> &c) {
  tbb::parallel_for(0, size, [&](int i) {
    for (int j = 0; j < size; ++j) {
      for (int k = 0; k < size; ++k) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  });
}

void Benchmark(benchmark::State &state) {
  const int size = state.range(0);
  std::vector a(size, std::vector<double>(size, 1.0));
  std::vector b(size, std::vector<double>(size, 2.0));
  std::vector c(size, std::vector<double>(size, 0.0));

  for (auto _ : state) {
    MatrixMultiply(size, a, b, c);
    benchmark::ClobberMemory();
  }
}
} // namespace tbb_benchmarks::matrix_multiplication

BENCHMARK(tbb_benchmarks::fibonacci::Benchmark)
    ->Name("tbb/fibonacci")
    ->DenseRange(20, 30)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(tbb_benchmarks::linear_chain::Benchmark)
    ->Name("tbb/linear_chain")
    ->RangeMultiplier(2)
    ->Range(1 << 15, 1 << 20)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(tbb_benchmarks::matrix_multiplication::Benchmark)
    ->Name("tbb/matrix_multiplication")
    ->RangeMultiplier(2)
    ->Range(64, 1024)
    ->Unit(benchmark::kMillisecond);
