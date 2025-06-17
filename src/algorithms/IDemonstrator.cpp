#include "IDemonstrator.h"
#include <map>

std::map<std::wstring, std::function<std::unique_ptr<IDemonstrator>()>> AlgorithmRegistry::demonstrator_constructors;

void AlgorithmRegistry::registerAlgorithm(const std::wstring& name_ref, std::function<std::unique_ptr<IDemonstrator>()> demonstrator_constructor) {
    demonstrator_constructors[name_ref] = demonstrator_constructor;
}

std::vector<std::wstring> AlgorithmRegistry::getAvailableAlgorithms() {
    std::vector<std::wstring> names;
    for (const auto& pair_ref : demonstrator_constructors) {
        names.push_back(pair_ref.first);
    }
    return names;
}

std::unique_ptr<IDemonstrator> AlgorithmRegistry::createAlgorithm(const std::wstring& name_ref) {
    auto iterator = demonstrator_constructors.find(name_ref);
    if (iterator != demonstrator_constructors.end()) {
        return iterator->second();
    }
    return nullptr;
}
