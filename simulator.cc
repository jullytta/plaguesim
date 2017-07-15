#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Edge {
    int dest;

    Edge() {};
    Edge(int d): dest(d) {};
};

struct Node {
    int id, status;
    vector<Edge> edge;

    Node() {};
    Node(int i, int s): id(i), status(s) {};
};

struct Graph {
    vector<Node> node;

    Graph() {};
    Graph(int n, int type): node(vector<Node>(n)) {
        if (type == 1) {
            for (int i = 0; i < n; i++) {
                node[i] = Node(i, 0);
            }
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        node[i].edge.push_back(Edge(j));
                    }
                }
            }
        }
    };

    void printAdjacencyMatrix() {
        for (unsigned int i = 0; i < node.size(); i++) {
            unsigned int cur = 0;
            for (unsigned int j = 0; j < node.size(); j++) {
                if (cur < node[i].edge.size() && node[i].edge[cur].dest == j) {
                    cout << "1 ";
                    cur++;
                }
                else {
                    cout << "0 ";
                }
            }
            cout << endl;
        }
    }
};

struct Data {
    bool set;
    double gamma, mu, C;
    int minPop, maxPop, increment, repeats, type;

    Data() {
        set = false;
        gamma = 1.0;
        mu = 1.0;
        C = 1.0;
        minPop = 5;
        maxPop = 30;
        increment = 5;
        repeats = 100;
        type = 1;
    }

    string printGraphType() {
        if (type == 1) {
            return "Clique";
        }
        else if (type == 2) {
            return "Star";
        }
        else if (type == 3) {
            return "Circular";
        }
        return "Custom";
    }
};

Data runUI(Data oldParam) {
    char option = 'x';
    Data newParam;

    if (oldParam.set == true) {
        newParam = oldParam;
    }
    newParam.set = true;

    while (option != 'R') {
        cout << "Welcome to PlagueSim. Please set your parameters before starting the simulation." << endl;
        cout << "[E]ndogenous Infection Rate = " << newParam.gamma << endl;
        cout << "[T]otal Exogenous Infection Rate = " << newParam.C << endl;
        cout << "[C]ure Rate = " << newParam.mu << endl;
        cout << "[S]tarting Population = " << newParam.minPop << endl;
        cout << "[M]aximum Population = " << newParam.maxPop << endl;
        cout << "[I]ncrement = " << newParam.increment << endl;
        cout << "[N]umber of Simulations per Population Size = " << newParam.repeats << endl;
        cout << "[G]raph Type = " << newParam.printGraphType() << endl;
        cout << "[R]un the Simulation" << endl << endl;

        cout << "Select an option: ";
        cin >> option;

        switch (option) {
            case 'E':
                cout << "This is the rate that nodes infect each other." << endl;
                cout << "Please set a new value for Endogenous Infection: ";
                cin >> newParam.gamma;
                break;
            case 'T':
                cout << "This is the total rate that nodes get infected." << endl;
                cout << "Node infection rate will be this value divided by the population." << endl;
                cout << "Please set a new value for the Exogenous Infection: ";
                cin >> newParam.C;
                break;
            case 'C':
                cout << "This is the rate that nodes cure themselves from infection." << endl;
                cout << "Please set a new value for Cure Rate: ";
                cin >> newParam.mu;
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
            case 'G':
                cout << "[1]. Clique" << endl;
                cout << "[2]. Star" << endl;
                cout << "[3]. Circular" << endl; 
                cout << "Please select a preset graph type for the simulation: ";
                cin >> newParam.type;
            default:
                cout << "Invalid command." << endl;
        }
        cout << endl;
    }

    return newParam;
}

void runSimulation(Data parameters) {
    Graph graph;

    for (int pop = parameters.minPop; pop <= parameters.maxPop; pop += parameters.increment) {
        graph = Graph(pop, parameters.type);
        
        for (int curRepeat = 0; curRepeat < parameters.repeats; curRepeat++) {

        }
    }
}

int main (int argc, char** argv) {
    Data parameters;

    if (argc == 8) {
        parameters.set = true;
        parameters.gamma = atof(argv[1]);
        parameters.C = atof(argv[2]);
        parameters.mu = atof(argv[3]);
        parameters.minPop = atoi(argv[4]);
        parameters.maxPop = atoi(argv[5]);
        parameters.increment = atoi(argv[6]);
        parameters.repeats = atoi(argv[7]);
    }
    
    parameters = runUI(parameters);
    runSimulation(parameters);

    return 0;
}