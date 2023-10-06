
#define SERVER_PORT1 9501
#define SERVER_PORT2 9502
#define SERVER_PORT3 9503
#define SERVER_PORT4 9504
#define SERVER_PORT5 9505
#define SERVER_PORT6 9506
#define SERVER_PORT7 9507
#define SERVER_PORT8 9508
#define SERVER_PORT9 9509
#define SERVER_PORT10 9510

#define MIN_INDEX_TAB_PORTS 0
#define MAX_INDEX_TAB_PORTS 9
#define NBR_PORTS 10

extern const int tabPorts[NBR_PORTS];

/**
 * PRE: ip : une adresse ip valide.
 *      sockFdPortsConnectedTab : tableau où ajouter les sockFD
 * POST: Teste les ports disponibles et s'y connecte.
 * Met à jour le tableau en parametre.
 *       si echec: affiche une erreur
 * RES: return le nombre de sockFD connectés
 */
int testAndConnectPorts(const char *ip, int *sockFdPortsConnectedTab);

/**
 * PRE:  port: un numéro de port valide
 * POST: bind un socket à 0.0.0.0:port et ecoute dessus
 *       si echec: affiche une erreur
 * RES:  return le socketFD
 */
int initSocketServer(int port);

/**
 * PRE:  int nbrToDisconnect: nombre de sockets à fermer
 *       int* tabToDisconnect: tableau contenant les sockets à fermer
 * POST: si succes, deconnecte tous les zombies
 *       si echec: affiche une erreur
 */
void disconnectZombies(int nbrToDisconnect, int *tabToDisconnect);