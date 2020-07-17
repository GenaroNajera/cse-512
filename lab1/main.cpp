/* Reads in input.txt. input.txt gives
   start and goal locations, one of four
   search algorithms (BFS, DFS, UCS, A*),
   and live and Sunday traffic information.
   Program uses given algorithm to find path
   from start location to goal location.
   Program writes path and accumulated time
   from start to each location in output.txt.
*/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <deque>
#include <stack>
#include <map>

using namespace std;

string GOAL;

// live traffic information
// a and b are locations
// c is time/cost it takes to go from a to b
class LiveRow {
public:
  LiveRow() {
    a = "";
	  b = "";
	  c = 999;
  }
  
  string a;
  string b;
  int c;
};

// information during traffic-free Sunday. heuristic
// a is location
// c is estimated time/cost it takes to go from a to goal location
class SundayRow {
public:
  SundayRow() {
    a = "";
    c = 999;
  }

  string a;
  int c;
};

// keeps track of current location, path, and cost
struct Node {
  string state;
  vector<string> path;
  vector<int> costs;
  int pathCost;
  int totalCost;
};

// checks if node is in frontier queue
bool inFrontier(Node n, deque<Node> q)
{
  Node t;
  for(int i = 0; i < q.size(); i++)
  {
    t = q[i];
    if(t.state == n.state)
      return true;
  }

  return false;
}

// checks if node is in frontier stack. used by DFS
bool inStackFrontier(Node n, stack<Node> s)
{
  Node t;
  for(int i = 0; i < s.size(); i++)
  {
    t = s.top();
    s.pop();
    if(t.state == n.state)
      return true;
  }

  return false;
}

//checks if node has been explored
bool inExplored(Node n, vector<Node>& v)
{
  Node t;
  for(int i = 0; i < v.size(); i++)
  {
    t = v[i];
    if(t.state == n.state)
      return true;
  }

  return false;
}

// creates child nodes
// child inherits path and costs from parent, then adds its own cost
// bool b used by A* to add heuristic to total path cost 
Node child_node(Node n, LiveRow lr, vector<SundayRow>& sr, bool b)
{
  Node child;
  child.state = lr.b;
  child.path = n.path;
  child.path.push_back(child.state);
  child.pathCost = n.pathCost + lr.c;
  child.totalCost = child.pathCost;
  child.costs = n.costs;
  child.costs.push_back(child.pathCost);

  if(b)
  {
    for(int i = 0; i < sr.size(); i++)
    {
      if(sr[i].a == child.state)
        child.totalCost = sr[i].c + child.totalCost;
    }
  }

  return child;
}

// returns total path cost of child node. used by UCS and A*
int child_totalCost(Node n, deque<Node> q)
{
  Node t;
  for(int i = 0; i < q.size(); i++)
  {
    t = q[i];
    if(t.state == n.state)
      return t.totalCost;
  }

  return 0;
}

// sorts frontier in ascending order by path cost. used by UCS and A*
deque<Node> sortfrontier(Node n, deque<Node> q, vector<Node>& v)
{
  int i = 0;
  deque<Node> f;

  if(n.totalCost < q[q.size()-1].totalCost)
  {
    while(inExplored(q[i], v))
    {
      f.push_back(q[i]);
      i++;
    }

    while(n.totalCost >= q[i].totalCost && i < q.size())
    {
      f.push_back(q[i]);
      i++;
    }
    f.push_back(n);

    for(i; i < q.size(); i++)
      f.push_back(q[i]);
  } else {
    q.push_back(n);
    return q;
  }

  return f;
}

// prints path and accumulated time from start to each location in output.txt
void printout(Node n)
{
  ofstream ofs;
  ofs.open("output.txt", std::ofstream::out | std::ofstream::trunc);

  for(int i = 0; i < n.path.size(); i++)
    ofs << n.path[i] << " " << n.costs[i] << endl;

  ofs.close();
}

// Breadth-first search
void bfs(Node n, vector<LiveRow>& lr, vector<SundayRow>& sr)
{
  Node node = n;
  node.costs.push_back(0);
  deque<Node> frontier;
  frontier.push_back(node);
  vector<Node> explored;
  int i = 0;

  while(!frontier.empty())
  {
    node = frontier[i];
    if(node.state == GOAL)
    {
      printout(node);
      return;
    }
    explored.push_back(node);

    for(int i = 0; i < lr.size(); i++)
    {
      if(lr[i].a == node.state)
      {
        Node child = child_node(node, lr[i], sr, false);
        if(!inFrontier(child, frontier) && !inExplored(child, explored))
          frontier.push_back(child);
      }
    }

    i++;
  }

  cout << "No solution \n";
}

// Depth-first search
void dfs(Node n, vector<LiveRow>& lr, vector<SundayRow>& sr)
{
  Node node = n;
  node.costs.push_back(0);
  stack<Node> frontier;
  frontier.push(node);
  vector<Node> explored;

  while(!frontier.empty())
  {
    node = frontier.top();
    frontier.pop();
    if(node.state == GOAL)
    {
      printout(node);
      return;
    }
    explored.push_back(node);

    for(int i = 0; i < lr.size(); i++)
    {
      if(lr[i].a == node.state)
      {
        Node child = child_node(node, lr[i], sr, false);
        if(!inStackFrontier(child, frontier) && !inExplored(child, explored))
          frontier.push(child);
      }
    }
  }

  cout << "No solution \n";
}

// Uniform cost search
void ucs(Node n, vector<LiveRow>& lr, vector<SundayRow>& sr)
{
  Node node = n;
  node.costs.push_back(0);
  deque<Node> frontier;
  frontier.push_back(node);
  vector<Node> explored;
  int i = 0;

  while(!frontier.empty())
  {
    node = frontier[i];
    if(node.state == GOAL)
    {
      printout(node);
      return;
    }
    explored.push_back(node);

    for(int i = 0; i < lr.size(); i++)
    {
      if(lr[i].a == node.state)
      {
        Node child = child_node(node, lr[i], sr, false);
        if(!inFrontier(child, frontier) && !inExplored(child, explored))
          frontier = sortfrontier(child, frontier, explored);
        else if(inFrontier(child, frontier) && (child.totalCost < child_totalCost(child, frontier)))
          frontier = sortfrontier(child, frontier, explored);
      }
    }

    i++;
  }

  cout << "No solution \n";
}

// A* search
void astar(Node n, vector<LiveRow>& lr, vector<SundayRow>& sr)
{
  Node node = n;
  node.costs.push_back(0);
  deque<Node> frontier;
  frontier.push_back(node);
  vector<Node> explored;
  int i = 0;

  while(!frontier.empty())
  {
    node = frontier[i];
    if(node.state == GOAL)
    {
      printout(node);
      return;
    }
    explored.push_back(node);

    for(int i = 0; i < lr.size(); i++)
    {
      if(lr[i].a == node.state)
      {
        Node child = child_node(node, lr[i], sr, true);
        if(!inFrontier(child, frontier) && !inExplored(child, explored))
          frontier = sortfrontier(child, frontier, explored);
        else if(inFrontier(child, frontier) && (child.totalCost < child_totalCost(child, frontier)))
          frontier = sortfrontier(child, frontier, explored);
      }
    }

    i++;
  }

  cout << "No solution \n";
}

int main()
{
  fstream in;
  string alg;
  int a;
  string start;
  int lines;
  string info;
  int cost;
  vector<LiveRow> liverow;
  vector<SundayRow> sundayrow;

  in.open("input.txt");

  in >> alg;
  in >> start;
  in >> GOAL;
  in >> lines;

  for(int i = 0; i < lines; i++)
  {
    LiveRow l;
    in >> info;
    l.a = info;
    in >> info;
    l.b = info;
    in >> cost;
    l.c = cost;
    liverow.push_back(l);
  }

  in >> lines;

  for(int i = 0; i < lines; i++)
  {
    SundayRow s;
    in >> info;
    s.a = info;
    in >> cost;
    s.c = cost;
    sundayrow.push_back(s);
  }

  in.close();

  Node node;
  node.state = start;
  node.path.push_back(start);
  node.pathCost = 0;
  node.totalCost = 0;

  map<string, int> algorithms = { {"BFS", 0}, {"DFS", 1}, {"UCS", 2}, {"A*", 3} };
  a = algorithms[alg];

  switch(a)
  {
    case 0:
      bfs(node, liverow, sundayrow);
      break;
    case 1:
      dfs(node, liverow, sundayrow);
      break;
    case 2:
      ucs(node, liverow, sundayrow);
      break;
    case 3:
      astar(node, liverow, sundayrow);
      break;
    default:
      break;
  }

  return 0;
}
