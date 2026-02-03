#pragma once

#include <string>

class Message {
public:
    int senderId;
    std::string payload;

    Message(int s, std::string p) : senderId(s), payload(std::move(p)) {}
};
