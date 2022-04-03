#pragma once

#include <variant>
#include "lox/callable/class.h"
#include "lox/callable/function.h"

namespace lox {

using Callable = std::variant<std::monostate, Class, Function>;

}
