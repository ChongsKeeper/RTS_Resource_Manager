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
	if (loopCheck(this, newDpen))
	{
		dpens.emplace_back(newDpen);
		newDpen->addObserver(this); // every node observes it's dependencies to watch for changes
	}
}

bool Node::loopCheck(Node* root, Node* newDpen)
{
	if (root == newDpen)
	{
		return false;
	}
	else
	{
		for (auto& node : newDpen->getDpens())
		{
			if (!loopCheck(root, node)) return false;
		}
	}
	return true;
}

void Node::removeDpen(Node* oldDpen)
{
	for (unsigned int i = 0; i < dpens.size(); i++)
	{
		if (oldDpen == dpens[i])
		{
			dpens.erase(dpens.begin() + i);
			return;
		}
	}
	oldDpen->removeObserver(this); // every node observes it's dependencies to watch for changes
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

void Node::removeObserver(Observer* observer)
{
	for (unsigned int i = 0; i < observersList.size(); i++)
	{
		if (observer == observersList[i])
		{
			observersList.erase(observersList.begin() + i);
			return;
		}
	}
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