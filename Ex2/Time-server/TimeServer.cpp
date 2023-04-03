#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#pragma warning( disable : 6386)
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include<windows.h>
#include "TimeServer.h"


#define TIME_PORT	27015
#define MAX_SIZE 255 


int main() {

	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		std::cout << "Time Server: Error at WSAStartup()\n";
		return -1;
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == m_socket)
	{
		std::cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		std::cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return -1;
	}


	time_t currentTime = time(NULL);

	std::cout << "Time Server Started at "<< ctime(&currentTime) << endl; 
	

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];


	std::cout << "Waiting for clients..\n";	

	while (true) {
		memset(sendBuff, 0, sizeof(sendBuff));  // SHOULD BE REMOVED
		

		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			std::cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return -1;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		currentTime = time(NULL);
		std::cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

	
		char *command, *param;
		command = strtok(recvBuff, " ");
		std::cout << "command: " << command << endl;
		param = strtok(NULL, "");
		if (param) {
			std::cout << "param: "<< param << endl;
		}

		if (!param && strcmp(command, "time") == 0)
		{			
			GetTime(currentTime, sendBuff);									
		}
		else if (strcmp(param, "HH:MM:ss") == 0) {
			GetTimeWithoutDate(currentTime, sendBuff);
		}
		else if (strcmp(param, "HH:MM") == 0) {
			GetTime_HH_MM(currentTime, sendBuff);
		}
		else if (strcmp(param, "epoch") == 0) {
			GetTimeSinceEpoch(currentTime, sendBuff);
		}
		else if (strcmp(param, "YYYY") == 0){
			GetYear(currentTime, sendBuff);
		}
		else if (strcmp(param, "DD-MM") == 0) {
			GetMonthAndDay(currentTime, sendBuff);
		}
		else if (strcmp(param, "sec_from_month_start") == 0) {
			GetSecondsSinceMonthStart(currentTime, sendBuff);
		}
		else if (strcmp(param, "WEEK_NO") == 0) {
			GetWeekOfYear(currentTime, sendBuff);
		}
		else if (strcmp(param, "DAYLIGHT") == 0) {
			GetDaylightSaving(currentTime, sendBuff);
		}
		else if (strcmp(command, "timezone") == 0) {
			GetTimeInCity(param, currentTime, sendBuff);
		}
		else if (strcmp(command, "diagnostics") == 0) {			
			DWORD tick = GetTickCount();
			strcpy(sendBuff, std::to_string(tick).c_str());			
		}else {
			strcpy(sendBuff, "Unrecognized request :(");
		}
	

		//sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string

		// Sends the answer to the client, using the client address gathered
		// by recvfrom. 
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			std::cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return -1;
		}

		std::cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	
	}



	// Closing connections and Winsock.
	std::cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
	return 0; 
}


void GetTime(const time_t &rawTime, char* buffer) {
	strcpy(buffer, ctime(&rawTime));
	return; 
}


void GetTimeWithoutDate(const time_t &rawTime ,char *buffer){
	struct tm* timeinfo = localtime(&rawTime); 
	strftime(buffer, MAX_SIZE, "%T", timeinfo);
}

void GetTime_HH_MM(const time_t& rawTime, char* buffer){
	struct tm* timeinfo = localtime(&rawTime);
	strftime(buffer, MAX_SIZE, "%R", timeinfo);
}

void GetYear(const time_t& rawTime, char* buffer){
	struct tm* timeinfo = localtime(&rawTime);
	strftime(buffer, MAX_SIZE, "%Y", timeinfo);
}

void GetMonthAndDay(const time_t& rawTime, char* buffer) {
	struct tm* timeinfo = localtime(&rawTime);
	strftime(buffer, MAX_SIZE, "%B %d", timeinfo);
}

void GetSecondsSinceMonthStart(const time_t& rawTime, char* buffer) {
	struct tm* timeinfo = localtime(&rawTime);
	// set day to first day in the month 
	timeinfo->tm_mday = 1; 

	// set time to midnight 
	timeinfo->tm_hour = 0; 
	timeinfo->tm_min = 0; 
	timeinfo->tm_sec = 0; 

	int secondsElapsed = (int) difftime(rawTime, mktime(timeinfo));

	
	strcpy(buffer, std::to_string(secondsElapsed).c_str());
}


void GetWeekOfYear(const time_t& rawTime, char* buffer) {
	struct tm* timeinfo = localtime(&rawTime);
	strftime(buffer, MAX_SIZE, "Week number %U", timeinfo);
}


void GetDaylightSaving(const time_t& rawTime, char* buffer) {
	struct tm* timeinfo = localtime(&rawTime);	

	if (timeinfo->tm_isdst > 0) {
		strcpy(buffer, "1");
	}
	else {
		strcpy(buffer, "0");
	}
}


void GetTimeInCity(std::string cityName, const time_t &rawTime, char* buffer){
	struct tm* timeinfo = gmtime(&rawTime);
	if (iequals(cityName, "Prague") || iequals(cityName, "Berlin")) {
		timeinfo->tm_hour = (timeinfo->tm_hour + CET)%24;
		strftime(buffer, MAX_SIZE, "Central Europe Time %R", timeinfo);
	}
	else if (iequals(cityName,"Doha")){
		timeinfo->tm_hour = (timeinfo->tm_hour + AST) % 24;
		strftime(buffer, MAX_SIZE, "Arabian Standard Time - Doha %R", timeinfo);
	}	
	else if (iequals(cityName,"New York")) {
		timeinfo->tm_hour = (timeinfo->tm_hour + EST) % 24;
		strftime(buffer, MAX_SIZE, "Eastern Standard Time - New York %R", timeinfo);
	}
	else {
		strftime(buffer, MAX_SIZE, "UTC time %R", timeinfo);
	}

	
}

static void GetTimeSinceEpoch(time_t currentTime, char* sendBuff) {
	strcpy(sendBuff, std::to_string(currentTime).c_str());
}



