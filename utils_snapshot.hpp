#pragma once
#include <string>

// calls "lsof -- command"
std::string call_lsof(const std::string& command);