#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Node.h"

class Manager
{
	std::vector<std::shared_ptr<Node>> nodes; // the vector storing all the Nodes
	std::shared_ptr<Node> findNode(std::string nodeToFind); // returns a shared_ptr to the named Node. Returns NULL if it doesn't find it

	void printDashes(int layer);
	void printDpens(std::vector<std::shared_ptr<Node>> dpens, int layer);
	//bool loopCheck(int nodeIndex, int currentIndex, std::vector<bool> isLoop);

public:
	void addNode(std::string newName, bool deleted); // constructs and adds a new item to the items vector
	std::vector<std::shared_ptr<Node>> getNodes(); // returns the vector of items
	void importFile(std::string fileName); // takes an input file and parses it into the item list
	void exportFile(std::string fileName); // takes the name and dependency data in its current state and writes it to a specified file
	void sortNodes();

	void printNodes();
	//void removeLoops(); // Follows each dependency chain to make sure there aren't any dependency loops. Deletes the root node being searched if there are.
};