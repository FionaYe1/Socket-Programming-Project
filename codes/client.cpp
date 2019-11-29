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

#define DEFAULT_IP "127.0.0.1"
#define AWS_TCP_PORT "24997" // the port client will be connecting to
#define MAXDATASIZE 500 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd;
    
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;



    if ((rv = getaddrinfo("127.0.0.1", AWS_TCP_PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure
    cout << "The client is up and running." << endl;

    struct parameter
    {
        char map;
        int vertexID;
        long long fileSize;
    };
    struct parameter param;

    
    param.map = argv[1][0];
    string str1 = argv[2];
    string str2 = argv[3];
    param.vertexID = atoi(str1.c_str());
    param.fileSize = atoll(str2.c_str());

    cout << " file size  " << param.fileSize << endl;


    if ((send(sockfd, &param, sizeof(struct parameter), 0)) == -1)
    {
        perror("recv");
        exit(1);
    }

    cout << "The client has sent query to AWS using TCP: start vertex " 
    << param.vertexID << "; map " << param.map 
    << "; file size " << argv[3] << "." << endl;

    struct AWS_To_Client
    {
        char dest[MAXDATASIZE];
        char minLength[MAXDATASIZE];
        double tt;
        char tp[MAXDATASIZE];
        char delay[MAXDATASIZE];
    };
    struct AWS_To_Client aws_to_client;

    if ((recv(sockfd, &aws_to_client, sizeof(AWS_To_Client), 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    cout << "The client has received results from AWS:" << endl;
    cout << "----------------------------------------------" << endl;
    cout << "Destination\tMin Length\tTt\tTp\tDelay\n" << endl;
    cout << "----------------------------------------------" << endl;


    istringstream iss(aws_to_client.dest);
    vector<string> dest((istream_iterator<string>(iss)), istream_iterator<std::string>());
    istringstream iss2(aws_to_client.minLength);
    vector<string> minLength((istream_iterator<string>(iss2)), istream_iterator<std::string>());
    istringstream iss3(aws_to_client.tp);
    vector<string> tp((istream_iterator<string>(iss3)), istream_iterator<std::string>());
    istringstream iss4(aws_to_client.delay);
    vector<string> delay((istream_iterator<string>(iss4)), istream_iterator<std::string>());

    vector<string>::iterator j, i, k, l;
    for (i = dest.begin(), j = minLength.begin(), k = tp.begin(), l = delay.begin(); 
         i != dest.end() && j != minLength.end() && k != tp.end() && l != delay.end(); 
         ++i, ++j, ++k, ++l)
    {

        cout << *i << "\t\t" << *j << "\t\t";
        printf("%.2f\t", aws_to_client.tt);
        printf("%.2f\t", stod(*k));
        printf("%.2f\n", stod(*l));
    }

    cout << "----------------------------------------------" << endl;
    close(sockfd); // close socket

    return 0;
}
