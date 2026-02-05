#pragma once

#include <string>
#include <vector>
#include <set>


class Message {
public:
    int senderId;
    std::string payload;
    std::set<int> signedBy; 
    std::vector<int> history;
    std::vector<std::vector<uint8_t>> signatures;

    Message(int nodeId, std::vector<uint8_t> privateKey, std::string payload);
    Message(const Message& other);

    std::string getPrintFormat();

    static Message acceptAndSign(int nodeId, std::vector<uint8_t> privateKey, Message& incomingMsg);
    static Message createFromCheckpoint(int senderId, const std::string& payload, 
                                        const std::vector<int>& history,
                                        const std::set<int>& signedBy,
                                        const std::vector<std::vector<uint8_t>>& signatures);
    bool checkValidity();
    
private:
    // Private constructor for checkpoint restoration
    Message() : senderId(0) {}
};
