/* Reads in input.txt. input.txt gives board size,
   one of two algorithms (minimax or alpha-beta pruning),
   player (X or O), depth limit, and value and state of each square.
   Program uses given algorithm to find best move to make (stake or raid).
   Stake is placing game piece on unoccupied square. Raid is placing
   game piece on unoccupied square adjacent to square occupied
   by player, then adjacent enemy pieces become player's pieces (conquered).
   Adjacent means vertical or horizontal, not diagonal. Score is difference
   between sum of values of all squares occupied by player and
   sum of values of all squares occupied by enemy. Program prints
   column, row, move, and board after making move in output.txt.
*/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

using namespace std;

ofstream ofs;

string PLAYER;
string ENEMY;

class Board {
public:
  Board() {
    index = -1;
    move = "";
    dir = "";
    temp = -999;
    temp2 = -999;
  }

  vector<int> values; // value of each square
  vector<string> states; // which square is occupied by which player
  int index; // square on board that achieves highest score
  string move; // best move player can make to achieve highest score. either Stake or Raid
  string dir; // direction to raid to achieve highest score
  int temp; // highest score player has achieved. used in raid function
  int temp2; // highest score player has achieved. used in minimax and alphabeta
};

// returns current score of player
int calculateScore(Board b)
{
  int score = 0;
  int enemyScore = 0;
  for(int i = 0; i < b.states.size(); i++)
  {
    if(b.states[i] == PLAYER)
      score += b.values[i];
    if(b.states[i] != PLAYER && b.states[i] != ".")
      enemyScore += b.values[i];
  }

  return score - enemyScore;
}

// conquers adjacent squares
Board conquer(Board b, string player, int i, int j)
{
  if(i - 1 >= 0 && (i % j) != 0 && b.states[i - 1] != player && b.states[i - 1] != ".") // conquers square left of raided square
    b.states[i - 1] = player;
  if(i + 1 < b.states.size() && ((i + 1) % j) != 0 && b.states[i + 1] != player && b.states[i + 1] != ".") // conquers square right of raided square
    b.states[i + 1] = player;
  if(i - j >= 0 && b.states[i - j] != player && b.states[i - j] != ".") // conquers square above raided square
    b.states[i - j] = player;
  if(i + j < b.states.size() && b.states[i + j] != player && b.states[i + j] != ".") // conquers square below raided square
    b.states[i + j] = player;

  return b;
}

int minimax(Board& b, int depth, int depthLimit, bool isMax, string player);
int alphabeta(Board& b, int depth, int depthLimit, bool isMax, string player, int al, int bt);

// raids adjacent squares
int raid(Board& b, int depth, int depthLimit, bool isMax, string player, int i, bool ab, int al, int bt)
{
  int j = sqrt(b.states.size());
  int l = i - 1;
  int r = i + 1;
  int u = i - j;
  int d = i + j;
  if(isMax)
  {
    int value = -999;
    if(l >= 0 && (i % j) != 0 && b.states[l] == ".") // raids to the left
    {
      Board save = b;
      b.states[l] = player;
      b = conquer(b, player, l, j);
      if(ab)
        value = max(value, alphabeta(b, depth + 1, depthLimit, false, ENEMY, al, bt));
      else
        value = max(value, minimax(b, depth + 1, depthLimit, false, ENEMY));
      b = save;
      if(value > b.temp)
      {
        b.temp = value;
        b.dir = "l";
      }
    }
    if(r < b.states.size() && ((i + 1) % j) != 0 && b.states[r] == ".") // raids to the right
    {
      Board save = b;
      b.states[r] = player;
      b = conquer(b, player, r, j);
      if(ab)
        value = max(value, alphabeta(b, depth + 1, depthLimit, false, ENEMY, al, bt));
      else
        value = max(value, minimax(b, depth + 1, depthLimit, false, ENEMY));
      b = save;
      if(value > b.temp)
      {
        b.temp = value;
        b.dir = "r";
      }
    }
    if(u >= 0 && b.states[u] == ".") // raids above
    {
      Board save = b;
      b.states[u] = player;
      b = conquer(b, player, u, j);
      if(ab)
        value = max(value, alphabeta(b, depth + 1, depthLimit, false, ENEMY, al, bt));
      else
        value = max(value, minimax(b, depth + 1, depthLimit, false, ENEMY));
      b = save;
      if(value > b.temp)
      {
        b.temp = value;
        b.dir = "u";
      }
    }
    if(d < b.states.size() && b.states[d] == ".") // raids below
    {
      Board save = b;
      b.states[d] = player;
      b = conquer(b, player, d, j);
      if(ab)
        value = max(value, alphabeta(b, depth + 1, depthLimit, false, ENEMY, al, bt));
      else
        value = max(value, minimax(b, depth + 1, depthLimit, false, ENEMY));
      b = save;
      if(value > b.temp)
      {
        b.temp = value;
        b.dir = "d";
      }
    }
    return value;
  } else {
    int value = 999;
    if(l >= 0 && (i % j) != 0 && b.states[l] == ".") // raids to the left
    {
      Board save = b;
      b.states[l] = player;
      b = conquer(b, player, l, j);
      if(ab)
        value = min(value, alphabeta(b, depth + 1, depthLimit, true, PLAYER, al, bt));
      else
        value = min(value, minimax(b, depth + 1, depthLimit, true, PLAYER));
      b = save;
    }
    if(r < b.states.size() && ((i + 1) % j) != 0 && b.states[r] == ".") // raids to the right
    {
      Board save = b;
      b.states[r] = player;
      b = conquer(b, player, r, j);
      if(ab)
        value = min(value, alphabeta(b, depth + 1, depthLimit, true, PLAYER, al, bt));
      else
        value = min(value, minimax(b, depth + 1, depthLimit, true, PLAYER));
      b = save;
    }
    if(u >= 0 && b.states[u] == ".") // raids above
    {
      Board save = b;
      b.states[u] = player;
      b = conquer(b, player, u, j);
      if(ab)
        value = min(value, alphabeta(b, depth + 1, depthLimit, true, PLAYER, al, bt));
      else
        value = min(value, minimax(b, depth + 1, depthLimit, true, PLAYER));
      b = save;
    }
    if(d < b.states.size() && b.states[d] == ".") // raids below
    {
      Board save = b;
      b.states[d] = player;
      b = conquer(b, player, d, j);
      if(ab)
        value = min(value, alphabeta(b, depth + 1, depthLimit, true, PLAYER, al, bt));
      else
        value = min(value, minimax(b, depth + 1, depthLimit, true, PLAYER));
      b = save;
    }
    return value;
  }
  return 0;
}

// checks if every square is occupied
bool terminalstate(Board b)
{
  for(int i = 0; i < b.states.size(); i++)
  {
    if(b.states[i] == ".") return false;
  }

  return true;
}

// prints the board
void printboard(Board b)
{
  int j = sqrt(b.states.size());
  for(int i = 0; i < b.states.size(); i++)
  {
    if(i % j == 0) ofs << endl;
    ofs << b.states[i];
  }
  ofs << endl;
}

// minimax
// compares scores achieved from stake and raid
int minimax(Board& b, int depth, int depthLimit, bool isMax, string player)
{
  if(depth >= depthLimit || terminalstate(b))
    return calculateScore(b);

  if(isMax)
  {
    int value = -999;
    int temp = -999;
    int stakevalue = -999;
    int raidvalue = -999;
    for(int i = 0; i < b.states.size(); i++)
    {
      if(b.states[i] == ".")
      {
        b.states[i] = player;
        stakevalue = max(stakevalue, minimax(b, depth + 1, depthLimit, false, ENEMY));
        b.states[i] = ".";
      }
      if(b.states[i] == player)
        raidvalue = max(raidvalue, raid(b, depth, depthLimit, isMax, player, i, false, 0, 0));

      if(stakevalue > raidvalue)
      {
        value = max(value, stakevalue);
        if(value > b.temp2)
        {
          b.move = "Stake";
          b.temp2 = value;
          b.index = i;
        }
      } else if(raidvalue > stakevalue) {
        value = max(value, raidvalue);
        if(value > b.temp2)
        {
          b.move = "Raid";
          b.temp2 = value;
          b.index = i;
        }
      }
    }
    return value;
  } else {
    int value = 999;
    int stakevalue = 999;
    int raidvalue = 999;
    for(int i = 0; i < b.states.size(); i++)
    {
      if(b.states[i] == ".")
      {
        b.states[i] = player;
        stakevalue = min(stakevalue, minimax(b, depth + 1, depthLimit, true, PLAYER));
        b.states[i] = ".";
      }
      if(b.states[i] == player)
        raidvalue = min(raidvalue, raid(b, depth, depthLimit, isMax, player, i, false, 0, 0));

      if(stakevalue < raidvalue)
        value = min(value, stakevalue);
      else if(raidvalue < stakevalue)
        value = min(value, raidvalue);
    }
    return value;
  }
  return 0;
}

// alpha-beta pruning
// compares scores achieved from stake and raid
int alphabeta(Board& b, int depth, int depthLimit, bool isMax, string player, int al, int bt)
{
  if(depth >= depthLimit || terminalstate(b))
    return calculateScore(b);

  if(isMax)
  {
    int value = -999;
    int temp = -999;
    int stakevalue = -999;
    int raidvalue = -999;
    for(int i = 0; i < b.states.size(); i++)
    {
      if(b.states[i] == ".")
      {
        b.states[i] = player;
        stakevalue = max(stakevalue, alphabeta(b, depth + 1, depthLimit, false, ENEMY, al, bt));
        b.states[i] = ".";
      }
      if(b.states[i] == player)
        raidvalue = max(raidvalue, raid(b, depth, depthLimit, isMax, player, i, true, al, bt));

      if(stakevalue > raidvalue)
      {
        value = max(value, stakevalue);
        if(value > b.temp2)
        {
          b.move = "Stake";
          b.temp2 = value;
          b.index = i;
        }
      } else if(raidvalue > stakevalue) {
        value = max(value, raidvalue);
        if(value > b.temp2)
        {
          b.move = "Raid";
          b.temp2 = value;
          b.index = i;
        }
      }

      if(value >= bt) return value;
      al = max(al, value);
    }
    return value;
  } else {
    int value = 999;
    int stakevalue = 999;
    int raidvalue = 999;
    for(int i = 0; i < b.states.size(); i++)
    {
      if(b.states[i] == ".")
      {
        b.states[i] = player;
        stakevalue = min(stakevalue, alphabeta(b, depth + 1, depthLimit, true, PLAYER, al, bt));
        b.states[i] = ".";
      }
      if(b.states[i] == player)
        raidvalue = min(raidvalue, raid(b, depth, depthLimit, isMax, player, i, true, al, bt));

      if(stakevalue < raidvalue)
        value = min(value, stakevalue);
      else if(raidvalue < stakevalue)
        value = min(value, raidvalue);

      if(value <= al) return value;
      bt = min(bt, value);
    }
    return value;
  }
  return 0;
}

int main()
{
  ofs.open("output.txt", std::ofstream::out | std::ofstream::trunc);
  fstream in;
  int n;
  string alg;
  int depthLimit;
  int value;
  string state;
  Board board;

  in.open("input.txt");

  in >> n;
  in >> alg;
  in >> PLAYER;

  if(PLAYER == "O")
    ENEMY = "X";
  else
    ENEMY = "O";

  in >> depthLimit;

  for(int i = 0; i < n*n; i++)
  {
    in >> value;
    board.values.push_back(value);
  }

  for(int i = 0; i < n; i++)
  {
    in >> state;
    for(int j = 0; j < n; j++)
    {
      string temp = state.substr(j, 1);
      board.states.push_back(temp);
    }
  }

  in.close();

  if(alg == "MINIMAX")
    minimax(board, 0, depthLimit, true, PLAYER);
  else
    alphabeta(board, 0, depthLimit, true, PLAYER, -999, 999);

  char c = '@';
  c += (board.index % n) + 1; // column

  if(board.move == "Stake")
  {
    ofs << c << (board.index / n) + 1 << " " << board.move;
    board.states[board.index] = PLAYER;
    printboard(board);
  }
  else
  {
    map<string, int> direction = { {"l", 0}, {"r", 1}, {"u", 2}, {"d", 3} };
    int d = direction[board.dir];
    int j = sqrt(board.states.size());

    switch(d)
    {
      case 0:
        c--;
        ofs << c << (board.index / n) + 1 << " " << board.move;
        board.states[board.index - 1] = PLAYER;
        board = conquer(board, PLAYER, (board.index - 1), j);
        printboard(board);
        break;
      case 1:
        c++;
        ofs << c << (board.index / n) + 1 << " " << board.move;
        board.states[board.index + 1] = PLAYER;
        board = conquer(board, PLAYER, (board.index + 1), j);
        printboard(board);
        break;
      case 2:
        ofs << c << (board.index / n) << " " << board.move;
        board.states[board.index - j] = PLAYER;
        board = conquer(board, PLAYER, (board.index - j), j);
        printboard(board);
        break;
      case 3:
        ofs << c << (board.index / n) + 2 << " " << board.move;
        board.states[board.index + j] = PLAYER;
        board = conquer(board, PLAYER, (board.index + j), j);
        printboard(board);
        break;
      default:
        break;
    }
  }

  ofs.close();

  return 0;
}
