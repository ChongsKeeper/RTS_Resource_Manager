#include "Node.h"
using namespace std;


Node::Node (string newName, bool newDeleted)
{
	setName(newName);
	dpens = {};
	deleted = newDeleted;
	complete = true;
	observersList = {};
}

void Node::setName(string newName)
{
	name = newName;
	sortName = "";
	for (auto letter : name)
	{
		sortName += tolower(letter);
	}
	sortName += name; // make sort deterministic
}

string Node::getName()
{
	return name;
}

string Node::getSortName()
{
	return sortName;
}

vector<Node *> Node::getDpens()
{
	return dpens;
}

void Node::addDpen(Node *newDpen)
{
	dpens.emplace_back(newDpen);
	newDpen->addObserver(this); // every node observes it's dependencies to watch for changes
}

void Node::sortDpens()
{
	sort(dpens.begin(), dpens.end(), [](Node *nodeA, Node *nodeB)
		{
			return nodeA->getSortName() < nodeB->getSortName();
		});
}

void Node::addObserver(Observer *observer)
{
	observersList.emplace_back(observer);
}

void Node::notifyObservers()
{
	for (auto observer : observersList)
	{
		observer->update();
	}
}

void Node::update()
{
	complete = true;
	for (const auto node : dpens)
	{
		if (node->isDeleted())
		{
			complete = false;
		}
	}
}

bool Node::isComplete()
{
	return complete;
}

bool Node::isDeleted()
{
	return deleted;
}

void Node::setDeleted(bool newDeleted)
{
	deleted = newDeleted;
	notifyObservers();
}