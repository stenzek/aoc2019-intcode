#include "intcode.h"
#include <map>
#include <utility>

using Coordinates = std::pair<int, int>;
enum class Colour
{
  Black,
  White
};
std::map<Coordinates, Colour> pained_coordinates;

enum class Direction
{
  Up,
  Right,
  Down,
  Left
};

Colour GetColour(const Coordinates& c)
{
  auto it = pained_coordinates.find(c);
  if (it == pained_coordinates.end())
    return Colour::Black;

  return it->second;
}

void SetColour(const Coordinates& c, Colour colour)
{
  printf("painting color %d,%d %s\n", c.first, c.second, colour == Colour::Black ? "black" : "white");
  pained_coordinates[c] = colour;
}

int main(int argc, char* argv[])
{
  auto code = Intcode::ParseCodeFromFile("day11-input.txt");

  Coordinates pos{};
  Direction dir = Direction::Up;
  bool waiting_for_dir = false;

  // p2
  int min_x = 0;
  int max_x = 0;
  int min_y = 0;
  int max_y = 0;
  SetColour(pos, Colour::White);

  Intcode::Computer comp(code);

  Intcode::Computer::State state;
  while ((state = comp.Run()) != Intcode::Computer::State::Halted)
  {
    if (state == Intcode::Computer::State::WaitingForInput)
    {
      printf("query (%d,%d)\n", pos.first, pos.second);
      comp.SetInput(GetColour(pos) == Colour::Black ? 0 : 1);
    }
    else if (state == Intcode::Computer::State::WaitingForOutput)
    {
      const auto output = comp.GetOutput();
      if (!waiting_for_dir)
      {
        // giving us the colour to paint
        SetColour(pos, output == 0 ? Colour::Black : Colour::White);
        waiting_for_dir = true;
      }
      else
      {
        printf("turn %d\n", (int)output);
        if (output == 0)
        {
          // turn left
          switch (dir)
          {
            case Direction::Up:
              dir = Direction::Right;
              break;
            case Direction::Right:
              dir = Direction::Down;
              break;
            case Direction::Down:
              dir = Direction::Left;
              break;
            case Direction::Left:
              dir = Direction::Up;
              break;
            default:
              break;
          }
        }
        else
        {
          // turn right
          switch (dir)
          {
            case Direction::Up:
              dir = Direction::Left;
              break;
            case Direction::Right:
              dir = Direction::Up;
              break;
            case Direction::Down:
              dir = Direction::Right;
              break;
            case Direction::Left:
              dir = Direction::Down;
              break;
            default:
              break;
          }
        }

        // move forward
        switch (dir)
        {
          case Direction::Up:
            pos.second -= 1;
            break;
          case Direction::Right:
            pos.first += 1;
            break;
          case Direction::Down:
            pos.second += 1;
            break;
          case Direction::Left:
            pos.first -= 1;
            break;
          default:
            break;
        }
        printf("now at %d,%d\n", pos.first, pos.second);
        waiting_for_dir = false;
        min_x = std::min(min_x, pos.first);
        min_y = std::min(min_y, pos.second);
        max_x = std::max(max_x, pos.first);
        max_y = std::max(max_y, pos.second);
      }
    }
  }

  printf("%zu\n", pained_coordinates.size());

  for (int y = min_y; y <= max_y; y++)
  {
    for (int x = max_x; x >= min_x; x--)
      printf("%c", GetColour(Coordinates(x, y)) == Colour::White ? '*' : ' ');
    printf("\n");
  }

  return 0;
}