#include "Node.h"
#include <iostream>

// Static var
int Node::nextId = 0;

void Node::broadcastMsg(const Message& msg)
{
	for (auto x : otherNodes) {
		x->recieveMsg(msg);
	}

}

void Node::recieveMsg(const Message& msg)
{
	std::cout << msg.senderId << "." << this->id << "(" << msg.payload << ")" << std::endl;
}
