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

int main()
{

    // vector<pair<int, int>> v;
    // v.push_back(pair<int, int>(1, 3));
    // v.push_back(pair<int, int>(2, 3));
    // v.push_back(pair<int, int>(4, 6));
    // v.push_back(pair<int, int>(1, 5));
    // v.push_back(pair<int, int>(1, 2));
    // v.push_back(pair<int, int>(1, 7));
    // for (auto k = v.begin(); k != v.end(); ++k)
    // {
    //     cout << k->first << " " << k->second << endl;
    // }


    // vector<int> sp;
    // // sp.push_back(1);
    // // sp.push_back(3);
    // // sp.push_back(14);
    // // sp.push_back(16);
    // // sp.push_back(9);

    // for (auto j = sp.begin(); j != sp.end(); ++j)
    // {
    //     cout << *j << "            " << endl;
    // }

    string text = "Let me split this into words";

    istringstream iss(text);
    vector<string> results((istream_iterator<string>(iss)), istream_iterator<std::string>());

    for (auto j = results.begin(); j != results.end(); ++j)
    {
        cout << *j << "            " << endl;
    }
    return 0;
}