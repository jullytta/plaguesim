#include <iostream>
#include <vector>

using namespace std;

class Edge {
    int dest;

    public:
        Edge() {};
        Edge(int d): dest(d) {};
};

class Node {
    int id, status;
    vector<Edge> connection;

    public:
        Node() {};
        Node(int i, int s): id(i), status(s) {};
};

struct Data {
    bool set;
    double gamma;
    int minPop, maxPop, increment, repeats;

    Data() {
        set = false;
        gamma = 1.0;
        minPop = 5;
        maxPop = 30;
        increment = 5;
        repeats = 100;
    }
};

Data runUI(Data oldParam) {
    char option = 'x';
    Data newParam;

    if (oldParam.set == true) {
        newParam = oldParam;
    }

    while (option != 'R') {
        cout << "Welcome to PlagueSim. Please set your parameters before starting the simulation." << endl;
        cout << "[G]amma = " << newParam.gamma << endl;
        cout << "[S]tarting Population = " << newParam.minPop << endl;
        cout << "[M]aximum Population = " << newParam.maxPop << endl;
        cout << "[I]ncrement = " << newParam.increment << endl;
        cout << "[N]umber of Simulations per Population Size = " << newParam.repeats << endl;
        cout << "[R]un the Simulation" << endl << endl;

        cout << "Select an option: ";
        cin >> option;

        switch (option) {
            case 'G':
                cout << "Gamma is the exogenous infection rate." << endl;
                cout << "Please set a new value for Gamma: ";
                cin >> newParam.gamma;
                break;
            case 'S':
                cout << "Starting Population is the smallest population size that will be simulated." << endl;
                cout << "Please set a new value for the Starting Population: ";
                cin >> newParam.minPop;
                break;
            case 'M':
                cout << "Maximum Population is the largest population size that will be simulated." << endl;
                cout << "Please set a new value for the Maximum Population: ";
                cin >> newParam.maxPop;
                break;
            case 'I':
                cout << "Increment will increase the population size until Maximum Population." << endl;
                cout << "Smaller values of increment means a smoother graph, but take longer to simulate." << endl;
                cout << "Please set a new Increment value: ";
                cin >> newParam.increment;
                break;
            case 'N':
                cout << "Represents how many times a certain population size will be simulated." << endl;
                cout << "Higher values have better accuracy, but take longer to simulate." << endl;
                cout << "Please set a new Number of Simulations: ";
                cin >> newParam.repeats;
                break;
            case 'R':
                cout << "The simulation will start now." << endl;
                break;
            default:
                cout << "Invalid command." << endl;
        }
        cout << endl;
    }

    return newParam;
}

int main (int argc, char** argv) {
    Data parameters;
    
    if (argc == 6) {
        parameters.set = true;
        parameters.gamma = atof(argv[1]);
        parameters.minPop = atoi(argv[2]);
        parameters.maxPop = atoi(argv[3]);
        parameters.increment = atoi(argv[4]);
        parameters.repeats = atoi(argv[5]);
    }
    
    parameters = runUI(parameters);

    return 0;
}