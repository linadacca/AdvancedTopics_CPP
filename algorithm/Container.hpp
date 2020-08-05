//
//  Container.hpp
//  EX1
//
//  Created by Weaam Masarweh on 09/04/2020.
//  Copyright © 2020 Weaam Masarweh. All rights reserved.
//

#ifndef Container_hpp
#define Container_hpp

#include <string>

//#endif /* Container_hpp */

class Container {

	int weight = -1;
	std::string destination = "";
	std::string id = "";

public:
	Container();
	Container(std::string id, int weight, std::string destination);
	int getWeight();
	std::string getDestination();
	std::string getId();
	void setId(std::string id);
	void setWeight(int weight);
	void setDestination(std::string destination);

	friend bool operator== (Container& lhs, Container& rhs) ;
};
#endif /* Container_hpp */
