#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;
//Server-Anwendung

int main(int argc, char *argv[])
{
    //Portnummer für den Server
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    //Portnummer aus Eingabe abfangen
    int port = atoi(argv[1]);
    //Nachrichtengröße definieren
    char msg[1500];
     
    //Setup des Sockets
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
 
    //open stream orientierter Socket mit Internetadresse
    //dabei die Socket-Beschreibung im Blick behalten
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //den Socket an seine lokale Adresse binden
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, 
        sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    cout << "Waiting for a client to connect..." << endl;
    //bis zu 5 Anfragen auf einmal listen
    listen(serverSd, 5);
    //eine Anfrage des Clients erhalten, indem man accept benutzt
    //wir benötigen eine neue Adresse, um eine Verbindung zum Client aufzubauen
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, eine neue Beschreibung des Sockets erstellen, damit man die neue Verbindung zum Client nutzen kann
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newSd < 0)
    {
        cerr << "Error accepting request from client!" << endl;
        exit(1);
    }
    cout << "Connected with client!" << endl;
    //Verbindungsdauer wird aufgezeichnet
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    //Datenverkehr wird aufgezeichnet
    int bytesRead, bytesWritten = 0;
    while(1)
    {
        //Message vom Clienten empfangen (listen)
        cout << "Awaiting client response..." << endl;
        memset(&msg, 0, sizeof(msg));//Buffer wird geleert
        bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
        if(!strcmp(msg, "exit"))
        {
            cout << "Client has quit the session" << endl;
            break;
        }
        cout << "Client: " << msg << endl;
        cout << ">";
        string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg)); //Buffer wird geleert
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            //teilt dem Client mit, dass der Server die Verbindung geschlossen hat
            send(newSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        //Sendet die Message an den Client
        bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
    }
    //Socket wird geschlossen
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec) 
        << " secs" << endl;
    cout << "Connection closed..." << endl;
    return 0;   
}
