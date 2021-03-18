#include "graph.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

vector <vector <int>> paths;
void depth_first(graph& s, int efrom, int eto, vector <int> path);
enum state_t{
    WORKING,
    NOTWORKING
} ;
// the probability is given
// of trouble-free operation of system elements during T hours
// T hours:
constexpr float T = 1000.0;
// multiplicity of total unloaded reservation
constexpr float Ktu = 1.0;
// multiplicity of separate reservation
constexpr float Ks = 1.0;

static bool set_nxtstate(vector<state_t>&states);
static bool path_found(vector<state_t>&states, vector<graph_connection> cons);
static void print_states(vector<state_t>&states, vector<graph_connection> cons);
static float probability(vector<state_t>&states, vector<graph_connection> cons);
int fact(int n);

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    // 1`st arg : edges file
    // 2`nd arg : connections file
    if( argc != 3) {
        cout << "invalid args"<<endl;
        return -1;
    }
    string fname_edges = string(argv[1]);
    string fname_conns = string(argv[2]);
    ifstream fedges(fname_edges), fconns(fname_conns);
    if(!fedges.is_open()) {
        cout << "cant open edges file" << endl;
        return -1;
    }
    if(!fconns.is_open()) {
        cout << "cant open conns file" << endl;
        return -1;
    }

    vector <int> edges;
    vector <graph_connection> connections;
    vector<vector<string> > parsedCsv;
    string line;
    int line_i = 0;
    while(getline(fconns,line)) {
        line_i ++;
        stringstream lineStream(line);
        string cell;
        vector<string> parsedRow;
        while(getline(lineStream,cell,';')) {
            parsedRow.push_back(cell);
        }
        // verify amount of elements in row
        if(parsedRow.size() != 4) {
            cout << "connections file data is invalid or corrupted at line " << line_i << endl;
            return -1;
        }
        // here output to connections
        // 0 from 1 to 2 probability 3 name
        graph_connection c;
        c.from = stod(parsedRow[0]);
        c.to = stod(parsedRow[1]);
        // the probability of trouble-free operation of system elements during T hours
        c.cost = stof(parsedRow[2]);
        c.name = stod(parsedRow[3]);
        connections.push_back(c);
        parsedCsv.push_back(parsedRow);
    }
    // edges are between connections
    // get edges
    line_i = 0;
    while(getline(fedges,line)) {
        line_i ++;
        stringstream lineStream(line);
        string cell;
        vector<string> parsedRow;
        while(getline(lineStream,cell,';')) {
            parsedRow.push_back(cell);
        }
        // verify amount of elements in row
        if(parsedRow.size() != 1) {
            cout << "edges file data is invalid or corrupted at line " << line_i << endl;
            return -1;
        }
        // here output to edges
        edges.push_back(stod(parsedRow[0]));
        parsedCsv.push_back(parsedRow);
    }

    graph sys(edges, connections);
    sys.print();
    // find path
    depth_first(sys, *min_element(edges.begin(), edges.end()),
                *max_element(edges.begin(), edges.end()), vector<int>());
    // find out working states
    vector <state_t> con_states;
    for(unsigned i=0; i<connections.size(); i++) {
        con_states.push_back(WORKING);
    }
    float Pres = 0;
    for(; ;) {
        if(path_found(con_states, connections)) {
            // calc probability
            float P = probability(con_states, connections);
            print_states(con_states, connections);
            cout << "P=" << P << endl;
            Pres += P;
        }
        bool res = set_nxtstate(con_states);
        if(res)
            break;
    }
    cout << "result P=" << Pres << endl;
    float Qres = 1 - Pres;
    cout << "result Q=" << Qres << endl;
    // find out intensity
    float lambda = - log(Pres) / T;
    cout << "system intensity of failures:" << lambda << " H-1" << endl;
    cout << "system operating time to failure:" << 1 / lambda << " H"<< endl;
    // findout P of failure for T hours with general unloaded redundancy
    float Qrs = 1.0 / fact((int)Ktu+1) * Qres;
    cout << "Q reserved system=" << Qrs << endl;
    float Prs = 1.0 - Qrs;
    cout << "P reserved system=" << Prs << endl;
    // find out intensity of res system
    float lambdars = - log(Prs) / T;
    cout << "reserved system intensity of failures:" << lambdars << " H-1" << endl;
    cout << "reserved system operating time to failure:" << 1 / lambdars << " H"<< endl;
    //    gain reliability
    float Gq  = Qrs / Qres;
    cout << "Gq = " << Gq << endl;
    float Gp  = Prs / Pres;
    cout << "Gp = " << Gp << endl;
    float Gt = lambda / lambdars;
    cout << "Gt = " << Gt << endl;
    // loaded redundancy, each element
    // edit probabilities
    for(unsigned c=0; c<connections.size(); c++) {
        float p = connections[c].cost;
        connections[c].cost = 1 - pow(1-p, (int)Ks + 1);
    }
    // get new probability
    // find out working states
     con_states.clear();
    for(unsigned i=0; i<connections.size(); i++) {
        con_states.push_back(WORKING);
    }
    float Pseparated = 0;
    for(; ;) {
        if(path_found(con_states, connections)) {
            // calc probability
            float P = probability(con_states, connections);
            //print_states(con_states, connections);
            //cout << "P=" << P << endl;
            Pseparated += P;
        }
        bool res = set_nxtstate(con_states);
        if(res)
            break;
    }

    cout << "result P separated=" << Pseparated<< endl;
     float Qseparated = 1.0 - Pseparated;
    cout << "Q reserved system=" << Qseparated << endl;
    // find out intensity of res system
    float lambdasep = - log(Pseparated) / T;
    cout << "separated reserved system intensity of failures:" << lambdasep << " H-1" << endl;
    cout << "separated reserved system operating time to failure:" << 1 / lambdasep << " H"<< endl;
    //    gain reliability
    float Gqsep  = Qseparated / Qres;
    cout << "Gq separated = " << Gqsep << endl;
    float Gpsep  = Pseparated / Pres;
    cout << "Gp separated = " << Gpsep << endl;
    float Gtsep = lambda / lambdasep;
    cout << "Gt separated = " << Gtsep << endl;
    return 0;
}
int fact(int n)
{
     return (n==0) || (n==1) ? 1 : n* fact(n-1);
}

static void print_states(vector<state_t>&states, vector<graph_connection> cons)
{
    cout << "working:";
    for(unsigned i=0; i<cons.size(); i++) {
        if(states[i] == WORKING) {
            cout << " " << cons[i].name;
        }
    }
    cout << " ";
}
static float probability(vector<state_t>&states, vector<graph_connection> cons)
{
    float P = 1;
    for(unsigned i=0; i<cons.size(); i++) {
        if(states[i] == WORKING) {
            P *= cons[i].cost;
        } else {
            P *= 1 - cons[i].cost;
        }
    }
    return P;
}
static bool path_found(vector<state_t>&states, vector<graph_connection> cons)
{
    bool found = false;
    // for each path verify
    for(unsigned i=0; i<paths.size(); i++) {
        // if each element of path found in WORKING states
        for(int c: paths[i]){
            // find number in cons
            int val = 0;
            for(unsigned ci =0; ci<cons.size(); ci++) {
                if(cons[ci].name == c) {
                    val = ci;
                    break;
                }
            }
            if(states[val] == WORKING) {
                found = true;
            } else {
                found = false;
                break;
            }
        }
        if(found == true) {
            return true;
        }
    }
    return false;
}
// return true if result is zero(all states not working)
static bool set_nxtstate(vector<state_t>&states)
{
    unsigned sts = 0;
    for(unsigned i=0; i<states.size(); i++ ) {
        if(states.at(i) == WORKING) {
            sts |= 1<<i;
        }
    }
    sts --;
    bool ret = false;
    if(sts == 0) {
        ret = true;
    }
    for(unsigned i=0; i<states.size(); i++) {
        if((sts&(1<<i)) == (1<<i)) {
            states.at(i) = WORKING;
        } else {
            states.at(i) = NOTWORKING;
        }
    }
    return ret;
}
void depth_first(graph& s, int efrom, int eto, vector <int> path)
{
    if(efrom == eto) {
        // finished job
        cout << "path:";
        for (vector<int>::const_iterator i = path.begin(); i != path.end(); ++i)
             std::cout << *i << ' ';
        cout << endl;
        paths.push_back(path);
    }
    vector <graph_connection> cons = s.edge_get_all_connections(efrom);
    for(graph_connection c: cons) {
        // check for loop , if it`s ignore
        if(!path.empty() && find(path.begin(), path.end(), c.name) != path.end()) {
            continue;
        }
        vector <int> npath = path;
        npath.push_back(c.name);
        if(c.to != efrom) {
            depth_first(s, c.to, eto, npath);
        } else if(c.from != efrom) {
            depth_first(s, c.from, eto, npath);
        }
    }
}
