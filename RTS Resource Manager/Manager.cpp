#include "Manager.h"
using namespace std;

/*
TODO: Input validation. Where do the checks go? User input/file output.
TODO: What is the correct way to read a file? Line by line, block by block?
TODO: Determine scale of things. Name length
TODO: Regular expressions
TODO: COMMENT EVERYTHING
*/



Node * Manager::findNode(string nodeToFind)
{
	for (auto node : nodes)
	{
		if (nodeToFind == node->getName())
		{
			return node;
		}
	}
	return NULL;
}

Manager::~Manager() // destructor to cleanup all the New Nodes created in the import step
{
	for (auto& node : nodes)
	{
		delete node;
	}
}

void Manager::addNode(string newName, bool deleted = false)
{
	nodes.emplace_back(new Node(newName, deleted));
}

vector<Node*> Manager::getNodes()
{
	return nodes;
}

void Manager::importFile(string fileName)
{
	//regex regexFilename("[a-zA-Z_]*\\.[txt]");
	bool validEntry = false;
	
	cout << "Enter a file name for import. Included file is named resources.txt or you may add a new file to the folder.\n";

	ifstream inFile(fileName);
	string line, nodeName, dependency;
	vector<Node*> dpenVec;
	Node *currentNode, *currentDpen;

	while (getline(inFile, line))
	{
		// reset all the values for each pass to prevent erroneous data
		nodeName = "";
		dependency = "";
		dpenVec = {};
		istringstream iss(line);
		// read the Node name from the file
		iss >> nodeName;

		currentNode = findNode(nodeName);

		if (!currentNode)
		{
			addNode(nodeName);
			currentNode = nodes.back();
		}
		else
		{
			currentNode->setDeleted(false);
		}
		while (iss >> dependency)
		{
			currentDpen = findNode(dependency);
			if (!currentDpen)
			{
				addNode(dependency, true);
				currentDpen = nodes.back();
			}
			currentNode->addDpen(currentDpen);
		}
	}
	if (inFile.is_open())
	{
		inFile.close();
	}
	sortNodes();
	for (auto node : nodes)
	{
		node->sortDpens();
	}
	// a chain of dependencies causes the print function to loop forever. removeLoops() checks for them and deletes the parent node if it loops back on itself
}

void Manager::exportFile(string fileName)
{
	ofstream outFile(fileName);

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
	sort(nodes.begin(), nodes.end(), [](Node *nodeA, Node *nodeB)
		{
			return nodeA->getSortName() < nodeB->getSortName(); 
		});
}

//std::vector<Node*> Manager::getSearchList(char search[])
//{
//
//	for (unsigned int i = 0; i < strlen(search); i++)
//	{
//		search[i] = tolower(search[i]);
//	}
//
//	std::vector<Node*> filteredList;
//	for (auto& node : nodes)
//	{
//		if (strstr(node->getSortName().c_str(), search))
//		{
//			filteredList.push_back(node);
//		}
//	}
//	return filteredList;
//}

//TODO: move all name printing to print dpens function
void Manager::printNodes()
{
	for (auto& node : nodes)
	{
		printDpens(node, 0);
		cout << "\n";
	}
}

void Manager::printDashes(int layer)
{
	for (int i = 0; i < layer*2; i++)
	{
		cout << "-";
	}
}

void Manager::printDpens(Node *node, int layer)
{

	if (node->isDeleted())
	{
		printDashes(layer);
		cout << node->getName() << "  (deleted)\n";
	}
	else
	{
		printDashes(layer);
		cout << node->getName();
		if (!node->isComplete())
		{
			cout << "  (incomplete)";
		}
		cout << "\n";
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