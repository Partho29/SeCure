#include "parser.h"
#include "semantic.h"
#include "main.h"
#include "globals.h"
#include <error.h>

int main(int argc, char **argv) {
  global::ARGC = argc;
  global::ARGV = argv;
  parser::parse();
  //parser::printCOMMANDS();
  if(semantic::analysis()) {/* Run execution engine*/}
  return 0;
}