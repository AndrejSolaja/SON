#pragma once

#include <string>
#include <vector>
#include <set>

class Message {
public:
    int senderId;
    std::string payload;
    std::vector<std::string> history;
    std::string getPrintFormat();
    std::set<int> signedBy;

    Message(int s, std::string p) : senderId(s), payload(std::move(p)) {}

    void addHistory(int id);
};
