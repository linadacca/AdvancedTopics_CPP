#include "AlgorithmRegistrar.h"

std::unique_ptr<AlgorithmRegistrar> AlgorithmRegistrar::instance = std::make_unique<AlgorithmRegistrar>();

void AlgorithmRegistrar::setFunction(std::function<std::unique_ptr<AbstractAlgorithm>()> function) {
    instance->algorithm = std::move(function);
}

std::function<std::unique_ptr<AbstractAlgorithm>()> AlgorithmRegistrar::getFunction() {
    return instance->algorithm;
}