#include <chrono>
#include <iostream>
#include <vector>

#include "scheduling_tasks.hpp"

// Function prototypes
int Fibonacci_Scheduling(scheduling::ThreadPool &thread_pool, const int n);
void Linear_Chain_Scheduling(int n);
void Matrix_Mul_Scheduling(const int n, std::vector<std::vector<int>> &a,
                           std::vector<std::vector<int>> &b,
                           std::vector<std::vector<int>> &c);

int main(int argc, char **argv) {
  scheduling::ThreadPool thread_pool;
  std::vector<scheduling::Task> tasks;

  // Reserve space for all tasks (Fibonacci + Linear Chain + Matrix Mul)
  tasks.reserve(5);

  int fibNum;
  int chainLength = 10;
  int matrixSize = 3;

  // Task 1: Read Fibonacci number
  auto &get_fibNum = tasks.emplace_back([&] {
    std::cout << "Enter a Fibonacci number: ";
    std::cin >> fibNum;
  });

  // Task 2: Calculate Fibonacci
  auto &calc_fib = tasks.emplace_back([&] {
    fibNum = Fibonacci_Scheduling(thread_pool, fibNum);
    std::cout << "Fibonacci number: " << fibNum << std::endl;
  });

  // Task 3: Linear Chain
  auto &linear_chain = tasks.emplace_back([&] {
    std::cout << "\nRunning Linear Chain:\n";
    Linear_Chain_Scheduling(chainLength);
  });

  // Task 4: Matrix Multiplication
  auto &matrix_mul = tasks.emplace_back([&] {
    std::cout << "\nRunning Matrix Multiplication:\n";
    std::vector a(matrixSize, std::vector<int>(matrixSize));
    std::vector b(matrixSize, std::vector<int>(matrixSize));
    std::vector c(matrixSize, std::vector<int>(matrixSize));
    Matrix_Mul_Scheduling(matrixSize, a, b, c);
  });

  // Task 5: Final output
  auto &final_output = tasks.emplace_back(
      [&] { std::cout << "\nAll computations completed!\n"; });

  // Set up task dependencies:
  // 1. Fibonacci calculation depends on input
  calc_fib.Succeed(&get_fibNum);

  // 2. Linear chain runs after Fibonacci
  linear_chain.Succeed(&calc_fib);

  // 3. Matrix multiplication runs after linear chain
  matrix_mul.Succeed(&linear_chain);

  // 4. Final output runs after everything
  final_output.Succeed(&matrix_mul);

  // Submit all tasks
  thread_pool.Submit(tasks);
  thread_pool.Wait();
  return 0;
}