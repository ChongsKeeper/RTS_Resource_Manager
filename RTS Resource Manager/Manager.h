#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <regex>
#include "Node.h"

class Manager
{
	std::vector<Node*> nodes; // the vector storing all the Nodes
	Node* findNode(std::string nodeToFind); // returns a shared_ptr to the named Node. Returns NULL if it doesn't find it

	void printDashes(int layer);
	void printDpens(Node *node, int layer);
	bool loopCheck(Node *currentNode, std::vector<Node*> isLoop);

public:
	~Manager();

	void addNode(std::string newName, bool deleted); // constructs and adds a new item to the items vector
	std::vector<Node*> getNodes(); // returns the vector of items
	void importFile(std::string fileName); // takes an input file and parses it into the item list
	void exportFile(std::string fileName); // takes the name and dependency data in its current state and writes it to a specified file
	void sortNodes();
	//std::vector<Node*> getSearchList(char search[]);

	void printNodes();
	void removeLoops(); // Follows each dependency chain to make sure there aren't any dependency loops. Deletes the root node being searched if there are.
};