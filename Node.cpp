#include "Node.h"
#include <iostream>

// Static var
int Node::nextId = 0;

void Node::broadcastMsg(Message msg)
{
	for (auto x : otherNodes) {
		x->recieveMsg(msg);
	}

}

void Node::recieveMsg(Message msg)
{
	msgsRecieved[msg.senderId] = msg.payload;
	msg.addHistory(this->getId());
	std::cout << msg.getPrintFormat() << std::endl;
	//std::cout << msg.senderId << "." << this->id << "(" << msg.payload << ")" << std::endl;
}

std::string Node::calcMajority()
{
	if (msgsRecieved.empty()) return "default";

	std::string candidate;
	int count = 0;

	// Boyer-Moore majority vote: find a candidate
	for (const auto& pair : msgsRecieved) {
		const std::string& value = pair.second;
		if (count == 0) {
			candidate = value;
			count = 1;
		}
		else if (value == candidate) {
			count++;
		}
		else {
			count--;
		}
	}

	// Verify the candidate is actually a majority (> n/2)
	int occurrences = 0;
	for (const auto& pair : msgsRecieved) {
		if (pair.second == candidate) occurrences++;
	}

	if (occurrences <= static_cast<int>(msgsRecieved.size() / 2)) {
		candidate = "default";
	}

	// temp log
	std::cout << "Node: " << this->getId() << " Result: " << candidate << std::endl;
	return candidate;
}
