#include "Message.h"
#include "CertificationBody.h"

Message::Message(int nodeId, std::vector<uint8_t> privateKey, std::string payload) 
    : senderId(nodeId), payload(std::move(payload)) {

	this->history.push_back(nodeId);
	// Sign
    CertificationBody& cb = CertificationBody::getInstance();
    auto signature = cb.signMsg(nodeId, privateKey, this->payload);
    signatures.push_back(signature);
    this->signedBy.insert(nodeId);
}

Message::Message(const Message& other)
    : senderId(other.senderId),
      payload(other.payload),
      signedBy(other.signedBy),
      history(other.history),
      signatures(other.signatures) {
}

std::string Message::getPrintFormat()
{
	std::string result = "";

	for (auto x : this->history) {
		result += std::to_string(x);
		result += ".";
	}

	// remove excess '.'
	result = result.substr(0, result.size() - 1);

	// add payload

	result += "(" + this->payload + ")";
	return result;
}


Message Message::acceptAndSign(int nodeId, std::vector<uint8_t> privateKey, Message& incomingMsg) {
    Message newMsg = Message(incomingMsg);

    // Accept
    newMsg.history.push_back(nodeId);
    newMsg.senderId = nodeId;

    // Sign
    CertificationBody& cb = CertificationBody::getInstance();
    auto signature = cb.signMsg(nodeId, privateKey, newMsg.payload);
    newMsg.signatures.push_back(signature);
    newMsg.signedBy.insert(nodeId);
    
    return newMsg;
}

bool Message::checkValidity() {
	CertificationBody& cb = CertificationBody::getInstance();
	
	for(int i = this->signatures.size() - 1; i >= 0 ; i--) {
		if(!cb.verifySignature(this->history[i],this->signatures[i],this->payload)){
			return false;
		}
	}

	return true;

}

Message Message::createFromCheckpoint(int senderId, const std::string& payload, 
                                       const std::vector<int>& history,
                                       const std::set<int>& signedBy,
                                       const std::vector<std::vector<uint8_t>>& signatures) {
    Message msg;
    msg.senderId = senderId;
    msg.payload = payload;
    msg.history = history;
    msg.signedBy = signedBy;
    msg.signatures = signatures;
    return msg;
}