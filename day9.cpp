#include "intcode.h"

int main(int argc, char* argv[])
{
  Intcode::RunProgramAndPrintOutput(
    "day9-example1", Intcode::ParseCode("109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99"), {});
  Intcode::RunProgramAndPrintOutput("day9-example2", Intcode::ParseCode("1102,34915192,34915192,7,4,7,99,0"), {});
  Intcode::RunProgramAndPrintOutput("day9-example3", Intcode::ParseCode("104,1125899906842624,99"), {});
  Intcode::RunProgramAndPrintOutput("day9-part1", Intcode::ParseCodeFromFile("day9-input.txt"), {1});
  Intcode::RunProgramAndPrintOutput("day9-part2", Intcode::ParseCodeFromFile("day9-input.txt"), {2});
  return 0;
}