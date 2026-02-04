#include "Simulation.h"
#include "Node.h"
#include "Message.h"
#include <memory>
#include <iostream>
#include "CertificationBody.h"
#include "Consts.h"
#include <random>

void Simulation::init()
{
	// Define m random traitors
	std::vector<bool> loyalVector(this->totalNumNodes, true);
	std::fill(loyalVector.begin(), loyalVector.begin() + this->numFaultyNodes, false);
	std::shuffle(loyalVector.begin(), loyalVector.end(), std::mt19937{ std::random_device{}() });

	// Choose general - find first node matching loyalty status
	int generalIndex = 0;
	for (int i = 0; i < loyalVector.size(); i++) {
		if (loyalVector[i] == this->isGeneralLoyal) {
			generalIndex = i;
			break;
		}
	}

	// Create nodes
	nodes.reserve(totalNumNodes);
	for (int i = 0; i < totalNumNodes; i++) {
		nodes.push_back(std::make_unique<Node>(this->numFaultyNodes, loyalVector[i]));
		// Define general
		if (i == generalIndex) {
			this->general = nodes[i].get();
			nodes[i]->setIsGeneral(true);
		}
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

	std::cout << "General: " << general->getId() << " (" << (general->getIsLoyal() == true ? "L" : "T") << ") " << std::endl;
	std::cout << 
	"Lieutenants: ";
	for (int i = 0; i < totalNumNodes; i++) {
		if (nodes[i].get() != general) {
			std::cout << nodes[i]->getId() << " (" << (nodes[i]->getIsLoyal() == true ? "L" : "T") << ") ";
		}
	}
	std::cout << std::endl;
	std::cout << "Message: " << MESSAGE_TEXT << std::endl;
	std::cout << "Default message: " << DEFAULT_MESSAGE_TEXT << std::endl;
	std::cout << "=====================================" << std::endl;

	// Create message from source and send to all other nodes
	Message msg(general->getId(),general->getPrivateKey(), MESSAGE_TEXT);
	general->broadcastMsg(msg);

	std::cout << "=====================================" << std::endl;
}

void Simulation::end() {

	// Should trigger when no more messages can arrive 
	for (int i = 0; i < totalNumNodes; i++) {
		std::cout << "Choice[" << nodes[i]->getId() << "](";
		for (auto x : nodes[i]->getRecievedValues()) {
			std::cout << x << ", ";
		}
		std::cout << ") = " << nodes[i]->choice() << std::endl;

	}

}
