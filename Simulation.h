#pragma once
#include "Node.h"
#include "Message.h"
#include <vector>
#include <memory>
#include <queue>
#include <string>
#include <fstream>
#include "spdlog/spdlog.h"

// Represents a message waiting to be delivered
struct PendingMessage {
	Message msg;
	int recipientId;
	
	PendingMessage(const Message& m, int recipient) : msg(m), recipientId(recipient) {}
};

class Simulation
{
public:
	Simulation(int N, int m, bool isGeneralLoyal);
	void init();
	void start(bool isRestore);
	void end();
	
	// Message queue operations
	void enqueueMessage(const Message& msg, int recipientId);
	void processNextMessage();
	void processAllMessages();
	bool hasMoreMessages() const { return !messageQueue.empty(); }
	size_t getQueueSize() const { return messageQueue.size(); }
	
	// Checkpoint operations
	void saveCheckpoint(const std::string& filename);
	void loadCheckpoint(const std::string& filename);
	
	// Get node by ID
	Node* getNodeById(int id);
	
	// Logger access
	std::shared_ptr<spdlog::logger> getLogger() const { return mainLogger; }
	
	// Slow mode
	void setSlowMode(bool enabled) { slowMode = enabled; }
	bool isSlowMode() const { return slowMode; }

	int getNumFaultyNodes() {return this->numFaultyNodes;}

private:
	int totalNumNodes; // N
	int numFaultyNodes; // m
	bool isGeneralLoyal;
	int currentRound = 0;

	int generalId = -1;
	Node* general;

	std::vector<std::unique_ptr<Node>> nodes;
	std::queue<PendingMessage> messageQueue;
	
	// For checkpoint restore
	std::vector<bool> savedLoyaltyVector;
	
	// Main simulation logger
	std::shared_ptr<spdlog::logger> mainLogger;
	
	// Slow mode - adds delay between messages
	bool slowMode = false;
};

