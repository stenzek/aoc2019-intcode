#include "intcode.h"
#include <map>
#include <utility>

using Coordinates = std::pair<int, int>;
enum class TileType
{
  Empty = 0,
  Wall = 1,
  Block =2,
  HorPaddle =3,
  Ball = 4
};
std::map<Coordinates, TileType> pained_coordinates;

enum class Direction
{
  Up,
  Right,
  Down,
  Left
};

TileType GetTile(const Coordinates& c)
{
  auto it = pained_coordinates.find(c);
  if (it == pained_coordinates.end())
    return TileType::Empty;

  return it->second;
}

void SetColour(const Coordinates& c, Intcode::MemoryCellType colour)
{
  printf("painting color %d,%d %d\n", c.first, c.second, (int)colour);
  pained_coordinates[c] = static_cast<TileType>(colour);
}

int main(int argc, char* argv[])
{
  auto code = Intcode::ParseCodeFromFile("day13-input.txt");

  int tstate = 0;
  Coordinates pos{};

  Intcode::Computer comp(code);

  Intcode::Computer::State state;
  while ((state = comp.Run()) != Intcode::Computer::State::Halted)
  {
    if (state == Intcode::Computer::State::WaitingForOutput)
    {
      if (tstate == 0)
      {
        pos.first = (int)comp.GetOutput();
        tstate = 1;
      }
      else if (tstate == 1)
      {
        pos.second = (int)comp.GetOutput();
        tstate = 2;
      }
      else
      {
        SetColour(pos, comp.GetOutput());
        tstate = 0;
      }
    }
  }

  int count = 0;
  for (const auto& it : pained_coordinates) {
    if (it.second == TileType::Block)
      count++;
  }
  printf("%d\n", count);

  return 0;
}