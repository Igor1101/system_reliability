compillation:
install qt:
pacman -Sy qt gcc make cmake

cd [project directory]
cmake ./
make

run:
./system_reliability [filename1].csv [filename2].csv

filename1:
edges(names of edges, integer format) of graph written in one column,
please note: the biggest edge is selected by program as end of graph
the least one is selected as the beginning of graph
filename2:
connections, columns are:
1: edge from(integer)
2: edge to (integer)
3: probability of fault-free work during some time
4: name of connection (integer)
names of connections should not be repeated

example:
./system_reliability example/edges.csv example/connections.csv
