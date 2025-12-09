#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define LGMESSAGE 256

int main()
{
	string ip;
	int port;

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0) {
		printf("Erreur WSAStartup\n");
		return 1;
	}

	SOCKET descripteurSocket;
	int iResult;

	// Creation de la socket TCP
	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (descripteurSocket == INVALID_SOCKET)
	{
		printf("Erreur creation socket : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	cout << "Quelle est l'adresse IP du serveur : ";
	cin >> ip;
	cout << "Quel est le port du serveur : ";
	cin >> port;

	// Adresse du serveur
	struct sockaddr_in pointDeRencontreDistant;
	pointDeRencontreDistant.sin_family = AF_INET;
	pointDeRencontreDistant.sin_addr.s_addr = inet_addr(ip.c_str()); 
	pointDeRencontreDistant.sin_port = htons(port);

	// Connexion
	iResult = connect(descripteurSocket, (SOCKADDR*)&pointDeRencontreDistant,
		sizeof(pointDeRencontreDistant));
	if (iResult == SOCKET_ERROR)
	{
		printf("Erreur connexion socket : %d\n", WSAGetLastError());
		closesocket(descripteurSocket);
		WSACleanup();
		return 1;
	}

	printf("Connexion au serveur reussie !\n");

	// Envoi message
	char messageEnvoi[LGMESSAGE];
	char messageRecu[LGMESSAGE];
	int ecrits, lus;

	sprintf(messageEnvoi, "Hello world !");

	ecrits = send(descripteurSocket, messageEnvoi, (int)strlen(messageEnvoi), 0);
	if (ecrits == SOCKET_ERROR)
	{
		printf("Erreur envoi socket : %d\n", WSAGetLastError());
		closesocket(descripteurSocket);
		WSACleanup();
		return 1;
	}

	printf("Message envoye (%d octets) : %s\n", ecrits, messageEnvoi);

	// Reception
	lus = recv(descripteurSocket, messageRecu, sizeof(messageRecu), 0);
	if (lus > 0)
	{
		messageRecu[lus] = '\0';
		printf("Reponse du serveur : %s (%d octets)\n", messageRecu, lus);
	}
	else if (lus == 0)
	{
		printf("Le serveur a ferme la connexion.\n");
	}
	else
	{
		printf("Erreur lecture socket : %d\n", WSAGetLastError());
	}

	// Fermeture
	closesocket(descripteurSocket);
	WSACleanup();

	return 0;
}
