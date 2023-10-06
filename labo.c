#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils_v2.h"
#include "network.h"

#define NB_ZOMBIES 2
#define BUFFER_SIZE_MAX 256

/**
 * PRE: arg: est le port sur lequel le zombie doit être lancé
 *      Doit être casté en int
 * POST: Un processus zombie est lancé sur le port arg
 */
void startZombie(void *arg)
{
    int port = *(int *)arg;
    char portStr[50];
    sprintf(portStr, "%d", port);
    execl("./zombie", "zombie", portStr, NULL);
}

int main(int argc, char const *argv[])
{

    int zombiesPID[NB_ZOMBIES];

    // Choix de deux numéros de ports différents.
    int portZombie1 = tabPorts[randomIntBetween(MIN_INDEX_TAB_PORTS, MAX_INDEX_TAB_PORTS)];
    int portZombie2 = tabPorts[randomIntBetween(MIN_INDEX_TAB_PORTS, MAX_INDEX_TAB_PORTS)];
    while (portZombie1 == portZombie2)
    {
        portZombie2 = tabPorts[randomIntBetween(MIN_INDEX_TAB_PORTS, MAX_INDEX_TAB_PORTS)];
    }

    // Lancement des zombies
    zombiesPID[0] = fork_and_run1(startZombie, &portZombie1);
    zombiesPID[1] = fork_and_run1(startZombie, &portZombie2);

    char buffer[BUFFER_SIZE_MAX];
    printf("CTRL-D pour tuer les zombies\n");
    while (fgets(buffer, BUFFER_SIZE_MAX, stdin) != NULL)
    {
        buffer[strlen(buffer)] = '\0';
    }

    // On tue tous les zombies
    for (size_t i = 0; i < NB_ZOMBIES; i++)
    {
        skill(zombiesPID[i], SIGINT);
    }
}