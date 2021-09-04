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
public:
	Node(std::string newName, bool newDeleted);

	void setName(std::string newName);
	std::string getName(); // returns the name of the item
	std::string getSortName(); // Returns the name in all lowercase
	std::vector<Node *> getDpens(); // returns a vector for the List class to process
	void addDpen(Node *newDpen);
	void removeDpen(Node* oldDpen);
	void sortDpens();

	void addObserver(Observer *observer);
	void removeObserver(Observer* observer);
	void notifyObservers();
	void update();

	bool isDeleted();
	void setDeleted(bool newDeleted);

	bool isUsable();

private:
	bool loopCheck(Node* root, Node* newDpen);

private:
	std::string name;

	// Node's name in lowercase. Used for sort comparisons.
	std::string sortName;

	// List of dependencies.
	std::vector<Node *> dpens;

	// Inverse of dpens. This list contains all the nodes that depend on this one.
	std::vector<Observer *> observersList;
	bool deleted;
	bool usable;
};