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

    std::string getPrintFormat();

    static Message acceptAndSign(int nodeId, std::vector<uint8_t> privateKey, Message& incomingMsg);
    bool checkValidity();
};
