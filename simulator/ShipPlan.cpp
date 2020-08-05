//
//  ShipPlan.cpp
//  EX1
//
//  Created by Weaam Masarweh on 07/04/2020.
//  Copyright © 2020 Weaam Masarweh. All rights reserved.
//

#include "ShipPlan.hpp"



ShipPlan::ShipPlan(int numOfFloors, int dimX, int dimY, std::vector <std::vector <std::vector <Container>>>  MyShipPlan) {

	this->numOfFloors = numOfFloors;
	this->numOfConatainersInDimX = dimX;
	this->numOfContainersInDimY = dimY;
	this->myShipPlan = MyShipPlan;
}

std::vector <std::vector <std::vector <Container> >>& ShipPlan::getShipPlan() {
	return this->myShipPlan;
}
int ShipPlan::getNumFloors() {
	return this->numOfFloors;
}
int ShipPlan::getNumContainersDimX() {
	return this->numOfConatainersInDimX;
}
int ShipPlan::getNumContainersDimY() {
	return this->numOfContainersInDimY;
}


void ShipPlan::setMyShiplan(int numFloor, int positionX, int positionY, Container& container) {
	this->myShipPlan[numFloor][positionX][positionY] = container;
}
