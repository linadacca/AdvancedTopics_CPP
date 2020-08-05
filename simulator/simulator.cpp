#include"simulator.h"

bool is_number(const std::string& s);

/* parse string with delimiter',' into a string vector*/
void tokenize(std::string const& str, std::vector<std::vector<std::string>>& out);

/*comparing two strings of the form: <5LETTERS>_<non-negative int> */
bool numeric_string_compare(const std::string& s1, const std::string& s2);


//Case - insensitive string comparison
int case_insensitive_match(std::string s1, std::string s2);


//return true if string is in correct port ID format
bool isCargoDataFormat(std::string id);

//we used a simpler function than the given one for the second causes out of  range exception
bool isISO6346Format(std::string id);


Simulator::Simulator() {
}

Simulator::~Simulator() = default;

void Simulator::resetParam() {
	operCounter = 0;
	portContMap.clear();
	idContMap.clear();
	portFreqMap2EH.clear();
	portFreqMap.clear();
	portEncounterFreqMap.clear();
	newErrorFile = true;
	newErrorFileSim = true;
	results = 0;
	numErrors = 0;

    const std::string algoTavelPath = getOutputPath() + getCurrentAlgoName() + "_" + getCurrentTravelName() + "_" + "crane_instructions";

    if (std::filesystem::is_directory(algoTavelPath)) {
        std::filesystem::remove_all(algoTavelPath);
    }
}

/*simulationRun interprets root folder and initializes the algorithm*/
void Simulator::simulationRun(std::string& travel_path, std::unique_ptr<AbstractAlgorithm>& algo) {

	smart_eh = std::make_unique<ErrorHandler>(this);

	bool foundPlan = false;
	bool foundRoute = false;
	resetParam();

	std::string routePath = "";
	std::string planPath = "";
	std::vector<std::string> containersAtEachPortPathVec;

	std::cout << "Reading: " << travel_path << " for algorithm:"<< getCurrentAlgoName() <<std::endl;

	for (auto& file : std::filesystem::directory_iterator(travel_path)) {
		if (file.path().extension().string().compare(".ship_plan") == 0) {/*if file is ship plan*/
			planPath = file.path().string();
			foundPlan = true;
		}
		else if (file.path().extension().string().compare(".route") == 0) {/*if file is ship route*/
			routePath = file.path().string();
			foundRoute = true;
		}
		else if (file.path().extension().string().compare(".cargo_data") == 0) {
			containersAtEachPortPathVec.push_back(file.path().string());/*insert all container paths in a vec*/
		}
	}

	if (!foundPlan) {
		std::cout << "ship plan file is missing for this travel, travel ignored." << std::endl;
		const std::string err = "travel: " + travel_path + " has missing ship plan, travel was ignored.\n";
		generateError(err, 1);
		return;
	}
	else if (!foundRoute) {
		std::cout << "ship route file is missing for this travel, travel ignored." << std::endl;
		const std::string err = "travel: " + travel_path + " has missing route file, travel was ignored.\n";
		generateError(err, 1);
		return;
	}

	simulationRun2(planPath, routePath, algo, containersAtEachPortPathVec);


}











void Simulator::simulationRun2(const std::string& planPath, const std::string& routePath,
	std::unique_ptr<AbstractAlgorithm>& algo, std::vector<std::string>& containersAtEachPortPathVec) {
	WeightBalanceCalculator calculator;

	int retValSim = simReadShipPlan(planPath);
	int retValAlg = algo->readShipPlan(planPath);
	bool continueRun = true;

 	if (retValAlg != 0) {
		// check error combinations 
		if ((retValAlg & (2 << 1)) == 2 << 1) {
			const std::string err = "ship plan: a given position exceeds the X/Y ship limits\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 2)) == 2 << 2) {
			const std::string err = "ship plan: bad line format after first line or duplicate x,y appearance with same data\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 3)) == 2 << 3) {
			const std::string err = "ship plan : travel error - bad first line or file cannot be read altogether\n";
			generateError(err, 2);
			continueRun = false;
		}
		if ((retValAlg & (2 << 4)) == 2 << 4) {
			const std::string err = "ship plan: travel error - duplicate x,y appearance with different data\n";
			generateError(err, 2);
			continueRun = false;
		}
	}

	if (retValSim != 0) {
		if ((retValSim & (2 << 3)) == 2 << 3) {
			continueRun = false;
		}
		if ((retValSim & (2 << 4)) == 2 << 4) {
			continueRun = false;
		}
	}

	if (retValSim != retValAlg) {

		for (int i = 1; i < 5; i++) {
			//interate through the errors and check which error is missing on each side
			if ((retValSim & (2 << i)) != (retValAlg & (2 << i))) {
				const std::string err = "Simulator reported different 2^" + std::to_string(i)
					+ " result from algorithm\n";
				generateError(err, 2);
			}
		}
	}
	// if the sim or the algo retured 2^3 or 2^4 abort travel
	if (!continueRun)
		return;

	std::vector<std::string> simShipRoute;
	retValSim = simReadShipRoute(routePath, simShipRoute);
	retValAlg = algo->readShipRoute(routePath);


	if (retValAlg != 0) {
		// check error combinations 
		if ((retValAlg & (2 << 5)) == 2 << 5) {
			const std::string err = "travel route: a port appears twice or more consecutively\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 6)) == 2 << 6) {
			const std::string err = "travel route: bad port symbol format\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 7)) == 2 << 7) {
			const std::string err = "travel route: travel error - empty file or file cannot be read altogether\n";
			generateError(err, 2);
			continueRun = false;
		}
		if ((retValAlg & (2 << 8)) == 2 << 8) {
			const std::string err = "travel route: travel error - file with only a single valid port\n";
			generateError(err, 2);
			continueRun = false;
		}
	}

	if (retValSim != 0) {
		if ((retValSim & (2 << 7)) == 2 << 7) {
			continueRun = false;
		}
		if ((retValSim & (2 << 8)) == 2 << 8) {
			continueRun = false;
		}
	}

	if (retValSim != retValAlg) {
		for (int i = 5; i < 9; i++) {
			//interate through the errors and check which error is missing on each side
			if ((retValSim & (2 << i)) != (retValAlg & (2 << i))) {
				const std::string err = "Simulator reported different 2^" + std::to_string(i)
					+ " result from algorithm\n";
				generateError(err, 2);
			}
		}
	}
	// if the sim or the algo retured 2^7 or 2^8 abort travel
	if (!continueRun)
		return;

	algo->setWeightBalanceCalculator(calculator);
	

	buildPortMap(simShipRoute); //////                check upper case / lower case .

	for (auto& e : containersAtEachPortPathVec) {/*when this finishes we get a map -> port_name:containers_awaiting*/
		processContainersAtEachPort(e, simShipRoute);
	}

	/*sort portContMap keys(full port name) into the following vector*/
	std::vector<std::string> sortedPortContVec;

	buildIdContMap();

	for (std::map<std::string, std::vector<Container>>::iterator it = portContMap.begin(); it != portContMap.end(); ++it) {
		sortedPortContVec.push_back(it->first);
	}
	std::sort(sortedPortContVec.begin(), sortedPortContVec.end(), numeric_string_compare);

	int routeSize = (int)simShipRoute.size();
	
	for (auto& currentPort : simShipRoute) {
		routeSize--;
		if (shipToNextPort(currentPort, sortedPortContVec, algo, routeSize) < 0)
			//file cannot be opened, continue to next travel
			break;
	}
}





/* go to next port and send the related data to the ship by calling getInstructionsForCargo, and check returned values from algorithm and simulator */
int Simulator::shipToNextPort(std::string& currentPort, std::vector<std::string>& sortedPortContVec, std::unique_ptr<AbstractAlgorithm>& algo, int routeSize) {
	// create empty file to send to algo in case of missing file
	const std::string emptyFilePath = getTravelPath() + "emptyFile_0.cargo_data";
	const std::string algoTavelPath = getOutputPath() + getCurrentAlgoName() + "_" + getCurrentTravelName() + "_" + "crane_instructions";
    // make algo_travel folder
	char* cAlgoTravelPath = new char[algoTavelPath.length() + 1];
    for (size_t i = 0; i < algoTavelPath.length(); i++) {
        cAlgoTravelPath[i] = algoTavelPath[i];
    }
    cAlgoTravelPath[algoTavelPath.length()] = '\0';

    if (mkdir(cAlgoTravelPath, 0777) == -1) {
        //std::cerr << "Error :  " << strerror(errno) << std::endl;
    }
    delete[] cAlgoTravelPath;

    bool foundCargoFile = false;
	int visitNum;
	std::vector<std::string>::iterator it;
	auto it1 = std::find_if(sortedPortContVec.begin(), sortedPortContVec.end(), [&](const std::string& str) { return str.find(currentPort) != std::string::npos; });
	portEncounterFreqMap.at(currentPort)++;
	if (it1 != sortedPortContVec.end() && routeSize != 0) {
		//visitNum = # of current encounters of this port in sortedVec 
		visitNum = portEncounterFreqMap.at(currentPort);
		std::string neededPort = currentPort + "_" + std::to_string(visitNum);
		it = std::find(sortedPortContVec.begin(), sortedPortContVec.end(), neededPort);
		if (it != sortedPortContVec.end()) {
			foundCargoFile = true;
		}
	}
	int retValSim = 0;
	int retValAlg = 0;

	if (foundCargoFile) {
		std::string fullPortName = currentPort + "_" + std::to_string(visitNum);
		const std::string outputPath = algoTavelPath + "/" + fullPortName + ".crane_instructions";
		const std::string inputFilePath = getTravelPath() + getCurrentTravelName() + "/" + fullPortName + ".cargo_data";
		retValAlg = algo->getInstructionsForCargo(inputFilePath, outputPath);

		retValSim = simGetInstructionsForCargo(outputPath, fullPortName);
		if (cargoInstructionsErrorMap.find(fullPortName) != cargoInstructionsErrorMap.end()) {
			retValSim |= cargoInstructionsErrorMap[fullPortName];
		}
		sortedPortContVec.erase(it);
	}
	else {
		std::ofstream emptyFile(emptyFilePath);
		std::string num = std::to_string(portEncounterFreqMap.at(currentPort));
		std::string currentPortName = currentPort + "_" + num;
		if (routeSize != 0) {
			std::cout << "\tmissing cargo File for this visit at port: " << currentPortName << std::endl;
			const std::string err = "missing cargo File for this visit at port: " + currentPort + '\n';
			generateError(err, 1);
		}
		const std::string outputPath = algoTavelPath + "/" + currentPortName + ".crane_instructions";
		retValAlg = algo->getInstructionsForCargo(emptyFilePath, outputPath);

		//check results of get instructions for cargo
		retValSim = simGetInstructionsForCargo(outputPath, currentPortName);
		retValSim |= cargoInstructionsErrorMap[currentPortName];

		if (cargoInstructionsErrorMap.find(currentPortName) != cargoInstructionsErrorMap.end()) {
			retValSim |= cargoInstructionsErrorMap[currentPortName];
		}
	}



	if (retValAlg != 0) {
		// check error combinations 
		if ((retValAlg & (2 << 10)) == 2 << 10) {
			const std::string err = " containers at port: duplicate ID on port\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 11)) == 2 << 11) {
			const std::string err = "containers at port: ID already on ship\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 12)) == 2 << 12) {
			const std::string err = "containers at port: bad line format, missing or bad weight\n";

			generateError(err, 2);
		}
		if ((retValAlg & (2 << 13)) == 2 << 13) {
			const std::string err = "containers at port: bad line format, missing or bad port dest\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 14)) == 2 << 14) {
			const std::string err = "containers at port: bad line format, ID cannot be read\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 15)) == 2 << 15) {
			const std::string err = "containers at port: illegal ID check ISO 6346\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 16)) == 2 << 16) {
			const std::string err = "containers at port: file cannot be read altogether\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 17)) == 2 << 17) {
			const std::string err = "containers at port: last port has waiting containers\n";
			generateError(err, 2);
		}
		if ((retValAlg & (2 << 18)) == 2 << 18) {
			const std::string err = "containers at port: total containers amount exceeds ship capacity\n";
			generateError(err, 2);
		}
	}
	
	if (retValSim != retValAlg) {

		for (int i = 10; i < 19; i++) {
			//interate through the errors and check which error is missing on each side
			if ((retValSim & (2 << i)) != (retValAlg & (2 << i))) {
				const std::string err = "Simulator reported different 2^" + std::to_string(i)
					+ " result from algorithm\n";
				generateError(err, 2);
			}
		}
	}
    if(retValSim < 0){
        return -1;
    }
	return 0;
}





/*get the operations from port output file, and check if the algorithm made any errors*/
int Simulator::simGetInstructionsForCargo(const std::string& outputFile, const std::string& fullPortName) {

	std::vector<std::vector<std::string>> outputVec;
	std::ifstream inFile;
	inFile.open(outputFile);
	if (inFile.fail()) {
		const std::string err = "Error opening file " + outputFile + '\n';
		generateError(err, 1);
		return 2 << 16;
	}
	processFile(inFile, outputVec);
	inFile.close();


	int retVal = smart_eh->checkAllErrors(fullPortName, outputVec);

    if(retVal != -1){
        results += outputVec.size();
    }
    else{
        results = -1;
    }
	return retVal;
}





// interpret ship plan file for simuator use
int Simulator::simReadShipPlan(const std::string& ship_plan_path) {
	int numFloors, numContainersInDimX, numContainersInDimY;
	std::cout << "Reading: " << ship_plan_path << std::endl;
	std::vector <std::vector <std::vector <Container> > >  shipPlanVec;
	std::ifstream inFile;
	std::vector<std::vector<std::string>> dataVec;

	inFile.open(ship_plan_path);
	if (inFile.fail()) {
		std::cerr << "Error opening file " << ship_plan_path << std::endl;
		const std::string err = "ship plan: file cannot be read altogether (cannot run this travel): " + ship_plan_path + '\n';
		generateError(err, 1);
		return 2 << 3;
	}

	processFile(inFile, dataVec);
	if (dataVec.empty()) {
		const std::string err = "ship plan: empty file (cannot run this travel): " + ship_plan_path + '\n';
		generateError(err, 1);
		return 2 << 3; /* empty file */
	}
    inFile.close();
	/*check validity of first line*/
	if (dataVec.at(0).size() != 3) {
		const std::string err = "ship plan: bad first line (cannot run this travel): " + ship_plan_path + '\n';
		generateError(err, 1);
		return 2 << 3;/*bad first line*/
	}
	for (auto& e : dataVec.at(0)) {
		if (!is_number(e)) {
			const std::string err = "ship plan: bad first line (cannot run this travel): " + ship_plan_path + '\n';
			generateError(err, 1);
			return  2 << 3;/*bad first line*/
		}
	}

	numFloors = std::stoi(dataVec.at(0).at(0));
	numContainersInDimX = std::stoi(dataVec.at(0).at(1));
	numContainersInDimY = std::stoi(dataVec.at(0).at(2));

	shipPlanVec.resize(numFloors, std::vector<std::vector<Container>>(numContainersInDimX, std::vector<Container>(numContainersInDimY)));


	initShipPlanVecToZeros(shipPlanVec);
	int retV = setActualNumOfFloors(shipPlanVec, dataVec, numFloors, numContainersInDimX, numContainersInDimY);
    this->simShipPlan = ShipPlan(numFloors, numContainersInDimX, numContainersInDimY, shipPlanVec);

	return retV; // success

}








void Simulator::initShipPlanVecToZeros(std::vector <std::vector <std::vector <Container> > >& shipPlanVec) {

	for (auto& d1 : shipPlanVec) {
		for (auto& d2 : d1) {
			for (auto& d3 : d2) {
				d3.setId("0");
				d3.setWeight(0);
				d3.setDestination("0");
			}
		}
	}
}



/*non existing floors contain a nullContainer with values of -1*/
int Simulator::setActualNumOfFloors(std::vector <std::vector <std::vector <Container> > >& shipPlanVec, std::vector<std::vector<std::string>>& dataVec, int& numFloors, int& numContainersInDimX, int& numContainersInDimY) {
	int floors, x, y;
	Container nullContainer("-1", -1, "-1");
	std::map<int, std::map<int, int>> floorPlanMap;//z -> x,y
	int retVal = 0;
	/*start from second vector, first vector is ship plan dimensions */
	for (size_t i = 1; i < dataVec.size(); i++) {
        if(dataVec.at(i).empty()){
            continue;
        }
		if (dataVec.at(i).size() != 3) {
			std::cout << "invalid number of parameters in ship plan file, line ignored." << std::endl;
			retVal |= 2 << 2;
			const std::string err = "invalid number of parameters in ship plan file, line ignored.\n";
			generateError(err, 1);
			continue;
		}
		int z = 0;
		/*check if all line data contains numbers*/
		x = is_number(dataVec.at(i).at(0)) ? std::stoi(dataVec.at(i).at(0)) : -1;
		y = is_number(dataVec.at(i).at(1)) ? std::stoi(dataVec.at(i).at(1)) : -1;
		floors = is_number(dataVec.at(i).at(2)) ? std::stoi(dataVec.at(i).at(2)) : -1;

		if (x == -1 || y == -1 || floors == -1) {
			std::cout << "Position " << x << "," << y << " is invalid, line ignored" << std::endl;
			const std::string err = "ship plan error: Position " + std::to_string(x) + ',' + std::to_string(y) + " is invalid, line ignored" + '\n';
			generateError(err, 1);
			retVal |= 2 << 2;
			continue;
		}

		if (x >= numContainersInDimX || y >= numContainersInDimY) {
			std::cout << "Position " << x << "," << y << " is invalid because it's outside the range. it'll be ignored." << std::endl;
			const std::string err = "ship plan error: Position " + std::to_string(x) + ',' + std::to_string(y) + " is invalid because it's outside the range. it'll be ignored" + '\n';
			generateError(err, 1);
			retVal |= 2 << 1;
			continue;
		}
		if (floors >= numFloors) {
			std::cout << "Position " << x << "," << y << " has invalid number of actual floors. it'll be ignored." << std::endl;
			const std::string err = "ship plan error: Position " + std::to_string(x) + ',' + std::to_string(y) + " has invalid number of actual floors. it'll be ignored." + '\n';
			generateError(err, 1);
			retVal |= 2 << 1;
			continue;
		}
		if (floorPlanMap.find(x) != floorPlanMap.end() &&
			floorPlanMap[x].find(y) != floorPlanMap[x].end() &&
			floorPlanMap[x][y] != floors) {
			const std::string err = "ship plan error: duplicate " + std::to_string(x) + ',' + std::to_string(y) + " appearance with different data." + '\n';
			generateError(err, 1);
			retVal |= 2 << 4;
			return retVal; /*duplicate x,y appearance with different data*/
		}
		floorPlanMap[x][y] = floors;

		while (z < numFloors - floors ) {
			shipPlanVec.at(z).at(x).at(y) = nullContainer;
			z++;
		}
	}
	return retVal;
}









/*interpret ship route file and returns erros if found*/
int Simulator::simReadShipRoute(const std::string& route_path, std::vector<std::string>& dataVec) {
	std::string line;
	std::ifstream inFile;

	std::cout << "Reading: " << route_path << std::endl;
	int counter = 1; /* for error lines numbers*/
	int retVal = 0;

	inFile.open(route_path);
	if (!inFile) {
		std::cerr << "Error opening file " << route_path << std::endl;
		const std::string err = "travel route error: file cannot be read altogether (cannot run this travel): " + route_path + '\n';
		generateError(err, 1);
		return 2 << 7;
	}
	while (std::getline(inFile, line))
	{
		line.erase(remove(line.begin(), line.end(), ' '), line.end()); // remove all white spaces from string
		line.erase(remove(line.begin(), line.end(), '\t'), line.end()); // remove all tabs from string
		line.erase(remove(line.begin(), line.end(), '\r'), line.end()); // remove all carriage returns from string
		line.erase(remove(line.begin(), line.end(), '\v'), line.end()); // remove all vertical tabs from string
		if (line.length() == 0) {
			counter++;
			continue;
		}
		else if (line.at(0) == '#') {
			counter++;
			continue;
		}
		else if (line.length() != PRT_ID_LEN) {
			std::cout << "invalid number of parameters, line " << counter << " is ignored." << std::endl;
			counter++;
			const std::string err = "travel route error: invalid number of parameters, line " + std::to_string(counter) + " is ignored." + '\n';
			generateError(err, 1);
			retVal |= 2 << 6;
			continue;
		}

		bool valid = true;
		for (auto& l : line) {
			if (!isalpha(l)) {
				std::cout << "invalid port name: " << line << " , it should contain letters only." << std::endl;
				valid = false;
				const std::string err = "travel route error :invalid port name: " + line + " , it should contain letters only." + '\n';
				generateError(err, 1);
				retVal |= 2 << 6;
				break;
			}
		}
		if (!valid) {
			continue;
		}

		if (dataVec.size() != 0 && case_insensitive_match(dataVec.at(dataVec.size() - 1), line)) {
			std::cout << "A port appears twice or more consecutively" << std::endl;
			const std::string err = "travel route error: A port appears twice or more consecutively in file: " + route_path + '\n';
			generateError(err, 1);
			counter++;
			retVal |= 2 << 5;
			continue;
		}
		dataVec.push_back(line);
		counter++;
	}
	inFile.close();
	if (dataVec.empty()) {
		const std::string err = "travel route error:empty file (cannot run this travel): " + route_path + '\n';
		generateError(err, 1);
		retVal |= 2 << 7;
		return retVal;
	}
	else if (dataVec.size() == 1) {
		const std::string err = "travel route error: file with only a single valid port (cannot run this travel): " + route_path + '\n';
		generateError(err, 1);
		retVal |= 2 << 8;
		return retVal;
	}
	return retVal;
}










/*interpret containers awaiting at each port's file*/
void Simulator::processContainersAtEachPort(const std::string& cargo_data_path, std::vector<std::string>& myShipRoute) {

	std::size_t p1 = cargo_data_path.find_last_of("/");
	std::size_t p2 = cargo_data_path.find_last_of(".");

	std::string fullFileName = cargo_data_path.substr(p1 + 1, p2 - p1 - 1);/* fileName_### */
	std::size_t p3 = fullFileName.find_last_of("_");
	std::string fileName = fullFileName.substr(0, p3);/* fileName */

	if (!isCargoDataFormat(fullFileName)) {
		std::cout << "\tinvalid cargo_data file name: " << fullFileName << ", file ignored." << std::endl;
		const std::string err = "invalid cargo_data file name: " + fullFileName + ", file ignored." + '\n';
		generateError(err, 1);
		cargoInstructionsErrorMap[fullFileName] = 2 << 16;
		return;
	}


	if (std::find(myShipRoute.begin(), myShipRoute.end(), fileName) == myShipRoute.end()) {
		/*checks if current port is in ship route*/
		std::cout << "\tport: " << fileName << " is not in current ship route. loaded containers for this port are ignored" << std::endl;
		const std::string err = "port: " + fileName + " is not in current ship route. loaded containers for this port are ignored" + '\n';
		generateError(err, 1);
		cargoInstructionsErrorMap[fullFileName] = 0;
		return;
	}
	else if (myShipRoute.at(myShipRoute.size() - 1) == fileName) {
		if (portFreqMap[fileName] == 1) {
			std::cout << "\tLast stop must not have any containers awaiting. file " << fullFileName << " is ignored" << std::endl;
			const std::string err = "Last stop must not have any containers awaiting. file: " + fullFileName + " is ignored" + '\n';
			generateError(err, 1);
			cargoInstructionsErrorMap[fullFileName] = 2 << 17;
			return;
		}
	}
	portFreqMap[fileName]--;

	std::vector<Container> containersVec;/*has containers for 1 port at a time*/
	std::ifstream inFile;
	inFile.open(cargo_data_path);
	if (inFile.fail()) {
		std::cerr << "Error opening file " << cargo_data_path << std::endl;
		const std::string err = "Error opening file " + cargo_data_path + '\n';
		generateError(err, 1);
		cargoInstructionsErrorMap[fullFileName] = 2 << 16;
		return;
	}
	std::cout << "\tReading: " << cargo_data_path << std::endl;
	processContainersAtEachPort2(inFile, containersVec, fullFileName);
	portContMap.insert({ fullFileName, containersVec });
	inFile.close();
}






void Simulator::processContainersAtEachPort2(std::ifstream& inFile, std::vector<Container>& containersVec,
	const std::string& fullFileName) {
	std::vector<std::vector<std::string>> dataVec;
	int counter = 0;
	int weight;
	std::string id, destP;
	processFile(inFile, dataVec);/*dataVec will contain vectors of: str ID, int Weight, str Dest Port*/

	for (auto& vec : dataVec) {
		counter++;
		if (vec.size() == 0) {
			continue;
		}
		if (vec.size() != 3) {
			std::cout << "\tinvalid container values in line " << counter << std::endl;
			const std::string err = "cargo file error: invalid container values in line " + std::to_string(counter) + '\n';
			generateError(err, 1);
			continue;
		}

		id = vec.at(0);
		weight = is_number(vec.at(1)) ? std::stoi(vec.at(1)) : -1;
		destP = vec.at(2);
		bool cont = false;
		if (weight == -1) {
			std::cout << "\tinvalid container weight value in line " << counter << std::endl;
			const std::string err = "cargo file error: invalid container weight value in line " + std::to_string(counter) + '\n';
			generateError(err, 1);
			cargoInstructionsErrorMap[fullFileName] = 2 << 12;
			cont = true;
		}
		if (destP.length() != PRT_ID_LEN) {
			std::cout << "\tinvalid port dest value in line " << counter << std::endl;
			const std::string err = "cargo file error: invalid port dest value in line " + std::to_string(counter) + '\n';
			generateError(err, 1);
			cargoInstructionsErrorMap[fullFileName] = 2 << 13;
			cont = true;
		}
		if (id.length() != CON_ID_LEN) {
			std::cout << "\tinvalid container ID value in line " << counter << std::endl;
			const std::string err = "cargo file error: invalid container ID value in line " + std::to_string(counter) + '\n';
			generateError(err, 1);
			cargoInstructionsErrorMap[fullFileName] = 2 << 14;
			cont = true;
		}
		if (cont)
			continue;
		Container container(id, weight, destP);/*init new container*/
		bool found = false;
		for (std::vector<Container>::iterator it = containersVec.begin(); it != containersVec.end(); it++) {
			if (container.getId() == (*it).getId()) {
				std::cout << "\tduplicate ID on port (ID " << id << " rejected)" << std::endl;
				found = true;
				const std::string err = "duplicate ID on port (ID " + id + " rejected)" + '\n';
				generateError(err, 1);
				cargoInstructionsErrorMap[fullFileName] = 2 << 10;
				break;
			}
		}
		if (!found && isISO6346Format(container.getId())) {/*if no duplicates found and id is in ISO6346 format*/
			containersVec.push_back(container);/*add new container to vec*/
		}
		else
			cargoInstructionsErrorMap[fullFileName] = 2 << 15;
	}
}






/*this function collects the data file into a string vector */
void Simulator::processFile(std::ifstream& inFile, std::vector<std::vector<std::string>>& dataVec) {
	int counter = 0;
	std::string line;

	while (std::getline(inFile, line))
	{
		counter++;
		line.erase(remove(line.begin(), line.end(), ' '), line.end()); // remove all white spaces from string
		line.erase(remove(line.begin(), line.end(), '\t'), line.end()); // remove all tabs from string
		line.erase(remove(line.begin(), line.end(), '\r'), line.end()); // remove all carriage returns from string
		line.erase(remove(line.begin(), line.end(), '\v'), line.end()); // remove all vertical tabs from string


		if (line.length() == 0) {
			std::cout << "\tline " << counter << " has invalid number of parameters, it'll be ignored." << std::endl;
			dataVec.push_back(std::vector<std::string> {});
			continue;
		}
		else if (line.at(0) == '#') {
			dataVec.push_back(std::vector<std::string> {});
			continue;
		}
		else {
			tokenize(line, dataVec);
		}
	}
}



void Simulator::setWeightBalanceCalculator(WeightBalanceCalculator& calculator) {
	calculator = calculator;

}




/*counts # of port appearances in route*/
void Simulator::buildPortMap(std::vector<std::string>& myShipRoute) {
	for (auto& port : myShipRoute)
	{
		if (portFreqMap.find(port) != portFreqMap.end()) {
			portFreqMap[port]++;
			portFreqMap2EH[port]++;

		}
		else {
			portFreqMap.insert({ port,1 });
			portFreqMap2EH.insert({ port,1 });
			portEncounterFreqMap.insert({ port,0 });
		}
	}

}






bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}





void tokenize(std::string const& str, std::vector<std::vector<std::string>>& out)
{
	size_t start;
	size_t end = 0;
	std::vector<std::string> tmp;

	while ((start = str.find_first_not_of(",", end)) != std::string::npos)
	{
		end = str.find(",", start);
		tmp.push_back(str.substr(start, end - start));
	}
	out.push_back(tmp);

}





bool numeric_string_compare(const std::string& s1, const std::string& s2)
{
	std::string::const_iterator it1 = s1.begin(), it2 = s2.begin();
	bool res1 = std::lexicographical_compare(it1, s1.begin() + 5, it2, s2.begin() + 5);
	bool res2 = std::lexicographical_compare(it2, s2.begin() + 5, it1, s1.begin() + 5);
	if (!(!res1 && !res2)) {
		return res1;
	}
	std::string tmp1 = s1.substr(6, s1.length() - 1); // 6 is index of '_'
	std::string tmp2 = s2.substr(6, s2.length() - 1);
	int n1 = std::stoi(tmp1);
	int n2 = std::stoi(tmp2);
	return n1 < n2;
}





/*sets the idCont map */
void Simulator::buildIdContMap() {
	std::vector<Container> contVec;
	for (auto& e : portContMap) {
		contVec = e.second;
		for (auto& cont : contVec) {
			idContMap.insert({ cont.getId() ,cont });
		}
	}
}









bool isCargoDataFormat(std::string fullId) {
	std::string id = fullId.substr(0, 4);// id= abcde , fullId= abcde_23

	for (auto& l : id) {
		// letter is in not alphabet 
		if (!isalpha(l)) {
			return false;
		}
	}
	if (fullId.at(5) != '_') {
		return false;
	}
	if (!is_number(fullId.substr(6, fullId.length() - 1))) {
		return false;
	}
	return true;
}








int case_insensitive_match(std::string s1, std::string s2) {
	//convert s1 and s2 into lower case strings
	transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
	transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
	if (s1.compare(s2) == 0) {
		return 1; //The strings are the same
	}
	return 0; //not matched
}






// for output files
void Simulator::setOutputPath(const std::string& outputpath) {
	output_path = outputpath;
}


std::string Simulator::getOutputPath() {
	return output_path;
}


void Simulator::setCurrentTravelName(const std::string& travelName) {
	currentTravelName = travelName;
}


std::string Simulator::getCurrentTravelName() {
	return currentTravelName;
}


void Simulator::setCurrentAlgoName(const std::string& algoName) {
	currentAlgoName = algoName;
}


std::string Simulator::getCurrentAlgoName() {
	return currentAlgoName;
}


void Simulator::setTravelPath(const std::string& travelPath) {
	travel_path = travelPath;
}


std::string Simulator::getTravelPath() {
	return travel_path;
}



std::vector <std::vector <std::vector <Container> >>& Simulator::getSimShipPlan() {
	return simShipPlan.getShipPlan();
}



void Simulator::setSimShipPlan(std::vector <std::vector <std::vector <Container> >>  shipVecAfter) {
	simShipPlan.getShipPlan() = shipVecAfter;
}





bool isISO6346Format(std::string id) {
    std::string ownerCode = id.substr(0, 2);
    char categoryIdentifier = id.at(3);
    std::string strNumber = id.substr(4, 10);

    for (auto& l : ownerCode) {
        // letter is in not alphabet or not capital letter
        if (!isalpha(l) || !isupper(l)) {
            std::cout << "\trejected container, illegal ISO 6346 " << id << '\n' << std::endl;
            return false;
        }
    }
    if (categoryIdentifier != 'U' && categoryIdentifier != 'J' && categoryIdentifier != 'Z') {
        std::cout << "\trejected container, illegal ISO 6346 " << id << '\n' << std::endl;
        return false;
    }
    // rest of id is not a number
    if (!is_number(strNumber)) {
        std::cout << "\trejected container, illegal ISO 6346 " << id << '\n' << std::endl;
        return false;
    }
    return true;
}








/*create error files if needed and write the sent error , dest = 1 for sim error file, and 2 for algo error file*/
void Simulator::generateError(const std::string& msg, int dest) {
	const std::string errorFolderPath = getOutputPath() + "errors";

	struct stat buffer;
	if (stat(errorFolderPath.c_str(), &buffer) != 0) { // if error folder doesnt exists then create it
		char* cErrorFolderPath = new char[errorFolderPath.length() + 1];
		for (size_t i = 0; i < errorFolderPath.length(); i++) {
			cErrorFolderPath[i] = errorFolderPath[i];
		}
		cErrorFolderPath[errorFolderPath.length()] = '\0';
		if (mkdir(cErrorFolderPath, 0777) == -1) {
			//	std::cerr << "Error :  " << strerror(errno) << std::endl;
		}
		delete[] cErrorFolderPath;
	}

	if (dest == 1) { // if it's a simulator error
		const std::string simErrorFilePath = errorFolderPath + "/" + "simulationErr.txt";
		std::ofstream err(simErrorFilePath, std::ios::app);

		if (!err) {
			std::cerr << "Error: simulator error file to write to could not be opened" << std::endl;
			exit(1);
		}
		if (newErrorFileSim) {
			err << "Errors found by Simulator in: " << getCurrentTravelName() << " for algorithm: " << getCurrentAlgoName() << std::endl;
			newErrorFileSim = false;
		}
		err << msg << std::endl;
		err.close();
	}

	else {// if it's algorithm error
		const std::string algoErrorFilePath = errorFolderPath + "/" + "algorithmErr.txt";
		std::ofstream err(algoErrorFilePath, std::ios::app);

		if (!err) {
			std::cerr << "Error: algoirthm error file to write to could not be opened" << std::endl;
			exit(1);
		}
		if (newErrorFile) {
			err << getCurrentAlgoName()<<", " << getCurrentTravelName()<< " errors:" << std::endl;
			newErrorFile = false;
		}
		err << msg << std::endl;
		err.close();
		numErrors++;
	}

}
