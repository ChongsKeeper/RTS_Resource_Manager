#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <memory>
#include "SubjectAndObserver.h"


class Node :
	public Subject, public Observer//, std::enable_shared_from_this<Node>
{
	std::string name;
	std::string sortName;
	std::vector<std::shared_ptr<Node>> dpens; // dependencies
	std::vector<std::shared_ptr<Observer>> observersList; // list of all other nodes that depend on this one
	bool complete; // stores whether the dependencies are all valid
	bool deleted;

public:
	Node(std::string newName, bool newDeleted);

	void setName(std::string newName);
	std::string getName(); // returns the name of the item
	std::string getSortName();
	std::vector<std::shared_ptr<Node>> getDpens(); // returns a vector for the List class to process
	void addDpen(const std::shared_ptr<Node>& newDpen);

	void addObserver(const std::shared_ptr<Observer>& observer);
	void notifyObservers();
	void update();

	bool isComplete();
	bool isDeleted();
	void setDeleted(bool newDeleted);
};
