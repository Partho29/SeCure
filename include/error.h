#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace error {

  extern int WARN_COUNT;
  extern int ERROR_COUNT;

  struct Diagnostic;

  struct Diagnostic {
    enum class Severity {
        WARNING,
        ERROR
    };

    enum class Category {
        SYNTAX,
        SEMANTIC,
        PATH
    };

    enum class Code {
        MISSING_MODE,
        UNKNOWN_TOKEN,
        INVALID_PATH,
        INVALID_FLAG_FOR_MODE,
        UNDO_LIST_CONFLICT,
        TRAILING_FLAG,
        REDUNDANT_LIST,
        INVALID_MODE_POSITION,
        NO_ATTRIBUTE_MODE,
        SINGLETON_MODE,
        ATTRIBUTE_MODE
    };

    Severity severity;
    Category category;
    Code code;

    std::string message;

    int position;

    int relatedPosition;

    int commandIndex;
    int commandStartPos;

    void print();
  };

  extern std::vector<error::Diagnostic> DIAGNOSTICS;

  struct CommandDiagnosticInfo {
    int cmdIndex = 1;
    int cmdStartPos = 1;
    int modePos = -1;
    int rFlagPos = -1;
    int uFlagPos = -1;
    int lFlagPos = -1;
    int qFlagPos = -1;
    int cFlagPos = -1;
    std::vector<int> pathsPos;
  };

  int findWordIndex(int, const std::string_view&);
  
}