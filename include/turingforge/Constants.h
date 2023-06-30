#pragma once

#include <unordered_map>
#include <string>
#include <any>

constexpr int MAX_DEGREE = 2;
constexpr int BATCH_DIM = 1;
constexpr int FEATURE_DIM = 0;
using RecordType = std::unordered_map<std::string, std::any>;

using DataType = double;
using LossType = double;