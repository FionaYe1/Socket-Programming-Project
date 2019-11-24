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
using namespace std;

#define DEFAULT_IP "127.0.0.1"
#define SERVER_A_PORT "21997" // the port users will be connecting to
#define MAXBUFLEN 100




// state the functions:
int initialUDP();




// public variables:
int sockfd;
struct addrinfo hints, *servinfo, *p;
int rv;
int numbytes;
struct sockaddr_storage their_addr;
char buf[MAXBUFLEN];
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];




int main(int argc, char const *argv[])
{

    

    printf("listener: waiting to recvfrom...\n");
    cout << "The Server A is up and running using UDP on port " << SERVER_A_PORT << "." << endl;

    addr_len = sizeof their_addr;

    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);
    close(sockfd);
    return 0;
}



int initialUDP() {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(DEFAULT_IP, SERVER_A_PORT, &hints, &servinfo)) != 0)
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
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
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


void storeDate() {
    map<char, map<int, vector<pair<int, int>>>> data;
    map<char, vector<string>> parameters;
    map<int, vector<pair<int, int>>> nodes;
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
    while (infile >> line)
    {
        if (!isdigit(line[0]))
        {
            mapID = line[0];
            vector<string> speeds;
            infile >> line;
            speeds.push_back(line);
            infile >> line;
            speeds.push_back(line);
            parameters[mapID] = speeds;
            nodes.clear();
        }
        else
        {
            int head = stoi(line);
            infile >> line;
            int tail = stoi(line);
            infile >> line;
            int length = stoi(line);

            auto it = nodes.find(head);
            if (it != nodes.end())
            {
                it->second.push_back(pair<int, int>(tail, length));
            }
            else
            {
                vector<pair<int, int>> leng;
                leng.push_back(pair<int, int>(tail, length));
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

    for (auto i = data.begin(); i != data.end(); ++i)
    {
        cout << "map id : " << i->first << endl;
        map<int, vector<pair<int, int>>> map1 = i->second;
        for (auto j = map1.begin(); j != map1.end(); ++j)
        {
            cout << "vertex: " << j->first << endl;
            vector<pair<int, int>> vector1 = j->second;
            for (auto k = vector1.begin(); k != vector1.end(); ++k)
            {
                cout << k->first << " length: " << k->second << endl;
            }
        }
    }
    infile.close();
}