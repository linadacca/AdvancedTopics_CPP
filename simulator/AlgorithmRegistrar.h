#pragma once
#include <functional>
#include <memory>
#include "AbstractAlgorithm.h"

class AlgorithmRegistrar {
    std::function<std::unique_ptr<AbstractAlgorithm>()> algorithm;
    static std::unique_ptr<AlgorithmRegistrar> instance;
public:
    static void setFunction(std::function<std::unique_ptr<AbstractAlgorithm>()> function);
    static std::function<std::unique_ptr<AbstractAlgorithm>()> getFunction();
};