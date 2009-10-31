#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../lib/sock.h"
#include "../lib/sockdist.h"

using namespace std;


void initTab(char t[],int size){
  for(int i=0;i<size;i++){
    t[i]='\0';
  }
}


void printArticle(int *descBrCli){
  int vRecv;
  char artBuff[1023];
  int sArtBuff=sizeof(artBuff);

  //initialisation du buff de reception
  initTab(artBuff,sArtBuff);

  //reception de l'article
  vRecv=recv(*descBrCli,artBuff,sArtBuff,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  //affichage de l'article
  cout<<artBuff<<endl;
}


void modifyArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception de l'entete de la version actuelle de l'article
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer<<endl;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de la version actuelle de l'article
  printArticle(descBrCli);

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception demande modification du titre
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie modification du titre
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception demande modification du contenu
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie modification du contenu
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'entete d'affichage de l'article modifie
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie modification du contenu
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  printArticle(descBrCli);
}


void createArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //envoie demande creation article
  initTab(buffer,sBuffer);
  strcpy(buffer,"article create");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);

  //saisie et envoie du titre
  while(strcmp(buffer,"#done")){
    initTab(buffer,sBuffer);
    vRecv=recv(*descBrCli,buffer,sBuffer,0);
    if(vRecv==-1){
      perror("--receive");
      exit(1);
    }
    if(strcmp(buffer,"#done")!=0){
      cout<<buffer;
      initTab(buffer,sBuffer);
      cin.getline(buffer,sBuffer);
      vSend=send(*descBrCli,buffer,strlen(buffer),0);
      if(vSend==-1){
	perror("--send");
	exit(1);
      }
    }
  }

  //affichage de l'article
  //printArticle(descBrCli);
  modifyArticle(descBrCli);
}


int main(int argc, char *argv[]){
  /*definition de l'adresse de la br publique*/
  if(argv[1]==NULL){
    argv[1]=(char*)"localhost";
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

  int vConnect=connect(descBrCli,(struct sockaddr *)adrBrPub,lgAdrBrPub);
  
  cout<<"res du connect: "<<vConnect<<endl;

  //Contenant d'envoi
  char buffer[255];
  char artBuff[1023];


  //on recoit les options
  int resR = recv(descBrCli,buffer,sizeof(buffer),0); 
  //affiche reception
  cout<<buffer<<endl;
  //prepare a input

  char input[255];

  cin.getline(input,255);

  //on envoit notre choix
  if(strcmp(input,"1")==0){
    createArticle(&descBrCli);
  }
}
