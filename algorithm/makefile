COMP = g++-9.3.0

SHARED_OBJS =  _301689667_a.so _301689667_b.so
OBJS = _301689667_a.o _301689667_b.o WeightBalanceCalculator.o ShipPlan.o Container.o AbstractAlgorithm.o AlgorithmRegistration.o

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden
CPP_LINK_FLAG = -shared

all: $(SHARED_OBJS)


_301689667_a.so: _301689667_a.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

_301689667_a.o: _301689667_a.hpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
_301689667_b.so: _301689667_b.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

_301689667_b.o: _301689667_b.hpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

WeightBalanceCalculator.o: WeightBalanceCalculator.hpp ShipPlan.hpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

ShipPlan.o: ShipPlan.hpp Container.hpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
Container.o: Container.hpp 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp


clean:
	rm -f $(OBJS) $(SHARED_OBJS)
	
	

