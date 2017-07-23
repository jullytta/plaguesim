class bcolors:
  HEADER = '\033[95m'
  OKBLUE = '\033[94m'
  OKGREEN = '\033[92m'
  WARNING = '\033[93m'
  FAIL = '\033[91m'
  ENDC = '\033[0m'
  BOLD = '\033[1m'
  UNDERLINE = '\033[4m'

def main():
  # Reading configuration file
  with open('parameters.cfg', 'r') as config_file:
    params = config_file.readlines()
    # Simulator output
    sim_out_file = open(params[11].rstrip())
    # Model output
    model_out_file = open('validation_' + params[11].rstrip())

  sim_out = sim_out_file.readlines()
  model_out = model_out_file.readlines()

  n_pops = len(model_out)

  sim_out_file.close()
  model_out_file.close()

  n_wrong = 0
  for i in range(1, n_pops):
    model_tokens = model_out[i].split()
    sim_tokens = sim_out[i].split()

    sim_lower = float(sim_tokens[1])
    sim_higher = float(sim_tokens[2])
    
    model_mean = float(model_tokens[1])

    if(sim_lower > model_mean or sim_higher < model_mean):
      n_wrong += 1
      result = bcolors.FAIL + 'fail' + bcolors.ENDC
    else:
      result = bcolors.OKGREEN + 'success' + bcolors.ENDC

    print('Verifying population with size', sim_tokens[0] + '... ' + result + '.')
    print('Confidence interval: [{0:.5f}'.format(sim_lower) + ', {0:.5f}'.format(sim_higher) + ']')
    print('Model mean: {0:.5f}'.format(model_mean) + '\n')

  print('Final results:')
  print(bcolors.OKGREEN + '{:.0%}'.format((n_pops-n_wrong)/n_pops) + bcolors.ENDC + ' pass, ' + bcolors.FAIL + '{:.0%}'.format(n_wrong/n_pops) + bcolors.ENDC + ' fail.')


if __name__ == '__main__':
  main()