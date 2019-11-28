//talker.c-- a datagram "client" demo

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
#define SERVER_B_PORT "22997"
#define AWS_UDP_PORT "23997"
#define MAXDATASIZE 500 // max number of bytes we can get at once

// public variables:
int sockfd;
struct addrinfo hints, *servinfo, *p;
int rv;
int numbytes;
struct sockaddr_storage their_addr;
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];
struct shortestPath
{
    char dest[MAXDATASIZE];
    char minLength[MAXDATASIZE];
    char propag[MAXDATASIZE];
    char trans[MAXDATASIZE];
};
struct shortestPath sp;

struct BtoAWS
{
    char dest[MAXDATASIZE];
    double tt;
    char tp[MAXDATASIZE];
    char delay[MAXDATASIZE];
};
struct BtoAWS bta;

// declare the functions:
int connectA();

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM; // UDP

    connectA();

    struct toServerB
    {
        char dest[MAXDATASIZE];
        char minLength[MAXDATASIZE];
        char propag[MAXDATASIZE];
        char trans[MAXDATASIZE];
        double fileSize;
    };
    struct toServerB tsb;
    strcpy(tsb.dest, sp.dest);
    strcpy(tsb.minLength, sp.minLength);
    strcpy(tsb.propag, sp.propag);
    strcpy(tsb.trans, sp.trans);
    tsb.fileSize = 10;

    if ((rv = getaddrinfo(localhost, SERVER_B_PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("talker: socket");
            continue;
        }
        break;
    }
    if (p == NULL)
    {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    if ((numbytes = sendto(sockfd, &tsb, sizeof(toServerB), 0, p->ai_addr, p->ai_addrlen)) == -1)
    {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    cout << "The AWS has sent path length, propagation speed and transmission speed to server B using UDP over port " << SERVER_B_PORT << "." << endl;

    addr_len = sizeof their_addr;


    
    if ((numbytes = recvfrom(sockfd, &bta, sizeof(struct BtoAWS), 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    cout << "The AWS has received delays from server B: " << endl;
    cout << "--------------------------------------------" << endl;
    cout << "Destination\tTt\tTp\tDelay" << endl;
    cout << "--------------------------------------------" << endl;


    istringstream issT(bta.dest);
    vector<string> dest1((istream_iterator<string>(issT)), istream_iterator<std::string>());
    istringstream issT2(bta.tp);
    vector<string> tp((istream_iterator<string>(issT2)), istream_iterator<std::string>());
    istringstream issT3(bta.delay);
    vector<string> delay((istream_iterator<string>(issT3)), istream_iterator<std::string>());
    
    vector<string>::iterator j, i, k;

    for (j = dest1.begin(), i = tp.begin(), k = delay.begin(); 
    j != dest1.end() && i != tp.end() && k != delay.end(); ++j, ++i, ++k)
    {
        cout << *j << "\t\t";
        printf("%.2f\t", bta.tt);
        printf("%.2f\t", stod(*i));
        printf("%.2f\n", stod(*k));
    }
    cout << "--------------------------------------------" << endl;
   // cout << "The AWS has sent calculated delay to client using TCP over port " << SERVER_B_PORT << "." << endl;
    close(sockfd);
    return 0;
}

int connectA()
{
    if ((rv = getaddrinfo(localhost, SERVER_A_PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("talker: socket");
            continue;
        }
        break;
    }
    if (p == NULL)
    {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
    struct parameter
    {
        char map;
        int vertexID;
    };
    struct parameter param;
    // struct parameter *test;
    // memset(test,0,sizeof(parameter));
    // test->map = 'A';
    // test->vertexID = 0;
    param.map = 'A';
    param.vertexID = 0;

    if ((numbytes = sendto(sockfd, &param, sizeof(parameter), 0, p->ai_addr, p->ai_addrlen)) == -1)
    {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);
    printf("talker: sent %d bytes to %s\n", numbytes, "Server A");

    addr_len = sizeof their_addr;

    if ((numbytes = recvfrom(sockfd, &sp, sizeof(shortestPath), 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    cout << "The AWS has received shortest path from server A: " << endl;
    cout << "-----------------------------" << endl;
    cout << "Destination\tMin Length" << endl;
    cout << "-----------------------------" << endl;

   // cout << "   " << sp.minLength << endl;

    istringstream iss(sp.dest);
    vector<string> dest((istream_iterator<string>(iss)), istream_iterator<std::string>());
    istringstream iss2(sp.minLength);
    vector<string> minLength((istream_iterator<string>(iss2)), istream_iterator<std::string>());
    vector<string>::iterator j, i;

    for (j = dest.begin(), i = minLength.begin(); j != dest.end() && i != minLength.end(); ++j, ++i)
    {
        cout << *j << "\t\t" << *i << endl;
    }

    cout << "-----------------------------" << endl;

    return 0;
}
