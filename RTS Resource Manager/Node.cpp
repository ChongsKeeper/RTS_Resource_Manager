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

vector<shared_ptr<Node>> Node::getDpens()
{
	return dpens;
}

void Node::addDpen(const shared_ptr<Node>& newDpen)
{
	dpens.emplace_back(newDpen);
	if (auto self = weak_from_this().lock()) {
		newDpen->addObserver(shared_from_this());
	}
	else {
		std::cout << "Bad weak_this\n";
	}
}

void Node::addObserver(const shared_ptr<Observer>& observer)
{
	observersList.emplace_back(observer);
}

void Node::notifyObservers()
{
	for (auto& observer : observersList)
	{
		observer->update();
	}
}

void Node::update()
{
	complete = true;
	for (auto& node : dpens)
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