#include "error.h"
#include "globals.h"
#include <iostream>

namespace error {
  int WARN_COUNT = 0;
  int ERROR_COUNT = 0;

  std::vector<error::Diagnostic> DIAGNOSTICS;

  void error::Diagnostic::print() {
    std::string ogCmdText = "";
    for(int i = this -> commandStartPos; i < global::ARGC && std::string(global::ARGV[i]) != "..."; i++) (ogCmdText += global::ARGV[i], ogCmdText += " ");

    std::cout << ogCmdText << std:: endl;

    std::string indicatorStr(findWordIndex(this -> position + 1, ogCmdText), ' ');
    indicatorStr += "^^^";
    if(this -> relatedPosition != -1) {
      indicatorStr += std::string(findWordIndex(this -> relatedPosition + 1, ogCmdText) - indicatorStr.size(), ' ');
      indicatorStr += "^^^";
    }
    std::cout << indicatorStr << std::endl;
    std::cout << "ARG : " << global::ARGV[this -> commandStartPos + this -> position] << "  ";
    if(this -> relatedPosition != -1) std::cout << global::ARGV[this -> commandStartPos + this -> relatedPosition];
    std::cout << std::endl;

    switch (this -> category) {
      case Category::SYNTAX : 
        std::cout << "SYNTAX "; break;
      case Category::SEMANTIC :
        std::cout << "SEMANTIC "; break;
      case Category::PATH :
        std::cout << "PATH "; break; 
    }
    switch (this -> severity) {
      case Severity::WARNING :
        std::cout << "WARNING : " << this -> message; break;
      case Severity::ERROR :
        std::cout << "ERROR : " << this -> message; break;
    }
    std::cout << std::endl << std::endl;
  }

  int findWordIndex(int n, const std::string_view &str) {
    if(n == 1) return 0;
    int cummIndex = 0;
    for(int i = 1; i < n; i++) {
      cummIndex = str.find(' ', cummIndex + 1);
    }
    return cummIndex + 1;
  }
}