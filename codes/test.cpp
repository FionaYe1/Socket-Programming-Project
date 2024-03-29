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

// read files
// int main(void)
// {

//     ifstream infile;
//     string fileName = "map.txt";
//     infile.open(fileName.c_str());

//     if (!infile) {
//         cerr << "Unable to open file map.txt";
//         exit(1);
//     }

//     string line;
//     while (infile >> line)
//     {

//         cout << line << endl;
//     }

//     infile.close();
//     return 0;
// }

// int main() {
//     vector<int> g1;

//     for (int i = 5; i <= 9; i++) {
//         g1.push_back(i);
//     }

//     vector<int>::iterator i;

//     cout << "Output of begin and end: ";
//     for (i = g1.begin(); i != g1.end(); ++i) {
//         cout << *i << " ";
//     }
//     cout << endl;

//     return 0;
// }

int main()
{
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
            if (itt != nodes.end()) {
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
    return 0;
}
