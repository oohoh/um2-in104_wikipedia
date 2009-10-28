#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "sock.h"
#include "sockdist.h"

using namespace std;

int main(int argc, char *argv[]){
  /*definition de l'adresse de la br publique*/
  if(argv[1]==NULL){
    argv[1]="localhost";
  }

  /*definition du port de la br publique*/
  if(argv[2]==NULL){
    argv[2]=(char*)"21345";
  }

  /*demande de br client*/
  Sock brCli(SOCK_STREAM,0);
  int descBrCli;
  if (brCli.good()){
    descBrCli=brCli.getsDesc();
  }

  SockDist brPub(argv[1],atoi(argv[2]));
  struct sockaddr_in * adrBrPub=brPub.getAdrDist();
  int lgAdrBrPub=sizeof(struct sockaddr_in);
  int boolConnect = connect(descBrCli,(struct sockaddr *)adrBrPub,lgAdrBrPub);
  char lemagne[256]="undeux troisquatre cinq pfiou c'est trop long";
  char *lot;
  int ensif=recv(descBrCli,lot,sizeof(lot),0);
  fprintf(stdout, "lot=%s strlen(lot)=%d",
	  lot, strlen(lot));
  fprintf(stdout, "Charlemagne=%s strlen(charlemagne)=%d",
	  lemagne, strlen(lemagne));
  int ox = send(descBrCli,lot,strlen(lot),0);
  return 0;
}
