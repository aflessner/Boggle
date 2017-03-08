//============================================================================
// Name        : Boggle.cpp
// Author      : Andy Flessner
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <sstream>
#include <string>

typedef unsigned char UCHAR;
typedef unsigned int UINT32;

// Boggle Board Layout
//
// 0  1  2  3
// 4  5  6  7
// 8  9  10 11
// 12 13 14 15

#define NULL_EDGE 254

const UCHAR x_boardSize = 16;
const UCHAR x_diceSize = 6;
const UCHAR x_numberOfLetters = 26;
const UCHAR x_maxEdges = 8;

struct Node
{
	UCHAR m_edges[x_maxEdges];
};

// Add 1 to x_diceSize for string null terminator (added by c++ compiler)
const UCHAR x_cubes[x_boardSize][x_diceSize + 1] = {
		"aaeegn", "elrtty", "aoottw", "abbjoo",
        "ehrtvw", "cimotu", "distty", "eiosst",
        "delrvy", "achops", "himnqu", "eeinsu",
        "eeghnw", "affkps", "hlnnrz", "deilrx"};

const Node x_graph[x_boardSize] =
{
		{1, 4, 5, NULL_EDGE, NULL_EDGE, NULL_EDGE, NULL_EDGE, NULL_EDGE},	//0
		{0, 2, 4, 5, 6, NULL_EDGE, NULL_EDGE, NULL_EDGE},					//1
		{1, 3, 5, 6, 7, NULL_EDGE, NULL_EDGE, NULL_EDGE},					//2
		{2, 6, 7, NULL_EDGE, NULL_EDGE, NULL_EDGE, NULL_EDGE, NULL_EDGE},	//3
		{0, 1, 5, 8, 9, NULL_EDGE, NULL_EDGE, NULL_EDGE},					//4
		{0, 1, 2, 4, 6, 8, 9, 10},											//5
		{1, 2, 3, 5, 7, 9, 10, 11},											//6
		{2, 3, 6, 10, 11, NULL_EDGE, NULL_EDGE, NULL_EDGE},					//7
		{4, 5, 9, 12, 13, NULL_EDGE, NULL_EDGE, NULL_EDGE},					//8
		{4, 5, 6, 8, 10, 12, 13, 14},										//9
		{5, 6, 7, 9, 11, 13, 14, 15},										//10
		{6, 7, 10, 14, 15, NULL_EDGE, NULL_EDGE, NULL_EDGE},				//11
		{8, 9, 13, NULL_EDGE, NULL_EDGE, NULL_EDGE, NULL_EDGE, NULL_EDGE},	//12
		{8, 9, 10, 12, 14, NULL_EDGE, NULL_EDGE, NULL_EDGE},				//13
		{9, 10, 11, 13, 15, NULL_EDGE, NULL_EDGE, NULL_EDGE},				//14
		{10, 11, 14, NULL_EDGE, NULL_EDGE, NULL_EDGE, NULL_EDGE, NULL_EDGE}	//15
};

bool x_dfsVisited[x_boardSize];

UCHAR x_board[x_boardSize];

Node x_graphRoot[x_numberOfLetters];

void RandomizeBoard()
{
	UCHAR order[x_boardSize];

	for (UCHAR i = 0; i < x_boardSize; i++)
	{
		order[i] = i;
	}

	for (UINT32 i = 0; i < 1000; i++)
	{
		int pos1 = rand() % x_boardSize;
		int pos2 = rand() % x_boardSize;
		std::swap(order[pos1], order[pos2]);
	}

	for (UCHAR i = 0; i < x_boardSize; i++)
	{
		int dicePos = rand() % x_diceSize;
		x_board[i] = x_cubes[order[i]][dicePos];
	}
}

void PrintBoard()
{
	printf("Board:\n");
	printf("%c %c %c %c\n%c %c %c %c\n%c %c %c %c\n%c %c %c %c\n",
			x_board[0], x_board[1], x_board[2], x_board[3],
			x_board[4], x_board[5], x_board[6], x_board[7],
			x_board[8], x_board[9], x_board[10],x_board[11],
			x_board[12],x_board[13],x_board[14],x_board[15]);
}

void InitGraph()
{
	memset(x_graphRoot, NULL_EDGE, sizeof(x_graphRoot));
	for(UCHAR i = 0; i < x_boardSize; i++)
	{
		UCHAR offset = x_board[i] - 'a';
		for (UCHAR j = 0; j < (x_maxEdges + 1); j++)
		{
			if (j == x_maxEdges)
			{
				// graph root is full this should not happen because
				// there should never be more than 8 duplicate letters
				exit(1);
			}
			else if (x_graphRoot[offset].m_edges[j] == NULL_EDGE)
			{
				x_graphRoot[offset].m_edges[j] = i;
				break;
			}
		}
	}
}

bool DFS(const char* word, UCHAR boardPos)
{
	if (*word == 0)
	{
		return true;
	}

	if (x_dfsVisited[boardPos])
	{
		return false;
	}

	if (x_board[boardPos] != *word)
	{
		return false;
	}

	x_dfsVisited[boardPos] = true;

	const char* nextLetter = word + 1;

	for (UCHAR i = 0; i < x_maxEdges; i++)
	{
		if (x_graph[boardPos].m_edges[i] != NULL_EDGE)
		{
			if (DFS(nextLetter, x_graph[boardPos].m_edges[i]))
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}

void InitDFS()
{
	memset(x_dfsVisited, 0, sizeof(x_dfsVisited));
}

bool WordFound(const char* word)
{
	UCHAR offset = *word - 'a';
	for (UCHAR i = 0; i < x_maxEdges; i++)
	{
		if (x_graphRoot[offset].m_edges[i] != NULL_EDGE)
		{
			InitDFS();
			if (DFS(word, x_graphRoot[offset].m_edges[i]))
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}

void Go()
{
	std::ifstream infile("a.txt");
	std::string word;
	unsigned int totalWords = 0;
	unsigned int foundWords = 0;
	while (std::getline(infile, word))
	{
		totalWords++;
		if (WordFound(word.c_str()))
		{
			printf("Found: %s\n", word.c_str());
			foundWords++;
		}
	}
	if (totalWords == 0)
	{
		printf("Error: %s\n", strerror(errno));
	}
	else
	{
		printf("Found %d words\n", foundWords);
		printf("Searched %d words\n", totalWords);
	}
}

int main() {
	srand(time(NULL));
	RandomizeBoard();
	InitGraph();
	Go();
	PrintBoard();
	return 0;
}
