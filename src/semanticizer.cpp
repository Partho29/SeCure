#include "semanticizer.h"
#include "parser.h"

namespace semantic {
  void analyze() {
    for(auto &command : parser::COMMANDS) {
      switch(command.mode) {
        case parser::Mode::EXPECTING : {
          // To Do
          // Provide error : Missing Mode
        }
        case parser::Mode::HELP : {
          
        }
        case parser::Mode::VERSION : {
          
        }
        case parser::Mode::MONITOR : {
          
        }
        case parser::Mode::SCAN : {
          
        }
        case parser::Mode::QUARANTINE : {
          
        }
        case parser::Mode::CURE : {
          
        }
        case parser::Mode::EXCLUDE : {
          
        }
        case parser::Mode::TRACE : {
          
        }
      }
    }
  }
}