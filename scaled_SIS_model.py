import numpy as np
import sys


def get_clique(N):
  'Returns an adjacency matrix corresponding to a NxN clique.'
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
    print('Something went wrong.')
    print('Sum of pi elements is not 1.0, but rather ', sum)
  
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
  'Returns the expected value of the number of infected nodes.'
  expected_infected = 0

  # Definition of expected value: sum of all possible values *
  # their probability.
  for i in range(0, n_infected.size):
    expected_infected = expected_infected + pi[i]*n_infected[i]

  return expected_infected


def get_graph_type(n):
  if n == 1:
    return 'Clique'
  if n == 2:
    return 'Star'
  if n == 3:
    return 'Circular'
  return 'Custom'


def main():
  # Reading configuration file
  with open('parameters.cfg', 'r') as config_file:
    params = config_file.readlines()
    # Endogenous infection rate
    gamma = float(params[0])
    # Total exogenous infection rate
    c = float(params[1])
    # Cure rate
    mu = float(params[2])
    # Starting population
    start_pop = int(params[3])
    # Maximum population
    max_pop = int(params[4])
    # Increment
    increment = int(params[5])
    # Graph type
    graph_type = get_graph_type(int(params[8]))
    # Output file
    out_file_name = 'model_' + params[11].rstrip()

  # Print the parameters for double checking
  print('Please check the following parameters.')
  print('Endogenous Infection Rate =', gamma)
  print('Total Exogenous Infection Rate =', c)
  print('Cure Rate =', mu)
  print('Starting Population =', start_pop)
  print('Maximum Population =', max_pop)
  print('Increment =', increment)
  print('Graph Type =', graph_type)
  print('Output File =', out_file_name)

  print()
  
  print('Starting computation...')
  print('This might take a while.')

  out_file = open(out_file_name, 'w')
  out_file.write(str(gamma)+'\n')

  # Calculates the expected value of infected nodes for
  # each size of population.
  for N in range(start_pop, max_pop+1, increment):
    print()
    print('Running population', N)

    # Exogenous infection rate (for each node)
    lambda_ = c/N
  
    # The number of different configurations our system
    # can assume. Each node can be either susceptible (0)
    # or infected (1), so there are 2^N possible settings.
    n_configs = pow(2, N)

    # Adjacency matrix
    if(graph_type == 'Clique'):
      A = get_clique(N)
    else:
      print('Sorry, this type of graph is currently not supported.')
      print('Try again later!')
      return

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
    
    e_infected = get_expected_infected(pi, n_infected)
    p_infected = e_infected/N

    out_file.write(str(N)+' '+'{0:.5f}'.format(p_infected)+'\n')

    # Print stats
    print('Finished population', N)
    print('Number of nodes:', N)
    print('Number of configurations:', n_configs)
    print('Expected value of infected: {0:.5f}'.format(e_infected))
    print('Probability of being infected: {0:.5f}'.format(p_infected))

  out_file.close()

if __name__ == '__main__':
  main()