///////////////////////////////////////////////
//                                           //
//              Malka Raphael                //
//              Offer Samy                   //
//              Vlaminck Maxime              //
//                                           //
///////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "utils_v2.h"
#include "network.h"

#define MAX_SOUS_PROCESS 30
#define MAX_COMMAND_LINE 256
#define MAX_RESPONSE_LINE 2000
#define TIME_OUT 10

int nbrSocketsConnected;
int sockFdPortsConnectedTab[MAX_SOUS_PROCESS] = {0};

/**
 * POST: Lis des commandes sur l'entrée standard
 *       Envoie les commandes aux sous processus zombies présents dans sockFdPortsConnectedTab
 *       Tourne en boucle tant qu'il ne reçoit pas de CTRL+D.
 */
void sendCommandToZombies()
{
    char command[MAX_COMMAND_LINE];
    while (fgets(command, MAX_COMMAND_LINE, stdin) != NULL)
    {
        for (size_t i = 0; i < nbrSocketsConnected; i++)
        {
            // Ecriture de command sur les sockets des sous processus
            swrite(sockFdPortsConnectedTab[i], command, strlen(command));
        }
    }
}

/**
 * POST:Ecoute sur les sockFD présents dans sockFdPortsConnectedTab.
 *      Utilise un poll pour éviter les appels bloquants.
 *      Dès qu'une réponse est reçue sur l'un des sockFD, il l'affiche sur la sortie standard.
 *      Tourne en boucle tant qu'il reste des processus à écouter.
 */
void listenToResponse()
{
    struct pollfd fds[MAX_SOUS_PROCESS];
    char buffer[MAX_RESPONSE_LINE];
    int nbZombisConnected = nbrSocketsConnected;

    for (int i = 0; i < nbrSocketsConnected; i++)
    {
        // initialisation du poll
        fds[i].fd = sockFdPortsConnectedTab[i];
        fds[i].events = POLLIN;
    }

    while (nbZombisConnected > 0)
    {
        spoll(fds, nbrSocketsConnected, TIME_OUT);
        for (int i = 0; i < nbrSocketsConnected; i++)
        {
            // Detection si il y a une réponse prete à être lue.
            if (fds[i].revents & POLLIN)
            {
                // Lecture
                int nbChar = sread(fds[i].fd, buffer, MAX_RESPONSE_LINE);

                // Si la réponse est de 0 caractère c'est que l'évènement qui a été détecté est la mort du zombie.
                if (nbChar == 0)
                {
                    nbZombisConnected--;
                    if (nbZombisConnected == 0)
                    {
                        // Si il ne reste plus de zombies on tue le processus d'écriture et on tue celui-ci.
                        printf("\nPlus de zombies à écouter\n");
                        skill(getppid(), SIGINT);
                        exit(0);
                    }
                }
                // Ecriture de la réponse sur la sortie standard.
                swrite(1, buffer, nbChar);
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    // Verification si il y a au moins une ip/nom en argument
    if (argc < 2)
    {
        printf("Il faut une cible en argument (ip ou nom)\n");
        exit(1);
    }

    //  Test des ports disponibles sur la ou les cibles.
    for (size_t i = 1; i < argc; i++)
    {
        const char *ip = argv[i];
        nbrSocketsConnected = testAndConnectPorts(ip, sockFdPortsConnectedTab);
    }

    printf("Entrez des commandes à envoyer aux zombies et les réponses s'afficheront ici\n");

    pid_t pid = sfork();

    if (pid == 0)
    {
        // Processus d'ecoute -> fils
        listenToResponse();
    }
    else
    {
        // Processus pour envoyer les commandes -> pere
        sendCommandToZombies();
        // CTRL+D Reçu
        disconnectZombies(nbrSocketsConnected, sockFdPortsConnectedTab);
        skill(pid, SIGINT);
        exit(0);
    }
}
