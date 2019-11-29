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
#include <numeric>
#include <utility>
using namespace std;

#define localhost "127.0.0.1"
#define SERVER_A_PORT "21997" // the port users will be connecting to
#define MAXDATASIZE 500

// declare the functions:
int initialUDP();
void storeDate();
void startDijkstra(char mapId, int start);
void printMapConstruction();
void printDijkstra();
int sendBack(char mapId);

// public variables:
map<char, map<int, vector<pair<int, int>>>> data;// store each map of the city, and each key is map ID
map<char, vector<string>> parameters;            // store the speed of each map, and each key is map ID
map<int, int> dist;                              // store the distances which calculated by Dijkstras algorithm
// cited from beej
int sockfd;
struct addrinfo hints, *servinfo, *p;
int rv;
int numbytes;
struct sockaddr_storage their_addr;
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];

int main(int argc, char const *argv[])
{
    // initial the UDP
    int status;
    if ((status = initialUDP()) != 0)
    {
        return status;
    }

    storeDate();

    cout << "The Server A is up and running using UDP on port " << SERVER_A_PORT << "." << endl;
    printMapConstruction();
    
    // cited from beej
    addr_len = sizeof their_addr;

    struct parameter
    {
        char map;               // mapID
        int vertexID;           // start vertex
        long long fileSize;     // file size
    };
    struct parameter param;

    while(true)
    {
        // receive the information from the AWS
        if ((numbytes = recvfrom(sockfd, &param, sizeof(parameter), 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }

        cout << "The Server A has received input for finding shortest paths : starting vertex " << param.vertexID << " of map " << param.map << "." << endl;
        startDijkstra(param.map, param.vertexID);
        printDijkstra();

        // send the information to the AWS
        if ((status = sendBack(param.map)) != 0)
        {
            perror("Can not send to AWS");
        }
        else
        {
            printf("The Server A has sent shortest paths to AWS.\n");
        }
    }

    return 0;
}

// initial the UDP, which cited from beej
int initialUDP()
{
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(localhost, SERVER_A_PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }
        if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    return 0;
}

// deal with the information from the map.txt, and store them
void storeDate()
{
    
    map<int, vector<pair<int, int>>> nodes;     // store every node into the same map
    ifstream infile;
    string fileName = "map.txt";
    infile.open(fileName.c_str());

    if (!infile)
    {
        cerr << "Unable to open file map.txt";
        exit(1);
    }

    string line;
    char mapID;
    // read the word by word
    while (infile >> line)
    {
        if (!isdigit(line[0]))          // if it is not a digit, then it will be the map ID
        {
            mapID = line[0];
            vector<string> speeds;
            infile >> line;             // it will be a propagation spped after the map ID
            speeds.push_back(line);
            infile >> line;             // it will be a transmission spped after the propagation spped
            speeds.push_back(line);
            parameters[mapID] = speeds;
            nodes.clear();
        }
        // read the nodes and length
        else
        {
            int head = stoi(line);      // change the string into the int
            infile >> line;
            int tail = stoi(line);
            infile >> line;
            int length = stoi(line);

            auto it = nodes.find(head);
            if (it != nodes.end())
            {
                it->second.push_back(pair<int, int>(tail, length));     // store the nodes
            }
            else
            {
                vector<pair<int, int>> leng;
                leng.push_back(pair<int, int>(tail, length));           // store the length
                nodes[head] = leng; 
            }
            auto itt = nodes.find(tail);
            if (itt != nodes.end())
            {
                itt->second.push_back(pair<int, int>(head, length));
            }
            else
            {
                vector<pair<int, int>> leng;
                leng.push_back(pair<int, int>(head, length));
                nodes[tail] = leng;
            }
            data[mapID] = nodes;
        }
    }
    infile.close();
}

// print the information from the map
void printMapConstruction()
{

    cout << "The Server A has constructed a list of " << data.size() << " maps : " << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Map ID\tNum Vertices\tNum Edges" << endl;
    cout << "-------------------------------------------" << endl;
    for (auto i = data.begin(); i != data.end(); ++i)
    {
        cout << i->first << "\t" << i->second.size() << "\t\t";
        map<int, vector<pair<int, int>>> temp = i->second;
        int size = 0;
        for (auto j = temp.begin(); j != temp.end(); ++j)
        {
            size += j->second.size();
        }
        cout << size / 2 << endl;
    }
    cout << "-------------------------------------------" << endl;
}

// find the shortest path by Dijkstras algorithm
// cited from the stackoverflow
// https://stackoverflow.com/questions/53184552/dijkstras-algorithm-w-adjacency-list-map-c
void startDijkstra(char mapId, int start)
{
    dist.clear();
    for (auto j = data[mapId].begin(); j != data[mapId].end(); ++j)
    {
        dist[j->first] = INT32_MAX / 2;
    }
    set<pair<int, int>> st;
    st.insert(make_pair(0, start));
    dist[start] = 0;

    while (!st.empty())
    {
        pair<int, int> now = *st.begin();
        st.erase(st.begin());

        int v = now.second;
        int w = now.first;

        const vector<pair<int, int>> &edges = data[mapId][v];
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
    dist.erase(start);
}

// print the shortest path after calculation by using Dijkstras algorithm
void printDijkstra()
{
    cout << "The Server A has identified the following shortest paths : " << endl;
    cout << "-----------------------------" << endl;
    cout << "Destination\tMin Length" << endl;
    cout << "-----------------------------" << endl;
    for (auto j = dist.begin(); j != dist.end(); ++j)
    {
        cout << j->first << "\t\t" << j->second << endl;
    }
    cout << "-----------------------------" << endl;
}

// send back the necessary information to the AWS
int sendBack(char mapId)
{
    struct shortestPath
    {
        char dest[MAXDATASIZE];      // destination vertices, which seperates each data by space
        char minLength[MAXDATASIZE]; // minimum length, which seperates each data by space
        char propag[MAXDATASIZE];    // propagation speed
        char trans[MAXDATASIZE];     // propagation speed
    };
    struct shortestPath sp;

    // store the data into the sp, and seperate each data by space
    string dest_s, minLength_s;
    for (auto j = dist.begin(); j != dist.end(); ++j)
    {
        dest_s += to_string(j->first);
        dest_s += " ";
        minLength_s += to_string(j->second);
        minLength_s += " ";
    }
    strcpy(sp.dest, dest_s.c_str());
    strcpy(sp.minLength, minLength_s.c_str());
    strcpy(sp.propag, parameters[mapId].at(0).c_str());
    strcpy(sp.trans, parameters[mapId].at(1).c_str());

    if ((numbytes = sendto(sockfd, &sp, sizeof(shortestPath), 0,
                           (struct sockaddr *)&their_addr, addr_len)) == -1)
    {
        perror("serverA:sendto");
        exit(1);
    }

    return 0;
}