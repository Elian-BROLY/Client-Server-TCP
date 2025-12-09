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
	int port = 5000;

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
	{
		printf("Erreur WSAStartup\n");
		return 1;
	}

	SOCKET socketEcoute;
	SOCKET socketCommunication;
	int iResult;

	// Creation de la socket TCP
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
	if (socketEcoute == INVALID_SOCKET)
	{
		printf("Erreur creation socket : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	/* cout << "Quel est le port du serveur : ";
	cin >> port; */

	// Preparation de l'adresse locale
	struct sockaddr_in pointLocal;
	pointLocal.sin_family = AF_INET;
	pointLocal.sin_addr.s_addr = htonl(INADDR_ANY); // toutes les interfaces
	pointLocal.sin_port = htons(port);

	// Etape 5 : bind
	iResult = bind(socketEcoute, (SOCKADDR*)&pointLocal, sizeof(pointLocal));
	if (iResult == SOCKET_ERROR)
	{
		printf("Erreur bind socket : %d\n", WSAGetLastError());
		closesocket(socketEcoute);
		WSACleanup();
		return 1;
	}

	printf("Socket attachee sur le port %d\n", port);

	// Etape 6 : listen
	if (listen(socketEcoute, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Erreur listen socket : %d\n", WSAGetLastError());
		closesocket(socketEcoute);
		WSACleanup();
		return 1;
	}

	printf("Socket en ecoute passive...\n");

	// --- Etape 7 : accept ---
	struct sockaddr_in infoClient;
	int taille = sizeof(infoClient);

	socketCommunication = accept(socketEcoute, (SOCKADDR*)&infoClient, &taille);
	if (socketCommunication == INVALID_SOCKET)
	{
		printf("Erreur accept : %d\n", WSAGetLastError());
		closesocket(socketEcoute);
		WSACleanup();
		return 1;
	}

	printf("Connexion acceptee avec un client.\n");

	// Reception du message du client
	char messageRecu[LGMESSAGE];
	int lus = recv(socketCommunication, messageRecu, sizeof(messageRecu), 0);

	if (lus > 0)
	{
		messageRecu[lus] = '\0';
		printf("Message recu : %s (%d octets)\n", messageRecu, lus);
	}
	else if (lus == 0)
	{
		printf("Le client a ferme la connexion.\n");
	}
	else
	{
		printf("Erreur lecture socket : %d\n", WSAGetLastError());
	}

	// Envoi de la reponse
	char reponse[] = "ok\n";
	int ecrits = send(socketCommunication, reponse, (int)strlen(reponse), 0);

	if (ecrits == SOCKET_ERROR)
	{
		printf("Erreur envoi socket : %d\n", WSAGetLastError());
		closesocket(socketCommunication);
		closesocket(socketEcoute);
		WSACleanup();
		return 1;
	}

	printf("Reponse envoyee au client.\n");

	// Fermetures
	closesocket(socketCommunication);
	closesocket(socketEcoute);
	WSACleanup();

	return 0;
}
