#ifndef CHESS_AI_BASE_H
#define CHESS_AI_BASE_H

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include "ChessTypes.h"

// Universal base class for all custom AI engines
class ChessAIBase {
public:
    virtual ~ChessAIBase() = default;
    
    // Pure virtual method each friend must implement
    virtual ChessMove makeMove(const std::vector<std::vector<char>>& board, char aiColor) = 0;
};

// Global Registry Factory System to map string names to AI classes dynamically
class AIRegistry {
public:
    using CreatorFunc = std::function<std::unique_ptr<ChessAIBase>()>;

    static AIRegistry& getInstance() {
        static AIRegistry instance;
        return instance;
    }

    void registerAI(const std::string& name, CreatorFunc creator) {
        registryMap[name] = creator;
    }

    std::unique_ptr<ChessAIBase> createAI(const std::string& name) {
        if (registryMap.find(name) == registryMap.end()) return nullptr;
        return registryMap[name]();
    }

    void printRegisteredNames() const {
        for (const auto& [name, _] : registryMap) {
            std::cout << " - " << name << "\n";
        }
    }

private:
    std::map<std::string, CreatorFunc> registryMap;
    AIRegistry() = default;
};

// Automation Macro to make registry simple for your friends
// Automation Macro updated to explicitly return the base class unique_ptr type
#define REGISTER_CHESS_AI(ClassName, StringName) \
    static bool ClassName##_registered = []() { \
        AIRegistry::getInstance().registerAI(StringName, []() -> std::unique_ptr<ChessAIBase> { \
            return std::make_unique<ClassName>(); \
        }); \
        return true; \
    }();

#endif
