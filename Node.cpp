#include "Consts.h"
#include "Node.h"

// spdlog
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>



// Static var
int Node::nextId = 0;

// Main logger (singleton)
std::shared_ptr<spdlog::logger> getMainLogger() {
	static std::shared_ptr<spdlog::logger> mainLogger = nullptr;
	if (!mainLogger) {
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/main_log.txt", true);
		std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
		mainLogger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
		spdlog::register_logger(mainLogger);
	}
	return mainLogger;
}

// Global slow mode flag
std::atomic<bool> g_slowMode{false};

Node::Node(int numFaultyNodes, bool isLoyal)
	: id(++nextId), numFaultyNodes(numFaultyNodes), isLoyal(isLoyal) {
	// Each node gets its own logger (console + file)
	// auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	std::string filename = "logs/node_" + std::to_string(id) + ".txt";
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
	std::vector<spdlog::sink_ptr> sinks {file_sink};
	nodeLogger = std::make_shared<spdlog::logger>("node_" + std::to_string(id), sinks.begin(), sinks.end());
	spdlog::register_logger(nodeLogger);
}
void Node::broadcastMsg(Message msg)
{
	// Only traitors can corrupt messages
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_int_distribution<> dis(0, 2);  // 33% chance

	std::string originalPayload = msg.payload;
	for (auto recipient : otherNodes) {
		// Delay for 1 second between msg if slow mode is enabled
		if (g_slowMode.load()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

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
		nodeLogger->error("Invalid message: {} -> {}", incomingMsg.getPrintFormat(), this->getId());
		getMainLogger()->error("Invalid message: {} -> {}", incomingMsg.getPrintFormat(), this->getId());
		return;
	}

	// Read
	this->recievedValues.insert(incomingMsg.payload);

	// Accept and sign
	Message newMsg = Message::acceptAndSign(this->getId(), this->getPrivateKey(), incomingMsg);

	// Log received msg
	nodeLogger->info("{}", newMsg.getPrintFormat());
	getMainLogger()->info("{}", newMsg.getPrintFormat());

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
