using namespace std;
#ifndef SOCK_H
#define SOCK_H
#include <netinet/in.h>//struct in_addr et sockaddr_in
#include <netdb.h>//struct hostent, servent, et fonctions get..by...
#include <arpa/inet.h>//diverses conversions d'adresses
#include <iostream>

class Sock{
 private:
  static const int sDom = AF_INET;//domaine adresses
  static const int sProto = PF_INET;//famille protocoles
  int sDesc;//descripteur
  int sType;//type socket DGRAM, STREAM, ...
  int sRetour;//retour
  struct sockaddr_in adrSock;//la struct interne qui contient le triplet

  /*Construction de la structure a partir des donnees nom hote, nom
    service et protocole  local ou distant */
  int sockaddLocRempli( const char * nomServ, const char *protoServ);

  /* Idem mais avec le numéro de port directement */
  int sockaddLocRempli( short numPort);


  /* liaison structure (descripteur de) et port */
  int bind(int desc, const char * nomServ, const char *protoServ);
  int bind(int desc, short nport);

  void initparam();


 public:
  /* Constructeur socket sans bind ;
     essentiellement pour client en mode connecté */
  Sock(int tip, int protocole);

  /* Constructeur avec bind ;
     nom du service et proto à fournir ; doit exister
     dans les services connus, ce qui permet l'attribution du n. port.*/
  Sock(int tip, const char *nomServ, const char *protoServ, int protocole=0);

  /* Constructeur avec bind en fournissant un num. port ;
     un pis-aller lorsqu'on ne peut pas utiliser l'appel ci-dessus.*/
  Sock(int tip, short nport, int protocole);

  ~Sock();
  
  bool good();//constructeurs OK ?

  int getsDesc();//retourne le descripteur de socket tout type

  int getsRetour();//erreur allocation, surtout pour defaut bind -- par exemple

};
#endif
