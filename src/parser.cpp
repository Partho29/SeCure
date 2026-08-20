#include "parser.h"
#include "globals.h"
#include "error.h"
#include <ostream>
#include <unordered_set>
#include <iostream>
#include <vector>

namespace parser {
  std::vector<Command> COMMANDS;
  State CURRENT = {Mode::EXPECTING, std::unordered_set<FlagType>(), true, false, true, error::CommandDiagnosticInfo()};

  Target::Target(std::string_view str, std::unordered_set<FlagType> _flags) : path(str), flags(std::move(_flags)) {}
  void Target::print() {
    std::cout << "Path : " << this -> path << std::endl;
    std::cout << "Flags : ";
    for(auto &fl : this -> flags) {
      switch(fl) {
        case FlagType::RECURSE :
          std::cout << "RECURSE "; break;
        case FlagType::CURE :
          std::cout << "CURE "; break;
        case FlagType::QUARANTINE :
          std::cout << "QUARANTINE "; break;
        case FlagType::UNDO :
          std::cout << "UNDO "; break;
        case FlagType::LIST :
          std::cout << "LIST "; break;
      }
    }
    std::cout << std::endl;
  }

  bool path(std::string_view token, const int &pos) {
    if(flag(token, pos)) return true;
    else {
      if(CURRENT.expectingPath) CURRENT.expectingPath = false;
      if(!CURRENT.atleastOnePathAcquired) CURRENT.atleastOnePathAcquired = true;
      if(CURRENT.newGroup) {
        CURRENT.cmdDiagnosticInfo.pathsPos.push_back(pos - CURRENT.cmdDiagnosticInfo.cmdStartPos);
        COMMANDS.push_back({CURRENT.mode, std::vector<Target>(1,Target(token, CURRENT.flags)), CURRENT.cmdDiagnosticInfo});
        CURRENT.newGroup = false;
      }
      else {
        COMMANDS.back().targets.push_back(Target(token, CURRENT.flags));
        CURRENT.cmdDiagnosticInfo.pathsPos.push_back(pos - CURRENT.cmdDiagnosticInfo.cmdStartPos);
        COMMANDS.back().cmdDiagnosticInfo = CURRENT.cmdDiagnosticInfo;
      }
      return true;
    }
  }

  bool flag(std::string_view token, const int &pos) {
    if(delimiter(token, pos)) return true;
    else {
      bool matched = false;
      if(token == "-r" || token == "--recurse") {
        if(!CURRENT.flags.count(FlagType::RECURSE)) {
          CURRENT.flags.insert(FlagType::RECURSE);
          CURRENT.cmdDiagnosticInfo.rFlagPos = pos - CURRENT.cmdDiagnosticInfo.cmdStartPos;
        }
        matched = true;
      }
      else if(token == "-u" || token == "--undo") {
        if(!CURRENT.flags.count(FlagType::UNDO)) {
          CURRENT.flags.insert(FlagType::UNDO);
          CURRENT.cmdDiagnosticInfo.uFlagPos = pos - CURRENT.cmdDiagnosticInfo.cmdStartPos;
        }
        matched = true;
      }
      else if(token == "-l" || token == "--list") {
        if(!CURRENT.flags.count(FlagType::LIST)) {
          CURRENT.flags.insert(FlagType::LIST);
          CURRENT.cmdDiagnosticInfo.lFlagPos = pos - CURRENT.cmdDiagnosticInfo.cmdStartPos;
        }
        matched = true;
      }
      else if(token == "-q" || token == "--quarantine") {
        if(!CURRENT.flags.count(FlagType::QUARANTINE)) {
          CURRENT.flags.insert(FlagType::QUARANTINE);
          CURRENT.cmdDiagnosticInfo.qFlagPos = pos - CURRENT.cmdDiagnosticInfo.cmdStartPos;
        }
        matched = true;
      }
      else if(token == "-c" || token == "--cure") {
        if(!CURRENT.flags.count(FlagType::CURE)) {
          CURRENT.flags.insert(FlagType::CURE);
          CURRENT.cmdDiagnosticInfo.cFlagPos = pos - CURRENT.cmdDiagnosticInfo.cmdStartPos;
        }
        matched = true;
      }
      if(!matched) return false;
      CURRENT.expectingPath = true;
      return true;
    }
  }

  bool delimiter(std::string_view token, const int &pos) {
    if(mode(token, pos)) return true;
    else {
      if(token != "...") return false;
      else {
        if(CURRENT.expectingPath) {
          if(CURRENT.atleastOnePathAcquired) {
            // Add warning for stray flag which is assumed to be redundant according to our grammar when the command is a pathful one
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::WARNING,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::TRAILING_FLAG,
              "Trailing flag is ignored for pathful commands.",
              pos - 1 - CURRENT.cmdDiagnosticInfo.cmdStartPos,
              -1,
              int(COMMANDS.size()),
              CURRENT.cmdDiagnosticInfo.cmdStartPos
            });
            error::WARN_COUNT++;
          }
          else {
            if(CURRENT.mode != Mode::EXPECTING || !CURRENT.flags.empty()) {
              CURRENT.cmdDiagnosticInfo.pathsPos.push_back(pos - CURRENT.cmdDiagnosticInfo.cmdStartPos);
              COMMANDS.push_back({CURRENT.mode, std::vector<Target>(1,Target("", CURRENT.flags)), CURRENT.cmdDiagnosticInfo});
            }
            // only in the case of mode being EXPECTING and flags being empty do we want to silently ignore it cuz it is an empty command.
            // In every other 3 cases, we push it and handle the errors in semantic analysis.
          }
              
        }
        CURRENT.cmdDiagnosticInfo.cmdStartPos = pos + 1;
        resetCurrent();
        return true;
      }
    }
  }

  bool mode(std::string_view token, const int &pos) {
    if(CURRENT.mode != Mode::EXPECTING) return false;
    bool matched = false;
    if(token == "-s" || token == "--scan") {
      CURRENT.mode = Mode::SCAN;
      matched = true;
    }
    else if(token == "-q" || token == "--quarantine") {
      CURRENT.mode = Mode::QUARANTINE;
      matched = true;
    }
    else if(token == "-c" || token == "--cure") {
      CURRENT.mode = Mode::CURE;
      matched = true;
    }
    else if(token == "-e" || token == "--exclude") {
      CURRENT.mode = Mode::EXCLUDE;
      matched = true;
    }
    else if(token == "-t" || token == "--trace") {
      CURRENT.mode = Mode::TRACE;
      matched = true;
    }
    else if(token == "-m" || token == "--monitor") {
      CURRENT.mode = Mode::MONITOR;
      matched = true;
    }
    else if(token == "-h" || token == "--help") {
      CURRENT.mode = Mode::HELP;
      matched = true;
    }
    else if(token == "-v" || token == "--version") {
      CURRENT.mode = Mode::VERSION;
      matched = true;
    }
    if(!matched) return false;
    CURRENT.cmdDiagnosticInfo.modePos = pos - CURRENT.cmdDiagnosticInfo.cmdStartPos;
    return true; 
  }

  void parse() {
    for(int tokenIndex = 1; tokenIndex < global::ARGC; tokenIndex++) {
      path(global::ARGV[tokenIndex], tokenIndex);
    }
    // Finalize the last group
    if(CURRENT.expectingPath) {
      if(CURRENT.atleastOnePathAcquired) {
        // Add warning for stray flag which is assumed to be redundant according to our grammar when the command is a pathful one
        error::DIAGNOSTICS.push_back({
          error::Diagnostic::Severity::WARNING,
          error::Diagnostic::Category::SYNTAX,
          error::Diagnostic::Code::TRAILING_FLAG,
          "Trailing flag is ignored for pathful commands.",
          global::ARGC - 1 - CURRENT.cmdDiagnosticInfo.cmdStartPos,
          -1,
          int(COMMANDS.size()),
          CURRENT.cmdDiagnosticInfo.cmdStartPos
        });
        error::WARN_COUNT++;
      }
      else {
        if(CURRENT.mode != Mode::EXPECTING || !CURRENT.flags.empty()) {
          CURRENT.cmdDiagnosticInfo.pathsPos.push_back(global::ARGC - CURRENT.cmdDiagnosticInfo.cmdStartPos);
          COMMANDS.push_back({CURRENT.mode, std::vector<Target>(1,Target("", CURRENT.flags)), CURRENT.cmdDiagnosticInfo});
        }
        // only in the case of mode being EXPECTING and flags being empty do we want to silently ignore it cuz it is an empty command.
        // In every other 3 cases, we push it and handle the errors in semantic analysis.
      }
    }
    
    CURRENT.cmdDiagnosticInfo.cmdStartPos = global::ARGC + 1;
    resetCurrent();

  }

  void parse(int argc, char **argv) {
    global::ARGC = argc;
    global::ARGV = argv;
    parser::parse();
  }

  std::string modeToStr(const Mode &mode) {
    switch(mode) {
      case Mode::SCAN :
        return "SCAN";
      case Mode::QUARANTINE :
        return "QUARANTINE";
      case Mode::CURE :
        return "CURE";
      case Mode::TRACE :
        return "TRACE";
      case Mode::EXCLUDE :
        return "EXCLUDE";
      case Mode::MONITOR :
        return "MONITOR";
      case Mode::VERSION :
        return "VERSION";
      case Mode::HELP :
        return "HELP";
      case Mode::EXPECTING :
        return "EXPECTING";
      default :
        return "UNEXPECTED : default";
    }
  }
  

  void printCOMMANDS() {
    int i = 0;
    for(auto &cmd : COMMANDS) {
      std::cout << "Command " << i++ << std::endl;
      std::cout << "Mode : " << modeToStr(cmd.mode) << std::endl;
      std::cout << "Targets : " << std::endl;
      for(auto &target : cmd.targets) {
        target.print();
        std::cout << std::endl;
      }
    }

    // std::cout << std::endl << std::endl << error::DIAGNOSTICS.size() << std::endl;
  }

  void resetCurrent() {
    CURRENT.mode = Mode::EXPECTING;
    CURRENT.expectingPath = true;
    CURRENT.atleastOnePathAcquired = false;
    CURRENT.flags.clear();
    CURRENT.newGroup = true;
    CURRENT.cmdDiagnosticInfo.cmdIndex++;
    CURRENT.cmdDiagnosticInfo.cFlagPos = -1;
    CURRENT.cmdDiagnosticInfo.lFlagPos = -1;
    CURRENT.cmdDiagnosticInfo.qFlagPos = -1;
    CURRENT.cmdDiagnosticInfo.rFlagPos = -1;
    CURRENT.cmdDiagnosticInfo.uFlagPos = -1;
    CURRENT.cmdDiagnosticInfo.pathsPos.clear();
  }

}