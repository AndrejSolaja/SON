#include "Node.h"
#include <iostream>
#include "Consts.h"
#include <random>
#include <thread>
#include <chrono>

// Static var
int Node::nextId = 0;

void Node::broadcastMsg(Message msg)
{
	// Only traitors can corrupt messages
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_int_distribution<> dis(0, 2);  // 33% chance

	std::string originalPayload = msg.payload;
	for (auto recipient : otherNodes) {
		// Delay for 1 second between msg
		//std::this_thread::sleep_for(std::chrono::seconds(1));

		if (msg.signedBy.count(recipient->getId()) == 0) {
			// Non general traitors can randomly corrupt messages for each recipient
			if (!this->isLoyal && !this->isGeneral && dis(gen) == 1) {
				msg.payload = TRAITOR_MESSAGE_TEXT;
			}
			// Generals can send wrong message
			else if (!this->isLoyal && this->isGeneral && dis(gen) == 1) {
				Message falseMsg(this->getId(), this->getPrivateKey(), TRAITOR_MESSAGE_TEXT);
				recipient->recieveMsg(falseMsg);
				continue;
			}
			else {
				msg.payload = originalPayload;
			}
			recipient->recieveMsg(msg);
		}
	}
}

void Node::recieveMsg(Message incomingMsg)
{
	// Check validity
	if (!incomingMsg.checkValidity()) {
		// Since message is faulty skip this
		std::cerr << "Invalid message: " << incomingMsg.getPrintFormat() << "->" << this->getId() << std::endl;
		return;
	}

	// Read
	this->recievedValues.insert(incomingMsg.payload);

	// Accept and sign
	Message newMsg = Message::acceptAndSign(this->getId(), this->getPrivateKey(), incomingMsg);

	// Print recieved msg
	std::cout << newMsg.getPrintFormat() << std::endl;

	// Check if round limit is reached before forwarding
	if (newMsg.history.size() <= this->numFaultyNodes + 1) {
		// Forward
		broadcastMsg(newMsg);
	}


}


std::string Node::choice()
{
	// If no values received, return default
	if (this->recievedValues.empty()) return DEFAULT_MESSAGE_TEXT;
	
	// If there are multiple values return default value
	if (this->recievedValues.size() > 1) return DEFAULT_MESSAGE_TEXT;
	
	// Single value received
	return *this->recievedValues.begin();
}
