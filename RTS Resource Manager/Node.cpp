#include "Node.h"


Node::Node (std::string newName, bool newDeleted)
{
	setName(newName);
	dpens = {};
	deleted = newDeleted;
	usable = true;
	observersList = {};
}

void Node::setName(std::string newName)
{
	name = newName;
	sortName = "";
	for (auto letter : name)
	{
		sortName += tolower(letter);
	}
	sortName += name; // make sort deterministic
}

std::string Node::getName()
{
	return name;
}

std::string Node::getSortName()
{
	return sortName;
}

std::vector<Node *> Node::getDpens()
{
	return dpens;
}

void Node::addDpen(Node *newDpen)
{
	if (loopCheck(this, newDpen))
	{
		dpens.push_back(newDpen);
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
			oldDpen->removeObserver(this); // every node observes it's dependencies to watch for changes
			update();
			return;
		}
	}
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
	for (const auto node : dpens)
	{
		if (node->isDeleted() || !node->isUsable())
		{
			usable = false;
			notifyObservers();
			return;
		}
	}
	usable = true;
	notifyObservers();
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

bool Node::isUsable()
{
	return usable;
}