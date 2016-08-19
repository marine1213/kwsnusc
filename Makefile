CC = gcc
CXX = g++
#CFLAGS =
#CXXFLAGS =
LINK = g++
#LIBS =

RASPFLAGS = -lwiringPi -DRaspberryPi

TARGET = AutoPilot
OBJS = Car_control_main.o carStruct.o lowInter.o LandRover_mod_KS.o
SRCS = $(OBJS:.o=.cpp)
#BUILD_PATH = -I/home/pi/project_16ss/RenewCar/build/ -g -c

$(TARGET) : Car_control_main.o carStruct.o LandRover_mod_KS.o lowInter.o $(RASPFLAGS)
	$(CXX) -c -o $(TARGET) Car_control_main.cpp Car_control_main.o carStruct.o LandRover_mod_KS.o lowInter.o $(RASPFLAGS)

Car_control_main.o : Car_control_main.cpp Car_control_main.h common.h
	$(CXX) -c -o Car_control_main.o Car_control_main.cpp

LandRover_mod_KS.o : LandRover_mod_KS.cpp LandRover_mod_KS.h common.h
	$(CXX) -c -o LandRover_mod_KS.o LandRover_mod_KS.cpp

lowInter.o : lowInter.cpp intersection.h carStruct.h
	$(CXX) -c -o lowInter.o lowInter.cpp

carStruct.o : carStruct.cpp carStruct.h
	$(CXX) -c -o carStruct.o carStruct.cpp

clean : rm $(TARGET) $(OBJS)

