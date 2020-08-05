#include <utility>
#include "AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"
AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> function) {
    AlgorithmRegistrar::setFunction(std::move(function));
}
