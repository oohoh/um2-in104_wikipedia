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
  /* R�servation de la structure seule ;
     utile pour la r�ception avec adresse inconnue par expl.*/
  SockDist();

  /* D�signation avec les noms h�te, service et protocole */
  SockDist(const char * nomHote, const char * nomServ,
			 const char *protoServ);

  /* D�signation avec nom h�te et num. port */
  SockDist(const char *nomHote, short numPort);

  ~SockDist();

  /* Structure d�signant toute BR */
  sockaddr_in * getAdrDist();

  /* longueur de la structure de la BR (sockaddr_in) ; n�cessaire pour
     les appels d'exp�dition ou r�ception qui le demandent */ 
  int getsLen();
};
#endif







