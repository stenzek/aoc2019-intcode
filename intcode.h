#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace Intcode {
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using s64 = std::int64_t;

using MemoryCellType = s64;
using CodeVector = std::vector<MemoryCellType>;

enum : u32
{
  MAX_OPERANDS_PER_INSTRUCTION = 3
};

enum class Opcode : u8
{
  add = 1,
  mul = 2,
  in = 3,
  out = 4,
  jnz = 5,
  jz = 6,
  slt = 7,
  seq = 8,
  rbaddr = 9,
  halt = 99
};

u32 GetNumOperandsForOpcode(Opcode opcode);

enum class OperandMode : u8
{
  Positional = 0,
  Immediate = 1,
  Relative = 2,
  None = 255
};

struct Instruction
{
  Opcode opcode;
  std::array<OperandMode, MAX_OPERANDS_PER_INSTRUCTION> operand_modes;
  std::array<MemoryCellType, MAX_OPERANDS_PER_INSTRUCTION> operand_values;

  std::string Disassemble() const;
};

CodeVector ParseCode(std::string_view code_string);
CodeVector ParseCodeFromFile(const char* filename);

class Computer
{
public:
  enum class State : u32
  {
    Paused,
    Executing,
    Halted,
    WaitingForInput,
    WaitingForOutput
  };

  Computer(const CodeVector& code, u32 memory_size = 16384);
  ~Computer();

  u32 GetPC() const { return m_pc; }
  s64 GetRelativeBase() const { return m_relative_base; }
  State GetState() const { return m_state; }

  MemoryCellType ReadMemory(u32 address) const { return m_memory.at(address); }
  void WriteMemory(u32 address, MemoryCellType value) { m_memory.at(address) = value; }

  void Reset();
  State Run(int num_instructions = -1);

  void SetInput(MemoryCellType value);
  MemoryCellType GetOutput();

private:
  bool IsValidAddress(MemoryCellType address) const;

  void FetchInstruction(Instruction* instr);
  void ExecuteInstruction(const Instruction& instr);

  MemoryCellType ReadOperand(const Instruction& instr, u32 index) const;
  void WriteOperand(const Instruction& instr, u32 index, MemoryCellType value);

  std::vector<MemoryCellType> m_memory;
  std::vector<MemoryCellType> m_orginal_code;

  u32 m_pc = 0;
  s64 m_relative_base = 0;
  State m_state = State::Paused;

  MemoryCellType m_input = 0;
  MemoryCellType m_output = 0;
  bool m_has_input = false;
  bool m_has_output = false;
};

void RunProgramAndPrintOutput(const char* progname, const CodeVector& code, const CodeVector& input);

} // namespace Intcode