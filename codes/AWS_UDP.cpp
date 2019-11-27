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

#define DEFAULT_IP "127.0.0.1"
#define SERVER_A_PORT "21997"         // the port users will be connecting to
#define SERVER_B_PORT "22997"
#define AWS_UDP_PORT "23997"
#define MAXDATASIZE 500               // max number of bytes we can get at once


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
    string dest;
    string minLength;
    string propag;
    string trans;
};
struct shortestPath sp;


// declare the functions:
int connectA();

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
//


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;   // 1

    if ((rv = getaddrinfo(DEFAULT_IP, SERVER_A_PORT, &hints, &servinfo)) != 0)
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
        // if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        // {
        //     close(sockfd);
        //     perror("listener: bind");
        //     continue;
        // }
        break;
    }
    if (p == NULL)
    {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
    struct parameter
    {
        string map;
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

    if ((numbytes = recvfrom(sockfd, &sp, MAXDATASIZE, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    cout << "The AWS has received shortest path from server A: " << endl;
    cout << "-----------------------------" << endl;
    cout << "Destination\tMin Length" << endl;
    cout << "-----------------------------" << endl;

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

    close(sockfd);
    return 0;
}



// int connectA()
// {
//     if ((rv = getaddrinfo(DEFAULT_IP, SERVER_A_PORT, &hints, &servinfo)) != 0)
//     {
//         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
//         return 1;
//     }

//     // loop through all the results and make a socket
//     for (p = servinfo; p != NULL; p = p->ai_next)
//     {
//         if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
//         {
//             perror("talker: socket");
//             continue;
//         }
//         // if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
//         // {
//         //     close(sockfd);
//         //     perror("listener: bind");
//         //     continue;
//         // }
//         break;
//     }
//     if (p == NULL)
//     {
//         fprintf(stderr, "talker: failed to create socket\n");
//         return 2;
//     }
//     struct parameter
//     {
//         string map;
//         int vertexID;
//     };
//     struct parameter param;
//     // struct parameter *test;
//     // memset(test,0,sizeof(parameter));
//     // test->map = 'A';
//     // test->vertexID = 0;
//     param.map = 'A';
//     param.vertexID = 0;

//     if ((numbytes = sendto(sockfd, &param, sizeof(parameter), 0, p->ai_addr, p->ai_addrlen)) == -1)
//     {
//         perror("talker: sendto");
//         exit(1);
//     }

//     freeaddrinfo(servinfo);
//     printf("talker: sent %d bytes to %s\n", numbytes, "Server A");

//     addr_len = sizeof their_addr;

//     if ((numbytes = recvfrom(sockfd, &sp, MAXDATASIZE, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
//     {
//         perror("recvfrom");
//         exit(1);
//     }

//     cout << "The AWS has received shortest path from server A: " << endl;
//     cout << "-----------------------------" << endl;
//     cout << "Destination\tMin Length" << endl;
//     cout << "-----------------------------" << endl;

//     istringstream iss(sp.dest);
//     vector<string> dest((istream_iterator<string>(iss)), istream_iterator<std::string>());
//     istringstream iss2(sp.minLength);
//     vector<string> minLength((istream_iterator<string>(iss2)), istream_iterator<std::string>());
//     vector<string>::iterator j, i;

//     for (j = dest.begin(), i = minLength.begin(); j != dest.end() && i != minLength.end(); ++j, ++i)
//     {
//         cout << *j << "\t\t" << *i << endl;
//     }

//     cout << "-----------------------------" << endl;

//     return 0;
// }
