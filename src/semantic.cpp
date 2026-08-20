#include "semantic.h"
#include "parser.h"
#include <error.h>
#include <filesystem>
#include <iostream>

namespace semantic {
  bool analysis() {
    if(!validateGrammar() || !validateAllPaths()) {
      int pastCommandIndex = -1;
      for(auto &e : error::DIAGNOSTICS) {
        if(e.severity == error::Diagnostic::Severity::ERROR) {
          if(e.commandIndex != pastCommandIndex) {
            std::cout << "COMMAND " << e.commandIndex << std::endl;
            pastCommandIndex = e.commandIndex;
          }
          e.print();
          pastCommandIndex = e.commandIndex;
        }
      }
    }
    if(error::WARN_COUNT) {
      int pastCommandIndex = -1;
      for(auto &e : error::DIAGNOSTICS) {
        if(e.severity == error::Diagnostic::Severity::WARNING) {
          if(e.commandIndex != pastCommandIndex) {
            std::cout << "COMMAND " << e.commandIndex << std::endl;
            pastCommandIndex = e.commandIndex;
          }
          e.print();
        }
      }
    }

    parser::printCOMMANDS();
    return !error::ERROR_COUNT;
  }

  bool validateGrammar() {
    for(auto &command : parser::COMMANDS) {
      switch(command.mode) {
        case parser::Mode::EXPECTING : {
          // To Do
          // Provide error : Missing Mode
          error::DIAGNOSTICS.push_back({
            error::Diagnostic::Severity::ERROR,
            error::Diagnostic::Category::SYNTAX,
            error::Diagnostic::Code::MISSING_MODE,
            "The mode is missing.",
            0,
            -1,
            command.cmdDiagnosticInfo.cmdIndex,
            command.cmdDiagnosticInfo.cmdStartPos,
          });
          error::ERROR_COUNT++;
          break;
        }
        case parser::Mode::HELP : {
          // More than ne commands present and one of them is a HELP command
          if(parser::COMMANDS.size() != 1) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::SINGLETON_MODE,
              "The command cannot be used alongside other commands.",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Arguments given to a HELP command
          if(command.targets.size() != 1 || !command.targets[0].flags.empty()) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::NO_ATTRIBUTE_MODE,
              "The command does not take any arguments.",
              command.cmdDiagnosticInfo.modePos,
              (command.cmdDiagnosticInfo.modePos == 0) ? 1 : 0,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          break;
        }
        case parser::Mode::VERSION : {
          // More than ne commands present and one of them is a VERSION command
          if(parser::COMMANDS.size() != 1) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::SINGLETON_MODE,
              "The command cannot be used alongside other commands.",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Arguments given to a VERSION command
          if(command.targets.size() != 1 || !command.targets[0].flags.empty()) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::NO_ATTRIBUTE_MODE,
              "The command does not take any arguments.",
              command.cmdDiagnosticInfo.modePos,
              (command.cmdDiagnosticInfo.modePos == 0) ? 1 : 0,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          break;
        }
        case parser::Mode::MONITOR : {
          // More than ne commands present and one of them is a MONITOR command
          if(parser::COMMANDS.size() != 1) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::SINGLETON_MODE,
              "The command cannot be used alongside other commands.",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Arguments given to a MONITOR command
          if(command.targets.size() != 1 || !command.targets[0].flags.empty()) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::NO_ATTRIBUTE_MODE,
              "The command does not take any arguments.",
              command.cmdDiagnosticInfo.modePos,
              (command.cmdDiagnosticInfo.modePos == 0) ? 1 : 0,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          break;
        }
        case parser::Mode::SCAN : {
          // Mode token not in position
          if(command.cmdDiagnosticInfo.modePos != 0) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::INVALID_MODE_POSITION,
              "The mode position should be at the start of each command",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Non-compatible argument given
          if(command.cmdDiagnosticInfo.lFlagPos != -1 || command.cmdDiagnosticInfo.uFlagPos != -1) {
            int flagPos;
            if(command.cmdDiagnosticInfo.lFlagPos == -1) flagPos = command.cmdDiagnosticInfo.uFlagPos;
            else if(command.cmdDiagnosticInfo.uFlagPos == -1) flagPos = command.cmdDiagnosticInfo.lFlagPos;
            else flagPos = std::min(command.cmdDiagnosticInfo.lFlagPos, command.cmdDiagnosticInfo.uFlagPos);

            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::INVALID_FLAG_FOR_MODE,
              "The flag is incompatible",
              flagPos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          break;
        }
        case parser::Mode::QUARANTINE : {
          // No argument given
          if(command.targets.size() == 1 && command.targets[0].flags.empty() && command.targets[0].path.empty()) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::ATTRIBUTE_MODE,
              "The mode requires arguments",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Mode token not in position
          if(command.cmdDiagnosticInfo.modePos != 0) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::INVALID_MODE_POSITION,
              "The mode position should be at the start of each command",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Non-compatible argument given
          if(command.cmdDiagnosticInfo.cFlagPos != -1 || command.cmdDiagnosticInfo.qFlagPos != -1) {
            int flagPos;
            if(command.cmdDiagnosticInfo.cFlagPos == -1) flagPos = command.cmdDiagnosticInfo.qFlagPos;
            else if(command.cmdDiagnosticInfo.qFlagPos == -1) flagPos = command.cmdDiagnosticInfo.cFlagPos;
            else flagPos = std::min(command.cmdDiagnosticInfo.cFlagPos, command.cmdDiagnosticInfo.qFlagPos);

            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::INVALID_FLAG_FOR_MODE,
              "The flag is incompatible",
              flagPos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // --undo & --list both present
          if(command.cmdDiagnosticInfo.lFlagPos != -1 && command.cmdDiagnosticInfo.uFlagPos != -1) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::UNDO_LIST_CONFLICT,
              "--undo and --list cannot be together in the same command",
              std::min(command.cmdDiagnosticInfo.uFlagPos, command.cmdDiagnosticInfo.lFlagPos),
              std::max(command.cmdDiagnosticInfo.uFlagPos, command.cmdDiagnosticInfo.lFlagPos),
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          break;
        }
        case parser::Mode::CURE : {
          // No argument given
          if(command.targets.size() == 1 && command.targets[0].flags.empty() && command.targets[0].path.empty()) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::ATTRIBUTE_MODE,
              "The mode requires arguments",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Mode token not in position
          if(command.cmdDiagnosticInfo.modePos != 0) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::INVALID_MODE_POSITION,
              "The mode position should be at the start of each command",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Non-compatible argument given
          if(command.cmdDiagnosticInfo.cFlagPos != -1 || command.cmdDiagnosticInfo.qFlagPos != -1) {
            int flagPos;
            if(command.cmdDiagnosticInfo.cFlagPos == -1) flagPos = command.cmdDiagnosticInfo.qFlagPos;
            else if(command.cmdDiagnosticInfo.qFlagPos == -1) flagPos = command.cmdDiagnosticInfo.cFlagPos;
            else flagPos = std::min(command.cmdDiagnosticInfo.cFlagPos, command.cmdDiagnosticInfo.qFlagPos);

            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::INVALID_FLAG_FOR_MODE,
              "The flag is incompatible",
              flagPos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // --undo & --list both present
          if(command.cmdDiagnosticInfo.lFlagPos != -1 && command.cmdDiagnosticInfo.uFlagPos != -1) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::UNDO_LIST_CONFLICT,
              "--undo and --list cannot be together in the same command",
              std::min(command.cmdDiagnosticInfo.uFlagPos, command.cmdDiagnosticInfo.lFlagPos),
              std::max(command.cmdDiagnosticInfo.uFlagPos, command.cmdDiagnosticInfo.lFlagPos),
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          break;
        }
        case parser::Mode::EXCLUDE : {
          // No argument given
          if(command.targets.size() == 1 && command.targets[0].flags.empty() && command.targets[0].path.empty()) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::ATTRIBUTE_MODE,
              "The mode requires arguments",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Mode token not in position
          if(command.cmdDiagnosticInfo.modePos != 0) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::INVALID_MODE_POSITION,
              "The mode position should be at the start of each command",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Non-compatible argument given
          if(command.cmdDiagnosticInfo.cFlagPos != -1 || command.cmdDiagnosticInfo.qFlagPos != -1) {
            int flagPos;
            if(command.cmdDiagnosticInfo.cFlagPos == -1) flagPos = command.cmdDiagnosticInfo.qFlagPos;
            else if(command.cmdDiagnosticInfo.qFlagPos == -1) flagPos = command.cmdDiagnosticInfo.cFlagPos;
            else flagPos = std::min(command.cmdDiagnosticInfo.cFlagPos, command.cmdDiagnosticInfo.qFlagPos);

            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::INVALID_FLAG_FOR_MODE,
              "The flag is incompatible",
              flagPos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // --undo & --list both present
          if(command.cmdDiagnosticInfo.lFlagPos != -1 && command.cmdDiagnosticInfo.uFlagPos != -1) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::UNDO_LIST_CONFLICT,
              "--undo and --list cannot be together in the same command",
              std::min(command.cmdDiagnosticInfo.uFlagPos, command.cmdDiagnosticInfo.lFlagPos),
              std::max(command.cmdDiagnosticInfo.uFlagPos, command.cmdDiagnosticInfo.lFlagPos),
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          break;
        }
        case parser::Mode::TRACE : {
          // Mode token not in position
          if(command.cmdDiagnosticInfo.modePos != 0) {
            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SYNTAX,
              error::Diagnostic::Code::INVALID_MODE_POSITION,
              "The mode position should be at the start of each command",
              command.cmdDiagnosticInfo.modePos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // Non-compatible argument given
          if(command.cmdDiagnosticInfo.cFlagPos != -1 || command.cmdDiagnosticInfo.qFlagPos != -1) {
            int flagPos;
            if(command.cmdDiagnosticInfo.cFlagPos == -1) flagPos = command.cmdDiagnosticInfo.qFlagPos;
            else if(command.cmdDiagnosticInfo.qFlagPos == -1) flagPos = command.cmdDiagnosticInfo.cFlagPos;
            else flagPos = std::min(command.cmdDiagnosticInfo.cFlagPos, command.cmdDiagnosticInfo.qFlagPos);

            error::DIAGNOSTICS.push_back({
              error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::INVALID_FLAG_FOR_MODE,
              "The flag is incompatible",
              flagPos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
            });
            error::ERROR_COUNT++;
          }
          // --list flag given
          if(command.cmdDiagnosticInfo.lFlagPos != -1) {
            // -u and -l both given
            if(command.cmdDiagnosticInfo.uFlagPos != -1) {
              error::DIAGNOSTICS.push_back({
                error::Diagnostic::Severity::ERROR,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::UNDO_LIST_CONFLICT,
              "--undo and --list cannot be together in the same command",
              std::min(command.cmdDiagnosticInfo.uFlagPos, command.cmdDiagnosticInfo.lFlagPos),
              std::max(command.cmdDiagnosticInfo.uFlagPos, command.cmdDiagnosticInfo.lFlagPos),
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
              });
              error::ERROR_COUNT++;
            }
            else {
              error::DIAGNOSTICS.push_back({
                error::Diagnostic::Severity::WARNING,
              error::Diagnostic::Category::SEMANTIC,
              error::Diagnostic::Code::REDUNDANT_LIST,
              "--list is redundant with --trace",
              command.cmdDiagnosticInfo.lFlagPos,
              -1,
              command.cmdDiagnosticInfo.cmdIndex,
              command.cmdDiagnosticInfo.cmdStartPos
              });
              error::WARN_COUNT++;
            }
          }
          break;
        }
      }
    }
    return !error::ERROR_COUNT;
  }

  bool validateAllPaths() {
    bool incorrectPathsPresent = false;
    for(auto &command : parser::COMMANDS) {
      size_t i = 0;
      for(auto &target : command.targets) {
        if(target.path.size() && target.path[0] == '-') {
          error::DIAGNOSTICS.push_back({
            error::Diagnostic::Severity::ERROR,
            error::Diagnostic::Category::SEMANTIC,
            error::Diagnostic::Code::UNKNOWN_TOKEN,
            "Unknown flag",
            command.cmdDiagnosticInfo.pathsPos[i],
            -1,
            command.cmdDiagnosticInfo.cmdIndex,
            command.cmdDiagnosticInfo.cmdStartPos
          });
          error::ERROR_COUNT++;
          incorrectPathsPresent = true;
        }
        else if(!validatePath(target.path, target.resolvedPath, command.mode)) {
          error::DIAGNOSTICS.push_back({
            error::Diagnostic::Severity::ERROR,
            error::Diagnostic::Category::PATH,
            error::Diagnostic::Code::INVALID_PATH,
            "Invalid path",
            command.cmdDiagnosticInfo.pathsPos[i],
            -1,
            command.cmdDiagnosticInfo.cmdIndex,
            command.cmdDiagnosticInfo.cmdStartPos
          });
          error::ERROR_COUNT++;
          incorrectPathsPresent = true;
        }
        i++;
      }
    }
    return !incorrectPathsPresent;
  }

  bool validatePath(const std::string_view path, std::string &resolvedPath, const parser::Mode &mode) {
    if(path.empty()) {
      if(mode == parser::Mode::SCAN) resolvedPath = std::filesystem::current_path().string();
      return true;
    }
    else {
      if(std::filesystem::exists(path)) {
        resolvedPath = std::filesystem::absolute(path).string();
        return true;
      }
      else return false;
    }
  }
}