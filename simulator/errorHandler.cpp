#include"errorHandler.h"


ErrorHandler::ErrorHandler(Simulator* sim) {
	this->sim = sim;
}



/* prase the output crane insttruction file from the algorithm and update simulator ship plan accordingly, and run all functions in this class*/
int ErrorHandler::checkAllErrors(const std::string& fullPortName, std::vector<std::vector<std::string>>& outputVec) {


    std::size_t t = fullPortName.find_last_of("_");
    std::string currentPort = fullPortName.substr(0, t); // currentPort = port name only
    auto& shipVecAfter = sim->getSimShipPlan(); // vector to be changed and returned

    int val = 0;
    if (checkIndexBounds(outputVec, shipVecAfter) < 0) {
        return -1;
    }

    Container nullCont("0", 0, "0");
    std::string id, op, floor, x, y, dest, zero, im, xm, ym;

    for (auto& vec : outputVec) {
        int parVal =parser(vec, id, op, floor, x, y, im, xm, ym);
        if(parVal==-1){
            continue;
        }
        if (op.compare("M") == 0) {
            shipVecAfter.at(stoi(floor)).at(stoi(x)).at(stoi(y)) = nullCont;
            if(!(shipVecAfter.at(stoi(im)).at(stoi(xm)).at(stoi(ym))==nullCont)){
                const std::string err = "Algorithm made invalid move operation on container: " + id + '\n';
                sim->generateError(err, 2);
                return -1;
            }
            shipVecAfter.at(stoi(im)).at(stoi(xm)).at(stoi(ym)) = sim->getIdContMap().at(id);
        }

        if (op.compare("U") == 0) {
            if (legalUnload2(shipVecAfter,floor,x,y,id) < 0) {
                return -1;
            }
            // if no containers were above it then empty the space
            shipVecAfter.at(stoi(floor)).at(stoi(x)).at(stoi(y)) = nullCont;
        }

        if (op.compare("L") == 0) {
           if(legalLoad2(shipVecAfter,floor,x,y,id) < 0) {
               return -1;
           }
            // load container on ship
            if(invalidAccess(id)){
                return -1;
            }
            shipVecAfter.at(stoi(floor)).at(stoi(x)).at(stoi(y)) = sim->getIdContMap().at(id);;
        }

    }



    if (rightDestPort(currentPort, outputVec) < 0) {
        return -1;
    }

    if (restOfRoute(currentPort, outputVec) < 0) {
        return -1;
    }



	int count = availableSpaceCount(shipVecAfter);

	if (0 != count) {
		if (count - forgotContainer(fullPortName, outputVec) < 0) {
			const std::string err = "containers at port: total containers amount exceeds ship capacity\n";
			sim->generateError(err, 1);
			val |= 2 << 18;
		}
	}

	sim->setSimShipPlan(shipVecAfter);
	return val;
}





/* returns true if ship is full */
int ErrorHandler::availableSpaceCount(std::vector <std::vector <std::vector <Container>>>& shipVecAfter) {

	int counterEmpty = 0;
	Container nullCont("0", 0, "0");


	for (size_t floor = 0; floor < shipVecAfter.size() ; floor++) {
		for (size_t x = 0; x < shipVecAfter.at(floor).size(); x++) {
			for (size_t y = 0; y < shipVecAfter.at(floor).at(x).size(); y++) {
				if (shipVecAfter.at(floor).at(x).at(y) == nullCont) {
					counterEmpty++;
				}
			}
		} 
	}
	return counterEmpty ;
}



/*make sure that dest of loaded container is in the rest of ships route */
int ErrorHandler::restOfRoute(const std::string& currentPort, std::vector<std::vector<std::string>>& outputVec) {
	std::string id, op, floor, x, y, dest, zero, im, xm, ym;
	std::vector<std::string> loadedContDest;
	for (auto& vec : outputVec) {
		int parVal=parser(vec, id, op, floor, x, y, im, xm, ym);
        if(parVal==-1){
            continue;
        }
		if (op.compare("L") == 0) {
			if (invalidAccess(id)) {
				return -1;
			}
			dest = sim->getIdContMap().at(id).getDestination();
			if (std::find(loadedContDest.begin(), loadedContDest.end(), dest) == loadedContDest.end()) {
				loadedContDest.push_back(dest);
			}
			if (sim->getPortFreqMap2().at(dest) < 1) {

				const std::string err = "destination of container : " + id + " is not in the rest of ship's rout." + '\n';
				sim->generateError(err, 2);
			}
		}
	}
	sim->getPortFreqMap2().at(currentPort)--;
    return 0;
}








/*returns true if algorithms tried accessing non-existing container*/
bool ErrorHandler::invalidAccess(const std::string& id) {

	if (sim->getIdContMap().find(id) == sim->getIdContMap().end()) {
		const std::string err = "Algorithm tried accessing non-existing container: " + id + '\n';
		sim->generateError(err, 2);
		return true;
	}
	return false;
}










/* check if the unloading operations are legal i.e. no containers were above it before unloading it*/
int ErrorHandler::legalUnload2( std::vector <std::vector <std::vector <Container>>>& shipVec,
        const std::string& floor,const std::string& x,const std::string& y,const std::string& id){

    Container nullCont("0", 0, "0");
    for (size_t i = stoi(floor) + 1; i < shipVec.size(); i++) {
		// check all containers above current one
		if (!(shipVec.at(i).at(stoi(x)).at(stoi(y)) == nullCont) ) {// if a container exists above current when unloading
			const std::string err = "Invalid unload operation for container: " + id + '\n';
			sim->generateError(err, 2);
			return -1;
    	}
	}
    return 0;
}







/* check if the loading operations are legal i.e. containers loaded into an available space and on top of a container*/
int ErrorHandler::legalLoad2( std::vector <std::vector <std::vector <Container>>>& shipVec,
                              const std::string& floor,const std::string& x,const std::string& y,const std::string& id){

	Container nullCont("0", 0, "0");
	Container noSpace("-1", -1, "-1");
    // illegal if loading to no space/ full space/ above empty space
    if (shipVec.at(stoi(floor)).at(stoi(x)).at(stoi(y)) == noSpace ||(stoi(floor) != 0 && ((!(shipVec.at(stoi(floor)).at(stoi(x)).at(stoi(y)) == nullCont)) ||
        (shipVec.at(stoi(floor) - 1).at(stoi(x)).at(stoi(y)) == nullCont)))) {
        const std::string err = "Invalid load operation for container : " + id + '\n';
        sim->generateError(err, 2);
        return -1;
    }
    for (size_t floor = 0; floor < shipVec.size() - 1; floor++) {
        for (size_t x = 0; x < shipVec.at(floor).size(); x++) {
            for (size_t y = 0; y < shipVec.at(floor).at(x).size(); y++) {
                if (id.compare(shipVec.at(floor).at(x).at(y).getId()) == 0) {
                    const std::string err = "containers at port: duplicate ID on ship\n";
                    sim->generateError(err, 2);
                    return -1;
                }
            }
        }
    }
    shipVec.at(stoi(floor)).at(stoi(x)).at(stoi(y)) = sim->getIdContMap().at(id);
	return 0;
}

















// all unloaded containers from algorithm output are meant for the current port
// output(per port) format per line :<L / U / M / R>, <container id>, <floor index>, <X index>, <Y index> , <floor index>, <X index>, <Y index>
int ErrorHandler::rightDestPort(const std::string& currentPort, std::vector<std::vector<std::string>>& outputVec) {
	std::string id, op, floor, x, y, zero, im, xm, ym;
	//insert containers that were unloaded and then delete them if they were loaded again,
	// if the vector was still full then notify the user
	std::vector<std::string> containerfrequ;

	for (auto& vec : outputVec) {
		int parVal=parser(vec, id, op, floor, x, y, im, xm, ym);
        if(parVal==-1){
            continue;
        }
		if (op.compare("R") != 0 && invalidAccess(id)) {// check container exists
			const std::string err = "container : " + id + " should be rejected but was not. " + '\n';
			sim->generateError(err, 2);
			return -1;
		}
        if(invalidAccess(id)){
            continue;
        }
		Container c = sim->getIdContMap().at(id);

		if (op.compare("U") == 0) {
			containerfrequ.push_back(c.getId());
		}
		else if (op.compare("L") == 0) {
			//check if the container was unloaded before hand and then loaded back
			auto it = std::find(containerfrequ.begin(), containerfrequ.end(), id);
			if (it != containerfrequ.end()) {
				containerfrequ.erase(it);
			}
		}
	}

	if (containerfrequ.size() != 0) {
		//vector was not empty, i.e. its content was loaded at the wrong port
		for (auto& str : containerfrequ) {
			Container c = sim->getIdContMap().at(str);
			if (0 != (c.getDestination().compare(currentPort))) {
				//res << "Container with id: " << str << " was unloaded at the wrong port " << currentPort << '\n';
				const std::string err = "Container with id: " + id + " was unloaded at the wrong port: " + currentPort + '\n';
				sim->generateError(err, 2);
				return -1;
			}
		}
	}
	return 0;
}









/*checks if containers to be loaded on current port haven't been loaded*/
int ErrorHandler::forgotContainer(const std::string& fullPortName, std::vector<std::vector<std::string>>& outputVec) {
	std::string id, op, floor, x, y, im, xm, ym;;

	auto it = sim->getPortContMap().find(fullPortName);
	//if path not found within the map, exit the test
	if (sim->getPortContMap().end() == it) {
	    //TODO
		std::cout << "port " << fullPortName << " not found" << std::endl;
		const std::string err = "port " + fullPortName + " not found\n";
		sim->generateError(err, 2);
		return 0;
	}
	auto & vec = sim->getPortContMap().at(fullPortName);

	for (auto& data : outputVec) {
		int parVal=parser(data, id, op, floor, x, y, im, xm, ym);
        if(parVal==-1){
            continue;
        }
		if (op.compare("L") != 0 && op.compare("R") != 0) {
			continue;
		}
		// iterate through loaded containers waiting vec and check if we find the required container
		for (std::vector<Container> ::iterator it = vec.begin(); it != vec.end();) {
			if (id.compare((*it).getId()) == 0) {
				it = vec.erase(it);
				continue;
			}
			it++;
		}
	}

	for (auto& v : vec) {
		const std::string err = "Container with id: " + v.getId() + " was not loaded from port : " + fullPortName + '\n';
		sim->generateError(err, 2);
		return -1;
	}
	return vec.size();
}








/*does the operations written in port output file, just like the algorithm, and returns the ship plan after adjustments*/
int ErrorHandler::checkIndexBounds(std::vector<std::vector<std::string>>& outputVec, std::vector<std::vector <std::vector <Container>>>& retAfterVec) {

	Container nullCont("0", 0, "0");
	Container cont;
	std::string id, op, floor, x, y, im, xm, ym;
	
	
	for (auto& vec : outputVec) {
		int parVal =parser(vec, id, op, floor, x, y, im, xm, ym);
        if(parVal==-1){
            continue;
        }
		if(op.compare("R") == 0){
			continue;
		}
		if ((size_t)stoi(floor) > retAfterVec.size() || (size_t)stoi(x) > retAfterVec.at(0).size() || (size_t)stoi(y) > retAfterVec.at(0).at(0).size()) {
			const std::string err = "ship index out of bounds in crane_instructions file, line ignored.\n";
			sim->generateError(err, 2);
			return -1;
		}
		if (op.compare("M") == 0) {

            if ((size_t)stoi(im) > retAfterVec.size() || (size_t)stoi(xm) > retAfterVec.at(0).size() ||
            (size_t)stoi(ym) > retAfterVec.at(0).at(0).size()) {
                const std::string err = "ship index out of bounds in crane_instructions file, line ignored.\n";
                sim->generateError(err, 2);
                return -1;
            }
		}
	}

	return 0;
}


/*gets the container data from output file*/
int ErrorHandler::parser(std::vector<std::string>& data, std::string& id, std::string& op, std::string& floor, std::string& x, std::string& y, std::string& im, std::string& xm, std::string& ym) {

	op = data.at(0);

	id = data.at(1);
	
	if(op.compare("R") == 0){
		return 0;
	}
    if (data.size() < 2) {
        return -1;
    }
	floor = data.at(2);
	x = data.at(3);
	y = data.at(4);

	if (data.size() > 5) { // if it's move operations
		im = data.at(5);
		xm = data.at(6);
		ym = data.at(7);
	}
	return 0;
}
