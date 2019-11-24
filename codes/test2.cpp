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

int main() {

    string s = "124";
    int i = stoi(s);
    cout << i << endl;
}