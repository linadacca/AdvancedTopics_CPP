#pragma once

#include "WeightBalanceCalculator.hpp"
#include "AbstractAlgorithm.h"
#include"errorHandler.h"

#include <fstream>
#include <iostream>
#include<map>
#include<algorithm>
#include <filesystem>
#include<sys/stat.h>
#include <bits/stdc++.h> // ON LINUX ONLY
#include <sys/types.h> 


const int PRT_ID_LEN = 5;
const int CON_ID_LEN = 11;
/*
const std::string resultsFileDir = "simulation.results";
const std::string errorFileDir = "simulation.errors";*/

class ErrorHandler;	//forward declaration

class Simulator {
	std::string output_path;
	std::string travel_path;
	std::string currentTravelName = "NULL";
	std::string currentAlgoName = "NULL";
	ShipPlan simShipPlan; // this one's for the simulator
	bool newErrorFileSim = true;
	bool newErrorFile = true;
	int operCounter = 0;/* counts # operations per travel*/
	std::unique_ptr<ErrorHandler> smart_eh = nullptr;
	ErrorHandler* eh = nullptr;

	std::map<std::string, std::vector<Container>> portContMap;// full port name:containers vector
	std::map<std::string, Container> idContMap; // maps the id to the relevant container object
	std::map<std::string, int> portFreqMap;/*port : # times it appeared in route*/
	std::map<std::string, int> portFreqMap2EH;/*port : # times it appeared in route - for error handler*/
	std::map<std::string, int> portEncounterFreqMap;/*port : # times it appeared in route -(doesn'y change) for cargo instructions function */
	std::map<std::string, int> cargoInstructionsErrorMap;/* gathers containers at port errors at processContainersAtEachPort and compares them with the errros of algo->getInstructionsForCargo*/
	int results = 0;
	int numErrors = 0;

public:

	Simulator();
	~Simulator();

	void resetParam();

	void simulationRun(std::string& path, std::unique_ptr<AbstractAlgorithm>& algo);
	std::map<std::string, int>& getPortFreqMap2() { return portFreqMap2EH; }
	std::map<std::string, Container>& getIdContMap() { return idContMap; }
	std::map<std::string, std::vector<Container>>& getPortContMap() { return portContMap; }
    int getResults() { return results; }
    int getNumErrors() { return numErrors; }
	void setOutputPath(const std::string& outputpath);
	std::string getOutputPath();
	void setCurrentTravelName(const std::string& travelName);
	std::string getCurrentTravelName();
	void setCurrentAlgoName(const std::string& algoName);
	std::string getCurrentAlgoName();
	std::string getTravelPath();
	void setTravelPath(const std::string& travelPath);
	std::vector <std::vector <std::vector <Container> >>& getSimShipPlan();
	void setSimShipPlan(std::vector <std::vector <std::vector <Container> >>  shipVecAfter);
	void generateError(const std::string& msg, int dest);

private:

	void simulationRun2(const std::string& plan, const std::string& route, std::unique_ptr<AbstractAlgorithm>& algo, std::vector<std::string>& containersAtEachPortPathVec);

	void processFile(std::ifstream& inFile, std::vector<std::vector<std::string>>& dataVec);

	void initShipPlanVecToZeros(std::vector <std::vector <std::vector <Container> > >& shipPlanVec);

	int setActualNumOfFloors(std::vector <std::vector <std::vector <Container> > >& shipPlanVec, std::vector<std::vector<std::string>>& dataVec, int& numFloors, int& numContainersInDimX, int& numContainersInDimY);

	void setWeightBalanceCalculator(WeightBalanceCalculator& calculator);

	void processContainersAtEachPort(const std::string& full_path_and_file_name, std::vector<std::string>& myShipRoute);

	void processContainersAtEachPort2(std::ifstream& inFile, std::vector<Container>& containersVec,
		const std::string& fullFileName);

	void buildPortMap(std::vector<std::string>& route);

	int shipToNextPort(std::string& currentPort, std::vector<std::string>& sortedPortContVec, std::unique_ptr<AbstractAlgorithm>& algo, int num);

	void buildIdContMap();
	/* new functions for ex2*/
	int simReadShipPlan(const std::string& ship_plan_path);

	int simReadShipRoute(const std::string& full_path_and_file_name, std::vector<std::string>& dataVec);

	int simGetInstructionsForCargo(const std::string& output_full_path, const std::string& fullPortName);


};
