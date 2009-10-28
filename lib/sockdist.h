using namespace std;
#ifndef SOCKDIST_H
#define SOCKDIST_H
#include <netinet/in.h>//struct in_addr et sockaddr_in
#include <netdb.h>//struct hostent, servent, et fonctions get..by...
#include <arpa/inet.h>//diverses conversions d'adresses
#include <iostream>
class SockDist{
 private:
  static const int sDom = AF_INET;//domaine adresses
  int sRetour;//retour
  int sLen;//longeur de la struct sockaddr_in rendue
  struct sockaddr_in *adrDist;//la struct interne qui contient le triplet


  void initparam();


 public:
  /* Réservation de la structure seule ;
     utile pour la réception avec adresse inconnue par expl.*/
  SockDist();

  /* Désignation avec les noms hôte, service et protocole */
  SockDist(const char * nomHote, const char * nomServ,
			 const char *protoServ);

  /* Désignation avec nom hôte et num. port */
  SockDist(const char *nomHote, short numPort);

  ~SockDist();

  /* Structure désignant toute BR */
  sockaddr_in * getAdrDist();

  /* longueur de la structure de la BR (sockaddr_in) ; nécessaire pour
     les appels d'expédition ou réception qui le demandent */ 
  int getsLen();
};
#endif







