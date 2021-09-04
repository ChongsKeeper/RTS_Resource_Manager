#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>

struct Node
{
    Node(std::string name)
        : name(name)
        , dpens{}
    {}

    std::string name;
    std::vector<Node*> dpens;
};


// Check how deep the dpen chain goes from the current node.

int depthCheck(Node* node, int curDepth = 0)
{
    curDepth++;
    int maxDepth = curDepth;
    for (auto& dpen : node->dpens)
    {
         int tempDepth = depthCheck(dpen, curDepth);
         if (tempDepth > maxDepth)
         {
             maxDepth = tempDepth;
         }
    }
    return maxDepth;
}

bool loopCheck(Node* root, Node* newDpen)
{
    if (newDpen == root)
    {
        return false;
    }
    else
    {
        for (auto& dpen : newDpen->dpens)
        {
            if (!loopCheck(root, dpen))
            {
                return false;
            }
        }
    }
    return true;
}

void loadingAnim(int frameCount)
{
    std::cout << "\rGenerating... ";

    switch(frameCount % 4)
    {
    case 0:
        std::cout << "|";
        break;
    case 1:
        std::cout << "/";
        break;
    case 2:
        std::cout << "-";
        break;
    case 3:
        std::cout << "\\";
        break;
    }
}


int main()
{
    std::vector<Node*> nodeList;

    const int nodeCount = 100000;
    const int dpenCount = 5;

    for (int i = 0; i < nodeCount; i++)
    {
        std::stringstream ss;
        ss << "Node" << i;
        nodeList.push_back(new Node(ss.str()));
    }

    int maxDepth = 0;
    int loadAnim = 0;


    for (auto& node : nodeList)
    {
        int randDpens = rand() % dpenCount;
        for (int i = 0; i < randDpens; i++)
        {
            loadingAnim(loadAnim++);
            
            int index = rand() % (nodeCount - 1);
            int depth = depthCheck(nodeList[index]);

            // This  if statement reduces the depth, but doesn't actually prevent it from going over five.
            // It doesn't account for nodes that depend on the one being added to.
            if (depth < 5)
            {
                maxDepth = depth > maxDepth ? depth : maxDepth;
                if (loopCheck(node, nodeList[index]))
                {
                    node->dpens.push_back(nodeList[index]);
                }
            }
        }
    }

    std::ofstream output("resource.txt");

    for (auto& node : nodeList)
    {
        output << node->name;
        for (auto dpen : node->dpens)
        {
            output << " " << dpen->name;
        }
        output << "\n";
    }

    std::cout << "\rGenerated " << nodeCount << " Nodes with up to " << dpenCount << " dpens each.\n";

    output.close();
}

