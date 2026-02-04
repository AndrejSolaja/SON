#include "Simulation.h"
#include "Node.h"
#include "Message.h"
#include <memory>
#include <iostream>
#include "CertificationBody.h"

void Simulation::init()
{
	// Create nodes
	nodes.reserve(totalNumNodes);
	for (int i = 0; i < totalNumNodes; i++) {
		nodes.push_back(std::make_unique<Node>());
	}
    
	CertificationBody& cb = CertificationBody::getInstance();
	// Register to certification body and get private key
	for(int i = 0 ; i < totalNumNodes; i++) {
		std::vector<uint8_t> privateKey = cb.registerNode(nodes[i]->getId());
		nodes[i]->setPrivateKey(privateKey);
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
	std::cout << "Source node: " << sourceNode->getId() << std::endl;


	// Create message from source and send to all other nodes
	Message msg(sourceNode->getId(),sourceNode->getPrivateKey(), "test");
	sourceNode->broadcastMsg(msg);






}
