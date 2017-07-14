import numpy as np


def get_clique(N):
  "Returns an adjacency matrix corresponding to a NxN clique."
  clique = np.ones([N, N], dtype=np.int_)

  for i in range(0, N):
    for j in range(0, N):
      if i == j:
        clique[i, j] = 0

  return clique


def get_configuration(N, i):
  x = np.zeros([N, 1], dtype=np.int_)

  for k in range(0, N):
    infected_flag = (i >> N-(k+1)) & 1
    x[k] = infected_flag

  return x


def main():
  # The population size.
  N = 3

  # Adjacency matrix
  # We will start working with cliques only
  A = get_clique(N)

  # pi[i] is the stationary distribution
  pi = np.zeros([N], dtype=np.float_)

  # Current configuration
  x = get_configuration(N, 4)
  x_t = np.transpose(x)

  # Transposed vector of ones
  one_t = np.ones([1, N], dtype=np.int_)

  # Number of infected nodes in this configuration
  n_infected = np.matmul(one_t, x)[0, 0]

  print("Current configuration: ", x_t)
  print("# of infected nodes: ", n_infected)

if __name__ == '__main__':
  main()