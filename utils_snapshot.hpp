#pragma once
#include <string>

// calls "lsof -- command"
void call_lsof(const std::string& command);