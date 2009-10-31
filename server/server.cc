#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "sock.h"
#include "sockdist.h"
#include "articlib.h"

using namespace std;


void initTab(char t[],int size){
  for(int i=0;i<size;i++){
    t[i]='\0';
  }
}

void printArticle(int *descBrCv,struct article *art){
  char artBuff[1023];
  int vSend;

  //initialisation du buffer d'envoie
  initTab(artBuff,sizeof(artBuff));

  //ajout du titre
  strcat(artBuff,"Titre : ");
  strcat(artBuff,art->title);
  strcat(artBuff,"\n");

  //ajout de l'auteur
  strcat(artBuff,"Auteur : ");
  strcat(artBuff,art->author);
  strcat(artBuff,"\n");

  //ajout de la date de creation
  strcat(artBuff,"Creation : ");
  strcat(artBuff,ctime(&art->create));

  //ajout de la date de derniere modification
  strcat(artBuff,"Modification : ");
  strcat(artBuff,ctime(&art->modify));

  //ajout du contenu
  strcat(artBuff,"Contenu : ");
  strcat(artBuff,art->content);
  strcat(artBuff,"\n");

  //envoie de l'article
  vSend=send(*descBrCv,artBuff,strlen(artBuff),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void modifyArticle(int *descBrCv,struct article *art){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //envoie de l'entete de la  version actuelle de l'article
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nModification de l'article");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception message de synchronisation
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  //envoie de la version actuelle de l'article
  printArticle(descBrCv,art);

  //reception message de synchronisation
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  //envoie demande modification du titre
  initTab(buffer,sBuffer);
  strcpy(buffer,"Nouveau titre : ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception des modification du titre
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(art->title,buffer);

  //envoie demande modification du contenu
  initTab(buffer,sBuffer);
  strcpy(buffer,"Nouveau contenu : ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception des modification du contenu
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(art->content,buffer);

  //date de modif
  art->modify=time(NULL);

  //envoie de l'entete d'affichage de l'article modifie
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nArticle modifie:\n");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de synchronisation
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  printArticle(descBrCv,art);
}

void createArticle(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //definition de l'article
  struct article art;

  //buffer
  char buffer[255];

  //on envoit saisir titre
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"\nCreation d'un Article\nSaisir le titre de l'article: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //recoit titre
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(art.title,buffer);

  //envoi saisir auteur
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"Saisir auteur: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //recoit auteur
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(art.author,buffer);

  //envoi saisir contenu
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"Saisir contenu: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
	
  //recoit contenu
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(art.content,buffer);

  //date de creation	
  art.create=time(NULL);

  //date de modif
  art.modify=time(NULL);

  //envoi du message de fin de procedure
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"#done");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  modifyArticle(descBrCv,&art);
}


void *actConnectClient (void *par){
  //initialisation du thread
  int descBrCv=*(int*)par;
  pthread_t idThread=pthread_self();
  cout<<"activite: "<<idThread<<" dans proc: "<<getpid()<<endl;

  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //Envoi de liste d'options:
  initTab(buffer,sBuffer);
  strcat(buffer,"press 1 to - create article\npress 2 to - list current articles");
  vSend=send(descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //on recoit l'option choisit
  initTab(buffer,sBuffer);
  vRecv=recv(descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  if(strcmp(buffer,"article create")==0){
    createArticle(&descBrCv);
  }

  pthread_exit(NULL);
}


int main(int argc, char *argv[]){
  /*declaration des variables*/
  /*-shared memory*/
  int shmid;
  key_t shmkey;
  size_t shmsize;
  /*-threads*/
  int tid;
  pthread_t idConnexion;
  /*-buffers*/

  /*-descripteurs*/
  int descBrPub;
  int descBrCv;

  int vRecv, vSend;

  /*generation de la cle key_t*/
  shmkey=ftok("../README",420);

  /*calcul de la taille de la memoire partagee*/
  shmsize=size_t(99*sizeof(article)+9702*sizeof(line));

  /*creation de la memoire partagee*/
  shmid=shmget(shmkey,shmsize,IPC_CREAT|0666);
  cout<<"shmid="<<shmid<<endl;
  shmat(shmid,NULL,0666);

  

  /*definition du port de la br publique*/
  if(argv[1]==NULL){
    argv[1]=(char*)"21345";
  }

  /*demande de br publique*/
  Sock brPub(SOCK_STREAM,atoi(argv[1]),0);

  if (brPub.good()){
    descBrPub=brPub.getsDesc();
  }else{
    perror("--recuperation descBrPub");
    exit(4);
  }

  /*longueur de la file d'attente*/
  if(listen(descBrPub,5)==-1){
    perror("--listen");
    exit(5);
  }
  struct sockaddr brCv;
  socklen_t lgBrCv=sizeof(struct sockaddr);

  while(true){
    /*acceptation de la connexion*/
    descBrCv=accept(descBrPub,(struct sockaddr *)&brCv,&lgBrCv);
    if(descBrCv==-1){
      perror("--acceptation descBrCv");			
      sleep(5);
    }else{
	
      if(pthread_create(&idConnexion,NULL,actConnectClient,&descBrCv)!=0){
	perror("--creation thread");
      }
    }
  }
}


