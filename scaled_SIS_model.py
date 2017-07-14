import numpy as np


def get_clique(N):
  "Returns an adjacency matrix corresponding to a NxN clique."
  clique = np.ones([N, N], dtype=np.int_)

  for i in range(0, N):
    for j in range(0, N):
      if i == j:
        clique[i, j] = 0

  return clique


def main():
  # The population size.
  N = 3

  # Adjacency matrix
  # We will start working with cliques only
  A = get_clique(N)

  # pi[i] is the stationary distribution
  pi = np.zeros([N], dtype=np.float_)

  # Current configuration
  x = np.zeros([N, 1], dtype=np.int_)
  x_t = np.transpose(x)

  result = np.matmul(A, x)
  result = np.matmul(x_t, result)
  print(result)


if __name__ == '__main__':
  main()