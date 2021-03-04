#ifndef GRAPH_H
#define GRAPH_H


#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>

using namespace std;

struct graph_connection {
    int from;
    int to;
    float cost;
    int name;
};

struct graph_edge {
    int time;
    // additional
    int prio;
    int time_started;
    int cpu;
};

class graph {
private:
    vector <graph_edge> edges;
    vector <graph_connection> connections;
public:
    graph(vector <int> edges, vector <graph_connection> conns)
    {
        for(int i: edges) {
            this->edges.push_back({i, 0, 0, 0});
        }
        this->connections = conns;
    }
    void edge_set_cpu(int edge, int time, int cpu)
    {
        edges.at(edge).time_started = time;
        edges.at(edge).cpu = cpu;
    }
    graph_edge edge_get(int edge)
    {
        return edges.at(edge);
    }
    int edge_time(int edge)
    {
        return edges.at(edge).time;
    }
    void edge_set_prio(int edge, int prio)
    {
        edges.at(edge).prio = prio;
    }
    int edge_prio(int edge)
    {
        return edges.at(edge).prio;
    }

    vector <graph_connection> connections_get()
    {
        vector <graph_connection> cons = connections;
        return cons;
    }
    vector <graph_edge> edges_get()
    {
        vector <graph_edge> edg = edges;
        return edg;
    }
    vector <graph_connection> edge_get_all_connections(int edge)
    {
        vector <graph_connection> cons;
        for(graph_connection i: connections) {
            if(i.from == edge || i.to == edge) {
                cons.push_back(i);
            }
        }
        return cons;
    }
    vector <graph_connection> edge_get_outgoing_connections(int edge)
    {
        vector <graph_connection> cons;
        for(graph_connection i: connections) {
            if(i.from == edge) {
                cons.push_back(i);
            }
        }
        return cons;
    }
    vector <graph_connection> edge_get_incoming_connections(int edge)
    {
        vector <graph_connection> cons;
        for(graph_connection i: connections) {
            if(i.to == edge) {
                cons.push_back(i);
            }
        }
        return cons;
    }

    vector <int> edge_get_parents(int edge)
    {
        vector <int> parents;
        vector <graph_connection> incc = edge_get_incoming_connections(edge);
        for(graph_connection i: incc) {
            parents.push_back(i.from);
        }
        return parents;
    }
    vector <int> edge_get_children(int edge)
    {
        vector <int> children;
        vector <graph_connection> incc = edge_get_outgoing_connections(edge);
        for(graph_connection i: incc) {
            children.push_back(i.to);
        }
        return children;
    }
    graph_connection get_connection(int from, int to)
    {
        for(graph_connection c: connections) {
            if(c.from == from && c.to == to) {
                return c;
            }
        }
        graph_connection cerr = {
             -1,
            -1,
            -1
        };
        return cerr;
    }
    void print()
    {
        print_from(0, 0);
    }
    void print_from(int edge, int nest)
    {
        cout << string(nest, '\t') <<"edge:" << edge << ":time:" << edge_time(edge) << " prio:"<< edge_prio(edge) << endl;
        for(int i: edge_get_children(edge)) {
            cout <<string(nest, '\t') << "connection:" <<
                    "from:" << get_connection(edge, i).from  <<
                    "to:" << get_connection(edge, i).to <<
                    "cost:" << get_connection(edge, i).cost
                 << endl;
            print_from(i, nest+1);
        }
    }

};


#endif // GRAPH_H
