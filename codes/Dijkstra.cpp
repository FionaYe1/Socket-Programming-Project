
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <map>
#include <set>

using namespace std;

map<int, vector<pair<int, int>>> mp;

void addEdge(int u, int v, int dist)
{
    mp[u].push_back(make_pair(v, dist));
}

void startDijkstra(int u)
{
    vector<int> dist(1e2 + 1, 1e9);

    set<pair<int, int>> st;
    st.insert(make_pair(0, u));
    dist[u] = 0;

    while (!st.empty())
    {
        pair<int, int> now = *st.begin();
        st.erase(st.begin());

        int v = now.second;
        int w = now.first;

        const vector<pair<int, int>> &edges = mp[v];
        for (const pair<int, int> &to : edges)
        {
            if (w + to.second < dist[to.first])
            {
                st.erase(make_pair(dist[to.first], to.first));
                dist[to.first] = w + to.second;
                st.insert(make_pair(dist[to.first], to.first));
            }
        }
    }

    for (int i = 1; i <= 8; i++)
    {
        cout << i << ' ' << dist[i] << endl;
    }
}

int main()
{
    addEdge(1, 2, 1);
    addEdge(1, 8, 2);

    addEdge(2, 1, 1);
    addEdge(2, 3, 1);

    addEdge(3, 2, 1);
    addEdge(3, 4, 1);

    addEdge(4, 3, 1);
    addEdge(4, 5, 1);

    addEdge(5, 4, 1);
    addEdge(5, 6, 1);

    addEdge(6, 5, 1);
    addEdge(6, 7, 1);

    addEdge(7, 6, 1);
    addEdge(7, 8, 1);

    addEdge(8, 9, 1);
    addEdge(8, 1, 2);

    startDijkstra(1);

    return 0;
}

// #include <iostream>
// #include <set>
// #include <list>
// #include <algorithm>

// using namespace std;

// typedef struct nodes 
// {
//     int dest;
//     int cost;
// }node;

// class Graph 
// {
//     int n;
//     list<node> *adjList;
//     private:
//         void showList(int src, list<node> lt) 
//         {
//             list<node> :: iterator i;
//             node tempNode;

//             for (i = lt.begin(); i!= lt.end(); ++i) 
//             {
//                 tempNode = *i;
//                 cout << "(" << src << ")---(" << tempNode.dest 
//                 << "|" << tempNode.cost << ")";
//             }
//             cout << endl;
//         }
//     public :
//         Graph() 
//         {
//             n = 0;
//         }
        
//         Graph(int nodeCount)
//         {
//             n = nodeCount;
//             adjList = new list<node>[n];
//         }

//         void addEdge(int source, int dest, int cost)
//         {
//             list<node> :: iterator it;
//             it = adjList.find(source);
//             if (it != adjList.end()) 
//             {

//             }
//             node newNode;
//             newNode.dest = dest;
//             newNode.cost = cost;
//             adjList[source].push_back(newNode);
//         }

//         void displayEdges()
//         {
//             for (int i = 0; i < n; i++)
//             {
//                 list<node> tempList = adjList[i];
//                 showList(i, tempList);
//             }
//         }

//         friend void dijkstra(Graph g, int *dist, int *prev, int start);
// };

// void dijkstra(Graph g, int *dist, int *prev, int start)
// {
//     int n = g.n;

//     for (int i = 0; i < n; i++)
//     {
//         dist[i] = 9999;     // set as infinity
//         prev[i] = -1;       // undefined previous
//     }

//     dist[start] = 0;        // distance of start is 0
//     set<int> S;             // Create empty set S
//     list<int> Q;

//     for (int i = 0; i < n; i++)
//     {
//         Q.push_back(i);     // add each node into queue
//     }

//     while (!Q.empty())
//     {
//         list<int> :: iterator it;
//         it = min_element(Q.begin(), Q.end());
//         int min = *it;         // the minimum element from queue
//         Q.remove(min);          
//         S.insert(min);          // add minimum element to the set
//         list<node> :: iterator it2;
        
//         for (it2 = g.adjList[min].begin(); it2 != g.adjList[min].end(); ++it2)
//         {
//             if ((dist[min]+(it2->cost)) < dist[it2->dest])      // relax (u, v)
//             {
//                 dist[it2->dest] = (dist[min] + (it2->cost));
//                 prev[it2->dest] = min;
//             }
//         }
//     }
// }

// int main() 
// {
//     int n = 6;
//    Graph g(n);
//    int dist[n], prev[n];
//    int start = 0;

// //    g.addEdge(0, 1, 3);
// //    g.addEdge(0, 2, 6);
// //    g.addEdge(1, 0, 3);
// //    g.addEdge(1, 2, 2);
// //    g.addEdge(1, 3, 1);
// //    g.addEdge(2, 1, 6);
// //    g.addEdge(2, 1, 2);
// //    g.addEdge(2, 3, 1);
// //    g.addEdge(2, 4, 4);

// //    g.addEdge(2, 5, 2);
// //    g.addEdge(3, 1, 1);
// //    g.addEdge(3, 2, 1);
// //    g.addEdge(3, 4, 2);
// //    g.addEdge(3, 6, 4);
// //    g.addEdge(4, 2, 4);
// //    g.addEdge(4, 3, 2);
// //    g.addEdge(4, 5, 2);
// //    g.addEdge(4, 6, 1);
// //    g.addEdge(5, 2, 2);
// //    g.addEdge(5, 4, 2);
// //    g.addEdge(5, 6, 1);
// //    g.addEdge(6, 3, 4);
// //    g.addEdge(6, 4, 1);
// //    g.addEdge(6, 5, 1);

//     g.addEdge(0, 1, 7);
//     g.addEdge(1, 2, 2);
//     g.addEdge(2, 3, 15);
//     g.addEdge(3, 0, 9);
//     g.addEdge(1, 3, 10);
//     g.addEdge(3, 4, 5);
//     g.addEdge(3, 5, 7);
//     g.addEdge(4, 5, 4);


//     dijkstra(g, dist, prev, start);

//     for (int i = 0; i < n; i++)
//         if (i != start)
//             cout << start << " to " << i << ", Cost: " << dist[i] << " Previous: " << prev[i] << endl;

// }