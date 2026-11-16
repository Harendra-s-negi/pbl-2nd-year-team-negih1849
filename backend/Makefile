CC = clang
CXX = clang++

CFLAGS = -Wall -Wextra -O2 -I c_ds
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -I c_ds -I.

C_SRCS = c_ds/heap.c c_ds/linked_list.c c_ds/hash_table.c c_ds/queue.c c_ds/avl_tree.c
C_OBJS = $(C_SRCS:.c=.o)

CPP_SRCS = cpp_oop/models/User.cpp cpp_oop/models/Roles.cpp cpp_oop/models/Patient.cpp cpp_oop/engine/HospitalSystem.cpp db/DatabaseManager.cpp
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

PBL_TARGET = smart_hospital_pbl
SERVER_TARGET = smart_hospital_server

all: $(PBL_TARGET) $(SERVER_TARGET)

c_ds/%.o: c_ds/%.c
	$(CC) $(CFLAGS) -c $< -o $@

cpp_oop/models/%.o: cpp_oop/models/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

cpp_oop/engine/%.o: cpp_oop/engine/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

db/%.o: db/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PBL_TARGET): $(C_OBJS) $(CPP_OBJS) main.o
	$(CXX) $(CXXFLAGS) $(C_OBJS) $(CPP_OBJS) main.o -lsqlite3 -o $(PBL_TARGET)

$(SERVER_TARGET): $(C_OBJS) $(CPP_OBJS) server.o
	$(CXX) $(CXXFLAGS) $(C_OBJS) $(CPP_OBJS) server.o -lsqlite3 -o $(SERVER_TARGET)

run: $(PBL_TARGET)
	./$(PBL_TARGET)

server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

clean:
	rm -f $(PBL_TARGET) $(SERVER_TARGET) c_ds/*.o cpp_oop/models/*.o cpp_oop/engine/*.o db/*.o *.o

.PHONY: all run server clean
