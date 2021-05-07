#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <map>
#include <cmath>
#include <algorithm>
#include <utility>
#include <time.h>

using namespace std;

// struct to represent a state and parent state
struct PBState {

		int rows;
		int cols;
		int** bState;
		PBState* pState;
		int hVal;
		int pCVal;

		PBState(int** bstate, PBState* pstate, int r, int c, int p) : bState(bstate), 
		pState(pstate), rows(r), cols(c), pCVal(p) {

			this->hVal = computeHeuristic(r, c);

		}
		/**
		* To string for displaying a state
		*
		**/
		string toString() {

			ostringstream os;
			for (int i = 0; i < rows; i++) {

				for (int j = 0; j < cols; j++)
					os << bState[i][j];
				os << "\n";
			}
			//cout << endl;

			return os.str();
		}

		/**
		* Heurisitc function is based on euclidean distance
		*
		**/
		int computeHeuristic(int r, int c) {

			int centerX = (rows / 2);
			int centerY = (c / 2);
			int sum = 0;
			for (int i = 0; i < rows; i++) {

				for (int j = 0; j < cols; j++) {

					if (bState[i][j] == 1) {

						sum += sqrt(pow(centerX - i, 2) + pow(centerY - j, 2));

					}
				}
			}
			return sum;
		}

		~PBState() {

			for (int i = 0; i < rows; i++) {

				delete[] this->bState[i];

			}

		}
		
};

// code instpired by https://www.geeksforgeeks.org/stl-priority-queue-for-structure-or-class/
/**
 * Struct for comparing heuristic values
 *
 **/
struct CompareHVal {

	bool operator()(PBState* pbs1, PBState* pbs2) {

		return (pbs1->hVal < pbs2->hVal);

	}

};

/**
 * Struct for comparing heuristic values + postCost
 *
 **/
struct ComparePCVal {

	bool operator()(PBState* pbs1, PBState* pbs2) {

		return ((pbs1->hVal + pbs1->pCVal) < pbs2->hVal + pbs2->pCVal);

	}

};

// globals
int numRows;
int numCols;

// helper functions declarations
void getGridBounds(string state);
int** construct2DArray(int rows, int cols);
void setStateEqualParent(int** current, int** parent);
int** stringTo2DArray(string);
list<PBState*> successorFunc(PBState*);
bool goalStateTest(PBState* state);
PBState* searchBFS(PBState*, map<string, PBState*>&);
PBState* searchDFS(PBState*, map<string, PBState*>&);
PBState* searchGDY(PBState*, map<string, PBState*>&);
PBState* searchASTR(PBState*, map<string, PBState*>&);
list<PBState*> getSolutionSet(PBState*);

int main(int argc, char** argv) {

	string searchType = "bfs";

	// checking input file was passed
	if (argc < 2) {

		perror("pass init state");
		exit(-1);

	}
	// checking search algorithm was choosen
	else if (argc < 3) {

		perror("must select search algorithm, defaulting to BFS");

	}
	else {

		searchType = argv[2];

	}

	// create a visited map to prevent re-visiting already visited states
    map<string, PBState*> visited;

	// read file initial state
    ifstream t(argv[1]);
    string input((std::istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
	getGridBounds(input);

	int** arrayState2D = stringTo2DArray(input);
    PBState* pbs = new PBState(arrayState2D, nullptr, numRows, numCols, 0);

	// return 0;
	PBState* goalState = nullptr;

	// initialize clock to compute compute time
	clock_t tInit = clock();

	if (searchType == "bfs") {

		goalState = searchBFS(pbs, visited);

	}
	else if (searchType == "dfs") {

		goalState = searchDFS(pbs, visited);

	}
	else if (searchType == "greedy") {

		goalState = searchGDY(pbs, visited);

	}
	else if (searchType == "astar") {

		goalState = searchASTR(pbs, visited);

	}

	// checking that goal was found
	if (goalState != nullptr) {

		cout << "found goal in (" << (double)(clock() - tInit) / (CLOCKS_PER_SEC) << ") seconds \n" 
		<< goalState->toString() << endl;
		cout << "solution:" << endl;

		list<PBState*> solutionSet = getSolutionSet(goalState);
		for (PBState* item : solutionSet) {

			cout << item->toString() << endl;

		}

	}
	else {

		cout << "No solution found";

	}

	// freeing heap memory
	for (auto it = visited.begin(); it != visited.end(); it++) {

		delete it->second;

	}

	return 0;
}

/**
 * Params: iState is the initial board state
 *			visited is the map that keeps track of visited states
 * function performs A* search for goal state
 **/
PBState* searchASTR(PBState* iState, map<string, PBState*>& visited) {

	priority_queue<PBState*, vector<PBState*>, ComparePCVal> fringe;
    fringe.push(iState);

    int i = 0;
    while (!fringe.empty()) {

        PBState* current = fringe.top();

        if (goalStateTest(current))
            return current;

		visited.insert({current->toString(), current});

        fringe.pop();

        list<PBState*> generatedStates = successorFunc(current);
        for (PBState* item : generatedStates) {

			// checking if item has been visited before adding to fringe
            if (!visited.count(item->toString())) {

                fringe.push(item);

            }

        }

        i++;
    }

    return nullptr;
}

/**
 * Params: iState is the initial board state
 *          visited is the map that keeps track of visited states
 * function performs greedy search for goal state
 **/
PBState* searchGDY(PBState* iState, map<string, PBState*>& visited) {

	priority_queue<PBState*, vector<PBState*>, CompareHVal> fringe;
	fringe.push(iState);

    int i = 0;
    while (!fringe.empty()) {

        PBState* current = fringe.top();

        if (goalStateTest(current))
            return current;

		visited.insert({current->toString(), current});

        fringe.pop();

        list<PBState*> generatedStates = successorFunc(current);
        for (PBState* item : generatedStates) {

			// checking if item has been visited before adding to fringe
            if (!visited.count(item->toString())) {

                fringe.push(item);

            }

        }

        i++;
    }

    return nullptr;


}

/**
 * Params: iState is the initial board state
 *          visited is the map that keeps track of visited states
 * function performs BFS search for goal state
 **/
PBState* searchBFS(PBState* iState, map<string, PBState*>& visited) {

	queue<PBState*> fringe;
	fringe.push(iState);

	int i = 0;
	while (!fringe.empty()) {

        PBState* current = fringe.front();

		if (goalStateTest(current)) 
			return current;

		visited.insert({current->toString(), current});

		fringe.pop();

		list<PBState*> generatedStates = successorFunc(current);
		for (PBState* item : generatedStates) {

			// checking if item has been visited before adding to fringe
			if (!visited.count(item->toString())) {

				fringe.push(item);

			}

		}

		i++;
    }

	return nullptr;
}

/**
 * Params: iState is the initial board state
 *          visited is the map that keeps track of visited states
 * function performs DFS search for goal state
 **/
PBState* searchDFS(PBState* iState, map<string, PBState*>& visited) {

	stack<PBState*> fringe;
    fringe.push(iState);

    int i = 0;
    while (!fringe.empty()) {

        PBState* current = fringe.top();

        if (goalStateTest(current))
            return current;

		visited.insert({current->toString(), current});

		fringe.pop();

        list<PBState*> generatedStates = successorFunc(current);
        for (PBState* item : generatedStates) {

			// checking if item has been visited before adding to fringe
            if (!visited.count(item->toString())) {

                fringe.push(item);

            }

        }

        i++;
    }

	return nullptr;
}

/**
 * params: state is the state being checked for goal
 * returns true if state is a goal, otherwise false
 **/
bool goalStateTest(PBState* state) {

	int counter = 0;
	for (int i = 0; i < numRows; i++) {

		for (int j = 0; j < numCols; j++) {

			if (state->bState[i][j] == 1)
				counter++; // increment for every peg

		}

	}
	// check that only 1 peg exists
	return (counter == 1);
}

/**
 * Params: pState is the initial state passed
 * returns list of states that can be created from
 * pState
 **/
list<PBState*> successorFunc(PBState* pState) {

	// get current empty spaces on the board
	list<std::pair<int, int>> zeros;
	for (int i = 0; i < numRows; i++) {

		for (int j = 0; j < numCols; j++) {

			if (pState->bState[i][j] == 0) {

				zeros.push_back(std::make_pair(i,j));

			}

		}

	}

	list<PBState*> generatedList;
	PBState* toAdd;
	int** baseState = pState->bState;
	int** newState;
	for (auto pair : zeros) {

		// check north
		if (pair.first - 2 >= 0 &&
			baseState[pair.first - 1][pair.second] == 1 &&
			baseState[pair.first - 2][pair.second] == 1) {

			newState = construct2DArray(numRows, numCols);
        	setStateEqualParent(newState, baseState);

			newState[pair.first - 1][pair.second] = 0;
			newState[pair.first - 2][pair.second] = 0;
			newState[pair.first][pair.second] = 1;

			toAdd = new PBState(newState, pState, numRows, numCols, pState->pCVal + 1);

			generatedList.push_back(toAdd);

		}
		// check south
		if (pair.first + 2 < numRows &&
			baseState[pair.first + 1][pair.second] == 1 &&
			baseState[pair.first + 2][pair.second] == 1) {

			newState = construct2DArray(numRows, numCols);
        	setStateEqualParent(newState, pState->bState);

			newState[pair.first + 1][pair.second] = 0;
			newState[pair.first + 2][pair.second] = 0;
			newState[pair.first][pair.second] = 1;

			toAdd = new PBState(newState, pState, numRows, numCols, pState->pCVal + 1);

            generatedList.push_back(toAdd);

		}
		// check west
		if (pair.second - 2 >= 0 &&
			baseState[pair.first][pair.second - 1] == 1 &&
			baseState[pair.first][pair.second - 2] == 1) {

			newState = construct2DArray(numRows, numCols);
            setStateEqualParent(newState, pState->bState);

			newState[pair.first][pair.second - 1] = 0;
            newState[pair.first][pair.second - 2] = 0;
			newState[pair.first][pair.second] = 1;

			toAdd = new PBState(newState, pState, numRows, numCols, pState->pCVal + 1);

            generatedList.push_back(toAdd);

		}
		// check east
		if (pair.second + 2 < numCols &&
			baseState[pair.first][pair.second + 1] == 1 &&
			baseState[pair.first][pair.second + 2] == 1) {

			newState = construct2DArray(numRows, numCols);
        	setStateEqualParent(newState, pState->bState);

			newState[pair.first][pair.second + 1] = 0;
			newState[pair.first][pair.second + 2] = 0;
			newState[pair.first][pair.second] = 1;

			toAdd = new PBState(newState, pState, numRows, numCols, pState->pCVal + 1);

            generatedList.push_back(toAdd);

		}

	}

	return generatedList;
}

/**
 * params: goalState is the found goal
 * Returns a list of states required to have reached the goal
**/
list<PBState*> getSolutionSet(PBState* goalState) {

	list<PBState*> stateSet;
	while (goalState->pState != nullptr) {

		stateSet.push_front(goalState);
		goalState = goalState->pState;

	}
	stateSet.push_front(goalState);

	return stateSet;
}

/**
 * Params: state string read from file
 * function to get the gridDimensions
**/
void getGridBounds(string state) {

    int maxRows = 0;
    for (int i = 0; i < state.length(); i++) {

        if (state[i] == '\n') {

            numCols++;
            numRows = 0;
        }
        else {

            numRows++;

        }
        if (maxRows < numRows)
            maxRows = numRows;
        
    }
    numRows = maxRows;

}

/**
 * Params: rows is # of rows and cols is # of cols
 * returns a dynamically allocted array
**/
int** construct2DArray(int rows, int cols) {

	int** array = new int*[numRows];
    for (int i = 0; i < numRows; i++) {

        array[i] = new int[numCols];

    }

	return array;
}

/**
 * sets the passed state equal to the parent
**/
void setStateEqualParent(int** current, int** parent) {

	for (int i = 0; i < numRows; i++) {

		for (int j = 0; j < numCols; j++) {

			current[i][j] = parent[i][j];

		}

	}

}

/**
 * Params: state is the passed string
 * Returns a 2D array that is defined by the values from the string arg
**/
int** stringTo2DArray(string state) {

    state.erase(std::remove(state.begin(), state.end(), '\n'), state.end());
	int** array = construct2DArray(numRows, numCols);

    for (int i = 0; i < numRows; i++) {

        for (int j = 0; j < numCols; j++) {

            array[i][j] = state[(i * numRows) + j] - '0';

        }

    }

	return array;
}

