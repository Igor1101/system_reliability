#include "graph.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

vector <vector <int>> paths;
void depth_first(graph& s, int efrom, int eto, vector <int> path);
enum state_t{
    WORKING,
    NOTWORKING
} ;
static bool set_nxtstate(vector<state_t>&states);
static bool path_found(vector<state_t>&states, vector<graph_connection> cons);
static void print_states(vector<state_t>&states, vector<graph_connection> cons);
static float probability(vector<state_t>&states, vector<graph_connection> cons);

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

    string line;
    vector<vector<string> > parsedCsv;
    while(getline(fconns,line))
    {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector<std::string> parsedRow;
        while(std::getline(lineStream,cell,','))
        {
            parsedRow.push_back(cell);
        }

        parsedCsv.push_back(parsedRow);
    }
     // defined graph
    // edges are between connections
    vector <int> edges = {0, 1, 2, 3, 4, 5};
    vector <graph_connection> connections = {
        // from to probability
        {0, 1, 0.50, 1}, // 1
        {1, 2, 0.60, 2}, // 2
        {1, 3, 0.70, 3}, // 3
        {2, 3, 0.80, 4}, // 4
        {2, 4, 0.85, 5}, // 5
        {3, 4, 0.90, 6}, // 6
        {4, 5, 0.92, 7}, // 7
        {3, 5, 0.94, 8}, // 8
    };
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
    return 0;
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
