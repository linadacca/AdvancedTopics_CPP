
#ifndef ShipPlan_hpp
#define ShipPlan_hpp

#include <vector>
#include "Container.hpp"


class ShipPlan {
	int numOfFloors = -1;/*for abort reasons*/
	int numOfConatainersInDimX = -1;
	int numOfContainersInDimY = -1;
	std::vector <std::vector <std::vector <Container> >>  myShipPlan;

public:
	ShipPlan() {};
	ShipPlan(int numOfFloors, int DimX, int DimY, std::vector <std::vector <std::vector <Container> > >  ShipPlan);

	//gettes, to add when needed
	std::vector <std::vector <std::vector <Container> >>& getShipPlan();
	int getNumFloors();
	int getNumContainersDimX();
	int getNumContainersDimY();


	//setters, to add when needed
	void setMyShiplan(int numFloor, int positionX, int positionY, Container& container);


};
#endif /* ShipPlan_hpp */