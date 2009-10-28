#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "sock.h"
#include "sockdist.h"
#include "articlib.h"

using namespace std;

void *actConnectClient (void * par){
  int *descBrCv=(int *)par;
  pthread_t idThread=pthread_self();
  cout<<"activite"<<idThread<<"dans proc."<<getpid()<<endl;

  //contenant de reception	
  char recu[255]="";

  //on recoit
  int resR = recv(*descBrCv,recu,sizeof(recu),0);
 
  cout<<"message recu: "<<recu<<endl<<endl;

  //contenant d'envoi
  char reponse[255]="j'ai recu le message: ";
  strcat(reponse,recu);

  //on envoit/repond
  int resS = send(*descBrCv,reponse,strlen(reponse),0);



  pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  /*declaration des variables*/
  /*-threads*/
  int tid;
  pthread_t idConnexion;
  /*-buffers*/

  /*-descripteurs*/
  int descBrPub;
  int descBrCv;

  int vRecv, vSend;

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
  struct sockaddr_in brCv;
  socklen_t lgBrCv=sizeof(struct sockaddr_in);

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

  /* descBrCv=accept(descBrPub,(struct sockaddr *)&brCv,&lgBrCv);

//contenant de reception	
  char recu[256];


//on recoit
  int resR = recv(descBrCv,recu,sizeof(recu),0);
 
cout<<"message recu: "<<recu<<endl<<endl;



//contenant d'envoi
  char reponse[256]="j'ai recu le message: ";

strcat(reponse,recu);

//on envoit/repond
  int resS = send(descBrCv,reponse,strlen(reponse),0);
  
  */
}


