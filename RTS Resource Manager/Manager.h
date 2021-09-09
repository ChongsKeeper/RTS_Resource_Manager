#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Node.h"

class Manager
{
public:
	Manager();
	~Manager();

	// constructs and adds a new item to the items vector
	Node* addNode(std::string newName); 

	std::vector<Node*> getNodes(); 

	// takes an input file and parses it into the item list
	bool importFile(std::string fileName); 

	// takes the name and dependency data in its current state and writes it to a specified file
    void exportFile(std::string fileName);
	// Sort using sort names
    void sortNodes();

private:

	// returns pointer to the named Node. Returns NULL if it doesn't find it
	Node* findNode(std::string nodeToFind); 

private:
	std::vector<Node*> nodes; // the vector storing all the Nodes
};