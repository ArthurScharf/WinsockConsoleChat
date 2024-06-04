#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h> // inetPtons
#include <tchar.h> // _T
#include <thread>

#pragma comment(lib, "ws2_32.lib") // ???

using namespace std;


int main()
{
	// -- Opening and Starting the Winsock DLL -- //
	WORD wVersionRequested = MAKEWORD(2, 2); // Version 2.2
	int wsaerr = 0;
	WSADATA wsaData;
	wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0)
	{
		cout << "Failed to load the DLL!" << endl;
		return 0;
	}
	else
	{
		cout << "Winsock DLL found!" << endl;
		cout << "Status: " << wsaData.szSystemStatus << endl;
	}


	// -- Create a Socket -- //
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "socket() is OK!" << endl;
	}



	// -- Client Step 3: Connect -- //
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	//InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
	InetPton(AF_INET, _T("192.168.2.74"), &clientService.sin_addr.s_addr);
	clientService.sin_port = htons(6969);
	if ( connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR )
	{
		cout << "Client: connect() - Failed to connect." << endl;
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "Client: connect() is OK." << endl;
		cout << "Client: Can start sending and receiving data..." << endl;
	}

	



	// -- Step 4 : Sending and Receiving Data -- //
	auto fpReceiveMessage = [&](SOCKET* pClientSocket)
		{
			cout << "Starting receiving thread" << endl;
			while (true)
			{
				char buffer[200];
				int byteCount = recv(*pClientSocket, buffer, 200, 0);
				if (byteCount == SOCKET_ERROR)
				{
					printf("Client receive error %ld \n", WSAGetLastError());
					WSACleanup();
					return -1;
				}
				else
				{
					printf("%s\n", buffer);
				}
			}
		};


	// -- Starting a thread to receive replicated messages -- //
	thread receiveMessageThread(fpReceiveMessage, &clientSocket);
	receiveMessageThread.detach();


	// -- Sending Messages -- //
	char buffer[200];
	while (true)
	{
		// -- Sending text data --//
		printf("Enter your message: ");
		cin.getline(buffer, 200); // Blocks application
		int byteCount = send(clientSocket, buffer, 200, 0);
		if (byteCount == SOCKET_ERROR)
		{
			printf("Server send error %ld.\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}
		else
		{
			// printf("%s\n", buffer);
		}
	}
	

	WSACleanup();
}
