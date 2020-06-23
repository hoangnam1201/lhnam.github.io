#include <iostream>
#include <vector>
#include <time.h>
#include <random>
#include <fstream>
#include <iomanip> 
#include <string>
#include <algorithm>

using namespace std;

#define N 3

typedef vector<vector<int>> State;
//0 = success 1 = step 2 = time
typedef vector<double> Result;

State goal{ {0,1,2},{3,4,5},{6,7,8} };

double randomDouble(double start, double end) {
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(start, end);
	return dist(mt);
}
class Puzzle {
public:
	State state;

	Puzzle() {}
	Puzzle(State input)
	{
		state = input;
	}
	void print() {
		cout << "_____________" << endl;
		for (int i = 0; i < N; i++) {
			cout << "|";
			for (int j = 0; j < N; j++)
				cout << "_" << state[i][j] << "_|";
			cout << endl;
		}
	}
	int getCost() {
		//manhattan
		int cost = 0;

		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
			{
				if (state[i][j] == 0)
					continue;
				int x, y;
				for (x = 0; x < N; x++)
					for (y = 0; y < N; y++)
						if (state[i][j] == goal[x][y])
							goto out;
			out:
				cost += abs(x - i) + abs(y - j);
			}
		return cost;
	}
	vector<Puzzle> getSuccessors() {
		vector<Puzzle> successors;
		int i, j;
		for (i = 0; i < N; i++)
			for (j = 0; j < N; j++)
				if (state[i][j] == 0)
					goto out;
	out:
		//UP
		if (i != 0)
		{
			Puzzle upMove(state);
			swap(upMove.state[i][j], upMove.state[i - 1][j]);
			successors.push_back(upMove);
		}
		//DOWN
		if (i != N - 1)
		{
			Puzzle downMove(state);
			swap(downMove.state[i][j], downMove.state[i + 1][j]);
			successors.push_back(downMove);
		}
		//LEFT
		if (j != 0)
		{
			Puzzle leftMove(state);
			swap(leftMove.state[i][j], leftMove.state[i][j - 1]);
			successors.push_back(leftMove);
		}
		//RIGHT
		if (j != N - 1)
		{
			Puzzle rightMove(state);
			swap(rightMove.state[i][j], rightMove.state[i][j + 1]);
			successors.push_back(rightMove);
		}
		return successors;
	}
	Puzzle getBestSuccessor() {
		int minCost = 100;
		vector<Puzzle> candidates;
		vector<Puzzle> successors = getSuccessors();
		for (int i = 0; i < successors.size(); i++)
			if (successors[i].getCost() < minCost)
				minCost = successors[i].getCost();
		for (int i = 0; i < successors.size(); i++)
			if (successors[i].getCost() == minCost)
				candidates.push_back(successors[i]);
		return candidates[rand() % candidates.size()];
	}
	Puzzle randomSuccessor() {
		return getSuccessors()[rand() % getSuccessors().size()];
	}
	bool goalTest() {
		if (getCost() == 0)
			return true;
		return false;
	}
};
State randomState() {
	State random(N);
	vector<int> value{ 0,1,2,3,4,5,6,7,8 };
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++) {
			int randomIndex = rand() % value.size();
			random[i].push_back(value[randomIndex]);
			value.erase(value.begin() + randomIndex);
		}
	return random;
}
State randomInputState() {
	Puzzle randomPuzzle(goal);
	for (int i = 0; i < 150; i++)
		randomPuzzle = randomPuzzle.randomSuccessor();
	return randomPuzzle.state;
}
Result Steepest_HillClimbing(Puzzle puzzle) {
	Result result(2);
	int countStep = 0;
	Puzzle current = puzzle;
	while (true) {
		if (current.goalTest()) {
			result[0] = 1;
			result[1] = countStep;
			return result;
		}
		Puzzle neighbor = current.getBestSuccessor();
		if (neighbor.getCost() > current.getCost())
		{
			result[0] = 0;
			result[1] = 0;
			return result;
		}
		current = neighbor;
		countStep++;
	}
}
Result FirstChoice_HillClimbing(Puzzle puzzle) {
	Result result(2);
	Puzzle current = puzzle;
	int countStep = 0;
	double d = clock();
	for (int i = 0; i < 150; i++) {
		if (current.goalTest())
		{
			result[0] = 1;
			result[1] = countStep;
			return result;
		}
		int count = 0;
		Puzzle random;
		do {
			random = current.randomSuccessor();
			if (random.getCost() < current.getCost())
				break;
			count++;
			if (count == 20) {
				result[0] = 0;
				result[1] = 0;
				return result;
			}
		} while (true);
		current = random;
		countStep++;
	}
	result[0] = 0;
	result[1] = 0;
	return result;
}
Result RandomRestart_HillClimbing(Puzzle init) {
	Result result(2);
	Puzzle current = init;
	int countStep;
	double d = clock();
	for (int x = 0; x < 150; x++)
	{
		countStep = 0;
		while (true) {
			if (current.goalTest()) {
				result[0] = 1;
				result[1] = countStep;
				return result;
			}
			Puzzle neighbor = current.getBestSuccessor();
			if (neighbor.getCost() > current.getCost())
				break;
			current = neighbor;
			countStep++;
		}
		current.state = randomState();
	}
	result[0] = 0;
	result[1] = 0;
	return result;
}
double schedule(double TMax, double t) {
	double random = randomDouble(0.0, 1.0);
	double result = abs(random * (TMax - t));
	return result;
}
Result Simulated_Annealing(Puzzle init) {
	Result result(2);
	Puzzle current = init;
	int countStep = 0;
	double d = clock();
	double TMax = 2.0;
	double t = -1.0;
	while (true) {
		if (current.goalTest())
		{
			result[0] = 1;
			result[1] = countStep;
			return result;
		}
		t += 0.00001;
		double T = schedule(TMax, t);
		if (T <= 0.0001) {
			result[0] = 0;
			result[1] = 0;
			return result;
		}
		Puzzle randomNeighbor = current.randomSuccessor();
		int deltaE = current.getCost() - randomNeighbor.getCost();
		if (deltaE > 0) {
			current = randomNeighbor;
			countStep++;
		}
		else {
			double p = exp((float)deltaE / T);
			if (randomDouble(0.0, 1.0) < p) {
				current = randomNeighbor;
				countStep++;
			}
		}
	}
}
vector<Puzzle> createInitStates(int numberOfStates) {
	vector<Puzzle> state;
	for (int i = 0; i < numberOfStates; i++)
		state.push_back(Puzzle(randomInputState()));
	return state;
}
void board(int numberOfState, Result steepest, Result firstchoice, Result random, Result simulated) {
	string emptyvalue = "              |               |            |";
	cout << "+-------------------------------+--------------+---------------+------------+";
	cout << "\n|" << left << setw(31) << " Algorithm" << "|";
	cout << " Success Rate | Average Steps | Total Time |";
	cout << "\n+-------------------------------+--------------+---------------+------------+";
	cout << "\n|" << left << setw(31) << " Steepest-Ascent Hill-Climbing" << "|";
	if (steepest.empty())
		cout << emptyvalue;
	else {
		cout << left << setw(14) << " " + to_string((int)steepest[0]) + "/" + to_string(numberOfState) << "|";
		cout << " " << left << setw(14) << steepest[1] << "|";
		cout << " " << left << setw(11) << steepest[2] << "|";
	}
	cout << "\n+-------------------------------+--------------+---------------+------------+";
	cout << "\n|" << left << setw(31) << " First-Choice Hill-Climbing" << "|";
	if (firstchoice.empty())
		cout << emptyvalue;
	else {
		cout << left << setw(14) << " " + to_string((int)firstchoice[0]) + "/" + to_string(numberOfState) << "|";
		cout << " " << left << setw(14) << firstchoice[1] << "|";
		cout << " " << left << setw(11) << firstchoice[2] << "|";
	}
	cout << "\n+-------------------------------+--------------+---------------+------------+";
	cout << "\n|" << left << setw(31) << " Random-Restart Hill-Climbing" << "|";
	if (random.empty())
		cout << emptyvalue;
	else {
		cout << left << setw(14) << " " + to_string((int)random[0]) + "/" + to_string(numberOfState) << "|";
		cout << " " << left << setw(14) << random[1] << "|";
		cout << " " << left << setw(11) << random[2] << "|";
	}
	cout << "\n+-------------------------------+--------------+---------------+------------+";
	cout << "\n|" << left << setw(31) << " Simulated Annealing" << "|";
	if (simulated.empty())
		cout << emptyvalue;
	else {
		cout << left << setw(14) << " " + to_string((int)simulated[0]) + "/" + to_string(numberOfState) << "|";
		cout << " " << left << setw(14) << simulated[1] << "|";
		cout << " " << left << setw(11) << simulated[2] << "|";
	}
	cout << "\n+-------------------------------+--------------+---------------+------------+\n";
}
int main()
{
	srand(time(NULL));
	int numberOfInitStates = 100;
	vector<Puzzle> initStates = createInitStates(numberOfInitStates);
	Result steepest;
	Result firstchoice;
	Result random;
	Result simulated;
	board(numberOfInitStates, steepest, firstchoice, random, simulated);
	for (int x = 0; x < 4; x++)
	{
		double start = clock();
		int stepcount = 0;
		int success = 0;
		double time;
		Result result;
		switch (x)
		{
		case 0:
			cout << "Steepest-Ascent Hill-Climbing\n";
			break;
		case 1:
			cout << "First-Choice Hill-Climbing\n";
			break;
		case 2:
			cout << "Random-Restart Hill-Climbing\n";
			break;
		case 3:
			cout << "Simulated Annealing\n";
			break;
		}
		for (int i = 0; i < numberOfInitStates; i++) {
			cout << "\r" << i + 1 << "/" << numberOfInitStates;
			if (x == 0) {
				result = Steepest_HillClimbing(initStates[i]);
				if (result[0] == 1) {
					success++;
					stepcount += result[1];
				}
				if (i == numberOfInitStates - 1)
				{
					time = (clock() - start) / (double)CLOCKS_PER_SEC;
					steepest.push_back(success);
					if (success != 0)
						steepest.push_back(stepcount / success);
					else
						steepest.push_back(0);
					steepest.push_back(time);
				}
			}
			if (x == 1) {
				result = FirstChoice_HillClimbing(initStates[i]);
				if (result[0] == 1) {
					success++;
					stepcount += result[1];
				}
				if (i == numberOfInitStates - 1)
				{
					time = (clock() - start) / (double)CLOCKS_PER_SEC;
					firstchoice.push_back(success);
					if (success != 0)
						firstchoice.push_back(stepcount / success);
					else
						firstchoice.push_back(0);
					firstchoice.push_back(time);
				}
			}
			if (x == 2) {
				result = RandomRestart_HillClimbing(initStates[i]);
				if (result[0] == 1) {
					success++;
					stepcount += result[1];
				}
				if (i == numberOfInitStates - 1)
				{
					time = (clock() - start) / (double)CLOCKS_PER_SEC;
					random.push_back(success);
					if (success != 0)
						random.push_back(stepcount / success);
					else
						random.push_back(0);
					random.push_back(time);
				}
			}
			if (x == 3) {
				result = Simulated_Annealing(initStates[i]);
				if (result[0] == 1) {
					success++;
					stepcount += result[1];
				}
				if (i == numberOfInitStates - 1)
				{
					time = (clock() - start) / (double)CLOCKS_PER_SEC;
					simulated.push_back(success);
					if (success != 0)
						simulated.push_back(stepcount / success);
					else
						simulated.push_back(0);
					simulated.push_back(time);
				}
			}
		}
		system("cls");
		board(numberOfInitStates, steepest, firstchoice, random, simulated);
	}
	return 0;
}