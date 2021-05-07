# PegboardSolver
A progam to find a solution for the Pegboard given an initial Pegboard state. The state space in which the solution might be found can be traversed in a breadth-first, depth-first, greedy, or A-Star manner.

## Requirements
C++11

## Usage
./peg [inputfile] [bfs, dfs, greedy, astar]

## Compile

```bash
g++ -o peg -std=c++11 pegBoardSolver.cpp
```

## Run/Execute

```bash
./peg input.txt bfs
```
