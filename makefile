CC = mpic++
EXECUTABLE_NAME = app

BASE_FLAGS = -Wall -Wextra 

DEBUG_DIR = build_debug
DEBUG_FLAGS = -g $(BASE_FLAGS)

RELEASE_DIR = build_release
RELEASE_FLAGS = -O3 $(BASE_FLAGS)

all: debug release

clean:
	rm -rf $(DEBUG_DIR)
	rm -rf $(RELEASE_DIR)

debug: main.cpp $(DEBUG_DIR)
	$(CC) $(DEBUG_FLAGS) main.cpp -o $(DEBUG_DIR)/$(EXECUTABLE_NAME) 

release: main.cpp $(RELEASE_DIR)
	$(CC) $(RELEASE_FLAGS) main.cpp -o $(RELEASE_DIR)/$(EXECUTABLE_NAME) 

$(DEBUG_DIR):
	mkdir -p $(DEBUG_DIR)

$(RELEASE_DIR):
	mkdir -p $(RELEASE_DIR)

