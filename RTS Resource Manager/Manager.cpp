#include "Manager.h"
using namespace std;

/*
TODO: Input validation. Where do the checks go? User input/file output.
TODO: What is the correct way to read a file? Line by line, block by block?
TODO: Determine scale of things. Name length
TODO: Regular expressions
TODO: COMMENT EVERYTHING
*/



shared_ptr<Node> Manager::findNode(string nodeToFind)
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

void Manager::addNode(string newName, bool deleted = false)
{
	nodes.emplace_back(make_shared<Node>(newName, deleted));
}

vector<shared_ptr<Node>> Manager::getNodes()
{
	return nodes;
}

void Manager::importFile(string fileName)
{
	ifstream inFile(fileName);
	string line, nodeName, dependency;
	vector<shared_ptr<Node>> dpenVec;
	shared_ptr<Node> currentNode, currentDpen;

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
	// Check the dependency chains to remove any infinite dependency loops
	//removeLoops();
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
	sort(nodes.begin(), nodes.end(), [](const shared_ptr<Node> nodeA, const shared_ptr<Node> nodeB)
		{
			return nodeA->getSortName() < nodeB->getSortName(); 
		});
}

void Manager::printNodes()
{
	for (auto& node : nodes)
	{
		if (!node->isDeleted())
		{
			cout << node->getName();
			if (!node->isComplete())
			{
				cout << "  (incomplete)";
			}
			cout << "\n";
			printDpens(node->getDpens(), 0);
			cout << "\n";
		}
	}
}

void Manager::printDashes(int layer)
{
	for (int i = 0; i < layer*2; i++)
	{
		cout << "-";
	}
}

void Manager::printDpens(vector<shared_ptr<Node>> dpens, int layer)
{
	layer++;
	for (const auto& node : dpens)
	{
		printDashes(layer);

		cout << node->getName();
		if (node->isDeleted())
		{
			cout << "  (deleted)\n";
		}
		else
		{
			cout << "\n";
			if (node->getDpens().size() > 0)
			{
				printDpens(node->getDpens(), layer);
			}
		}
	}
}

//void Manager::removeLoops()
//{
//	vector<bool> isLoop;
//	for (unsigned int j = 0; j < nodes.size(); j++)
//	{
//		isLoop.push_back(false);
//	}
//	for (unsigned int i = 0; i < nodes.size(); i++)
//	{
//		if (loopCheck(i, i, isLoop) == false)
//		{
//			nodes[i]->setDeleted(true);
//		}
//	}
//}
//
//bool Manager::loopCheck(int nodeIndex, int currentIndex, vector<bool> isLoop)
//{
//	auto dpens = nodes[currentIndex]->getDpens();
//
//	bool result = true;
//
//	if (dpens.size() == 0)
//	{
//		return true;
//	}
//	for (unsigned int i = 0; i < dpens.size(); i++)
//	{
//		if (isLoop[currentIndex])
//		{
//			return false;
//		}
//		else if (nodes[dpens[i]]->isDeleted())
//		{
//			return true;
//		}
//		else
//		{
//			vector<bool> nextLoop = isLoop;
//			nextLoop[currentIndex] = true;
//			result = loopCheck(nodeIndex, dpens[i], nextLoop);
//		}
//	}
//	return result;
//}