import numpy as np


def get_clique(N):
  "Returns an adjacency matrix corresponding to a NxN clique."
  clique = np.zeros([N, N], dtype=np.byte)

  for i in range(0, N):
    for j in range(0, N):
      if i != j:
        clique[i, j] = 1

  return clique


def main():
  # The population size.
  N = 3

  # Adjacency matrix
  # We will start working with cliques only
  A = get_clique(N)

  # pi[i] is the stationary distribution
  pi = np.zeros([N], dtype=np.int32)


if __name__ == '__main__':
  main()