#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::cout;
using std::ifstream;
using std::istringstream;
using std::string;
using std::vector;
using std::abs;

enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};

constexpr int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};


vector<State> ParseLine(string line) {
    istringstream sline(line);
    int n;
    char c;
    vector<State> row;
    while (sline >> n >> c && c == ',') {
      if (n == 0) {
        row.push_back(State::kEmpty);
      } else {
        row.push_back(State::kObstacle);
      }
    }
    return row;
}


vector<vector<State>> ReadBoardFile(string path) {
  ifstream myfile (path);
  vector<vector<State>> board{};
  if (myfile) {
    string line;
    while (getline(myfile, line)) {
      vector<State> row = ParseLine(line);
      board.push_back(row);
    }
  }
  return board;
}

bool Compare(const vector<int>& node1, const vector<int>& node2)
{
    return ((node1[2] + node1[3]) > (node2[2] + node2[3]));
}

void CellSort(vector<vector<int>>& open)
{
    std::sort(open.begin(), open.end(), Compare);
}

int Heuristic(int x1, int y1, int x2, int y2)
{
    return abs(x2 - x1) + abs(y2 - y1);
}

bool CheckValidCell(int x, int y, const vector<vector<State>>& board)
{
    bool xValid = (x < board.size() && x >= 0);
    bool yValid = (y < board[0].size() && y >= 0);
    return xValid && yValid && (board[x][y] == State::kEmpty);
}

void AddToOpen(int x, int y, int g, int h, vector<vector<int>>& open, vector<vector<State>>& board)
{
    open.push_back({x, y, g, h});
    board[x][y] = State::kClosed;
}

void ExpandNeighbours(const vector<int>& currentNode, int goal[2], vector<vector<int>>& open, vector<vector<State>>& board)
{
   const int& currentX = currentNode[0];
   const int& currentY = currentNode[1];
   const int& g = currentNode[2];

   for (const int* dir : delta)
   {
       int neighbourX = currentX + dir[0];
       int neighbourY = currentY + dir[1];
       if (CheckValidCell(neighbourX, neighbourY, board))
       {
           int h = Heuristic(neighbourX, neighbourY, goal[0], goal[1]);
           AddToOpen(neighbourX, neighbourY, g + 1, h, open, board);
       }
   } 
}
vector<vector<State>> Search(vector<vector<State>> board, int init[2], int goal[2])
{
    vector<vector<int>> open {};
    AddToOpen(init[0], init[1], 0, Heuristic(init[0], init[1], goal[0], goal[1]), open, board);
    while (!open.empty())
    {
        CellSort(open);
        vector<int> currentNode = open.back();
        open.pop_back();
        int currentX = currentNode[0];
        int currentY = currentNode[1];
        board[currentX][currentY] = State::kPath;
        if (currentX == goal[0] && currentY == goal[1])
        {
            board[init[0]][init[1]] = State::kStart;
            board[goal[0]][goal[1]] = State::kFinish;
            return board;
        }
        ExpandNeighbours(currentNode, goal, open, board);
    }
    std::cout << "No path found";
    return vector<vector<State>> {};
}


string CellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗   ";
    case State::kFinish: return "🏁   ";
    case State::kStart: return "🚦   ";
    default: return "0   "; 
  }
}


void PrintBoard(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << CellString(board[i][j]);
    }
    cout << "\n";
  }
}


int main() {
    int init[2] = {0, 0};
    int goal[2] = {4, 5};
    vector<vector<State>> board = ReadBoardFile("1.board");
    vector<vector<State>> solution = Search(board, init, goal);
    PrintBoard(solution);
}