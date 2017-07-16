#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#define MAX_ITERATIONS 500

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
mt19937 randomGen(seed);
ofstream outputFile;

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

struct Event {
    int node;
    double eventTime;

    Event() {};
    Event(int n, double t): node(n), eventTime(t) {};
};

struct Edge {
    unsigned int dest;

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

    Graph() {}
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
    }

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

    Event findNextInfection(Data parameters) {
        Event infection(-1, 0);
        double lambda = parameters.C / node.size();

        for (unsigned int curNode = 0; curNode < node.size(); curNode++) {
            double infTime;
            double ratio = lambda;

            if (node[curNode].status == 0) {
                for (unsigned int neighbor = 0; neighbor < node[curNode].edge.size(); neighbor++) {
                    if (node[node[curNode].edge[neighbor].dest].status == 1) {
                        ratio *= parameters.gamma;
                    }
                }

                exponential_distribution<double> infectionTime(ratio);
                infTime = infectionTime(randomGen);

                if (infection.node == -1 || infection.eventTime > infTime) {
                    infection = Event(curNode, infTime);
                }
            }
        }

        return infection;
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

bool eventSort(Event a, Event b) {
    return (a.eventTime < b.eventTime);
}

void runSimulation(Data parameters) {
    int curSim, totalSims;
    Graph graph;
    vector<Event> cureList;

    curSim = 1;
    totalSims = ((parameters.maxPop - parameters.minPop)/parameters.increment + 1) * parameters.repeats;
    cout << "Preparing to run " << totalSims << " simulations..." << endl;
    for (int pop = parameters.minPop; pop <= parameters.maxPop; pop += parameters.increment) {
        double averageInfected = 0.0;

        for (int curRepeat = 0; curRepeat < parameters.repeats; curRepeat++) {
            graph = Graph(pop, parameters.type);
            double simulationTime = 0.0;
            vector<Event> cures;

            cout << "Running simulation " << curSim << " of " << totalSims << "..." << endl; 

            for (int count = 0; count < MAX_ITERATIONS; count++) {
                cout << fixed << setprecision(5);
                Event nextInfection = graph.findNextInfection(parameters);

                if (nextInfection.node != -1) {
                    if (cures.size() == 0) {
                        graph.node[nextInfection.node].status = 1;
                        simulationTime += nextInfection.eventTime;
                        exponential_distribution<double> cureTime(parameters.mu);
                        cures.push_back(Event(nextInfection.node, cureTime(randomGen)));
                        sort(cures.begin(), cures.end(), eventSort);

                        cout << "(" << simulationTime << "t) Node " << nextInfection.node << " has been infected." << endl;
                    }
                    else {
                        if (cures[0].eventTime > nextInfection.eventTime) {
                            graph.node[nextInfection.node].status = 1;
                            simulationTime += nextInfection.eventTime;
                            for (unsigned int i = 0; i < cures.size(); i++) {
                                cures[i].eventTime -= nextInfection.eventTime;
                            }
                            exponential_distribution<double> cureTime(parameters.mu);
                            cures.push_back(Event(nextInfection.node, cureTime(randomGen)));
                            sort(cures.begin(), cures.end(), eventSort);

                            cout << "(" << simulationTime << "t) Node " << nextInfection.node << " has been infected." << endl;
                        }
                        else {
                            simulationTime += cures[0].eventTime;

                            cout << "(" << simulationTime << "t) Node " << cures[0].node << " has been healed." << endl;

                            graph.node[cures[0].node].status = 0;
                            for (unsigned int i = 1; i < cures.size(); i++) {
                                cures[i].eventTime -= cures[0].eventTime;
                            }
                            cures.erase(cures.begin());
                        }
                    }
                }
                else {
                    simulationTime += cures[0].eventTime;
                    graph.node[cures[0].node].status = 0;
                    for (unsigned int i = 1; i < cures.size(); i++) {
                        cures[i].eventTime -= cures[0].eventTime;
                    }
                    cures.erase(cures.begin());
                }

                cout << "------------------------" << endl;
                cout << "Currently infected nodes:" << endl;
                for (unsigned int i = 0; i < cures.size(); i++) {
                    cout << cures[i].node << " will be healed in " << cures[i].eventTime << "." << endl;
                }
                cout << "------------------------" << endl;
            }

            int numberOfInfected = 0;
            for (unsigned int i = 0; i < graph.node.size(); i++) {
                if (graph.node[i].status == 1) {
                    numberOfInfected++;
                }
            }
            averageInfected += numberOfInfected;

            curSim++;
        }

        averageInfected /= parameters.repeats;
        cout << "Average number of infected for " << pop << " nodes is " << averageInfected << "." << endl;
        cout << "Probability of a node being infected is " << averageInfected/pop << endl;
        outputFile << pop << " " << averageInfected/pop << endl;
    }
}

int main (int argc, char** argv) {
    Data parameters;

    if (argc == 1) {
        cout << "Please set an output file." << endl;
        return 0;
    }

    outputFile.open(argv[1]);

    if (argc == 9) {
        parameters.set = true;
        parameters.gamma = atof(argv[2]);
        parameters.C = atof(argv[3]);
        parameters.mu = atof(argv[4]);
        parameters.minPop = atoi(argv[5]);
        parameters.maxPop = atoi(argv[6]);
        parameters.increment = atoi(argv[7]);
        parameters.repeats = atoi(argv[8]);
    }
    
    parameters = runUI(parameters);
    outputFile << parameters.gamma << endl;
    runSimulation(parameters);

    outputFile.close();

    return 0;
}