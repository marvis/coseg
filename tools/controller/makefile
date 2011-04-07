CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff

OBJ_CONTROLLER = ../../myalgorithms.o ../../component_tree.o ../../CT3D/cell_track.o ../../CT3D/palette.o ../../CT3D/cell_track_controller.o controller.o

controller:$(OBJ_CONTROLLER)
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_CONTROLLER) -o controller

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm -f *.o
	rm -f ../../*.o
	rm -f ../../CT3D/*.o
