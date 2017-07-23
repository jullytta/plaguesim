P=plaguesim
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -O2

default: ${P}.cc
	${CXX} ${P}.cc -o ${P} ${CXXFLAGS}

run: default
	./${P}

w_validation_flag: ${P}.cc
	@printf 'Recompiling with appropriate flags...'
	@${CXX} ${P}.cc -o ${P} ${CXXFLAGS} -DVALIDATION_FLAG
	@printf ' done.\n'

validate: w_validation_flag
	@printf 'Running the simulation...'
	@printf 'r x' | ./${P} 1>/dev/null
	@printf ' done.\n'
	@printf 'Calculating the analytical results...'
	@python3 scaled_SIS_model.py 1>/dev/null
	@printf ' done.\n\n'
	@python3 validation.py

clean:
	rm -f ${P}