//
//  _301689667_b.cpp
//  EX2
//
//  Created by Weaam Masarweh on 12/05/2020.
//  Copyright © 2020 Weaam Masarweh. All rights reserved.
//

#include "_301689667_b.hpp"


//  ############################################## declerations ##############################################################
int repeateIDShipPlanB(std::ifstream& inFile, std::vector<std::string>& dataVec);
void processFileGetInsCargoB(std::ifstream& inFile, std::vector<std::vector<std::string> >& dataVec, int& errCodeNum);
bool is_numberB(const std::string& s);
void initShipPlanVecToZerosB(std::vector <std::vector <std::vector <Container> > >& shipPlanVec);
int setActualNumOfFloorsB(std::vector <std::vector <std::vector <Container> > >& shipPlanVec, std::vector<std::string>& dataVec, int& numFloors, int& numContainersInDimX, int& numContainersInDimY);
int tokenizeForReadShipPlanB(std::string const& str, std::vector<std::string>& out, int counter);
void  tokenizeGetInsCargoB(std::string& line, std::vector<std::vector<std::string> >& dataVec, int& errCodeNum);
int checkDuplicatedXYB(std::vector<std::string>& dataVec, int numContainersInDimX,int numContainersInDimY);

void unloadCargoForThisPortB(ShipPlan& shipPlan, std::vector<std::string>& shipRoute, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers);
void loadCargoB(ShipPlan& shipPlan, std::vector<std::string>& shipRoute, std::vector<Container>& containersToBeLoaded, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers, int& errCodeNum); //todo
void unloadThisContainerB(int numFloors, int numDimX, int numDimY, Container& container, ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers);
void prepareToMoveContainerB(int NumFloorsIndex, int tmpDimxIndex, int tmpDimYIndex, Container& container, ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers);
void moveB(int numFloorsIndex, int dimXIndex, int dimYIndex, Container& container, ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers);

void tmpUnloadContainerB(int numFloorsIndex, int dimXIndex, int dimYIndex, Container& container, ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpUnloadedContainers);
int freePlacesB(std::vector <std::vector <std::vector <Container> > >& shipPlanVec);
void loadTmpUnLoadedContainersB(ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpUnloadedContainers);
void loadThisContainerB(ShipPlan& shipPlan, Container& container, std::vector<std::string>& operationsString);
void rejectContainerB(Container& container, std::vector<std::string>& operationsString);
bool validIdB(Container& container);
bool validDestinationB(Container& container, std::vector<std::string>& shipRoute);
bool repeateIDShipPlanB(Container& container, ShipPlan& shipPlan);
bool repeateIDPortB(Container& container, std::vector<Container>& containersToBeLoaded, std::vector<std::string>& operationsString);
bool valIdLengthB(std::string id);
bool validWeightB(Container& container);
bool isISO6346FormatB(std::string id);
//  ############################################ end of decleartions #########################################################

REGISTER_ALGORITHM(_301689667_b)

_301689667_b::_301689667_b() {}

void _301689667_b::setParameters(ShipPlan& shipPlan, std::vector<std::string>& shipRoute, WeightBalanceCalculator& weightBalanceCalculator) {
    this->shipPlan = shipPlan;
    this->shipRoute = shipRoute;
    this->weightBalanceCalculator = weightBalanceCalculator;
}

std::vector <std::vector <std::vector <Container> > >& _301689667_b::getShipPlan() {
    return this->shipPlan.getShipPlan();
}
std::vector <std::string> _301689667_b::getShipRoute (){
    return this->shipRoute;
}

std::string _301689667_b::getName() {
    return this->name;
}

void _301689667_b::setName(std::string algoName) {
    this->name = algoName;
}

std::vector <std::vector <std::vector <Container> > > _301689667_b::getCurrentShipPlanVec() {
    
    return this->shipPlan.getShipPlan();
}


int _301689667_b::readShipPlan(const std::string& full_path_and_file_name){
    
    std::string binary;
    int setActualNumFloorsErrCodeNum =0, duplicateXYErrCodenum = 0;
    int numFloors, numContainersInDimX, numContainersInDimY;
    std::vector <std::vector <std::vector <Container> > >  shipPlanVec;
    std::ifstream inFile;
    std::vector<std::string> dataVec;
    
    inFile.open(full_path_and_file_name);
    if (inFile.fail()) {
        std::cerr << "Error opening file, cannot run this travel" << std::endl;
        this->errCodeNum += pow(2,3);
        return errCodeNum;
    }
    this->errCodeNum = repeateIDShipPlanB(inFile, dataVec);
    if(errCodeNum == pow(2,3)){
       binary = std::bitset<32>(errCodeNum).to_string(); //to binary
        if((int)binary[28] - 48 == 0){
            errCodeNum += pow(2,3);
        }
    }
    if(errCodeNum == pow(2,2)){
       binary = std::bitset<32>(errCodeNum).to_string(); //to binary
        if((int)binary[29] - 48 == 0){
            errCodeNum += pow(2,2);
        }
    }
    for (size_t i = 0; i < 3; i++) {
        if (! is_numberB(dataVec.at(i)) ) {
            std::cout << "bad first line, invalid ship dimensions(not int x || y || z), travel aborted." << std::endl;
            binary = std::bitset<32>(errCodeNum).to_string(); //to binary
            if((int)binary[28] - 48 == 0){
                errCodeNum += pow(2,3);
            }
           return errCodeNum;
        }
    }
    numFloors = std::stoi(dataVec.at(0));
    numContainersInDimX = std::stoi(dataVec.at(1));
    numContainersInDimY = std::stoi(dataVec.at(2));
    shipPlanVec.resize(numFloors, std::vector<std::vector<Container> >(numContainersInDimX, std::vector<Container>(numContainersInDimY)));
    inFile.close();
    initShipPlanVecToZerosB(shipPlanVec);
    setActualNumFloorsErrCodeNum = setActualNumOfFloorsB(shipPlanVec, dataVec, numFloors, numContainersInDimX, numContainersInDimY);
    
    binary = std::bitset<32>(setActualNumFloorsErrCodeNum).to_string(); //to binary
    if((int)binary[29] - 48 != 0 && std::bitset<32>(errCodeNum).to_string()[29]  - 48 ==0){
        errCodeNum += pow(2,2);
    }
    if((int)binary[30] - 48 != 0 && std::bitset<32>(errCodeNum).to_string()[30] - 48 ==0){
        errCodeNum += pow(2,1);
    }
    if((int)binary[31] - 48 != 0 && std::bitset<32>(errCodeNum).to_string()[31] -48 ==0){
        errCodeNum += pow(2,0);
    }
    if((int)binary[29] - 48 == 0){ // call this function just if all x y z are numbers
        duplicateXYErrCodenum = checkDuplicatedXYB(dataVec,numContainersInDimX,numContainersInDimY);
        binary = std::bitset<32>(duplicateXYErrCodenum).to_string(); //to binary
        if((int)binary[29] - 48 != 0 && std::bitset<32>(errCodeNum).to_string()[29]  - 48 ==0){
            errCodeNum += pow(2,2);
        }
        if((int)binary[27] - 48 != 0 && std::bitset<32>(errCodeNum).to_string()[27] - 48 ==0){
            errCodeNum += pow(2,4);
        }
    }
    this->shipPlan = ShipPlan(numFloors, numContainersInDimX, numContainersInDimY, shipPlanVec);
    inFile.close();
    return errCodeNum;
}



int _301689667_b::readShipRoute(const std::string& full_path_and_file_name){
        
        bool contains_non_alpha;
        std::string binary, line, prevLine ="NULLLine";
        std::ifstream inFile;
        std::vector<std::string> dataVec;
        int counter = 1, validLinesCounter = 0; /* for error lines numbers*/
        std::map<std::string, int>::iterator it;
        
        this->errCodeNum = 0;
        inFile.open(full_path_and_file_name);
        if (!inFile) {
            std::cerr << "Error opening file" << std::endl;
            this->errCodeNum += pow(2,7);
            return errCodeNum;
        }
        while (std::getline(inFile, line))
        {
            line.erase(remove(line.begin(), line.end(), ' '), line.end()); // remove all white spaces from string
            line.erase(remove(line.begin(), line.end(), '\t'), line.end()); // remove all tabs from string
            line.erase(remove(line.begin(), line.end(), '\r'), line.end()); // remove all carriage returns from string
            line.erase(remove(line.begin(), line.end(), '\v'), line.end()); // remove all vertical tabs from string
//            std::cout<< "counter: " <<counter <<std::endl;
//            std::cout<< "line: " <<line <<std::endl;
//            std::cout<< "prev line: " <<prevLine <<std::endl;
//            std::cout<< "compare result: " <<prevLine.compare(line) <<std::endl;
            if(counter > 1 && !prevLine.compare(line) ){ // 2 ports or more in row..
                std::cout << "a port appears twice or more consequentively" <<std::endl;
               binary = std::bitset<32>(errCodeNum).to_string(); //to binary
               if((int)binary[26] - 48 == 0){
                   errCodeNum +=pow(2,5);
               }
                counter++;
                continue;
            }
//            line.erase(remove(line.begin(), line.end(), ' '), line.end()); // remove all white spaces from string
//            line.erase(remove(line.begin(), line.end(), '\t'), line.end()); // remove all tabs from string
//            line.erase(remove(line.begin(), line.end(), '\r'), line.end()); // remove all carriage returns from string
//            line.erase(remove(line.begin(), line.end(), '\v'), line.end()); // remove all vertical tabs from string
//
            contains_non_alpha = !std::regex_match(line, std::regex("^[A-Za-z]+$"));
            if(line.length() == 0){
                std::cout << "bad port symbol format (empty line), line " << counter << " is ignored." << std::endl;
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[25] - 48 == 0){
                    errCodeNum += pow(2,6);
                }
                counter++;
                continue;
            }
            if ( (line.length() != PRT_IDB  || contains_non_alpha) && line.at(0) != '#' ){
                std::cout << "bad port symbol format, line " << counter << " is ignored." << std::endl;
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[25] - 48 == 0){
                    errCodeNum += pow(2,6);
                }
                counter++;
                continue;
            }
            
            else if (line.length() != 0 && line.at(0) == '#') {//counter++; dont count # lines
                continue;
            }
            else {
                dataVec.push_back(line);
            }
            prevLine = line;
            counter++;
            validLinesCounter ++;
        }
    
    if(counter == 1){ //empty file (i assumed that file with only # lines is considerd empty)
         std::cout << "empty file or file with only # lines" <<std::endl;
        binary = std::bitset<32>(errCodeNum).to_string(); //to binary
        if((int)binary[24] - 48 == 0){
            errCodeNum += pow(2,7);
        }
    }
        if(validLinesCounter == 1){ //only one valid, a single valid port
             std::cout << "onely 1 valid port in file" <<std::endl;
            binary = std::bitset<32>(errCodeNum).to_string(); //to binary
            if((int)binary[23] - 48 == 0){
                errCodeNum += pow(2,8);
            }
    }
    inFile.close();
    this->shipRoute =  dataVec;
    for(auto& port: this->shipRoute){ // init all visits to 0
        it =portVisitCounter.find(port);
        if(it !=  portVisitCounter.end()){
            continue;
        }
        else{
            portVisitCounter[port] = 0;
        }
    }
    return errCodeNum;
}

int _301689667_b::setWeightBalanceCalculator(WeightBalanceCalculator& calculator){
	calculator = calculator;
    return 0;
}


int _301689667_b::getInstructionsForCargo(const std::string& input_full_path_and_file_name, const std::string& output_full_path_and_file_name){
    
    std::map<std::string, int>::iterator it;
    std::vector<std::string> operationsString;
    std::string binary, portSymbol, fileName, fileVisitNum;
    std::ifstream inFile;
    std::ofstream outFile;
    std::vector<Container> containersVec;
    std::vector<std::vector<std::string> > dataVec;
    int counter = 0, weight;
    size_t start, end = 0;
    this->errCodeNum = 0;
    inFile.open(input_full_path_and_file_name);
    if (inFile.fail()) {
        std::cerr << "Error opening file, ill assume no cargo to be loaded" << std::endl;
        this->errCodeNum += pow(2,16);
    }
    it = this->portVisitCounter.find(this->shipRoute[0]); // number of visits according to the algorithm
    if(it != portVisitCounter.end()){ //no need for this check
        it->second += 1;
    }
    start = input_full_path_and_file_name.find_first_of("_", end);
    end =input_full_path_and_file_name.find(".", start);
    fileVisitNum =input_full_path_and_file_name.substr(start + 1, end - start);
    if(std::stoi( fileVisitNum ) == it->second){ // if not the same visit num ignore this cargo file
        processFileGetInsCargoB(inFile, dataVec, this->errCodeNum);
        for (auto& vec : dataVec) {
            counter++;
            if (vec.size() == 0) { // i think there is no need for this
                continue;
            }
                if(vec.size() ==1){
                    std::cout <<"bad line format, missing wieght &&  missing destination" <<std::endl;
                    binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                    if((int)binary[19] - 48 == 0){errCodeNum += pow(2,12);}
                    if((int)binary[18] - 48 == 0){errCodeNum += pow(2,13);}
                    Container container(vec.at(0), 0 ,"Null");
                    containersVec.push_back(container);/*add new container to vec*/
                    counter++;
                    continue;
                }
                if(vec.size() ==2){
                    std::cout <<"bad line format, missing destination" <<std::endl;
                    binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                    if((int)binary[18] - 48 == 0){errCodeNum += pow(2,13);}
                    weight = is_numberB(vec.at(1)) ? std::stoi(vec.at(1)) : -1;
                    Container container(vec.at(0),weight ,"Null");
                    containersVec.push_back(container);/*add new container to vec*/
                    counter++;
                    continue;
                }
                if(vec.size() >3){
                    std::cout <<"bad line format,too much param, treat it as illegal ID" <<std::endl;
                    binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                    if((int)binary[16] - 48 == 0){errCodeNum += pow(2,15);}
                    Container container("Null",0 ,"Null");
                    containersVec.push_back(container);/*add new container to vec*/
                    counter++;
                    continue;
                }
            else{ //size == 3
                weight = is_numberB(vec.at(1)) ? std::stoi(vec.at(1)) : -1;
                Container container(vec.at(0),weight ,vec.at(2));
                containersVec.push_back(container);/*add new container to vec*/
            }
        }
    }
    else{ //wrong file visit number, cargo ignored
        std::cout << "wrong visit number, file will be ignored" << std::endl;
        binary = std::bitset<32>(errCodeNum).to_string(); //to binary
        if((int)binary[15] - 48 == 0){
            errCodeNum += pow(2,16);
        }
    }
    portSymbol = this->shipRoute[0];
    //visitNum = it->second;
    operationsString = runAlgorithm(containersVec);
    //fileName += "/" + std::string(portSymbol) + "_" + std::to_string(visitNum) + std::string(".crane_instructions.txt");
    outFile.open(output_full_path_and_file_name); // to remove fileName
    if(outFile.fail()){
        std::cerr << "Error opening  output file" << std::endl ;
        return errCodeNum; // should i return any thing else?
    }
    for (const auto& line : operationsString ) outFile << line << "\n";
    outFile.close();
    inFile.close();
    return errCodeNum;
}



std::vector<std::string>  _301689667_b::runAlgorithm(std::vector<Container>& containersToBeLoaded) {

    std::vector<std::string> operationsString;
    std::vector<Container> tmpunloadedContainers;
    
    unloadCargoForThisPortB(this->shipPlan, this->shipRoute, operationsString, tmpunloadedContainers);
    loadCargoB(this->shipPlan, this->shipRoute, containersToBeLoaded, operationsString, tmpunloadedContainers, this->errCodeNum);
    this->shipRoute.erase(this->shipRoute.begin());
    return operationsString;
}



//    ########################################## aux for readShipPlan ######################################################


/*this function collects the data file into a string vector */
    
int repeateIDShipPlanB(std::ifstream& inFile, std::vector<std::string>& dataVec){
    int errCodeNum = 0;
    int tokenErrCodeNum = 0;
    int counter = 1;
    std::string line;
    std::string binary;

    while (std::getline(inFile, line))
    {
        line.erase(remove(line.begin(), line.end(), ' '), line.end()); // remove all white spaces from string
        line.erase(remove(line.begin(), line.end(), '\t'), line.end()); // remove all tabs from string
        line.erase(remove(line.begin(), line.end(), '\r'), line.end()); // remove all carriage returns from string
        line.erase(remove(line.begin(), line.end(), '\v'), line.end()); // remove all vertical tabs from string
        if (line.length() == 0) {
            if(counter == 1 ){ //bad first line
                std::cout << "bad first line format, empty line, travel cannot be run" << std::endl;
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[28] - 48 == 0){
                    errCodeNum += pow(2,3);
                    counter++;
                    continue;
                }
            }
            std::cout << "line " << counter << " has invalid number of parameters(empty line), it'll be ignored." << std::endl;
            binary = std::bitset<32>(errCodeNum).to_string(); //to binary
            if((int)binary[29] - 48 == 0){
                errCodeNum += pow(2,2);
            }
            counter++;
            continue;
        }
        else if (line.at(0) == '#') {//            counter++; doesnt count '#' lines
            continue;
        }
        else {
            tokenErrCodeNum = tokenizeForReadShipPlanB(line, dataVec, counter);
            if (tokenErrCodeNum == (pow(2,3))){
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[28] - 48 == 0){
                    errCodeNum += pow(2,3);
                }
            }
            else if( tokenErrCodeNum == (pow(2,2) ) ){ // tokenerrNum == 2^2
                 binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                 if((int)binary[29] - 48 == 0){
                     errCodeNum += pow(2,2);
                 }
            }
            counter++;
        }
    }
    return errCodeNum;
}

int tokenizeForReadShipPlanB(std::string const& str, std::vector<std::string>& out, int counter)
{
    size_t start;
    size_t end = 0;
    std::vector<std::string> tmp;

    while ((start = str.find_first_not_of(",", end)) != std::string::npos)
    {
        end = str.find(",", start);
        tmp.push_back(str.substr(start, end - start));
    }
    if (tmp.size() != 3) { /*if line has invalid # of parameters*/
        if(counter == 1) {
            std::cout << "\tline " << counter << " has invalid number of parameters of first line, cant run this travel" << std::endl;
            return pow(2,3); // bad first line
        }
        std::cout << "\tline " << counter << " has invalid number of parameters, it'll be ignored" << std::endl;
        return  pow(2,2) ;
    }
    else {
        for (auto& e : tmp) {
            out.push_back(e);
        }
    }
    return 0;
}

void initShipPlanVecToZerosB(std::vector <std::vector <std::vector <Container> > >& shipPlanVec) {

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

int setActualNumOfFloorsB(std::vector <std::vector <std::vector <Container> > >& shipPlanVec, std::vector<std::string>& dataVec, int& numFloors, int& numContainersInDimX, int& numContainersInDimY) {
    std::string binary;
    int errCodeNum = 0, floors, x, y;
    Container nullContainer("-1", -1, "-1");

    std::vector<std::string>::iterator it = dataVec.begin() + 3; //start from second line
    while (it != dataVec.end()) {
        /*check if all line data contains numbers*/
        x = is_numberB(*it) ? std::stoi(*it) : -1;
        ++it;
        y = is_numberB(*it) ? std::stoi(*it) : -1;
        ++it;
        floors = is_numberB(*it) ? std::stoi(*it) : -1;
        if (x == -1 || y == -1 || floors == -1) {
            binary = std::bitset<32>(errCodeNum).to_string(); //to binary
            if((int)binary[29] - 48 == 0){
                errCodeNum += pow(2,2);
            }
            ++it;
            std::cout << "Position " << x << "," << y << " is invalid, line ignored" << std::endl;
            continue;
        }
        if (x >= numContainersInDimX || y >= numContainersInDimY) {
            binary = std::bitset<32>(errCodeNum).to_string(); //to binary
            if( (int)binary[30] - 48 == 0){
                errCodeNum += pow(2,1);
            }
            std::cout << "Position " << x << "," << y << " is invalid because it's outside the range. it'll be ignored." << std::endl;
            ++it;
            continue;
        }
        if (floors >= numFloors) {
            binary = std::bitset<32>(errCodeNum).to_string(); //to binary
            if( (int)binary[31] - 48  == 0){
                errCodeNum += pow(2,0);
            }
            std::cout << "Position " << x << "," << y << " has invalid number of actual floors. it'll be ignored." << std::endl;
            ++it;
            continue;
        }
        for(int i=0; i <  numFloors - floors; i++){
            shipPlanVec[i][x][y] = nullContainer ;
        }
        ++it;
    }
    return errCodeNum;
}


int checkDuplicatedXYB(std::vector<std::string>& dataVec,int numContainersInDimX,int numContainersInDimY){
    std::string binary;
    int errCodeNum = 0;
    int floors, x, y;
    std::vector <std::vector <int> > XYZVec;
    XYZVec.resize(numContainersInDimX, std::vector<int>(numContainersInDimY));
    for (auto& d1 : XYZVec) {
        for (auto& d2 : d1) {
                d2= 0;
        }
    }
    std::vector<std::string>::iterator it = dataVec.begin() + 3; //start from second line
    while (it != dataVec.end()) {
        x = std::stoi(*it);
        ++it;
        y = std::stoi(*it);
        ++it;
        floors = std::stoi(*it);
        ++it;
        if(x < numContainersInDimX && y < numContainersInDimY){
            if(XYZVec[x][y] ==0){ //no duplicate
                XYZVec[x][y] = floors;
            }
            else{
                if(XYZVec[x][y] == floors){ //duplicarte same data
                    std::cout << "duplicate x y with same data, to be ignored" << std::endl;
                    binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                    if((int)binary[29] - 48 == 0){
                        errCodeNum += pow(2,2);
                    }
                }
                else{ //duplicate not same data
                     std::cout << "duplicate x y with  not same data, can not run this travel" << std::endl;
                    binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                    if((int)binary[27] - 48 == 0){
                        errCodeNum += pow(2,4);
                    }
                }
            }
        }
    }
    return errCodeNum;
}

bool is_numberB(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}


// ############################################## aux for getInstructionsForCargo #############################################################


void processFileGetInsCargoB(std::ifstream& inFile, std::vector<std::vector<std::string> >& dataVec, int& errCodeNum){
    
   // int tokenErrCodeNum = 0;
    int counter = 1;
    std::string line;
    std::string binary;
    //std::vector<Container> containersVec;
    
    while (std::getline(inFile, line))
    {
        counter++;
        line.erase(remove(line.begin(), line.end(), ' '), line.end()); // remove all white spaces from string
        line.erase(remove(line.begin(), line.end(), '\t'), line.end()); // remove all tabs from string
        line.erase(remove(line.begin(), line.end(), '\r'), line.end()); // remove all carriage returns from string
        line.erase(remove(line.begin(), line.end(), '\v'), line.end()); // remove all vertical tabs from string


        if (line.length() == 0) {
            std::cout << "line " << counter << ", bad line foramt: empty line, so cant read ID, it'll be ignored." << std::endl;
            binary = std::bitset<32>(errCodeNum).to_string(); //to binary
            if((int)binary[17] - 48 == 0){
                errCodeNum += pow(2,14);
            }
           // dataVec.push_back(std::vector<std::string> {}); what is written here?
            continue;
        }
        else if (line.at(0) == '#') {
            //dataVec.push_back(std::vector<std::string> {}); what is written here?
            continue;
        }
        else {
            tokenizeGetInsCargoB(line, dataVec, errCodeNum);
        }
    }
}

void tokenizeGetInsCargoB(std::string& line, std::vector<std::vector<std::string> >& dataVec, int& errCodeNum){
    
    std::string binary;
    size_t start;
    size_t end = 0;
    std::vector<std::string> tmp;
    
    if(line[0] == ','){
        std::cout <<"missing id, container should be ignored" << std::endl;
        binary = std::bitset<32>(errCodeNum).to_string(); //to binary
        if((int)binary[17] - 48 == 0){
            errCodeNum += pow(2,14);
        }
        return;
    }
    while ((start = line.find_first_not_of(",", end)) != std::string::npos)
    {
        end = line.find(",", start);
        tmp.push_back(line.substr(start, end - start));
    }
   dataVec.push_back(tmp);
}

void unloadCargoForThisPortB(ShipPlan& shipPlan, std::vector<std::string>& shipRoute, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers) {

    Container container;
    std::string unloadString;
    
//    Container container1 ("id1", 2, "ABCDE");
//    Container container2 ("id2", 3, "FGHIK");
//    shipPlan.getShipPlan()[0][0][0] = container1;
//    shipPlan.getShipPlan()[0][0][1] = container2;
//    std::cout <<"port: " << shipRoute[0]<< std::endl;;
    

    for (int numFloors = shipPlan.getNumFloors() - 1; numFloors >= 0; numFloors--) {
        for (int numContainersDimX = shipPlan.getNumContainersDimX() - 1; numContainersDimX >= 0; numContainersDimX--) {
            for (int numContainersDimY = shipPlan.getNumContainersDimY() - 1; numContainersDimY >= 0; numContainersDimY--) {
                if (shipPlan.getShipPlan()[numFloors][numContainersDimX][numContainersDimY].getDestination() == shipRoute[0]) {
                    container = shipPlan.getShipPlan()[numFloors][numContainersDimX][numContainersDimY];
                     unloadThisContainerB(numFloors, numContainersDimX, numContainersDimY, container, shipPlan, operationsString, tmpunloadedContainers);
                    unloadString = "U," + container.getId() + "," + std::to_string(numFloors) + "," + std::to_string(numContainersDimX) + "," + std::to_string(numContainersDimY);
                    operationsString.push_back(unloadString);
                }
            }
        }

    }

}

void unloadThisContainerB(int numFloorsIndex, int dimXIndex, int dimYIndex,Container& container, ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers) {
    if (container == container) {}
    int tmpNumFloorsIndex = numFloorsIndex, tmpDimx = dimXIndex, tmpDimY = dimYIndex;
    int actualNumFloors = (int)shipPlan.getShipPlan().size() - 1; //notice the minus 1
    Container contToMove;
    Container emptyContainer("0", 0, "0");
    if (tmpNumFloorsIndex == actualNumFloors) {
        shipPlan.getShipPlan()[numFloorsIndex][dimXIndex][dimYIndex] = emptyContainer;
        return;
    }
    if ((tmpNumFloorsIndex + 1) <= actualNumFloors) {
        if (shipPlan.getShipPlan()[tmpNumFloorsIndex + 1][tmpDimx][tmpDimY] == emptyContainer) {
            shipPlan.getShipPlan()[numFloorsIndex][dimXIndex][dimYIndex] = emptyContainer;
            return;
        }
        contToMove = shipPlan.getShipPlan()[tmpNumFloorsIndex + 1][tmpDimx][tmpDimY];
        prepareToMoveContainerB(tmpNumFloorsIndex + 1, tmpDimx, tmpDimY, contToMove, shipPlan, operationsString, tmpunloadedContainers);
        shipPlan.getShipPlan()[numFloorsIndex][dimXIndex][dimYIndex] = emptyContainer;
        return;
    }
    return;
}

void prepareToMoveContainerB(int numFloorsIndex, int dimXIndex, int dimYIndex, Container& container, ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers) {
    int actualNumFloors = (int)shipPlan.getShipPlan().size() - 1; //notice the minus 1
    int numOfMoves = 0;
    Container emptyContainer("0", 0, "0");
    if (container == container) {};
    while ((numFloorsIndex <= actualNumFloors) && !(shipPlan.getShipPlan()[numFloorsIndex][dimXIndex][dimYIndex] == emptyContainer)) {
        numOfMoves++;
        numFloorsIndex++;
    }
    numFloorsIndex--;
    while (numOfMoves > 0) {
        moveB(numFloorsIndex, dimXIndex, dimYIndex, shipPlan.getShipPlan()[numFloorsIndex][dimXIndex][dimYIndex], shipPlan, operationsString, tmpunloadedContainers);
        numFloorsIndex--;
        numOfMoves--;
    }

}

void moveB(int numFloorsIndex, int dimXIndex, int dimYIndex, Container& container, ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpunloadedContainers) {

//    int actualNumFloors = (int)shipPlan.getShipPlan().size() - 1; //notice the minus 1
//    int actualDimX = (int)shipPlan.getShipPlan()[0].size() - 1; //notice the minus 1
//    int actualdimY = (int)shipPlan.getShipPlan()[0][0].size() - 1; //notice the minus 1
    std::string moveString;
    Container emptyContainer("0", 0, "0");

//    for (int numFloor = 0; numFloor <= actualNumFloors; numFloor++) {
//        for (int posX = 0; posX <= actualDimX; posX++) {
//            for (int posY = 0; posY <= actualdimY; posY++) {
//                if (shipPlan.getShipPlan()[numFloor][posX][posY] == emptyContainer) {
//                    shipPlan.getShipPlan()[numFloor][posX][posY] = container;
//                     shipPlan.getShipPlan()[numFloorsIndex][dimXIndex][dimYIndex] = emptyContainer;
//                    moveString = "M," + shipPlan.getShipPlan()[numFloor][posX][posY].getId() + "," + std::to_string(numFloorsIndex) + "," + std::to_string(dimXIndex) + "," + std::to_string(dimYIndex) + "," + std::to_string(numFloor) + "," + std::to_string(posX) + "," + std::to_string(posY);
//                    operationsString.push_back(moveString);
//                    return;
//
//                }
//            }
//        }
//    }
    tmpUnloadContainerB(numFloorsIndex, dimXIndex, dimYIndex, container, shipPlan, operationsString, tmpunloadedContainers);
}

void tmpUnloadContainerB(int numFloors, int posX, int posY, Container& container, ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpUnloadedContainers) {

    std::string unloadString;
    Container emptyContainer("0", 0, "0");
    tmpUnloadedContainers.push_back(container);
    unloadString = "U," + container.getId() + "," + std::to_string(numFloors) + "," + std::to_string(posX) + "," + std::to_string(posY);
    operationsString.push_back(unloadString);
    shipPlan.getShipPlan()[numFloors][posX][posY] = emptyContainer;
}


void loadCargoB(ShipPlan& shipPlan, std::vector<std::string>& shipRoute, std::vector<Container>& containersToBeLoaded, std::vector<std::string>& operationsString, std::vector<Container>& tmpUnloadedContainers, int& errCodeNum) {
    
    std::string loadString, binary;
    int numOfFreePlaces;
    if (tmpUnloadedContainers.size() > 0) {
        loadTmpUnLoadedContainersB(shipPlan, operationsString, tmpUnloadedContainers);
    }

    numOfFreePlaces = freePlacesB(shipPlan.getShipPlan());
    if (containersToBeLoaded.size() == 0) {
        return;
    }
     for (std::vector<std::string>::iterator routeIt = shipRoute.begin(); routeIt != shipRoute.end(); routeIt++) {
        if ((routeIt == shipRoute.begin()) && (next(routeIt) == shipRoute.end())) {
            std::cout << "this is the last port, only unload operations allowed, ignore cargo" << std::endl;
            binary = std::bitset<32>(errCodeNum).to_string(); //to binary
            if((int)binary[14] - 48 == 0){
                errCodeNum += pow(2,16);
            }
            break;
        }
        for (std::vector<Container>::iterator contIt = containersToBeLoaded.begin(); contIt != containersToBeLoaded.end();) {
            if (numOfFreePlaces == 0) {
                while (contIt != containersToBeLoaded.end()) {
                    std::cout << "container rejected, full ship, container id: " << (*contIt).getId()<<'\n'  << std::endl;
                    binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                        if((int)binary[13] - 48 == 0){
                            errCodeNum += pow(2,18);
                        }
                    rejectContainerB(*contIt, operationsString);
                    contIt = containersToBeLoaded.erase(contIt);
                }
                return;
            }

            if (!validIdB(*contIt)  ){
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[16] - 48 == 0){
                    errCodeNum += pow(2,15);
                }
                rejectContainerB(*contIt, operationsString);
                contIt = containersToBeLoaded.erase(contIt);
                continue;
            }
            
            if(repeateIDPortB(*contIt, containersToBeLoaded, operationsString)){
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[21] - 48 == 0){
                    errCodeNum += pow(2,10);
                }
                //rejectContainerB(*contIt, operationsString);
                //contIt = containersToBeLoaded.erase(contIt);
                //continue;
            }
            
            if(repeateIDShipPlanB(*contIt, shipPlan)){
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[20] - 48 == 0){
                    errCodeNum += pow(2,11);
                }
                rejectContainerB(*contIt, operationsString);
                contIt = containersToBeLoaded.erase(contIt);
                continue;
            }
            
            if(!validDestinationB(*contIt, shipRoute)){
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[18] - 48 == 0){
                    errCodeNum += pow(2,13);
                }
                rejectContainerB(*contIt, operationsString);
                contIt = containersToBeLoaded.erase(contIt);
                continue;
            }
            
            if(!validWeightB(*contIt)){
                binary = std::bitset<32>(errCodeNum).to_string(); //to binary
                if((int)binary[19] - 48 == 0){
                    errCodeNum += pow(2,12);
                }
                rejectContainerB(*contIt, operationsString);
                contIt = containersToBeLoaded.erase(contIt);
                continue;
            }
            
            
            if (!(*routeIt).compare((*contIt).getDestination())) {
                loadThisContainerB(shipPlan, *contIt, operationsString);
                numOfFreePlaces--;
                contIt = containersToBeLoaded.erase(contIt);
                continue;
            }
            contIt++;
        }
    }
    return;
}

void loadTmpUnLoadedContainersB(ShipPlan& shipPlan, std::vector<std::string>& operationsString, std::vector<Container>& tmpUnloadedContainers) {

    int actualNumFloors = (int)shipPlan.getShipPlan().size() - 1; //notice the minus 1
    int actualDimX = (int)shipPlan.getShipPlan()[0].size() - 1; //notice the minus 1
    int actualdimY = (int)shipPlan.getShipPlan()[0][0].size() - 1; //notice the minus 1
    int cont = 0;
    std::string loadString;
    Container emptyContainer("0", 0, "0");

    for (int numFloor = 0; numFloor <= actualNumFloors; numFloor++) {
        for (int posX = 0; posX <= actualDimX; posX++) {
            for (int posY = 0; posY <= actualdimY; posY++) {
                if (shipPlan.getShipPlan()[numFloor][posX][posY] == emptyContainer) {
                    shipPlan.getShipPlan()[numFloor][posX][posY] = tmpUnloadedContainers[cont];
                    loadString = "L," + shipPlan.getShipPlan()[numFloor][posX][posY].getId() + "," + std::to_string(numFloor) + "," + std::to_string(posX) + "," + std::to_string(posY);
                    operationsString.push_back(loadString);
                    cont++;
                    if (cont == (int)tmpUnloadedContainers.size()) {
                        return;
                    }
                }
            }
        }
    }
}


void loadThisContainerB(ShipPlan& shipPlan, Container& container, std::vector<std::string>& operationsString) {

    int actualNumFloors = (int)shipPlan.getShipPlan().size() - 1; //notice the minus 1
    int actualDimX = (int)shipPlan.getShipPlan()[0].size() - 1; //notice the minus 1
    int actualdimY = (int)shipPlan.getShipPlan()[0][0].size() - 1; //notice the minus 1
    std::string loadString;
    Container emptyContainer("0", 0, "0");
    for (int numFloor = 0; numFloor <= actualNumFloors; numFloor++) {
        for (int posX = 0; posX <= actualDimX; posX++) {
            for (int posY = 0; posY <= actualdimY; posY++) {
                if (shipPlan.getShipPlan()[numFloor][posX][posY] == emptyContainer) {
                    shipPlan.getShipPlan()[numFloor][posX][posY] = container;
                    loadString = "L," + shipPlan.getShipPlan()[numFloor][posX][posY].getId() + "," + std::to_string(numFloor) + "," + std::to_string(posX) + "," + std::to_string(posY);
                    operationsString.push_back(loadString);
                    return;
                }
            }
        }
    }
}









int freePlacesB(std::vector <std::vector <std::vector <Container> > >& shipPlanVec) {
    int numOfFreePlaces = 0;
    for (auto& floor : shipPlanVec) {
        for (auto& posX : floor) {
            for (auto& posY : posX) {
                if (posY.getId() == "0" && posY.getWeight() == 0 && posY.getDestination() == "0") {
                    numOfFreePlaces++;
                }
            }
        }
    }
     return numOfFreePlaces;
}









void rejectContainerB(Container& container, std::vector<std::string>& operationsString) {

    std::string rejectString;
    rejectString = "R," + container.getId() + ",-1,-1,-1";
    operationsString.push_back(rejectString);

}



bool validIdB(Container& container) {
    
    if (!valIdLengthB(container.getId())) {  //}|| repeateIDShipPlanB(container, shipPlan)) {
        return 0;
    }
    if(! isISO6346FormatB(container.getId())){
        return 0;
    }

    return 1;
}








bool validDestinationB(Container& container, std::vector<std::string>& shipRoute) {
    std::string rejectstring;
    for (std::vector<std::string>::iterator shipIt = shipRoute.begin(); shipIt != shipRoute.end(); shipIt++) {
          if (!(*shipIt).compare(container.getDestination())) {
            return 1;
        }
    }
    std::cout << "container rejected, invalid destination, container id: " << container.getId() << '\n' << std::endl;
    return 0;
}



bool isISO6346FormatB(std::string id) {
    if(!valIdLengthB(id)){
        return false;
    }
    std::string ownerCode = id.substr(0, 2);
    char categoryIdentifier = id.at(3);
    std::string strNumber = id.substr(4, 10);

    for (auto& l : ownerCode) {
        // letter is in not alphabet or not capital letter
        if (!isalpha(l) || !isupper(l)) {
            std::cout << "rejected container, illegal ISO 6346 " << id << '\n' << std::endl;
            return false;
        }
    }
    if (categoryIdentifier != 'U' && categoryIdentifier != 'J' && categoryIdentifier != 'Z') {
        std::cout << "rejected container, illegal ISO 6346 " << id << '\n' << std::endl;
        return false;
    }
    // rest of id is not a number
    if (!is_numberB(strNumber)) {
        std::cout << "rejected container, illegal ISO 6346 " << id << '\n' << std::endl;
        return false;
    }
    return true;
}






bool repeateIDShipPlanB(Container& container, ShipPlan& shipPlan) {
    for (auto& floor : shipPlan.getShipPlan()) {
        for (auto& posX : floor) {
            for (auto& posY : posX) {
                if (!container.getId().compare(posY.getId())) {
                    std::cout << "rejectd container, invalid id, this container alreadt on ship, container id: " << container.getId() << std::endl;
                    return 1;
                }
            }
        }
    }
    return 0;
}

bool repeateIDPortB(Container& container, std::vector<Container>& containersToBeLoaded, std::vector<std::string>& operationsString ){
    
    int counter = -1;
    for (std::vector<Container>::iterator contIt = containersToBeLoaded.begin(); contIt != containersToBeLoaded.end();){
        if(!container.getId().compare((*contIt).getId()) ){
            counter++;
            if(counter >0){
                std::cout << "rejected container, repeated id at Port, container id: " << (*contIt).getId() << '\n' << std::endl;
                rejectContainerB(*contIt, operationsString);
                contIt = containersToBeLoaded.erase(contIt);
                continue;
            }
        }
        contIt++;
    }

    if(counter >0){
        return 1;
    }
    return 0;
}





bool valIdLengthB(std::string  id) {
    if (id.length() != CONT_IDB) {
        std::cout << "rejected container, invalid id length (illegal ISO), container id: " << id << '\n' << std::endl;
        return 0;
    }
    return 1;
}

bool validWeightB(Container& container){
    if (container.getWeight() > 0){
        return 1;
    }
    std::cout << "rejected container, invalid wieght, container id: " << container.getId()<< '\n' << std::endl;
    return 0;
}
