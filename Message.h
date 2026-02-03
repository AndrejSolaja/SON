#pragma once

#include <string>
#include <vector>

class Message {
public:
    int senderId;
    std::string payload;

    std::vector<std::string> history;

    std::string getPrintFormat();

    Message(int s, std::string p) : senderId(s), payload(std::move(p)) {}

    void addHistory(int id);
};
