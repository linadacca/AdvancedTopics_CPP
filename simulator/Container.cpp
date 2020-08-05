

#include "Container.hpp"


//Container::Container():weight(0), destination("0"), id("0"){}
Container::Container() {}
Container::Container(std::string id, int weight, std::string destination) {
	this->weight = weight;
	this->destination = destination;
	this->id = id;
}

int Container::getWeight() {
	return this->weight;
}

std::string Container::getId() {
	return this->id;
}

std::string Container::getDestination() {
	return this->destination;
}

void Container::setId(std::string id) {
	this->id = id;
}
void Container::setWeight(int weight) {
	this->weight = weight;
}
void Container::setDestination(std::string destination) {
	this->destination = destination;
}

bool operator== (Container& lhs, Container& rhs) {
	if (lhs.id == rhs.id && lhs.weight == rhs.weight && lhs.destination == rhs.destination) {
		return true;
	}
	return false;
}
