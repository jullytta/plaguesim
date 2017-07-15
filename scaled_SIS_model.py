import numpy as np


def get_clique(N):
  "Returns an adjacency matrix corresponding to a NxN clique."
  clique = np.ones([N, N], dtype=np.int_)

  for i in range(0, N):
    for j in range(0, N):
      if i == j:
        clique[i, j] = 0

  return clique


# We define a configuration as a state the system can assume.  #
# Each configuration is a combination of possible values for   #
# the nodes. A node can be susceptible (0) or infected (1).    #
# Therefore, a configuration vector is a binary vector of      #
# size N, number of nodes.                                     #
#                                                              #
# Suppose we have N = 3. That means each configuration has     #
# 3 bits. Configuration 2 equals 010, which means nodes zero   #
# and two are susceptible, while node one is infected.         #
def get_configuration_vector(N, i):
  x = np.zeros([N, 1], dtype=np.int_)

  for k in range(0, N):
    infected_flag = (i >> N-(k+1)) & 1
    x[k] = infected_flag

  return x


# Since pi is a vector of probabilities that should cover all  #
# possible scenarios with no overlapping, the sum of its       #
# elements has to be 1.                                        #
def sanity_check_pi(pi):
  sum = 0
  for i in pi:
    sum = i + sum
  
  # Weird floating point precision
  if(sum < 0.995 or sum > 1.005):
    print("Something went wrong.")
    print("Sum of pi elements is not 1.0, but rather ", sum)
  
  return


def calculate_pi_and_n_infected(A, N, pi, n_infected, lambda_, mu, gamma):
  # Transposed vector of ones
  one_t = np.ones([1, N], dtype=np.int_)

  # Z is the sum of all partial pi
  # In the end, pi has to be divided by Z to ensure all values
  # lie between 0 and 1 - they are probabilities
  Z = 0
  # i is the current configuration
  for i in range(0, n_infected.size):
    x = get_configuration_vector(N, i)
    x_t = np.transpose(x)

    # Number of infected nodes in this configuration
    n_infected[i] = np.matmul(one_t, x)[0, 0]

    # Number of edges where both ends are infected
    n_infected_edges = np.matmul(np.matmul(x_t, A), x)[0, 0]//2

    # Partial pi
    pi[i] = pow((lambda_/mu), n_infected[i])*pow(gamma, n_infected_edges)

    Z = Z + pi[i]

  pi = pi/Z

  # Note that the reference can't be changed inside the function,
  # so we must return it and reassign.
  # Python sucks.
  return pi


def get_expected_infected(pi, n_infected):
  "Returns the expected value of the number of infected nodes."
  expected_infected = 0

  # Definition of expected value: sum of all possible values *
  # their probability.
  for i in range(0, n_infected.size):
    expected_infected = expected_infected + pi[i]*n_infected[i]

  return expected_infected


def main():
  # TODO(jullytta): the following should become parameters
  # The population size.
  N = 3
  # The cure rate
  mu = 1
  # The exogenous infection rate
  lambda_ = 1/N
  # The endogenous infection rate
  gamma = 1

  # The number of different configurations our system
  # can assume. Each node can be either susceptible (0)
  # or infected (1), so there are 2^N possible settings.
  n_configs = pow(N, 2)-1

  # Adjacency matrix
  # We will start working with cliques only
  A = get_clique(N)

  # pi[i] is the stationary distribution
  # i.e., the probability of seeing configuration i
  pi = np.zeros([n_configs], dtype=np.float_)

  # n_infected[i] is the number of infected nodes
  # at configuration i
  n_infected = np.zeros([n_configs], dtype=np.int_)

  # We will need the number of infected nodes for each
  # configuration later on to calculate the expected value of
  # the number of infected nodes.
  # This information is also needed to calculate pi, so we might
  # as well get these values from the following function instead
  # of recalculating them.
  # tl;dr: n_infected will be updated for later use
  pi = calculate_pi_and_n_infected(A, N, pi, n_infected, lambda_, mu, gamma)

  # Check if the sum of all pi[i] equals 1
  sanity_check_pi(pi)
  
  # Print stats
  print("# of nodes: ", N)
  print("# of configurations: ", n_configs+1)
  print("# of infected nodes (expected value): ", get_expected_infected(pi, n_infected))


if __name__ == '__main__':
  main()