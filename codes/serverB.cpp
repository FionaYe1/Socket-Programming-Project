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
#define SERVER_B_PORT "22997" // the port users will be connecting to
#define MAXDATASIZE 500

// public variables:
// cited from beej
int sockfd;
struct addrinfo hints, *servinfo, *p;
int rv;
int numbytes;
struct sockaddr_storage their_addr;
char buf[MAXDATASIZE];
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];

struct BtoAWS
{
    char dest[MAXDATASIZE];
    double tt;
    char tp[MAXDATASIZE];
    char delay[MAXDATASIZE];
};
struct BtoAWS bta;

// declare the funtions:
int sendBack();
int initialUDP();

// cited from beej
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
    int status;
    if ((status = initialUDP()) != 0)
    {
        return status;
    }

    cout << "The Server B is up and running using UDP on port " << SERVER_B_PORT << "." << endl;

    addr_len = sizeof their_addr;

    struct toServerB
    {
        char dest[MAXDATASIZE];      // destination vertices, which seperates each data by space
        char minLength[MAXDATASIZE]; // minimum length, which seperates each data by space
        char propag[MAXDATASIZE];    // propagation speed
        char trans[MAXDATASIZE];     // propagation speed
        long long fileSize;          // file size
    };
    struct toServerB tsb;

    while (true)
    {
        // receive the information from the AWS
        if ((numbytes = recvfrom(sockfd, &tsb, sizeof(toServerB), 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }

        cout << "The Server B has received data for calculation:" << endl;
        cout << "* Propagation speed : " << tsb.propag << " km / s;" << endl;
        cout << "* Transmission speed : " << tsb.trans << " Bytes / s;" << endl;
        
        // split the data and store them into the vector
        istringstream iss(tsb.dest);
        vector<string> dest((istream_iterator<string>(iss)), istream_iterator<std::string>());
        istringstream iss2(tsb.minLength);
        vector<string> minLength((istream_iterator<string>(iss2)), istream_iterator<std::string>());
        vector<string>::iterator k, l;
        for (k = dest.begin(), l = minLength.begin(); k != dest.end() && l != minLength.end(); ++k, ++l)
        {
            cout << "* Path length for destination " << *k << " : " << *l << ";" << endl;
        }

        cout << "The Server B has finished the calculation of the delays: " << endl;
        cout << "------------------------" << endl;
        cout << "Destination\tDelay" << endl;
        cout << "------------------------" << endl;

        strcpy(bta.dest, tsb.dest);
        double tt = tsb.fileSize / (8 * stod(tsb.trans));
        bta.tt = tt;
        string tp_s, delay_s;
        vector<string>::iterator j, i;

        // calculate the delay and print out the necessary information
        for (i = minLength.begin(), j = dest.begin(); i != minLength.end() && j != dest.end(); ++i, ++j)
        {
            double temp = stod(*i) / stod(tsb.propag);
            cout << *j << "\t\t";
            printf("%.2f\n", temp + tt);
            tp_s += to_string(temp);
            tp_s += " ";
            delay_s += to_string(temp + tt);
            delay_s += " ";
        }
        cout << "------------------------" << endl;

        strcpy(bta.delay, delay_s.c_str());
        strcpy(bta.tp, tp_s.c_str());

        // send back the information to the AWS
        if ((status = sendBack()) != 0)
        {
            perror("Can not send to AWS");
        }
        else
        {
            printf("The Server B has finished sending the output to AWS.\n");
        }
    }

    close(sockfd);
    return 0;
}

// cited from the beej
int initialUDP()
{
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(localhost, SERVER_B_PORT, &hints, &servinfo)) != 0)
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

int sendBack()
{
    if ((numbytes = sendto(sockfd, &bta, sizeof(struct BtoAWS), 0,
                           (struct sockaddr *)&their_addr, addr_len)) == -1)
    {
        perror("serverB:sendto");
        exit(1);
    }
    return 0;
}