#pragma once
#include<string>
#include<vector>
#include"simulator.h"

class Simulator;
 
class ErrorHandler {
	Simulator* sim;
public:

	ErrorHandler(Simulator* sim);

	int checkAllErrors(const std::string& fullPortName, std::vector<std::vector<std::string>>& outputVec);
	int rightDestPort(const std::string& currentPort, std::vector<std::vector<std::string>>& outputVec);
	int forgotContainer(const std::string& fullPortName, std::vector<std::vector<std::string>>& outputVec);
	int parser(std::vector<std::string>& data, std::string& id, std::string& op, std::string& floor, std::string& x, std::string& y, std::string& im , std::string& xm, std::string& ym );
	bool invalidAccess(const std::string& id);
	int restOfRoute(const std::string& currentPort, std::vector<std::vector<std::string>>& outputVec);
	int availableSpaceCount(std::vector <std::vector <std::vector <Container>>>& shipVecAfter);

	// new functions for ex2:
	int checkIndexBounds(std::vector<std::vector<std::string>>& outputVec, std::vector<std::vector <std::vector <Container>>>& retAfterVec);

    int legalUnload2( std::vector <std::vector <std::vector <Container>>>& shipVec,
                                    const std::string& floor,const std::string& x,const std::string& y,const std::string& id);
    int legalLoad2( std::vector <std::vector <std::vector <Container>>>& shipVec,
                                  const std::string& floor,const std::string& x,const std::string& y,const std::string& id);

};