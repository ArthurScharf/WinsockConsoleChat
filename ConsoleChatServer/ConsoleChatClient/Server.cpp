#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h> // inetPtons
#include <tchar.h> // _T
#include <thread>
#include <set>

#pragma comment(lib, "ws2_32.lib") // ???

using namespace std;

#define MAX_CLIENTS 10

// -- UDP -- //
/*
int main()
{

	// -- Server Step 1: Load the DLL -- //
	int wsaerr = 0;
	WORD wVersionRequested = MAKEWORD(2, 2); // version 2.2
	WSADATA wsaData;
	wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0)
	{
		cout << "DLL not found!" << endl;
		return 0;
	}
	else
	{
		cout << "Winsock DLL loaded!" << endl;
		cout << "Status: " << wsaData.szSystemStatus << endl;
	}


	// -- Server Step 2: Create the socket -- //
	SOCKET serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET)
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "socket() is OK!" << endl;
	}


	// -- Server Step 3: Send and Receive Data -- //
	
}
*/






// TCP
int main()
{

	// -- Startin Winsock DLL -- //
	int wsaerr = 0;
	WORD wVersionRequested = MAKEWORD(2, 2); // Setting the highest and lowest versions to be version 2 of the Windows Socket API DLL. We're using version 2.2 here, for instance
	WSADATA wsaData;
	wsaerr = WSAStartup(
		wVersionRequested,
		&wsaData
	);
	if (wsaerr != 0)
	{
		cout << "The Winsock dll not found!" << endl;
		return 0;
	}
	else
	{
		cout << "Winsock DLL found!" << endl;
		cout << "Status: " << wsaData.szSystemStatus << endl;
	}

	// -- Create Server Socket -- //
	SOCKET serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else 
	{
		cout << "socket() is OK!" << endl;
	}

	// -- Binding Socket -- //
	sockaddr_in service;
	service.sin_family = AF_INET;
	//service.sin_addr.s_addr = INADDR_ANY;
	//InetPtonW(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr); // Configures the sockaddr_in struct???
	InetPtonW(AF_INET, _T("192.168.2.74"), &service.sin_addr.s_addr); // 172.16.0.1 is reserved for LANs
	// _T : converts string literal into a unicode wide literal if compiling with unicode support
	service.sin_port = htons(6969);
	if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		cout << "bind() failed: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "bind() OK!" << endl;
	}


	// -- Set Socket to Listen -- //
	if (listen(serverSocket, 1) == SOCKET_ERROR)
	{
		cout << "listen(): Error listening on socket " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else
	{
		cout << "listen() is OK, I'm waiting for connections..." << endl;
	}


	// -- Waiting to accept client -- //
	// SOCKET* clientSockets[10]{}; // Universal Initialization Syntax
	set<SOCKET> clientSockets = {};
	auto fpClientThreadFunction = [&](SOCKET clientSocket)
	{
			cout << "Executing new thread" << endl;
			while (true)
			{
				// -- Receiving Values -- //
				char receiveBuffer[200] = "";


				int byteCount = recv(clientSocket, receiveBuffer, 200, 0); // Blocks
				if (byteCount < 0)
				{
					printf("Client: error %ld\n", WSAGetLastError());
				}
				else
				{
					printf("Receiving Client Message: %s\n", receiveBuffer);
				}

				// -- Replicating to Other clients -- //
				for (SOCKET socket : clientSockets)
				{
					if (socket != clientSocket)
					{
						cout << "Replicating" << endl;
						byteCount = send(socket, receiveBuffer, 200, 0);
						if (byteCount < 0)
						{
							printf("Server: error %ld\n", WSAGetLastError());
						}
						else
						{
							//cout << "Message sent. Beginning Cleanup" << endl;
						}
					}
				}
			}
	};
	
	
	// -- Accepting Connections -- //
	SOCKET acceptSocket = INVALID_SOCKET;
	while (acceptSocket = accept(serverSocket, NULL, NULL)) // accept blocks
	{
		if (acceptSocket != INVALID_SOCKET)
		{
			cout << "accept success!" << endl;
			clientSockets.insert(acceptSocket);
			std::thread t(fpClientThreadFunction, acceptSocket);
			t.detach();
		}
	}

	WSACleanup(); // So other applications can use the DLL
}
//*/