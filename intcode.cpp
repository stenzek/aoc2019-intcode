#include "intcode.h"
#include "scope_timer.h"
#include <cassert>
#include <cctype>
#include <cinttypes>
#include <cstdio>
#include <deque>
#include <fstream>
#include <sstream>

namespace Intcode {

u32 GetNumOperandsForOpcode(Opcode opcode)
{
  switch (opcode)
  {
    case Opcode::add:
      return 3;
    case Opcode::mul:
      return 3;
    case Opcode::in:
      return 2;
    case Opcode::out:
      return 1;
    case Opcode::jnz:
      return 2;
    case Opcode::jz:
      return 2;
    case Opcode::slt:
      return 3;
    case Opcode::seq:
      return 3;
    case Opcode::rbaddr:
      return 1;
    case Opcode::halt:
      return 0;
    default:
      assert(false && "unknown opcode");
      return 0;
  }
}

CodeVector ParseCode(std::string_view code_string)
{
  CodeVector ret;

  std::string buffer;
  for (auto it = code_string.begin(); it != code_string.end(); it++)
  {
    char ch = *it;
    if (ch == ',')
    {
      if (buffer.size() > 0)
      {
        ret.push_back(std::strtoll(buffer.c_str(), nullptr, 10));
        buffer.clear();
      }

      continue;
    }
    else if (!std::isdigit(ch) && ch != '-')
    {
      continue;
    }

    buffer.push_back(ch);
  }

  if (buffer.size() > 0)
    ret.push_back(std::strtoll(buffer.c_str(), nullptr, 10));

  return ret;
}

CodeVector ParseCodeFromFile(const char* filename)
{
  std::ifstream ifs(filename);
  if (!ifs.is_open())
    return {};

  ifs.seekg(0, SEEK_END);
  const auto size = ifs.tellg();
  ifs.seekg(0, SEEK_SET);

  std::string str;
  str.resize(size);
  ifs.read(str.data(), size);
  ifs.close();

  return ParseCode(str);
}

std::string Instruction::Disassemble() const
{
  std::ostringstream ss;
  switch (opcode)
  {
    case Opcode::add:
      ss << "add ";
      break;

    case Opcode::mul:
      ss << "mul ";
      break;

    case Opcode::in:
      ss << "in ";
      break;

    case Opcode::out:
      ss << "out ";
      break;

    case Opcode::jnz:
      ss << "jnz ";
      break;

    case Opcode::jz:
      ss << "jz ";
      break;

    case Opcode::slt:
      ss << "slt ";
      break;

    case Opcode::seq:
      ss << "seq ";
      break;

    case Opcode::rbaddr:
      ss << "rbaddr ";
      break;

    case Opcode::halt:
      ss << "halt";
      break;

    default:
      assert(false && "unknown opcode");
      break;
  }

  for (u32 i = 0; i < MAX_OPERANDS_PER_INSTRUCTION; i++)
  {
    if (operand_modes[i] == OperandMode::None)
      break;

    if (i > 0)
      ss << ", ";

    switch (operand_modes[i])
    {
      case OperandMode::Positional:
      {
        ss << "[" << operand_values[i] << "]";
      }
      break;

      case OperandMode::Immediate:
      {
        ss << "#" << operand_values[i];
      }
      break;

      case OperandMode::Relative:
      {
        if (operand_values[i] < 0)
          ss << "[rb - " << -operand_values[i] << "]";
        else
          ss << "[rb + " << operand_values[i] << "]";
      }
      break;

      default:
        break;
    }
  }

  return ss.str();
}

Computer::Computer(const CodeVector& code, u32 memory_size) : m_orginal_code(code), m_memory(memory_size)
{
  assert(!code.empty() && "has code to execute");
  assert(memory_size >= code.size() && "code size smaller than memory size");
  Reset();
}

Computer::~Computer() = default;

void Computer::Reset()
{
  std::fill(m_memory.begin(), m_memory.end(), 0);
  std::copy(m_orginal_code.begin(), m_orginal_code.end(), m_memory.begin());
  m_pc = 0;
  m_relative_base = 0;
  m_state = State::Paused;
}

Computer::State Computer::Run(int num_instructions /*= -1*/)
{
  assert(m_state != State::Halted);

  m_state = State::Executing;
  while (m_state == State::Executing)
  {
    Instruction instr;
    FetchInstruction(&instr);

    // std::printf("%u: %s\n", m_pc, instr.Disassemble().c_str());

    ExecuteInstruction(instr);

    if (num_instructions > 0)
    {
      num_instructions--;
      if (num_instructions == 0)
      {
        if (m_state == State::Executing)
          m_state = State::Paused;

        break;
      }
    }
  }

  return m_state;
}

void Computer::SetInput(MemoryCellType value)
{
  assert(!m_has_input);
  m_input = value;
  m_has_input = true;
}

MemoryCellType Computer::GetOutput()
{
  assert(m_has_output);
  MemoryCellType output = m_output;
  m_has_output = false;
  m_output = 0;
  return output;
}

bool Computer::IsValidAddress(MemoryCellType address) const
{
  return (address >= 0 && static_cast<size_t>(address) < m_memory.size());
}

void Computer::FetchInstruction(Instruction* instr)
{
  u32 new_pc = m_pc;

  const MemoryCellType first = ReadMemory(new_pc++);
  instr->opcode = static_cast<Opcode>(static_cast<u8>(first % 100));
  instr->operand_modes[0] = static_cast<OperandMode>(static_cast<u8>((first / 100) % 10));
  instr->operand_modes[1] = static_cast<OperandMode>(static_cast<u8>((first / 1000) % 10));
  instr->operand_modes[2] = static_cast<OperandMode>(static_cast<u8>((first / 10000) % 10));

  const u32 num_parameters = GetNumOperandsForOpcode(instr->opcode);
  for (u32 i = 0; i < num_parameters; i++)
    instr->operand_values[i] = ReadMemory(new_pc++);
  for (u32 i = num_parameters; i < MAX_OPERANDS_PER_INSTRUCTION; i++)
    instr->operand_modes[i] = OperandMode::None;
}

void Computer::ExecuteInstruction(const Instruction& instr)
{
  switch (instr.opcode)
  {
    case Opcode::add:
    {
      const MemoryCellType lhs = ReadOperand(instr, 0);
      const MemoryCellType rhs = ReadOperand(instr, 1);
      WriteOperand(instr, 2, lhs + rhs);
      m_pc += 4;
      return;
    }

    case Opcode::mul:
    {
      const MemoryCellType lhs = ReadOperand(instr, 0);
      const MemoryCellType rhs = ReadOperand(instr, 1);
      WriteOperand(instr, 2, lhs * rhs);
      m_pc += 4;
      return;
    }

    case Opcode::in:
    {
      if (!m_has_input)
      {
        // leave pc as-is so we re-execute after input is provided
        m_state = State::WaitingForInput;
        return;
      }

      WriteOperand(instr, 0, m_input);
      m_input = 0;
      m_has_input = false;
      m_pc += 2;
      return;
    }

    case Opcode::out:
    {
      if (m_has_output)
      {
        // leave pc as-is so we re-execute after consuming output
        m_state = State::WaitingForOutput;
        return;
      }

      // write output, increment pc
      m_output = ReadOperand(instr, 0);
      m_has_output = true;
      m_state = State::WaitingForOutput;
      m_pc += 2;
      return;
    }

    case Opcode::jnz:
    {
      const MemoryCellType value = ReadOperand(instr, 0);
      if (value != 0)
      {
        const MemoryCellType new_pc = ReadOperand(instr, 1);
        assert(new_pc >= 0 && "jumping to positive pc");
        m_pc = static_cast<u32>(new_pc);
      }
      else
      {
        // branch not taken
        m_pc += 3;
      }

      return;
    }

    case Opcode::jz:
    {
      const MemoryCellType value = ReadOperand(instr, 0);
      if (value == 0)
      {
        const MemoryCellType new_pc = ReadOperand(instr, 1);
        assert(new_pc >= 0 && "jumping to positive pc");
        m_pc = static_cast<u32>(new_pc);
      }
      else
      {
        // branch not taken
        m_pc += 3;
      }

      return;
    }

    case Opcode::slt:
    {
      const MemoryCellType lhs = ReadOperand(instr, 0);
      const MemoryCellType rhs = ReadOperand(instr, 1);
      WriteOperand(instr, 2, lhs < rhs ? 1 : 0);
      m_pc += 4;
      return;
    }

    case Opcode::seq:
    {
      const MemoryCellType lhs = ReadOperand(instr, 0);
      const MemoryCellType rhs = ReadOperand(instr, 1);
      WriteOperand(instr, 2, lhs == rhs ? 1 : 0);
      m_pc += 4;
      return;
    }

    case Opcode::rbaddr:
    {
      const MemoryCellType mod = ReadOperand(instr, 0);
      m_relative_base += mod;
      m_pc += 2;
      return;
    }

    case Opcode::halt:
    {
      m_state = State::Halted;
      m_pc++;
      return;
    }

    default:
    {
      assert(false && "Unknown opcode");
      return;
    }
  }
}

MemoryCellType Computer::ReadOperand(const Instruction& instr, u32 index) const
{
  switch (instr.operand_modes[index])
  {
    case OperandMode::Positional:
    {
      const MemoryCellType address = instr.operand_values[index];
      assert(IsValidAddress(address));
      return ReadMemory(static_cast<u32>(address));
    }

    case OperandMode::Immediate:
    {
      return instr.operand_values[index];
    }

    case OperandMode::Relative:
    {
      const MemoryCellType address = m_relative_base + instr.operand_values[index];
      assert(IsValidAddress(address));
      return ReadMemory(static_cast<u32>(address));
    }

    default:
    {
      assert(false && "Unknown operand type");
      return 0;
    }
  }
}

void Computer::WriteOperand(const Instruction& instr, u32 index, MemoryCellType value)
{
  switch (instr.operand_modes[index])
  {
    case OperandMode::Positional:
    {
      const MemoryCellType address = instr.operand_values[index];
      assert(IsValidAddress(address));
      WriteMemory(static_cast<u32>(address), value);
    }
    break;

    case OperandMode::Immediate:
    {
      assert(false && "immediate write operand");
    }
    break;

    case OperandMode::Relative:
    {
      const MemoryCellType address = m_relative_base + instr.operand_values[index];
      assert(IsValidAddress(address));
      WriteMemory(static_cast<u32>(address), value);
    }
    break;

    default:
    {
      assert(false && "Unknown operand type");
    }
    break;
  }
}

void RunProgramAndPrintOutput(const char* progname, const CodeVector& code, const CodeVector& input)
{
  ScopeTimer timer(progname);

  std::deque<MemoryCellType> input_queue;
  std::deque<MemoryCellType> output_queue;

  std::copy(input.begin(), input.end(), std::back_inserter(input_queue));

  Computer comp(code);

  Computer::State state;
  while ((state = comp.Run()) != Computer::State::Halted)
  {
    if (state == Computer::State::WaitingForInput)
    {
      if (input_queue.empty())
      {
        std::printf("%s: input requested and none available\n", progname);
        std::abort();
      }

      comp.SetInput(input_queue.front());
      input_queue.pop_front();
    }
    else if (state == Intcode::Computer::State::WaitingForOutput)
    {
      const MemoryCellType output = comp.GetOutput();
      // std::printf("%s: %" PRId64 "\n", progname, output);
      output_queue.push_back(output);
    }
  }

  timer.Print();

  {
    bool first = true;
    std::fprintf(stdout, "%s output: [", progname);
    for (MemoryCellType value : output_queue)
    {
      std::fprintf(stdout, "%s%" PRId64, first ? "" : ", ", value);
      first = false;
    }
    std::fprintf(stdout, "]\n");
  }
}

} // namespace Intcode