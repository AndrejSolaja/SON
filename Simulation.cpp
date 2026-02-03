#include "Simulation.h"
#include "Node.h"
#include "Message.h"
#include <memory>


void Simulation::init()
{
	// Create nodes
	nodes.reserve(totalNumNodes);
	for (int i = 0; i < totalNumNodes; i++) {
		nodes.push_back(std::make_unique<Node>());
	}
    
	// Add references 
	for (int i = 0; i < totalNumNodes; i++) {
		std::vector<Node*> tempNodes = std::vector<Node*>();
		tempNodes.reserve(totalNumNodes - 1);
		for (int j = 0; j < totalNumNodes; j++) {
			if (i != j) {
				tempNodes.push_back(nodes[j].get());
			}
		}
		nodes[i]->setOtherNodes(tempNodes);
	}
}

void Simulation::start()
{
	// Choose one node to be source
	// For now hardcode to be first node 
	Node* sourceNode = nodes[0].get();

	// Create message from source and send to all other nodes
	Message m(sourceNode->getId(), "test");
	sourceNode->broadcastMsg(m);

}
