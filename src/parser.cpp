#include "parser.h"
#include <unordered_set>
#include <iostream>

namespace parser {
  std::vector<Command> COMMANDS;
  State CURRENT = {Mode::EXPECTING, std::unordered_set<FlagType>(), true, false, true};

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

  bool path(std::string_view token) {
    if(flag(token)) return true;
    else {
      if(CURRENT.expectingPath) CURRENT.expectingPath = false;
      if(!CURRENT.atleastOnePathAcquired) CURRENT.atleastOnePathAcquired = true;
      if(CURRENT.newGroup) {
        COMMANDS.push_back({CURRENT.mode, std::vector<Target>(1,Target(token, CURRENT.flags))});
        CURRENT.newGroup = false;
      }
      else {
        COMMANDS.back().targets.push_back(Target(token, CURRENT.flags));
      }
      return true;
    }
  }

  bool flag(std::string_view token) {
    if(delimiter(token)) return true;
    else {
      if(token == "-r" || token == "--recurse") {
        CURRENT.flags.insert(FlagType::RECURSE);
        CURRENT.expectingPath = true;
        return true;
      }
      else if(token == "-u" || token == "--undo") {
        CURRENT.flags.insert(FlagType::UNDO);
        CURRENT.expectingPath = true;
        return true;
      }
      else if(token == "-l" || token == "--list") {
        CURRENT.flags.insert(FlagType::LIST);
        CURRENT.expectingPath = true;
        return true;
      }
      else if(token == "-q" || token == "--quarantine") {
        CURRENT.flags.insert(FlagType::QUARANTINE);
        CURRENT.expectingPath = true;
        return true;
      }
      else if(token == "-c" || token == "--cure") {
        CURRENT.flags.insert(FlagType::CURE);
        CURRENT.expectingPath = true;
        return true;
      }
      else return false;
    }
  }

  bool delimiter(std::string_view token) {
    if(mode(token)) return true;
    else {
      if(token != "...") return false;
      else {
        if(CURRENT.expectingPath) {
          if(CURRENT.atleastOnePathAcquired) {
            // To Do
            // Add warnings for stray flag which is assumed to be redundant according to our grammar
          }
          else {
            if(CURRENT.mode != Mode::EXPECTING || !CURRENT.flags.empty()) {
              COMMANDS.push_back({CURRENT.mode, std::vector<Target>(1,Target("", CURRENT.flags))});
            }
          }
        }
        CURRENT.mode = Mode::EXPECTING;
        CURRENT.expectingPath = true;
        CURRENT.atleastOnePathAcquired = false;
        CURRENT.flags.clear();
        CURRENT.newGroup = true;
        return true;
      }
    }
  }

  bool mode(std::string_view token) {
    if(CURRENT.mode != Mode::EXPECTING) return false;
    if(token == "-s" || token == "--scan") {
      CURRENT.mode = Mode::SCAN;
      return true;
    }
    else if(token == "-q" || token == "--quarantine") {
      CURRENT.mode = Mode::QUARANTINE;
      return true;
    }
    else if(token == "-c" || token == "--cure") {
      CURRENT.mode = Mode::CURE;
      return true;
    }
    else if(token == "-e" || token == "--exclude") {
      CURRENT.mode = Mode::EXCLUDE;
      return true;
    }
    else if(token == "-t" || token == "--trace") {
      CURRENT.mode = Mode::TRACE;
      return true;
    }
    else if(token == "-m" || token == "--monitor") {
      CURRENT.mode = Mode::MONITOR;
      return true;
    }
    else if(token == "-h" || token == "--help") {
      CURRENT.mode = Mode::HELP;
      return true;
    }
    else if(token == "-v" || token == "--version") {
      CURRENT.mode = Mode::VERSION;
      return true;
    }
    else return false;
  }

  void parse(int argc, char **argv) {
    for(int tokenIndex = 1; tokenIndex < argc; tokenIndex++) {
      path(argv[tokenIndex]);
    }
    // Finalize the last group
    if(CURRENT.expectingPath) {
      if(CURRENT.atleastOnePathAcquired) {
        // To Do
        // Add warnings for stray flag which is assumed to be redundant according to our grammar
      }
      else {
        if(CURRENT.mode != Mode::EXPECTING || !CURRENT.flags.empty()) {
          COMMANDS.push_back({CURRENT.mode, std::vector<Target>(1,Target("", CURRENT.flags))});
        }
      }
    }
    CURRENT.mode = Mode::EXPECTING;
    CURRENT.expectingPath = true;
    CURRENT.atleastOnePathAcquired = false;
    CURRENT.flags.clear();
    CURRENT.newGroup = true;

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
  }

}