#include "parser.h"
#include "main.h"


int main(int argc, char **argv) {

  parser::parse(argc, argv);
  parser::printCOMMANDS();
  parser::COMMANDS.clear();

  
  

}