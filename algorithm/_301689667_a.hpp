//
//  _301689667_a.hpp
//  EX2
//
//  Created by Weaam Masarweh on 12/05/2020.
//  Copyright © 2020 Weaam Masarweh. All rights reserved.
//

#ifndef _301689667_a_hpp
#define _301689667_a_hpp

#include <stdio.h>
#include <vector>
#include <sstream>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <regex>
#include <cmath>
#include "WeightBalanceCalculator.hpp"
#include "ShipPlan.hpp"
#include "Container.hpp"
#include "AbstractAlgorithm.h"
#include "AlgorithmRegistration.h"

const int CONT_ID = 11;
const int PRT_ID = 5;
class _301689667_a : public AbstractAlgorithm{
    
    ShipPlan shipPlan;
    std::vector<std::string> shipRoute;
    WeightBalanceCalculator weightBalanceCalculator;
    std::string name;
    int errCodeNum = 0;
    std::map<std::string, int> portVisitCounter ;
    
    public:
    _301689667_a();
    std::vector <std::vector <std::vector <Container> > >& getShipPlan();
    std::vector <std::string> getShipRoute ();
    std::vector<std::string> runAlgorithm(std::vector<Container>& containersTobeLoaded);
    std::string getName();
    void setName(std::string name);
    std::vector <std::vector <std::vector <Container> > > getCurrentShipPlanVec();
    void setParameters(ShipPlan& shipPlan, std::vector<std::string>& shipRoute, WeightBalanceCalculator& weightBalanceCalculator);
    
    
   virtual int readShipPlan(const std::string& full_path_and_file_name) ;
   virtual int readShipRoute(const std::string& full_path_and_file_name) ;
   virtual int setWeightBalanceCalculator(WeightBalanceCalculator& calculator);
   virtual int getInstructionsForCargo(
   const std::string& input_full_path_and_file_name,
   const std::string& output_full_path_and_file_name);

    
};


#endif /* _301689667_a_hpp */
