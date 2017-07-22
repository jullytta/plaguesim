P=plaguesim
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -O2

default: ${P}.cc
	${CXX} ${P}.cc -o ${P} ${CXXFLAGS}

w_validation_flag: ${P}.cc
	${CXX} ${P}.cc -o ${P} ${CXXFLAGS} -DVALIDATION_FLAG

run: default
	./${P}

validate: w_validation_flag
	./${P}

clean:
	rm -f ${P}