#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#define MIN_ITERATIONS 30
#define MAX_ITERATIONS 100
#define MAX_TIME 10.0

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
mt19937 randomGen(seed);
ofstream outputFile;

struct Data {
    int pop, minPop, maxPop, increment, type, mainParameter, infectionType;
    bool set, run, hack;
    double gamma, mu, C, confidence;
    string inputFileName, outputFileName;

    Data() {
        hack = false;
        set = false;
        gamma = 1.1;
        mu = 1.0;
        C = 10.0;
        pop = 5;
        minPop = 5;
        maxPop = 60;
        increment = 5;
        confidence = 0.1;
        type = 1;
        infectionType = 1;
        mainParameter = 1;
        outputFileName = "dataout.csv";
        inputFileName = "adj.matrix";
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
        else if (type == 4) {
            return "Custom";
        }
        return "Error";
    }

    string printMainParameter() {
        if (mainParameter == 1) {
            return "Gamma";
        }
        else if (mainParameter == 2) {
            return "Mu";
        }
        else if (mainParameter == 3) {
            return "C";
        }
        return "Error";
    }

    string printInfectionType() {
        if (infectionType == 1) {
            return "Multiplicative";
        }
        else if (infectionType == 2) {
            return "Additive";
        }
        return "Error";
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
    int weight;

    Edge() {};
    Edge(int d, int w): dest(d), weight(w) {};
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
    Graph(int n, Data parameters): node(vector<Node>(n)) {
        for (int i = 0; i < n; i++) {
            node[i] = Node(i, 0);
        }

        if (parameters.type == 1) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        node[i].edge.push_back(Edge(j, 1));
                    }
                }
            }
        }
        else if (parameters.type == 2) {
            for (int i = 1; i < n; i++) {
                node[0].edge.push_back(Edge(i, 1));
            }
            for (int i = 1; i < n; i++) {
                node[i].edge.push_back(Edge(0, 1));
            }
        }
        else if (parameters.type == 3) {
            for (int i = 0; i < n; i++) {
                int prevNode = i-1;
                if (prevNode < 0) {
                    prevNode += n;
                }

                int nextNode = i+1;
                if (nextNode == n) {
                    nextNode = 0;
                }

                node[i].edge.push_back(Edge(prevNode, 1));
                node[i].edge.push_back(Edge(nextNode, 1));
            }
        }
        else if (parameters.type == 4) {
            ifstream adjacencyMatrixFile;

            adjacencyMatrixFile.open(parameters.inputFileName);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    int weight;

                    adjacencyMatrixFile >> weight;
                    if (weight != 0) {
                        node[i].edge.push_back(Edge(j, weight));
                    }
                }
            }
            adjacencyMatrixFile.close();
        }
    }

    /*void printAdjacencyMatrix() {
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
    }*/

    Event findNextInfection(Data parameters) {
        Event infection(-1, 0);
        double lambda = parameters.C / node.size();

        for (unsigned int curNode = 0; curNode < node.size(); curNode++) {
            double infTime;
            double ratio = lambda;

            if (node[curNode].status == 0) {
                for (unsigned int neighbor = 0; neighbor < node[curNode].edge.size(); neighbor++) {
                    if (node[node[curNode].edge[neighbor].dest].status == 1) {
                        if (parameters.infectionType == 1) {
                            ratio *= parameters.gamma * node[curNode].edge[neighbor].weight;
                        }
                        else if (parameters.infectionType == 2) {
                            ratio += parameters.gamma * node[curNode].edge[neighbor].weight;
                        }
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
    newParam.run = false;

    while (option != 'R' && option != 'r') {
        cout << "--------------------------------------" << endl;
        cout << "|        Welcome to PlagueSim        |" << endl;
        cout << "--------------------------------------" << endl;
        cout << endl << "RATES:" << endl;
        cout << "[E]ndogenous Infection Rate = " << newParam.gamma << endl;
        cout << "[T]otal Exogenous Infection Rate = " << newParam.C << endl;
        cout << "[H]ealing Rate = " << newParam.mu << endl;
        cout << endl << "GRAPH SETTINGS:" << endl;
        if (newParam.type != 4) {
            cout << "[S]tarting Population = " << newParam.minPop << endl;
            cout << "[M]aximum Population = " << newParam.maxPop << endl;
            cout << "[J]ump = " << newParam.increment << endl;
        }
        else {
            cout << "[A]djacency Matrix File = " << newParam.inputFileName << endl;
            cout << "Popu[L]ation = " << newParam.pop << endl;
        }
        cout << endl << "SIMULATOR SETTINGS:" << endl;
        cout << "[G]raph Type = " << newParam.printGraphType() << endl;
        cout << "[I]nfection Type = " << newParam.printInfectionType() << endl;
        cout << "[C]onfidence Interval = " << newParam.confidence << endl;
        cout << "[O]utput File = " << newParam.outputFileName << endl;
        cout << "[P]rint Parameter = " << newParam.printMainParameter() << endl;
        cout << endl << "ACTIONS:" << endl;
        cout << "[R]un the Simulation" << endl;
        cout << "E[X]it PlagueSim." << endl << endl;

        cout << "Select an option: ";
        cin >> option;

        switch (option) {
            case '@':
                cout << "YOU ARE HACK!" << endl;
                newParam.hack = true;
                break;
            case 'E':
            case 'e':
                cout << "This is the rate that nodes infect each other." << endl;
                cout << "Please set a new value for Endogenous Infection: ";
                cin >> newParam.gamma;
                break;
            case 'T':
            case 't':
                cout << "This is the total rate that nodes get infected." << endl;
                cout << "Node infection rate will be this value divided by the population." << endl;
                cout << "Please set a new value for the Exogenous Infection: ";
                cin >> newParam.C;
                break;
            case 'H':
            case 'h':
                cout << "This is the rate that nodes heal themselves from infection." << endl;
                cout << "Please set a new value for the Healing Rate: ";
                cin >> newParam.mu;
                break;
            case 'S':
            case 's':
                cout << "Starting Population is the smallest population size that will be simulated." << endl;
                cout << "Please set a new value for the Starting Population: ";
                cin >> newParam.minPop;
                break;
            case 'M':
            case 'm':
                cout << "Maximum Population is the largest population size that will be simulated." << endl;
                cout << "Please set a new value for the Maximum Population: ";
                cin >> newParam.maxPop;
                break;
            case 'J':
            case 'j':
                cout << "Increment will increase the population size until Maximum Population." << endl;
                cout << "Smaller values of increment means a smoother graph, but take longer to simulate." << endl;
                cout << "Please set a new Increment value: ";
                cin >> newParam.increment;
                break;
            case 'A':
            case 'a':
                cout << "Please select the file containing the adjacency matrix for your graph." << endl;
                cin >> newParam.inputFileName;
                break;
            case 'L':
            case 'l':
                cout << "Please select your population size." << endl;
                cout << "Please note your adjacency matrix must fit the selected size." << endl;
                cin >> newParam.pop;
                break;
            case 'G':
            case 'g':
                cout << "[1]. Clique" << endl;
                cout << "[2]. Star" << endl;
                cout << "[3]. Circular" << endl; 
                cout << "[4]. Custom (requires adjacency matrix file)" << endl;
                cout << "Please select a preset graph type for the simulation: ";
                cin >> newParam.type;
                if (newParam.type < 1 || newParam.type > 4) {
                    cout << "Invalid Graph type. Setting to default type: Clique." << endl;
                    newParam.type = 1;
                }
                break;
            case 'I':
            case 'i':
                cout << "[1]. Multiplicative" << endl;
                cout << "[2]. Additive" << endl;
                cout << "Please select how endogenous infection will be modeled: ";
                cin >> newParam.infectionType;
                if (newParam.infectionType < 1 || newParam.infectionType > 2) {
                    cout << "Invalid Infection type. Setting to default: Multiplicative." << endl;
                    newParam.infectionType = 1;
                }
                break;
            case 'C':
            case 'c':
                cout << "Lower values have better accuracy, but take longer to simulate." << endl;
                cout << "Please set a new Confidence Interval: ";
                cin >> newParam.confidence;
                break;
            case 'O':
            case 'o':
                cout << "Please set where to save output for graph making: ";
                cin >> newParam.outputFileName;
                break;
            case 'P':
            case 'p':
                cout << "[1]. Endogenous Infection Rate" << endl;
                cout << "[2]. Healing Rate" << endl;
                cout << "[3]. Total Exogenous Infection Rate" << endl;
                cout << "Please choose which parameter you are interested in: ";
                cin >> newParam.mainParameter;
                if (newParam.mainParameter < 1 || newParam.mainParameter > 3) {
                    cout << "Invalid parameter. Setting to default: Endogenous Infection Rate" << endl;
                    newParam.mainParameter = 1;
                }
                break;
            case 'R':
            case 'r':
                cout << "The simulation will start now." << endl;
                newParam.run = true;
                break;
            case 'X':
            case 'x':
                cout << "Closing application..." << endl;
                option = 'R';
                break;
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

double calculateSampleMean(vector<int> nInfected) {
    double sampleMean = 0.0;

    for (auto value : nInfected) {
        sampleMean += value;
    }
    sampleMean /= nInfected.size();

    return sampleMean;
}

double calculateSampleVariance(vector<int> nInfected, double sampleMean) {
    double sampleVariance = 0.0;

    for (auto value : nInfected) {
        sampleVariance += pow((value - sampleMean), 2);
    }
    sampleVariance /= (nInfected.size() - 1);

    return sampleVariance;
}

void runSimulation(Data parameters) {
    Graph graph;
    vector<Event> cureList;

    for (int pop = parameters.minPop; pop <= parameters.maxPop; pop += parameters.increment) {
        int curSim = 1;
        double sampleMean, sampleVariance, confidenceInterval;
        vector<int> nInfected;

        if (parameters.type == 4) {
            pop = parameters.pop;
        }
        if (parameters.hack) {
            outputFile << "Population " << pop << endl;
        }

        do {
            graph = Graph(pop, parameters);
            double simulationTime = 0.0;
            vector<Event> cures;

            cout << "\r" << "Running simulation number " << curSim << " for population size " << pop << "..." << flush; 

            //for (int count = 0; count < MAX_ITERATIONS * pop; count++) {
            //double maxTime = (1.0 * pop)/((parameters.C / pop) + parameters.gamma + parameters.mu);
            double maxTime = MAX_TIME * pop;
            while (simulationTime < maxTime) {
                Event nextInfection = graph.findNextInfection(parameters);

                if (nextInfection.node != -1) {
                    if (cures.size() == 0) {
                        simulationTime += nextInfection.eventTime;
                        if (simulationTime > maxTime) {
                            break;
                        }
                        graph.node[nextInfection.node].status = 1;
                        exponential_distribution<double> cureTime(parameters.mu);
                        cures.push_back(Event(nextInfection.node, cureTime(randomGen)));
                        sort(cures.begin(), cures.end(), eventSort);

                        //cout << "(" << simulationTime << "t) Node " << nextInfection.node << " has been infected." << endl;
                    }
                    else {
                        if (cures[0].eventTime > nextInfection.eventTime) {
                            simulationTime += nextInfection.eventTime;
                            if (simulationTime > maxTime) {
                                break;
                            }
                            graph.node[nextInfection.node].status = 1;
                            for (unsigned int i = 0; i < cures.size(); i++) {
                                cures[i].eventTime -= nextInfection.eventTime;
                            }
                            exponential_distribution<double> cureTime(parameters.mu);
                            cures.push_back(Event(nextInfection.node, cureTime(randomGen)));
                            sort(cures.begin(), cures.end(), eventSort);

                            //cout << "(" << simulationTime << "t) Node " << nextInfection.node << " has been infected." << endl;
                        }
                        else {
                            simulationTime += cures[0].eventTime;
                            if (simulationTime > maxTime) {
                                break;
                            }

                            //cout << "(" << simulationTime << "t) Node " << cures[0].node << " has been healed." << endl;

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
                    if (simulationTime > maxTime) {
                        break;
                    }

                    //cout << "(" << simulationTime << "t) Node " << cures[0].node << " has been healed." << endl;

                    graph.node[cures[0].node].status = 0;
                    for (unsigned int i = 1; i < cures.size(); i++) {
                        cures[i].eventTime -= cures[0].eventTime;
                    }
                    cures.erase(cures.begin());
                }

                /*cout << "------------------------" << endl;
                cout << "Currently infected nodes:" << endl;
                for (unsigned int i = 0; i < cures.size(); i++) {
                    cout << cures[i].node << " will be healed in " << cures[i].eventTime << "." << endl;
                }
                cout << "------------------------" << endl;*/

                if (parameters.hack) {
                    int hackedInfected = 0;

                    for (unsigned int i = 0; i < graph.node.size(); i++) {
                        if (graph.node[i].status == 1) {
                            hackedInfected++;
                        }
                    }

                    outputFile << simulationTime << " " << hackedInfected << endl;
                }
            }

            int numberOfInfected = 0;
            for (unsigned int i = 0; i < graph.node.size(); i++) {
                if (graph.node[i].status == 1) {
                    numberOfInfected++;
                }
            }
            nInfected.push_back(numberOfInfected);

            sampleMean = calculateSampleMean(nInfected);
            sampleVariance = calculateSampleVariance(nInfected, sampleMean);
            confidenceInterval = (2 * 1.96 * sqrt(sampleVariance)) / (sqrt(nInfected.size()));

            curSim++;

            if (parameters.hack) {
                break;
            }

        } while (curSim <= MIN_ITERATIONS || confidenceInterval / sampleMean > parameters.confidence);
        
        double infectedProbability = (sampleMean/pop) * 100;

        if (!parameters.hack) {
            cout << endl;
            cout << "Average number of infected for " << pop << " nodes is " << sampleMean << "." << endl;
            cout << "Probability of a node being infected is " << infectedProbability << "%" << endl;
            cout << "Confidence Interval is [" << sampleMean - confidenceInterval << ", " << sampleMean + confidenceInterval << "]" << endl;
            //cout << "Confidence percentage is " << confidenceInterval / sampleMean << endl;
            cout << endl;

            #ifndef VALIDATION_FLAG
            outputFile << pop << " " << fixed << setprecision(5) << sampleMean/pop << endl;
            #else
            outputFile << pop << " " << sampleMean-confidenceInterval << " " << sampleMean+confidenceInterval << endl;
            #endif
        }
        else {
            outputFile << endl;
        }

        if (parameters.type == 4) {
            break;
        }
    }

    cout << endl;
    cout << "Simulation finished. All outputs saved at " << parameters.outputFileName << "." << endl;
    cout << endl;
}

void readParameters(Data& parameters) {
    ifstream iParamFile;

    iParamFile.open("parameters.cfg");
    if (iParamFile) {
        parameters.set = 1;
        iParamFile >> parameters.gamma;
        iParamFile >> parameters.C;
        iParamFile >> parameters.mu;
        iParamFile >> parameters.minPop;
        iParamFile >> parameters.maxPop;
        iParamFile >> parameters.increment;
        iParamFile >> parameters.inputFileName; 
        iParamFile >> parameters.pop;
        iParamFile >> parameters.type;
        iParamFile >> parameters.infectionType;
        iParamFile >> parameters.confidence;
        iParamFile >> parameters.outputFileName;
        iParamFile >> parameters.mainParameter;
    }
    iParamFile.close();
}

void saveParameters(Data& parameters) {
    ofstream oParamFile;

    oParamFile.open("parameters.cfg");
    oParamFile << parameters.gamma << endl;
    oParamFile << parameters.C << endl;
    oParamFile << parameters.mu << endl;
    oParamFile << parameters.minPop << endl;
    oParamFile << parameters.maxPop << endl;
    oParamFile << parameters.increment << endl;
    oParamFile << parameters.inputFileName << endl;
    oParamFile << parameters.pop << endl;
    oParamFile << parameters.type << endl;
    oParamFile << parameters.infectionType << endl;
    oParamFile << parameters.confidence << endl;
    oParamFile << parameters.outputFileName << endl;
    oParamFile << parameters.mainParameter << endl;
    oParamFile.close();
}

void writeMainParameter(Data parameters) {
    outputFile << parameters.printMainParameter() << " ";
    if (parameters.mainParameter == 1) {
        outputFile << parameters.gamma;
    }
    else if (parameters.mainParameter == 2) {
        outputFile << parameters.mu;
    }
    else if (parameters.mainParameter == 3) {
        outputFile << parameters.C;
    }
    outputFile << endl;
}

int main () {
    Data parameters;

    readParameters(parameters);
    parameters = runUI(parameters);
    saveParameters(parameters);

    while (parameters.run) {
        outputFile.open(parameters.outputFileName);
        if (!parameters.hack) {
            writeMainParameter(parameters);
        }
        runSimulation(parameters);
        outputFile.close();
        parameters = runUI(parameters);
        saveParameters(parameters);
    }

    return 0;
}