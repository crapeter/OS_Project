#pragma once

#include "scheduling/scheduling.hpp"

// Function declarations
int Fibonacci_Scheduling(scheduling::ThreadPool &thread_pool, int n);
void Linear_Chain_Scheduling(int n);
void Matrix_Mul_Scheduling(const int n, std::vector<std::vector<int>> &a,
                           std::vector<std::vector<int>> &b,
                           std::vector<std::vector<int>> &c);