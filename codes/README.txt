Full Name: Shuna Ye
Student ID: 2418710997


What you have done in the assignment:

client:
1. Connect with the AWS by TCP
2. Send the information of the mapID, start vertex and file size to the AWS
4. Receive the information of the destination vertices, minimum length, 
   transmission time, propagation time and totoal delay from the AWS

AWS:
1. Receive the information of the the mapID, start vertex 
   and file size from the client
2. Send the information of the mapID, start vertex and file size to the sever A
3. Receive the information of the destination vertices, minimum length, 
   transmission speed, propagation speed from the sever A
4. Send the information of the destination vertices, minimum length, 
   transmission speed, propagation speed, and file size to the sever B
5. Receive the information of the destination vertices, minimum length, 
   transmission time, propagation time and totoal delay from the server B
6. Send the information of the destination vertices, minimum length, 
   transmission time, propagation time and totoal delay to the client

serverA:
1. Load the map.txt and store the map information
2. Receive the information of the mapID, start vertex and file size from the AWS
3. Find the destination vertices from the start vertex in the map, and calculate
   the minimum length
4. Send the information of the destination vertices, minimum length, 
   transmission speed, propagation speed to the AWS

serverB:
1. Receive the information of the destination vertices, minimum length, 
   transmission speed, propagation speed, and file size from the server B
2. Calculate the transmission time, propagation time and totoal delay of 
   every vertices
3. Send the information of the destination vertices, minimum length, 
   transmission time, propagation time and totoal delay to the AWS


What your code files are and what each one of them does. (Please do not repeat the
project description, just name your code files and briefly mention what they do):

Each code file is a part of the project.
client.cpp: send the request
aws.cpp: assign the requirement to every server
serverA.cpp: find the shortest path
serverB.cpp: calculate the delay


The format of all the messages exchanged:

The way to send the information is to use the struct.
client -> aws:     
    struct parameter
    {
        char map;               // mapID
        int vertexID;           // start vertex
        long long fileSize;     // file size
    };
aws -> serverA:
    struct ParamFromClient
    {
        char map;                // mapID
        int vertexID;            // start vertex
        long long fileSize;      // file size
    };
serverA -> aws:
    struct shortestPath
    {
        char dest[MAXDATASIZE];         // destination vertices, which seperates each data by space
        char minLength[MAXDATASIZE];    // minimum length, which seperates each data by space
        char propag[MAXDATASIZE];       // propagation speed
        char trans[MAXDATASIZE];        // propagation speed
    };
aws -> serverB:
    struct toServerB
    {
        char dest[MAXDATASIZE];         // destination vertices, which seperates each data by space
        char minLength[MAXDATASIZE];    // minimum length, which seperates each data by space
        char propag[MAXDATASIZE];       // propagation speed
        char trans[MAXDATASIZE];        // propagation speed
        long long fileSize;             // file size
    };
serverB -> aws:
    struct BtoAWS
    {
        char dest[MAXDATASIZE];         // destination vertices, which seperates each data by space
        double tt;                      // transmission time
        char tp[MAXDATASIZE];           // propagation time, which seperates each data by space
        char delay[MAXDATASIZE];        // delat time, which seperates each data by space
    };
aws -> client:
    struct AWS_To_Client
    {
        char dest[MAXDATASIZE];         // destination vertices, which seperates each data by space
        char minLength[MAXDATASIZE];    // minimum length, which seperates each data by space
        double tt;                      // transmission time
        char tp[MAXDATASIZE];           // propagation time, which seperates each data by space
        char delay[MAXDATASIZE];        // delat time, which seperates each data by space
    };


Any idiosyncrasy of your project. It should say under what conditions the project fails,
if any:

Incorrect input, multiple thread(run two cilents in the meantime)


Cited codes:

For socket:
http://beej.us/guide/bgnet/examples/client.c
http://beej.us/guide/bgnet/examples/listener.c
http://beej.us/guide/bgnet/examples/talker.c
http://beej.us/guide/bgnet/examples/server.c
For Dijkstras algorithm:
https://stackoverflow.com/questions/53184552/dijkstras-algorithm-w-adjacency-list-map-c
