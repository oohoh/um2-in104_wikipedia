#include <string.h>//pour memcpy au moins
#include <stdio.h>//perror
#include <sys/types.h>
#include <sys/socket.h>//constantes, sockaddr generique, socket...
#include <stdlib.h>
#include "sockdist.h"

/////////////////////////////////////////////////////////////////
//surtout pour un �ventule �largissement des m�thodes
void SockDist::initparam(){
   sRetour=sLen=0;
   adrDist=NULL;
}

/////////////////////////////////////////////////////////////////
//initialisation strucutre de la BR distante � partir des
//noms h�te, service, protocole
SockDist::SockDist( const char *nomHote, const char * nomServ,
                         const char *protoServ){
   struct hostent *hst; /* pour recuperer les param de l'h�te */
   struct servent *srv; /* pour recuperer les param du service */

   initparam();
   adrDist=new sockaddr_in;
   adrDist->sin_family = sDom;
      /* r�cup�ration param�tres (dns) du h�te distant */
   hst = gethostbyname (nomHote);
   if (hst == NULL){
     cout << "hote "<<nomHote<< " inconnu\n";
      exit (0);
   }
   adrDist->sin_addr.s_addr = ((struct in_addr
                               *)(hst->h_addr))->s_addr;
      /* ou au choix :
   memcpy((char *)&adrDist->sin_addr,(char *)hst->h_addr,
                                             hst->h_length)
                                             eh oui....*/

   srv = getservbyname (nomServ, protoServ);
   if (srv == NULL){
     cout <<nomServ<< "non trouve dans services\n";
     exit (0);
      }
   adrDist->sin_port = srv->s_port; /* numero de port trouv� */
   sLen=sizeof(*adrDist);
}

/////////////////////////////////////////////////////////////////
//comme la pr�c�dente, avec le num�ro de port 
SockDist::SockDist( const char *nomHote, short numPort){

   struct hostent *hst; /* pour recuperer les param du host */

   initparam();
   adrDist=new sockaddr_in;
   adrDist->sin_family = sDom;
   hst = gethostbyname (nomHote);
   if (hst == NULL){
     cout << "hote "<<nomHote<< " inconnu\n";
      exit (0);
   }
   adrDist->sin_addr.s_addr = ((struct in_addr
                               *)(hst->h_addr))->s_addr;
      /* ou au choix toujours:
   memcpy((char *)&adrDist->sin_addr,(char *)hst->h_addr,
                                             hst->h_length)
                                             eh oui....*/

   adrDist->sin_port = htons(numPort); /* numero de port du service */
   sLen= sizeof(*adrDist);
}

/////////////////////////////////////////////////////////////////
SockDist::SockDist(){
   initparam();
   adrDist=new sockaddr_in;
   sLen=sizeof(*adrDist);
}

/////////////////////////////////////////////////////////////////
SockDist::~SockDist(){
   delete adrDist;
}

/////////////////////////////////////////////////////////////////
sockaddr_in * SockDist::getAdrDist(){
   return adrDist;
}

/////////////////////////////////////////////////////////////////
int SockDist::getsLen(){
   return sLen;
}





