#include <string.h>//pour memcpy au moins
#include <stdio.h>//perror
#include <sys/types.h>
#include <sys/socket.h>//constantes, sockaddr generique, socket...
#include <stdlib.h>
#include "sock.h"

/////////////////////////////////////////////////////////////////
//rempli le triplet à partir du nom de service et protocole
int Sock::sockaddLocRempli( const char * nomServ, const char *protoServ){

   struct servent *srv; /* pour recuperer les param du service */
 
   adrSock.sin_family = sDom;
      /* on recupere sa propre adresse internet */
   adrSock.sin_addr.s_addr = INADDR_ANY;
      /* recherche des parametres du service qu'on veut assurer*/
   srv = getservbyname (nomServ, protoServ);
   if (srv == NULL){
     cout <<nomServ<< "non trouve dans services\n";
     exit (0);
      }
   adrSock.sin_port = srv->s_port; /* numero de port trouvé */
   return sizeof(adrSock);
}

/////////////////////////////////////////////////////////////////
//rempli le triplet à partir du numéro de port
int Sock::sockaddLocRempli( short numPort){

   adrSock.sin_family = sDom;
      /* on recupere sa propre adresse internet */
   adrSock.sin_addr.s_addr = INADDR_ANY;
   adrSock.sin_port = htons(numPort); /* numero de port direct */
   return sizeof(adrSock);

}

/////////////////////////////////////////////////////////////////
//liaison structure interne (programme) et externe (système)
int Sock::bind(int desc, const char * nomServ, const char *protoServ){
   int lg = sockaddLocRempli(nomServ, protoServ);
   if ((sRetour=::bind(desc,(struct sockaddr *)&adrSock,lg))<0)
     perror ("pb bind");return sRetour;
}

/////////////////////////////////////////////////////////////////
//comme la précédente avec le numéro de port
int Sock::bind(int desc, short nport){
  int lg = sockaddLocRempli(nport);
if ((sRetour=::bind(desc,(struct sockaddr *)&adrSock,lg))<0)
     perror ("pb bind");return sRetour;
}

/////////////////////////////////////////////////////////////////
//initialisation, surtout pour un éventuel élargissement des méthodes
void Sock::initparam(){
  sRetour=-1;
}

/////////////////////////////////////////////////////////////////
Sock::Sock(int tip, int protocole){
//tip contient SOCK_DGRAM ou SOCK_STREAM ou ...
  initparam();
  if ((sDesc=socket(sProto, tip, protocole))<0) perror("pb socket");
  else sRetour=1;//car sRetour relatif a bind et ici sans bind
}

/////////////////////////////////////////////////////////////////
Sock::Sock(int tip, short nport, int protocole){
  initparam();
  if ((sDesc=socket(sProto, tip, protocole))<0) perror("pb socket");
  else bind(sDesc, nport);
}

/////////////////////////////////////////////////////////////////
Sock::Sock(int tip, const char *nomServ, const char *protoServ, int
                                                     protocole){
  initparam();
  if ((sDesc=socket(sProto, tip, protocole))<0) perror("pb socket");
  else bind(sDesc, nomServ, protoServ);
};

/////////////////////////////////////////////////////////////////
Sock::~Sock(){}

/////////////////////////////////////////////////////////////////
bool Sock::good(){
  return (sRetour>=0 && sDesc>0);
}

/////////////////////////////////////////////////////////////////
int Sock::getsDesc(){
  return sDesc;
}

/////////////////////////////////////////////////////////////////
int Sock::getsRetour(){
  return sRetour;
}

