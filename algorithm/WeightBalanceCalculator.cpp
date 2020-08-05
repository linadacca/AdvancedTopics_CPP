#include "WeightBalanceCalculator.hpp"


int WeightBalanceCalculator::readShipPlan(const std::string& full_path_and_file_name) {
	// remove compiler warning 
	full_path_and_file_name.compare("");
	return 0;
}


WeightBalanceCalculator::BalanceStatus tryOperation(char loadUnload, int kg, int x, int y){
    if(loadUnload == 'a' && kg == 1 && x == 2 && y == 2){

    }
    return WeightBalanceCalculator::BalanceStatus::APPROVED;
}
