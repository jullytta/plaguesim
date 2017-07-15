import numpy as np


def get_clique(N):
  "Returns an adjacency matrix corresponding to a NxN clique."
  clique = np.ones([N, N], dtype=np.int_)

  for i in range(0, N):
    for j in range(0, N):
      if i == j:
        clique[i, j] = 0

  return clique


################################################################
# We define a configuration as a state the system can assume.  #
# Each configuration is a combination of possible values for   #
# the nodes. A node can be susceptible (0) or infected (1).    #
# Therefore, a configuration vector is a binary vector of      #
# size N, number of nodes.                                     #
#                                                              #
# Suppose we have N = 3. That means each configuration has     #
# 3 bits. Configuration 2 equals 010, which means nodes zero   #
# and two are susceptible, while node one is infected.         #
################################################################
def get_configuration_vector(N, i):
  x = np.zeros([N, 1], dtype=np.int_)

  for k in range(0, N):
    infected_flag = (i >> N-(k+1)) & 1
    x[k] = infected_flag

  return x


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
  n_configs = pow(N, 2)

  # Adjacency matrix
  # We will start working with cliques only
  A = get_clique(N)

  # pi[i] is the stationary distribution
  # i.e., the probability of seeing configuration i
  pi = np.zeros([n_configs], dtype=np.float_)

  # n_infected[i] is the number of infected nodes
  # at configuration i
  n_infected = np.zeros([n_configs], dtype=np.int_)

  # Transposed vector of ones
  one_t = np.ones([1, N], dtype=np.int_)

  # TODO(jullytta): This is where the for loop should start
  # Even better, make it into a function
  # Current configuration
  i = 4

  x = get_configuration_vector(N, i)
  x_t = np.transpose(x)

  # Number of infected nodes in this configuration
  n_infected[i] = np.matmul(one_t, x)[0, 0]

  # Number of edges where both ends are infected
  n_infected_edges = np.matmul(np.matmul(x_t, A), x)[0, 0]//2

  pi[i] = pow((lambda_/mu), n_infected[i])*pow(gamma, n_infected_edges)

  # Print stats
  print("# of nodes: ", N)
  print("# of configurations: ", n_configs)
  print("Current configuration: ", x_t)
  print("Partial pi: ", pi[i])
  print("# of infected nodes: ", n_infected[i])
  print("# of infected edges: ", n_infected_edges)


if __name__ == '__main__':
  main()