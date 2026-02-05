#pragma once

#include <string>
#include <vector>
#include "Message.h"
#include <map>
#include <string>
#include <set>
#include <memory>

#include "spdlog/spdlog.h"

// Forward declaration
class Simulation;

class Node
{
public:
	Node(bool isLoyal = true);

	void broadcastMsg(Message msg);
	void recieveMsg(Message msg);
	std::string choice();

	int getId() const { return id; }
	void setOtherNodes(std::vector<Node*> nodes) { otherNodes = nodes; }

	std::vector<uint8_t> getPrivateKey() const { return privateKey; }
	void setPrivateKey(std::vector<uint8_t> pk) { privateKey = pk;}

	bool getIsLoyal() const { return isLoyal; }
	void setIsGeneral(bool val) { isGeneral = val; }
	std::set<std::string> getRecievedValues() const { return recievedValues; }
	void addReceivedValue(const std::string& val) { recievedValues.insert(val); }
	
	// Simulation reference for message queue
	void setSimulation(Simulation* sim) { simulation = sim; }

	// Logger access
	std::shared_ptr<spdlog::logger> getLogger() { return nodeLogger; }
	
	// Reset static ID counter (for checkpoint restore)
	static void resetIdCounter() { nextId = 0; }

private:
	static int nextId;
	const int id;
	std::vector<uint8_t> privateKey;
	bool isLoyal;
	bool isGeneral = false;

	std::vector<Node*> otherNodes;
	std::set<std::string> recievedValues;
	Simulation* simulation = nullptr;

	std::shared_ptr<spdlog::logger> nodeLogger;
};

