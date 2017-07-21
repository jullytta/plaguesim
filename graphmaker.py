'''
    To install numpy and matplotlib you can use pip (or anaconda). 
    Using pip is as simple as running the following commands:

    $ pip install numpy
    $ pip install matplotlib

    If decide to use anaconda, you just have to replace pip with conda. 
'''
import numpy as np
import numpy.polynomial.polynomial as poly
import matplotlib.pyplot as plt
from enum import Enum
from glob import glob
from itertools import cycle

class Color(Enum):
    BLUE      = '#0473bd'
    ORANGE    = '#d95218'
    YELLOW    = '#eeb226'
    PURPLE    = '#9656a3'
    GREEN     = '#a9ca7b'
    LIGHTBLUE = '#72cbf2'

def readFile(fname):
    with open(fname) as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    rate   = content[0]
    rate_label, rate_value = rate.split()
    rate_value = float(rate_value)
    x_axis, y_axis = list(), list()
    for c in range(len(content)-1):
        x, y = map(float, content[c+1].split())
        x_axis.append(x)
        y_axis.append(y)
    return x_axis, y_axis, rate_label.lower(), rate_value

def smoothCurve(x, y, degree=3):
    xnew   = np.linspace(x[0], x[-1], num=len(x)*10)
    coefs  = poly.polyfit(x, y, degree)
    ynew   = poly.polyval(xnew, coefs)
    return xnew, ynew

# plots a single curve per csv file with a defined color
def plotCurve(filename, color='b', smooth=True):
    x, y, rate_label, rate_value = readFile(filename)
    if smooth: 
        x, y = smoothCurve(x, y)
    line, = plt.plot(x, y, label='$\{0}$={1:.6f}'.format(rate_label, rate_value), linewidth=0.7, color=color)

    plt.xlabel('probability of a tagged node is infected')
    plt.ylabel('number of nodes in the network')
    plt.legend(loc='best')

    # creates an axis object and remove borders
    ax = plt.gca()
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.tick_params(labeltop='off', labelright='off')

    # adjust tick position and step
    ticks = np.arange(0, 1.1, 0.1) # y-axis go from 0 to 1 with step 0.1
    ax.set_yticks(ticks)
    ax.get_yaxis().set_tick_params(direction='in')
    ax.get_xaxis().set_tick_params(direction='in')

    # set x-axis from 0 to 60 and y-axis from 0 to 1  
    plt.axis([0.0, 60.0, 0.0, 1.0])  


def main():
    files  = sorted(glob('*.csv'))
    # creates a circular list of colors in case there are more csv files than colors
    colors = [e.value for e in Color]
    colors = cycle(colors) 
    for file in files:
        plotCurve(file, next(colors))
    plt.savefig('plot.png', bbox_inches='tight')
    plt.show()

if __name__ == '__main__':
    main()