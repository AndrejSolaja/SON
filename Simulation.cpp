#include "Simulation.h"
#include "Node.h"
#include "Message.h"
#include <memory>
#include <iostream>
#include "CertificationBody.h"
#include "Consts.h"
#include <random>
#include <fstream>
#include <sstream>

// spdlog
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

Simulation::Simulation(int N, int m, bool isGeneralLoyal)
	: totalNumNodes(N), numFaultyNodes(m), isGeneralLoyal(isGeneralLoyal)
{
	// Initialize main logger
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_pattern("%v");  // Only show the message, no timestamp/level
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/main_log.txt", true);
	std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
	mainLogger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
	spdlog::register_logger(mainLogger);
}

void Simulation::init()
{
	// Define m random traitors
	std::vector<bool> loyalVector(this->totalNumNodes, true);
	std::fill(loyalVector.begin(), loyalVector.begin() + this->numFaultyNodes, false);
	std::shuffle(loyalVector.begin(), loyalVector.end(), std::mt19937{ std::random_device{}() });

	// Save for checkpoint
	this->savedLoyaltyVector = loyalVector;

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
		nodes.push_back(std::make_unique<Node>(loyalVector[i]));
		nodes[i]->setSimulation(this); // Set simulation reference for queue access
		// Define general
		if (i == generalIndex) {
			this->general = nodes[i].get();
			this->generalId = nodes[i]->getId();
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
	mainLogger->info("N = {} | m = {}", this->totalNumNodes, this->numFaultyNodes);
	mainLogger->info("General: {} ({})", general->getId(), general->getIsLoyal() ? "L" : "T");
	
	std::string lieutenantsStr = "Lieutenants: ";
	for (int i = 0; i < totalNumNodes; i++) {
		if (nodes[i].get() != general) {
			lieutenantsStr += std::to_string(nodes[i]->getId()) + " (" + (nodes[i]->getIsLoyal() ? "L" : "T") + ") ";
		}
	}
	mainLogger->info("{}", lieutenantsStr);
	
	mainLogger->info("Message: {}", MESSAGE_TEXT);
	mainLogger->info("Default message: {}", DEFAULT_MESSAGE_TEXT);
	mainLogger->info("=====================================");

	// Create message from source and send to all other nodes
	Message msg(general->getId(),general->getPrivateKey(), MESSAGE_TEXT);
	general->broadcastMsg(msg);
	
	// Process all queued messages
	// processAllMessages();

	int totalRounds = this->numFaultyNodes + 1;
	while(!this->messageQueue.empty() && totalRounds > 0) {
		processAllMessages();
		totalRounds--;
	}

	std::cout << "=====================================" << std::endl;
}

void Simulation::end() {
	// Should trigger when no more messages can arrive 
	for (int i = 0; i < totalNumNodes; i++) {
		std::string receivedStr = "";
		for (auto x : nodes[i]->getRecievedValues()) {
			receivedStr += x + ", ";
		}
		mainLogger->info("Choice[{}]({}) = {}", nodes[i]->getId(), receivedStr, nodes[i]->choice());
	}

}

// ============== Message Queue Operations ==============

void Simulation::enqueueMessage(const Message& msg, int recipientId) {
	messageQueue.emplace(msg, recipientId);
}

void Simulation::processNextMessage() {
	if (messageQueue.empty()) return;
	
	PendingMessage pending = messageQueue.front();
	messageQueue.pop();
	
	Node* recipient = getNodeById(pending.recipientId);
	if (recipient) {
		recipient->recieveMsg(pending.msg);
	}
}

void Simulation::processAllMessages() {
	while (!messageQueue.empty()) {
		processNextMessage();
	}
}

Node* Simulation::getNodeById(int id) {
	for (auto& node : nodes) {
		if (node->getId() == id) {
			return node.get();
		}
	}
	return nullptr;
}

// ============== Checkpoint Operations ==============

void Simulation::saveCheckpoint(const std::string& filename) {
	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs) {
		std::cerr << "Error: Could not open checkpoint file for writing: " << filename << std::endl;
		return;
	}
	
	// Write simulation parameters
	ofs.write(reinterpret_cast<const char*>(&totalNumNodes), sizeof(totalNumNodes));
	ofs.write(reinterpret_cast<const char*>(&numFaultyNodes), sizeof(numFaultyNodes));
	ofs.write(reinterpret_cast<const char*>(&isGeneralLoyal), sizeof(isGeneralLoyal));
	ofs.write(reinterpret_cast<const char*>(&generalId), sizeof(generalId));
	
	// Write loyalty vector
	for (bool b : savedLoyaltyVector) {
		ofs.write(reinterpret_cast<const char*>(&b), sizeof(b));
	}
	
	// Write each node's received values
	for (const auto& node : nodes) {
		auto receivedValues = node->getRecievedValues();
		size_t numValues = receivedValues.size();
		ofs.write(reinterpret_cast<const char*>(&numValues), sizeof(numValues));
		for (const auto& val : receivedValues) {
			size_t len = val.size();
			ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
			ofs.write(val.data(), len);
		}
	}
	
	// Write message queue
	size_t queueSize = messageQueue.size();
	ofs.write(reinterpret_cast<const char*>(&queueSize), sizeof(queueSize));
	
	// Copy queue to write it (since std::queue doesn't have iterators)
	std::queue<PendingMessage> tempQueue = messageQueue;
	while (!tempQueue.empty()) {
		const PendingMessage& pm = tempQueue.front();
		
		// Write recipient ID
		ofs.write(reinterpret_cast<const char*>(&pm.recipientId), sizeof(pm.recipientId));
		
		// Write message
		ofs.write(reinterpret_cast<const char*>(&pm.msg.senderId), sizeof(pm.msg.senderId));
		
		size_t payloadLen = pm.msg.payload.size();
		ofs.write(reinterpret_cast<const char*>(&payloadLen), sizeof(payloadLen));
		ofs.write(pm.msg.payload.data(), payloadLen);
		
		// Write history
		size_t historySize = pm.msg.history.size();
		ofs.write(reinterpret_cast<const char*>(&historySize), sizeof(historySize));
		for (int h : pm.msg.history) {
			ofs.write(reinterpret_cast<const char*>(&h), sizeof(h));
		}
		
		// Write signedBy
		size_t signedBySize = pm.msg.signedBy.size();
		ofs.write(reinterpret_cast<const char*>(&signedBySize), sizeof(signedBySize));
		for (int s : pm.msg.signedBy) {
			ofs.write(reinterpret_cast<const char*>(&s), sizeof(s));
		}
		
		// Write signatures
		size_t signaturesSize = pm.msg.signatures.size();
		ofs.write(reinterpret_cast<const char*>(&signaturesSize), sizeof(signaturesSize));
		for (const auto& sig : pm.msg.signatures) {
			size_t sigLen = sig.size();
			ofs.write(reinterpret_cast<const char*>(&sigLen), sizeof(sigLen));
			ofs.write(reinterpret_cast<const char*>(sig.data()), sigLen);
		}
		
		tempQueue.pop();
	}
	
	std::cout << "Checkpoint saved to: " << filename << " (queue size: " << queueSize << ")" << std::endl;
}

void Simulation::loadCheckpoint(const std::string& filename) {
	//std::ifstream ifs(filename, std::ios::binary);
	//if (!ifs) {
	//	std::cerr << "Error: Could not open checkpoint file for reading: " << filename << std::endl;
	//	return;
	//}
	//
	//// Clear existing state
	//nodes.clear();
	//while (!messageQueue.empty()) messageQueue.pop();
	//CertificationBody::getInstance().reset();
	//
	//// Read simulation parameters
	//ifs.read(reinterpret_cast<char*>(&totalNumNodes), sizeof(totalNumNodes));
	//ifs.read(reinterpret_cast<char*>(&numFaultyNodes), sizeof(numFaultyNodes));
	//ifs.read(reinterpret_cast<char*>(&isGeneralLoyal), sizeof(isGeneralLoyal));
	//ifs.read(reinterpret_cast<char*>(&generalId), sizeof(generalId));
	//
	//// Read loyalty vector
	//savedLoyaltyVector.resize(totalNumNodes);
	//for (int i = 0; i < totalNumNodes; i++) {
	//	ifs.read(reinterpret_cast<char*>(&savedLoyaltyVector[i]), sizeof(bool));
	//}
	//
	//// Reset node ID counter so nodes get the same IDs
	//Node::resetIdCounter();
	//
	//// Recreate nodes
	//nodes.reserve(totalNumNodes);
	//for (int i = 0; i < totalNumNodes; i++) {
	//	nodes.push_back(std::make_unique<Node>(numFaultyNodes, savedLoyaltyVector[i]));
	//	nodes[i]->setSimulation(this);
	//}
	//
	//// Find and set general
	//for (auto& node : nodes) {
	//	if (node->getId() == generalId) {
	//		general = node.get();
	//		node->setIsGeneral(true);
	//		break;
	//	}
	//}
	//
	//// Register nodes with certification body
	//CertificationBody& cb = CertificationBody::getInstance();
	//for (auto& node : nodes) {
	//	std::vector<uint8_t> privateKey = cb.registerNode(node->getId());
	//	node->setPrivateKey(privateKey);
	//}
	//
	//// Set other nodes references
	//for (int i = 0; i < totalNumNodes; i++) {
	//	std::vector<Node*> tempNodes;
	//	tempNodes.reserve(totalNumNodes - 1);
	//	for (int j = 0; j < totalNumNodes; j++) {
	//		if (i != j) {
	//			tempNodes.push_back(nodes[j].get());
	//		}
	//	}
	//	nodes[i]->setOtherNodes(tempNodes);
	//}
	//
	//// Read each node's received values
	//for (auto& node : nodes) {
	//	size_t numValues;
	//	ifs.read(reinterpret_cast<char*>(&numValues), sizeof(numValues));
	//	for (size_t v = 0; v < numValues; v++) {
	//		size_t len;
	//		ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
	//		std::string val(len, '\0');
	//		ifs.read(&val[0], len);
	//		node->addReceivedValue(val);
	//	}
	//}
	//
	//// Read message queue
	//size_t queueSize;
	//ifs.read(reinterpret_cast<char*>(&queueSize), sizeof(queueSize));
	//
	//for (size_t i = 0; i < queueSize; i++) {
	//	int recipientId;
	//	ifs.read(reinterpret_cast<char*>(&recipientId), sizeof(recipientId));
	//	
	//	int senderId;
	//	ifs.read(reinterpret_cast<char*>(&senderId), sizeof(senderId));
	//	
	//	size_t payloadLen;
	//	ifs.read(reinterpret_cast<char*>(&payloadLen), sizeof(payloadLen));
	//	std::string payload(payloadLen, '\0');
	//	ifs.read(&payload[0], payloadLen);
	//	
	//	// Read history
	//	size_t historySize;
	//	ifs.read(reinterpret_cast<char*>(&historySize), sizeof(historySize));
	//	std::vector<int> history(historySize);
	//	for (size_t h = 0; h < historySize; h++) {
	//		ifs.read(reinterpret_cast<char*>(&history[h]), sizeof(int));
	//	}
	//	
	//	// Read signedBy
	//	size_t signedBySize;
	//	ifs.read(reinterpret_cast<char*>(&signedBySize), sizeof(signedBySize));
	//	std::set<int> signedBy;
	//	for (size_t s = 0; s < signedBySize; s++) {
	//		int id;
	//		ifs.read(reinterpret_cast<char*>(&id), sizeof(id));
	//		signedBy.insert(id);
	//	}
	//	
	//	// Read signatures
	//	size_t signaturesSize;
	//	ifs.read(reinterpret_cast<char*>(&signaturesSize), sizeof(signaturesSize));
	//	std::vector<std::vector<uint8_t>> signatures(signaturesSize);
	//	for (size_t sig = 0; sig < signaturesSize; sig++) {
	//		size_t sigLen;
	//		ifs.read(reinterpret_cast<char*>(&sigLen), sizeof(sigLen));
	//		signatures[sig].resize(sigLen);
	//		ifs.read(reinterpret_cast<char*>(signatures[sig].data()), sigLen);
	//	}
	//	
	//	// Reconstruct message and enqueue
	//	Message msg = Message::createFromCheckpoint(senderId, payload, history, signedBy, signatures);
	//	messageQueue.emplace(msg, recipientId);
	//}
	//
	//std::cout << "Checkpoint loaded from: " << filename << " (queue size: " << queueSize << ")" << std::endl;
}
