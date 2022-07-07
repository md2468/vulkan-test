#pragma once

#include "diagnostic/critical_error.hpp"


#define A_ASSERT(expression) static_cast<bool>(expression) ? void() : void(A::critical_error("assertion failed: ", #expression))
