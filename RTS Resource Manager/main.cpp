
#include "main.h"
using namespace std;

int main()
{
	Manager nodeList;

	nodeList.importFile("resource_list.txt");

	nodeList.printNodes();

	nodeList.exportFile("test.txt");

	/*auto node1 = make_shared<Node>("node1", false);
	auto node2 = make_shared<Node>("node2", false);

	node1->addDpen(node2);

	foo();*/

	return 0;
}