#pragma once

#include "scheduling/scheduling.hpp"
#include <benchmark/benchmark.h>

// Benchmark function declarations
void BM_Fibonacci_Scheduling(benchmark::State &state);
void BM_Linear_Chain_Scheduling(benchmark::State &state);
void BM_Matrix_Mul_Scheduling(benchmark::State &state);