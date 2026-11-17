CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

OMPFLAGS = -fopenmp

BUILD_DIR = build

COMMON_SRC = src/CVRP.cpp \
             src/Solution.cpp \
             src/LocalSearch.cpp \
             src/GeneticAlgorithm.cpp


.PHONY: all main sequential parallel clean



all: main sequential parallel adaptive

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

adaptive: $(BUILD_DIR)/adaptive.exe

$(BUILD_DIR)/adaptive.exe: adaptive.cpp $(COMMON_SRC) src/Island.cpp src/Migration.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) adaptive.cpp $(COMMON_SRC) src/Island.cpp src/Migration.cpp -o $@




main: $(BUILD_DIR)/cvrp.exe

$(BUILD_DIR)/cvrp.exe: src/main.cpp $(COMMON_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) src/main.cpp $(COMMON_SRC) -o $@


sequential: $(BUILD_DIR)/sequential.exe

$(BUILD_DIR)/sequential.exe: sequential.cpp $(COMMON_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) sequential.cpp $(COMMON_SRC) -o $@


parallel: $(BUILD_DIR)/parallel.exe

$(BUILD_DIR)/parallel.exe: parallel.cpp $(COMMON_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) parallel.cpp $(COMMON_SRC) -o $@


clean:
	rm -f $(BUILD_DIR)/*.exe