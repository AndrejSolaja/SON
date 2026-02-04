#include "Node.h"
#include <iostream>
#include "Consts.h"

// Static var
int Node::nextId = 0;

int Node::broadcastMsg(Message msg)
{
	int sendCount = 0;
	// Forward to everyone that hasn't signed this msg
	for (auto recipient : otherNodes) {
		if (msg.signedBy.count(recipient->getId()) == 0)
		{
			sendCount++;
			recipient->recieveMsg(msg);
		}
	}
	return sendCount;

}

void Node::recieveMsg(Message msg)
{
	// Check validity , read
	this->recievedValues.insert(msg.payload);

	// temp
	msg.addHistory(this->getId());

	// Sign 
	msg.signedBy.insert(this->getId());
	std::cout << msg.getPrintFormat() << std::endl;

	// Forward
	if (broadcastMsg(msg) == 0) {
		// No one to send anymore, do choice
		std::cout << "Choice(" << this->getId() << ") = " << choice() << std::endl;
	}

}


std::string Node::choice()
{
	// If there are multiple values return default value
	if (this->recievedValues.size() > 1) return DEFAULT_VALUE;
	return *this->recievedValues.begin();
}
