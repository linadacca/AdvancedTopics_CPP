COMP = g++-9.3.0

OBJS = main.o simulator.o WeightBalanceCalculator.o ShipPlan.o Container.o AlgorithmRegistrar.o errorHandler.o AlgorithmRegistration.o
EXEC = simulator

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -pedantic-errors -DNDEBUG -Werror
CPP_LINK_FLAG = -ldl -export-dynamic #(-rdynamic)
 
$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@

#use g++ -MM main.cpp to see dependencies

simulator.o: simulator.h WeightBalanceCalculator.hpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

WeightBalanceCalculator.o: WeightBalanceCalculator.hpp ShipPlan.hpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
ShipPlan.o: ShipPlan.hpp Container.hpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
Container.o: Container.hpp 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
errorHandler.o: errorHandler.h simulator.h 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
AlgorithmRegistrar.o: AbstractAlgorithm.h 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

AlgorithmRegistration.o: AbstractAlgorithm.h AlgorithmRegistrar.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
main.o: AlgorithmRegistrar.h simulator.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
