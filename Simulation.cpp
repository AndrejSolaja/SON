#include "Simulation.h"
#include "Node.h"
#include "Message.h"
#include <memory>
#include <iostream>
#include "CertificationBody.h"
#include "Consts.h"

void Simulation::init()
{
	// Create nodes
	nodes.reserve(totalNumNodes);
	for (int i = 0; i < totalNumNodes; i++) {
		nodes.push_back(std::make_unique<Node>(this->numFaultyNodes));
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
	std::cout << "N = " << this->totalNumNodes << " | m = " << this->numFaultyNodes << std::endl;

	// Choose first node to be general
	Node* sourceNode = nodes[0].get();
	std::cout << "General: " << sourceNode->getId() << std::endl;
	std::cout << "Message: " << MESSAGE_TEXT << std::endl;
	std::cout << "Default message: " << DEFAULT_MESSAGE_TEXT << std::endl;


	// Create message from source and send to all other nodes
	Message msg(sourceNode->getId(),sourceNode->getPrivateKey(), "test");
	sourceNode->broadcastMsg(msg);

}

void Simulation::end() {
	// Should trigger when no more messages can arrive 
	for (int i = 0; i < totalNumNodes; i++) {
		std::cout << "Choice(" << nodes[i]->getId() << ") = " << nodes[i]->choice() << std::endl;
	}


}
