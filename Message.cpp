#include "Message.h"

std::string Message::getPrintFormat()
{
	std::string result = "";

	for (auto x : this->history) {
		result += x;
		result += ".";
	}

	// remove excess '.'
	result = result.substr(0, result.size() - 1);

	// add payload

	result += "(" + this->payload + ")";
	return result;
}

void Message::addHistory(int id)
{
	this->history.push_back(std::to_string(id));
}
