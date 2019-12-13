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
Coordinates paddle_position;
Coordinates ball_position;
int score;

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
  //printf("painting color %d,%d %d\n", c.first, c.second, (int)colour);
  pained_coordinates[c] = static_cast<TileType>(colour);
}

void Draw()
{
  int min_x = 999999;
  int max_x = -999999;
  int min_y = 999999;
  int max_y = -999999;

  for (const auto& it : pained_coordinates)
  {
    const auto& pos = it.first;
    min_x = std::min(min_x, pos.first);
    min_y = std::min(min_y, pos.second);
    max_x = std::max(max_x, pos.first);
    max_y = std::max(max_y, pos.second);
  }

  for (int y = min_y; y <= max_y; y++)
  {
    for (int x = max_x; x >= min_x; x--)
    {
      TileType t = GetTile(Coordinates(x, y));
      if (t == TileType::Ball)
      {
        //putchar('o');
        ball_position = Coordinates(x, y);
      }
      else if (t == TileType::Block)
      {
        //putchar('#');
      }
      else if (t == TileType::HorPaddle)
      {
        //putchar('-');
        paddle_position = Coordinates(x, y);
      }
      else if (t == TileType::Wall)
      {
        //putchar('*');
      }
      else
      {
        //putchar(' ');
      }
    }
      
    printf("\n");
  }
}

int main(int argc, char* argv[])
{
  auto code = Intcode::ParseCodeFromFile("day13-input.txt");

  int tstate = 0;
  Coordinates pos{};

  Intcode::Computer comp(code);
  comp.WriteMemory(0, 2);

  Intcode::Computer::State state;
  while ((state = comp.Run()) != Intcode::Computer::State::Halted)
  {
    if (state == Intcode::Computer::State::WaitingForInput)
    {
      Draw();

      int val;
#if 0
      int ch = getchar();
      if (ch == 'a' || ch == 'A')
        val = -1;
      else if (ch == 'd' || ch == 'D')
        val = 1;
      else
        val = 0;
#else
      if (paddle_position.first < ball_position.first)
        val = 1;
      else if (paddle_position.first > ball_position.first)
        val = -1;
      else
        val = 0;

      comp.SetInput(val);
#endif
    }
    else if (state == Intcode::Computer::State::WaitingForOutput)
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
        if (pos.first == -1 && pos.second == 0)
        {
          score = (int)comp.GetOutput();
          printf("score: %d\n", score);
        }
        else
        {
          SetColour(pos, comp.GetOutput());
        }

        tstate = 0;
      }
    }
  }


  printf("score at end: %d\n", score);
  int count = 0;
  for (const auto& it : pained_coordinates)
  {
    if (it.second == TileType::Block)
      count++;
  }
  printf("blocks: %d\n", count);

  return 0;
}