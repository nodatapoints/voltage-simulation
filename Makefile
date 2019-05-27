.PHONY: run clean

APP=volt
GPP_FLAGS=-std=c++11
LIBRARIES=-lsfml-graphics -lsfml-window -lsfml-system -lGL -lGLEW

${APP}: main.o shaderUtils.o parser.o
	g++ ${GPP_FLAGS} -o ${APP} *.o ${LIBRARIES} 

main.o: main.cpp glUtils.inl
	g++ ${GPP_FLAGS} -c main.cpp

shaderUtils.o: shaderUtils.cpp shaderUtils.hpp
	g++ ${GPP_FLAGS} -c shaderUtils.cpp

parser.o: parser.cpp parser.hpp
	g++ ${GPP_FLAGS} -c parser.cpp

run: ${APP}
	./${APP} shape.txt  # for debug purposes

clean:
	rm -rvf ${APP} *.o

