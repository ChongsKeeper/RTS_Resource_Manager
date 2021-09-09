#include "Manager.h"


Manager::Manager()
	: nodes{} {}

Manager::~Manager() 
{
	// Memory cleanup
	for (auto& node : nodes)
	{
		delete node;
	}
}

Node* Manager::findNode(std::string nodeToFind)
{
	// Node list is sorted. Use lower bound search to check if the new node is already in the list
	auto const it = lower_bound(nodes.begin(), nodes.end(), nodeToFind, [](Node* node, std::string val)
		{
			return node->getName() < val;
		}
	);
	
	// if the iterator is at the end, the new node isn't in the list
	if (it == nodes.end())
		return nullptr;

	// Only return the pointer if it matches. The iterator is either a match or the first node after the new node's position in the list
	if ((*it)->getName() == nodeToFind)
	{
		return *it;
	}
	else
	{
		return nullptr;
	}
}

Node* Manager::addNode(std::string newName)
{
	// Insertion sort
    const auto it = std::upper_bound(nodes.begin(), nodes.end(), newName, [](std::string val, Node* node)
        {
            return val < node->getName();
        }
	);
	
	// Create a new node
	Node* node = new Node(newName);

	nodes.insert(it, node);

	// Return a pointer to the new node
	return node;
}

std::vector<Node*> Manager::getNodes()
{
	return nodes;
}

bool Manager::importFile(std::string fileName)
{
	std::ifstream inFile(fileName);

	if (!inFile.is_open())
	{
		return false;
	}

	// String for the current line and nodes pulled out from it
	std::string line, nodeName, dependency;
	Node *currentNode, *currentDpen;

	// Read the file line by line
	while (getline(inFile, line))
	{
		// Reset all the values for each pass to prevent erroneous data
		nodeName = "";
		dependency = "";
		std::istringstream iss(line);

		iss >> nodeName;
		
		// Some nodes will be added from the dependencies of other nodes before their line is reached.
		// Check to see if those nodes are already present before adding them
		currentNode = findNode(nodeName);

		if (!currentNode)
		{
			currentNode = addNode(nodeName);
		}
        
		// Read the dependencies one at a time and add them to the parent node
		while (iss >> dependency)
		{
			currentDpen = findNode(dependency);
			if (!currentDpen)
            {
                currentDpen = addNode(dependency);
			}
			currentNode->addDpen(currentDpen);
		}
	}
	
	if (inFile.is_open())
	{
		inFile.close();
	}

	// Sort nodes is case insensitive.
	sortNodes();

	for (auto& node : nodes)
	{
		node->sortDpens();
	}
	
	return true;
}

void Manager::exportFile(std::string fileName)
{
	std::ofstream outFile(fileName);

	if (outFile.is_open())
	{
		for (auto& node : nodes)
		{
			// Don't write deleted nodes to the output
			if (!node->isDeleted())
			{
				outFile << node->getName();
				for (auto& dpen : node->getDpens())
				{
					outFile << " " << dpen->getName();
				}
				outFile << "\n";
			}
		}
	}

	if (outFile.is_open())
	{
		outFile.close();
	}
}

void Manager::sortNodes()
{
    sort(nodes.begin(), nodes.end(), [](Node* a, Node* b)
        {
            return a->getSortName() < b->getSortName();
        });
}
