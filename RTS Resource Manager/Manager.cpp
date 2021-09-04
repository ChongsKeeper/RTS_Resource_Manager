#include "Manager.h"

/*
TODO: Input validation. Where do the checks go? User input/file output.
TODO: What is the correct way to read a file? Line by line, block by block?
TODO: Determine scale of things. Name length
TODO: Regular expressions
TODO: COMMENT EVERYTHING
*/


Manager& Manager::getInstance()
{
	static Manager instance;
	return instance;
}

Manager::Manager()
	: nodes{} {}

Manager::~Manager() // destructor to cleanup all the New Nodes created in the import step
{
	for (auto& node : nodes)
	{
		delete node;
	}
}

Node* Manager::findNode(std::string nodeToFind)
{
	auto const it = lower_bound(nodes.begin(), nodes.end(), nodeToFind, [](Node* node, std::string val)
		{
			return node->getName() < val;
		}
	);
	
	if (it == nodes.end())
		return NULL;

	if ((*it)->getName() == nodeToFind)
	{
		return *it;
	}
	else
	{
		return NULL;
	}
}

Node* Manager::addNode(std::string newName, bool deleted = false)
{
    const auto it = std::upper_bound(nodes.begin(), nodes.end(), newName, [](std::string val, Node* node)
        {
            return val < node->getName();
        }
	);
	
	Node* node = new Node(newName, deleted);

	nodes.insert(it, node);

	return node;
}

std::vector<Node*> Manager::getNodes()
{
	return nodes;
}

bool Manager::importFile(std::string fileName)
{
	auto* timer = ModuleTimer::GetInstance();

	std::ifstream inFile(fileName);

	if (!inFile.is_open())
	{
		return false;
	}

	std::string line, nodeName, dependency;
	std::vector<Node*> dpenVec;
	Node *currentNode, *currentDpen;

	while (getline(inFile, line))
	{
		// reset all the values for each pass to prevent erroneous data
		nodeName = "";
		dependency = "";
		dpenVec = {};
		std::istringstream iss(line);
		// read the Node name from the file
		iss >> nodeName;
		
		currentNode = findNode(nodeName);

		if (!currentNode)
		{
			currentNode = addNode(nodeName);
		}
        
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

	for (auto node : nodes)
	{
		node->sortDpens();
	}

	std::cout << "There are " << nodes.size() << " nodes in the list.\n";
	return true;
}

void Manager::exportFile(std::string fileName)
{
	std::ofstream outFile(fileName);

	if (outFile.is_open())
	{
		for (auto& node : nodes)
		{
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

void Manager::printDashes(int layer)
{
	for (int i = 0; i < layer*2; i++)
	{
		std::cout << "-";
	}
}

void Manager::printDpens(Node *node, int layer)
{

	if (node->isDeleted())
	{
		printDashes(layer);
		std::cout << node->getName() << "  (deleted)\n";
	}
	else
	{
		printDashes(layer);
		std::cout << node->getName();

		std::cout << "\n";
		if (node->getDpens().size() > 0)
		{
			layer++;
			for (auto nextNode : node->getDpens())
			{
				printDpens(nextNode, layer);
			}
		}
	}
}