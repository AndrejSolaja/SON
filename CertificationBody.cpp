#include "CertificationBody.h"
#include <cstring>
#include <iostream>

CertificationBody::CertificationBody() {
}

CertificationBody& CertificationBody::getInstance() {
    static CertificationBody instance;
    return instance;
}

std::vector<uint8_t> CertificationBody::registerNode(int nodeId) {
    /*
    Generates pair of keys for node.
    Returns private key.
    */
    uint8_t privateKey[32];
    uint8_t publicKey[64];
    if (publicKeys.count(nodeId) > 0) {
        std::cerr << "Node: " << nodeId << " has already registred" << std::endl;
        return {};
    }

    if (!uECC_make_key(publicKey, privateKey, curve)) {
        std::cerr << "ECC Error in generating keys" << std::endl;
        return {};
    }
    
    this->publicKeys[nodeId] = std::vector<uint8_t>(publicKey, publicKey + 64);

    return std::vector<uint8_t>(privateKey, privateKey + 32);
}

std::vector<uint8_t> CertificationBody::getPublicKey(int nodeId) {
    /*
        returns public key for specific node
    */
    if (publicKeys.count(nodeId) == 0) {
        std::cerr << "Node: " << nodeId << " has not been registred" << std::endl;
        return {};
    }

    return publicKeys[nodeId];
}

std::vector<uint8_t> CertificationBody::signMsg(int nodeId, std::vector<uint8_t> privateKey, std::string msg) {
    /*
        Signs a message with private key and returns a signature
    */
    uint8_t signature[64];

    if (!uECC_sign(privateKey.data(), (const uint8_t*)msg.c_str(), strlen(msg.c_str()), signature, curve)) {
        std::cerr << "Node: " << nodeId << " failed to sign a msg" << std::endl;
        return {};
    }

    return std::vector<uint8_t>(signature, signature + 64);
}

bool CertificationBody::verifySignature(int nodeId, std::vector<uint8_t> signature, std::string msg) {
    /*
        Checks if users signature is valid
    */
    std::vector<uint8_t> publicKey = this->getPublicKey(nodeId);

    if (uECC_verify(publicKey.data(), (const uint8_t*)msg.c_str(), strlen(msg.c_str()), signature.data(), curve)) {
        return true;
    } else {
        return false;
    }
}

void CertificationBody::reset() {
    publicKeys.clear();
}

void CertificationBody::registerNodeWithKeys(int nodeId, const std::vector<uint8_t>& publicKey) {
    /*
    Register a node with existing public key (for checkpoint restore)
    */
    if (publicKeys.count(nodeId) > 0) {
        std::cerr << "Node: " << nodeId << " has already been registered" << std::endl;
        return;
    }
    
    this->publicKeys[nodeId] = publicKey;
}