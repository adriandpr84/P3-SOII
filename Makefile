CXX := g++
CXXFLAGS := -Wall -Wextra -Werror -std=c++20 -pthread -g -Icode/include

SRC_DIR := code/src
OBJ_DIR := code/obj
BIN_DIR := code/bin
TEST_DIR = code/tests
BUILD_DIR = $(TEST_DIR)/build
TARGET  := $(BIN_DIR)/SSOOIIGLE
RESULTS := simulation_results


SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: all
	./$(TARGET)

clean: clean-test clean-results
	rm -rf $(OBJ_DIR) $(BIN_DIR)

clean-results:
	rm -rf $(RESULTS)

re: clean all

test:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. > /dev/null
	@$(MAKE) -C $(BUILD_DIR)
	@cd $(BUILD_DIR) && ctest --output-on-failure

clean-test:
	@rm -rf $(BUILD_DIR)

.PHONY: all clean run re test clean-test