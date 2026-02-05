#pragma once

#include "micro-ecc/uECC.h"
#include <map>
#include <vector>
#include <string>

class CertificationBody
{
public:
    static CertificationBody& getInstance();
    
    std::vector<uint8_t> registerNode(int nodeId);
    std::vector<uint8_t> getPublicKey(int nodeId);
    std::vector<uint8_t> signMsg(int nodeId, std::vector<uint8_t> privateKey, std::string msg);
    bool verifySignature(int nodeId, std::vector<uint8_t> signature, std::string msg);
    
    // Register node with existing keys (for checkpoint restore)
    void registerNodeWithKeys(int nodeId, const std::vector<uint8_t>& publicKey);
    
    // Clear all registered nodes (for checkpoint restore)
    void reset();

private:
    CertificationBody();
    
    const struct uECC_Curve_t* curve = uECC_secp256r1();
    std::map<int, std::vector<uint8_t>> publicKeys;
};

