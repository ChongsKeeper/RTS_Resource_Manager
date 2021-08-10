#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <memory>
#include "SubjectAndObserver.h"


class Node :
	public Subject, public Observer
{
	std::string name;
	std::string sortName;
	std::vector<Node *> dpens; // dependencies
	std::vector<Observer *> observersList; // list of all other nodes that depend on this one
	bool complete; // stores whether the dependencies are all valid
	bool deleted;

	bool loopCheck(Node* root, Node* newDpen);

public:

	Node(std::string newName, bool newDeleted);

	void setName(std::string newName);
	std::string getName(); // returns the name of the item
	std::string getSortName();
	std::vector<Node *> getDpens(); // returns a vector for the List class to process
	void addDpen(Node *newDpen);
	void removeDpen(Node* oldDpen);
	void sortDpens();

	void addObserver(Observer *observer);
	void removeObserver(Observer* observer);
	void notifyObservers();
	void update();

	bool isComplete();
	bool isDeleted();
	void setDeleted(bool newDeleted);
};