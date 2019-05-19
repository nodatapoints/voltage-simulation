.PHONY: run clean

APP=volt
GPP_FLAGS=-std=c++11
LIBRARIES=-lsfml-graphics -lsfml-window -lsfml-system -lGL -lGLEW

${APP}: main.o shaderUtils.o
	g++ ${GPP_FLAGS} -o ${APP} *.o ${LIBRARIES} 

main.o: main.cpp
	g++ ${GPP_FLAGS} -c main.cpp

shaderUtils.o: shaderUtils.cpp shaderUtils.hpp
	g++ ${GPP_FLAGS} -c shaderUtils.cpp

run: ${APP}
	./${APP}

clean:
	rm -rvf ${APP} *.o

