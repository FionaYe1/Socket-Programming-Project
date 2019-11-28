#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
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

// 接收的人用自己的port来初始化getaddrinfo，发送的人用对方的port
#define AWS_TCP_PORT "24997"  // the port users will be connecting to
#define AWS_UDP_PORT "23997"
#define SERVER_A_PORT "21997" // the port users will be connecting to
#define SERVER_B_PORT "22997"

#define BACKLOG 10   // how many pending connections queue will hold
#define MAXDATASIZE 500 // max number of bytes we can get at once
#define DEFAULT_IP "127.0.0.1"


// public variables:
int sockfd_TCP, new_fd_TCP, sockfd_UDP, numbytes; // listen on sock_fd, new connection on new_fd
struct addrinfo hints_TCP, hints_UDP, *servinfo_TCP, *servinfo_UDP, *p;
struct sockaddr_storage their_addr; // connector's address information
struct addrinfo *info_UDP;
socklen_t sin_size;
struct sigaction sa;
int yes = 1;
char s[INET6_ADDRSTRLEN];
int rv;
char buf[MAXDATASIZE];
socklen_t addr_len;


// declare funtions:
int connectA();
int connectB();
int initialTCP();
int initialUDP();



void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
}
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{




    // while(1) {  // main accept() loop
    //     sin_size = sizeof their_addr;
    //     new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    //     if (new_fd == -1) {
    //         perror("accept");
    //         continue; 
    //     }
    //     inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    //     printf("server: got connection from %s\n", s);

    //     if (!fork()) { // this is the child process
    //         close(sockfd); // child doesn't need the listener
    //         struct parameter
    //         {
    //             string map;
    //             int vertexID;
    //             int fileSize;
    //         };
    //         struct parameter param;
    //         if ((numbytes = recv(new_fd, &param, sizeof(struct parameter), 0)) == -1) {
    //             perror("recv");
    //             exit(1);
    //         }

    //         cout << "The AWS has received map ID " << param.map << ", start vertex " << param.vertexID
    //              << " and file size " << param.fileSize << " from the client using TCP over port "
    //              << param.fileSize << endl;

    //         exit(1);
    //     }
    //     close(new_fd_TCP);  // parent doesn't need this


        
    // }
    return 0; 
}

int initialTCP(){
    memset(&hints_TCP, 0, sizeof hints_TCP);
    hints_TCP.ai_family = AF_UNSPEC;
    hints_TCP.ai_socktype = SOCK_STREAM; /// TCP
    hints_TCP.ai_flags = AI_PASSIVE;     // use my IP

    if ((rv = getaddrinfo(NULL, AWS_TCP_PORT, &hints_TCP, &servinfo_TCP)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo_TCP; p != NULL; p = p->ai_next)
    {
        if ((sockfd_TCP = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd_TCP, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        if (::bind(sockfd_TCP, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd_TCP);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo_TCP); // all done with this structure
    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd_TCP, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    cout << "The AWS is up and running." << endl;
}


int initialUDP()
{
    memset(&hints_UDP, 0, sizeof hints_UDP);
    hints_UDP.ai_family = AF_UNSPEC;
    hints_UDP.ai_socktype = SOCK_DGRAM; // 1

    if ((rv = getaddrinfo(DEFAULT_IP, SERVER_A_PORT, &hints_UDP, &servinfo_UDP)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo_UDP; p != NULL; p = p->ai_next)
    {
        if ((sockfd_UDP = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
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

    return 0;
}



int connectA()
{
    if ((rv = getaddrinfo(DEFAULT_IP, SERVER_A_PORT, &hints_UDP, &servinfo_UDP)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo_UDP; p != NULL; p = p->ai_next)
    {
        if ((sockfd_UDP = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
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

    if ((numbytes = sendto(sockfd_UDP, &param, sizeof(parameter), 0, p->ai_addr, p->ai_addrlen)) == -1)
    {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo_UDP);
    printf("talker: sent %d bytes to %s\n", numbytes, "Server A");

    addr_len = sizeof their_addr;

    struct shortestPath
    {
        char dest[MAXDATASIZE];
        char minLength[MAXDATASIZE];
        char propag[MAXDATASIZE];
        char trans[MAXDATASIZE];
    };
    struct shortestPath sp;
    if ((numbytes = recvfrom(sockfd_UDP, &sp, sizeof(shortestPath), 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
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

    return 0;
}


int connectB()
{



    return 0;
}