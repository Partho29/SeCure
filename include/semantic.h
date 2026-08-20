#pragma once
#include "parser.h"
#include <string_view>


namespace semantic {
  bool analysis();
  bool validateGrammar();
  bool validateAllPaths();
  bool validatePath(const std::string_view, std::string&, const parser::Mode&);
}