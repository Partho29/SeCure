#pragma once

#include <error.h>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>

namespace parser {

  enum class Mode {
    EXPECTING,
    SCAN,
    QUARANTINE,
    CURE,
    EXCLUDE,
    TRACE,
    VERSION,
    HELP,
    MONITOR
  };
  

  enum class FlagType {
    RECURSE,
    UNDO,
    LIST,
    CURE,
    QUARANTINE
  };

  struct Target {
    std::string path;
    std::unordered_set<FlagType> flags;
    std::string resolvedPath = "";
    Target(std::string_view, std::unordered_set<FlagType>);
    void print();
  };

  struct State {
    Mode mode;
    std::unordered_set<FlagType> flags;
    bool expectingPath;
    bool atleastOnePathAcquired;
    bool newGroup;
    error::CommandDiagnosticInfo cmdDiagnosticInfo;
  };

  extern State CURRENT;



  struct Command {
    Mode mode;
    std::vector<Target> targets;
    error::CommandDiagnosticInfo cmdDiagnosticInfo;
  };

  extern std::vector<Command> COMMANDS;

  bool path(std::string_view, const int&);
  bool flag(std::string_view, const int&);
  bool mode(std::string_view, const int&);
  bool delimiter(std::string_view, const int&);

  std::string modeToStr(const Mode&);
  void printCOMMANDS();

  void resetCurrent();

  void parse();
  void parse(int, char**);
}