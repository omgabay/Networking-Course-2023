// Ex2 Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
using namespace std; 
#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>

#define TIME_PORT	27015


void displayMenu();
void displayCityMenu();


int main()
{
    // Initialize Winsock (Windows Sockets)
    WSAData wsaData;
    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        cout << "Time Client: Error at WSAStartup()\n";
        return -1;
    }

    // Client side:
    // Create a socket and connect to an internet address.
    SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == connSocket)
    {
        cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    // For a client to communicate on a network, it must connect to a server.
    // Need to assemble the required data for connection in sockaddr structure.

    // Create a sockaddr_in object called server. 
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(TIME_PORT);

    // Send and receive data.
    int bytesSent = 0;
    int bytesRecv = 0;
    char sendBuff[255];
    char recvBuff[255];



    std::cout << "Welcome to Omer's Time Client!\n";
    std::string userInput; 
    do {
        displayMenu();
        std::cin >> userInput; 
        try {
            int option = stoi(userInput);
            switch (option) {
                case 1:
                    strcpy(sendBuff, "time");
                    break;
                case 2: 
                    strcpy(sendBuff, "time HH:MM:ss");
                    break; 
                case 3: 
                    strcpy(sendBuff, "time epoch");
                    break; 
                case 4:
                    strcpy(sendBuff, "banana");
                    cerr << "NOT YET IMPLEMENTED\n";
                    break;
                case 5:
                    strcpy(sendBuff, "banana");
                    cerr << "NOT YET IMPLEMENTED\n";
                    break;
                case 6: 
                    strcpy(sendBuff, "time HH:MM");
                    break;
                case 7: 
                    strcpy(sendBuff, "time YYYY");
                    break;
                case 8: 
                    strcpy(sendBuff, "time DD-MM");
                    break;
                case 9: 
                    strcpy(sendBuff, "time sec_from_month_start");
                    break;
                case 10: 
                    strcpy(sendBuff, "time WEEK_NO");
                    break;
                case 11: 
                    strcpy(sendBuff, "time DAYLIGHT");
                    break;
                case 12:
                    displayCityMenu();                    
                    std::cin >> userInput; 
                    int cityChoice = stoi(userInput);                    
                    if (cityChoice == 1) {
                        strcpy(sendBuff, "timezone PRAGUE");
                    }
                    else if (cityChoice == 2) {
                        strcpy(sendBuff, "timezone NEW YORK");
                    }
                    else if (cityChoice == 3) {
                        strcpy(sendBuff, "timezone BERLIN");
                    }
                    else if (cityChoice == 4) {
                        strcpy(sendBuff, "timezone DOHA");
                    }
                    else {
                        strcpy(sendBuff, "timezone UTC");
                    }             
                    break;
              

            }
        }
        catch (const std::invalid_argument& ia) {       
            continue; 
        }

        bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
        if (SOCKET_ERROR == bytesSent)
        {
            cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
            closesocket(connSocket);
            WSACleanup();
            return -1;
        }

        // Gets the server's answer using simple recieve (no need to hold the server's address).
        bytesRecv = recv(connSocket, recvBuff, 255, 0);
        if (SOCKET_ERROR == bytesRecv)
        {
            cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
            closesocket(connSocket);
            WSACleanup();
            return -1;
        }

        recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
        cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

        
    }while (userInput[0] != 'q' || userInput[0] != 'Q');

    
    
}



void displayMenu() {
    std::cout << "\n\nChoose one of the following options\n";
    cout << "   1) Get Local Time\n";
    cout << "   2) Get Time without date (HH:MM:ss)\n";
    cout << "   3) Get Time in seconds since epoch - Jan 1st 1970\n";
    cout << "   4) Get Client to Server Delay Estimation\n";
    cout << "   5) Measure RTT\n";
    cout << "   6) Get Time Without Date or Seconds\n";
    cout << "   7) Get Year\n";
    cout << "   8) Get Month and Day\n";
    cout << "   9) Get Seconds Elapsed since the start of the month\n";
    cout << "  10) Get Week of the Year\n";
    cout << "  11) Get Daylight saving\n";
    cout << "  12) Get Time Around the World\n";
    cout << "  13) Measure Time Lap\n";
    cout << "or press q to exit\n";
}


void displayCityMenu(){
    cout << "\nSelct the option corresponding to the city you want (1-4)\n"; 
    cout << "   1) Prague, Czech Republic\n";
    cout << "   2) New York, USA\n";
    cout << "   3) Berlin, Germany\n"; 
    cout << "   4) Doha, Qatar\n";
    cout << "   5) Other - UTC +0\n";
    cout << "or press q to exit\n";
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
