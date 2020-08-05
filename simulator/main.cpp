#include"simulator.h"
#include "AlgorithmRegistrar.h"
#include <iostream>
#include <string>
#include <dlfcn.h>  //LINUX ONLY


typedef std::pair<std::string,std::pair<int, int>> resultType;




/*
 * Parse command line flags for the program.
 * Getting argc and argv as arguments, and they should be the given arguments to main function.
 * The function should change travel_path, algorithm_path, output_path to the correct values
 * according to the command line arguments.
 * If any parameter wasn't given, it stays as it was defined before.
 * If a parameter was given without any value (i.e. "... -travel_path -algorithm_path") it will ignore this
 * parameter and won't change its value.
 * If a parameter is set twice it will take the last value.
 * On success, the function will return 1.
 * In one of the 2 following cases of failure, the function will print an error message and return -1:
 *     1. Invalid parameter flag (i.e. "-ttravel_path", or "travel_path" (without "-")).
 *     2. The given value for num of args is not a valid and positive number.
 */
int parseFlag(int argc, char** argv, std::string& travel_path, std::string& algorithm_path, std::string& output_path) {
    if (argc < 3) {
        std::cout << "fatal error- Missing parameter: -travel_path" << std::endl;
        return -1;
    }
    std::string current_flag = argv[1];
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            current_flag = arg;
            continue;
        }
        if (current_flag.compare("-travel_path")==0)
            travel_path = arg;
        else if (current_flag.compare("-algorithm_path")==0)
            algorithm_path = arg;
        else if (current_flag.compare("-output")==0)
            output_path = arg;
        else {
            std::cout << "fatal error- Invalid parameter: " << current_flag << std::endl;
            return -2;
        }
    }
    return 1;
}



/*
 * Get all the files and their names in the given path that ends with the given extension type.
 * The parameter files is a map where a full file path points the absolute file name (without the extension).
 * The function will go over the files in the given directory.
 * For every file that ends with the given extension, the function will add an entry to files,
 * where the key is the full file path, and the value is the absolute file name.
 * On success, the function will return 0.
 * On one of the 2 following cases of failure, the function will print an error message and reutnr -1:
 *     1. The given directory does not exist.
 *     2. There aren't any files in the given directory that have the given extension.
 */
int getFilesAndNames(const std::string& path, std::map<std::string, std::string>& files, std::string type) {
    // Check if directory exists.
    if (!std::filesystem::is_directory(path)) {
        std::cout << "Given parameter " << path << " is not a directory" << std::endl;
        return -1;
    }

    bool has_files = false;
    for (auto& entry : std::filesystem::directory_iterator(path)) {
        auto& filepath = entry.path();
        if (filepath.extension().string().compare(type) == 0 ) {
            // filepath.string() is the full file path, and filepath.filename().string() is the absolute file name. 
	    std::string filepath_string = filepath.string();
            files[filepath_string] = filepath.filename().string();
            has_files = true;
        }
    }

    // Check if didn't found any files.
    if (!has_files) {
        std::cout << "There are no files in path " << path << " with extension " << type << std::endl;
        return -1;
    }
    return 0;
}







int getFolderAndfiles(const std::string& path, std::map<std::string, std::string>& files) {
    // Check if directory exists.
    if (!std::filesystem::is_directory(path)) {
        std::cout << "Given parameter " << path << " is not a directory" << std::endl;
        return -1;
    }

    bool has_files = false;
    for (auto& entry : std::filesystem::directory_iterator(path)) {
        if (!std::filesystem::is_directory(entry)) {
            continue;
        }
        auto& filepath = entry.path();
        // filepath.string() is the full file path, and filepath.filename().string() is the absolute file name.
        std::string filepath_string = filepath.string();
        files[filepath_string] = filepath.filename().string();
        has_files = true;
    }

    // Check if didn't found any files.
    if (!has_files) {
        std::cout << "There are no files in path " << path << std::endl;
        return -1;
    }
    return 0;
}







/*
 * Load both travels and algorithms from the given path.
 * At the beginning, for both travels and algorithms, the function will get all the relevant files.
  * For each algorithm file: the function will dynamically load the .so file and save the current function that
 * was loaded by AlgorithmRegistration. Since we can't change the header of AbstractAlgorithm, the functions will be
 * saved in a map where the algorithm name (the file name) will point the function.
 * On success the function will return 0.
 * On one of the 3 following cases of failure, the function will print an error message and return -1:
 *     1. Getting travel files or algorithm files failed.
 *     2. Loading one of the algorithms failed.
 */
int loadAll(std::map<std::string, std::function<std::unique_ptr<AbstractAlgorithm>()>>& algorithms, std::map<std::string, std::string>& travel_files,
        const std::string& travel_path, const std::string& algorithm_path, std::vector<void*>& handlers) {

    if (getFolderAndfiles(travel_path, travel_files) < 0)
        return -1;
  
    std::map<std::string, std::string>  algorithm_files;
    if (getFilesAndNames(algorithm_path, algorithm_files, ".so") < 0)
        return -2;
                                                    
    for (auto& entry : algorithm_files) {
        char* chars = new char[entry.first.length()];
        for (int i = 0; i < (int)entry.first.length(); i++)
            chars[i] = entry.first[i];
        chars[entry.first.length()] = '\0';
        
         void* handler = dlopen(chars, RTLD_LAZY);
         if (!handler) {
            std::cout << "Failed to dynamically load file " << entry.first << std::endl;
            return -1;
        }
        delete[] chars;
        
        std::function<std::unique_ptr<AbstractAlgorithm>()> function = AlgorithmRegistrar::getFunction();
        //segmentation fault problem -> //dlclose(handler);
        handlers.push_back(handler);
        algorithms[entry.second] = function;
    }

    return 0;
}




int sumStep(std::vector<resultType>& vec){
    int sum = 0;
    for(auto & v : vec)
        sum += v.second.first > 0  ? v.second.first : 0;
    return sum;
}

int sumError(std::vector<resultType>& vec){
    int sum = 0;
    for(auto & v : vec)
        sum += v.second.second;
    return sum;
}



bool error_score_compare(const std::pair<std::string, std::vector<resultType>>& s1,
    const std::pair<std::string, std::vector<resultType>>& s2){
    auto v1 = s1.second, v2 = s2.second; // remove const cast
    int sumError1 = sumError(v1);
    int sumError2 = sumError(v2);
    if(sumError1 != sumError2)
        return sumError1 < sumError2;

    int sumSteps1 = sumStep(v1);
    int sumSteps2 = sumStep(v2);
    return sumSteps1 > sumSteps2;
}



// <algon_name,<travel,<#operations, numErrors>>>
void printResults(std::vector<std::pair<std::string, std::vector<resultType>>>& m, std::string& outputPath) {
    outputPath += "simulation.results";

    std::filesystem::remove(outputPath);
    std::ofstream resultFile(outputPath, std::ios::app );

    if (!resultFile) {
        std::cerr << "Error: simulator results file can't be opened/created" << std::endl;
        exit(1);
    }

    std::sort(m.begin(), m.end(), error_score_compare);


    std::cout << "RESULTS,\t";
    resultFile << "RESULTS,";

    for (auto &v: m.at(0).second) {
        std::cout << v.first << ",\t";
        resultFile << v.first << ",";
    }
    std::cout << "Sum,\tNum Errors\n";
    resultFile << "Sum, Num Errors\n";

    for (auto &algoName: m) {
        std::cout << algoName.first << ",\t";
        resultFile << algoName.first << ",";
        for (auto &v: algoName.second) {
            std::cout << v.second.first << ",\t";
            resultFile << v.second.first << ",";
        }
        std::cout << sumStep(algoName.second) << ",\t" << sumError(algoName.second) << ",\n";
        resultFile << sumStep(algoName.second) << "," << sumError(algoName.second) << ",\n";

    }
    std::cout << std::endl;
    resultFile << std::endl;
    resultFile.close();
}



int main(int argc, char* argv[]) {  
    std::map<std::string, std::function<std::unique_ptr<AbstractAlgorithm>()>>  algorithms;
    std::map<std::string, std::string> travel_files;
    Simulator sim;
	std::vector<void*> handlers;


    // Default path for outputs and algorithms is current path.
    std::string travel_path, algorithm_path("./"), output_path("./");
 
 
    // Parse command line arguments. If parsing failed, exit the program.
    int res = parseFlag(argc, argv, travel_path, algorithm_path, output_path);

    // remove error folder if exists
    std::string errorF = output_path + "errors/";
    if (std::filesystem::is_directory(errorF)) {
        std::filesystem::remove_all(errorF);
    }
    if(std::filesystem::is_directory("./errors/")) {
        std::filesystem::remove_all("./errors/");        
    }

    if (res == -1) {
        const std::string str = "fatal error-  -travel_path, exiting program\n";
        sim.generateError(str, 1);
        return EXIT_FAILURE;

    }
    else if (res == -2) {
        // generate error file and write that a non-valid path was found and exit the program
        const std::string str = "fatal error- Invalid parameter, exiting program.\n";
        sim.generateError(str, 1);
        return EXIT_FAILURE;
    }


	res = loadAll(algorithms, travel_files, travel_path, algorithm_path, handlers);
    // Load travels and algorithms from files. If loading failed, exit the program.
    if (res == -1) {
        const std::string str = "fatal error- invalid travel_path, exiting program\n";
        sim.generateError(str, 1);
        return EXIT_FAILURE;

    }
    else if (res == -2) {
        // generate error file and write that a non-valid path was found and exit the program
        const std::string str = "fatal error- Invalid travel_path or algorithm_path, exiting program.\n";
        sim.generateError(str, 1);
        return EXIT_FAILURE;
    }


    sim.setOutputPath(output_path);
    sim.setTravelPath(travel_path);

    std::vector<std::pair<std::string, std::vector<resultType>>> resultsM;
    
    for (auto& algo : algorithms) {
 	std::unique_ptr<AbstractAlgorithm> _algorithm = algo.second();
        std::vector<resultType> vec;
    	for (auto& trav : travel_files) { 
            std::string _travel = trav.first;
            sim.setCurrentTravelName(trav.second);
            std::size_t p_ = algo.first.find_last_of(".");
            sim.setCurrentAlgoName(algo.first.substr(0, p_));
            sim.simulationRun(_travel, _algorithm);
	        vec.emplace_back(resultType{trav.second, std::pair{sim.getResults(),sim.getNumErrors()}});
    	}
        std::size_t p1 = algo.first.find_last_of(".");
        std::string algoName = algo.first.substr(0, p1);
        resultsM.emplace_back(std::pair{algoName, vec});
    }

    printResults(resultsM, output_path);
	std::cout << "End of Program" << std::endl;


	return EXIT_SUCCESS;
}
