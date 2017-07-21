P=plaguesim
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -O2

${P}: ${P}.cc
	${CXX} ${P}.cc -o ${P} ${CXXFLAGS}

run: ${P}
	./${P}

clean:
	rm -f ${P}