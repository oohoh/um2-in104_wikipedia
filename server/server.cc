#include <iostream>
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

void printArticle(struct article *art){
  struct line *reader;
  int idLine=1;

  cout<<"Title : "<<art->title<<endl;
  cout<<"Author : "<<art->author<<endl;
  cout<<"Create : "<<ctime(&art->create);
  cout<<"Modify : "<<ctime(&art->modify);

  cout<<"Content :"<<endl;
  reader=art->content;
  while(reader->next!=NULL){
    cout<<"`#00"<<idLine<<":"<<reader->text<<endl;
    reader=reader->next;
    idLine++;
  }  
}
void createArticle(){
  struct article art1;
  struct line *writer;
  int idLine=1;

  cout<<"title:";
  cin.getline(art1.title,255);
  cout<<"author:";
  cin.getline(art1.author,255);

  /*writer=art1.content;
  cout<<"line"<<idLine<<":";
  cin.getline(writer->text,255);
  idLine++;
  cout<<"before-seg"<<endl;
  writer=writer->next;
  cout<<"after-seg"<<endl;
  cout<<"line"<<idLine<<":";
  cin.getline(writer->text,255);
  idLine++;
  writer=writer->next;
  cout<<"line"<<idLine<<":";
  cin.getline(writer->text,255);
  idLine++;
  writer=writer->next;
  cout<<"line"<<idLine<<":";
  cin.getline(writer->text,255);
  idLine++;
  writer->next=NULL;*/

  while(writer->next!=NULL){
    cout<<"line"<<idLine<<":";
    cin.getline(writer->text,255);
    if(strcmp(writer->text,"#EOF")==0){
      cout<<"before"<<endl;
      writer=writer->next;
      cout<<"after"<<endl;
    }else{
      writer->next=NULL;
    }
    idLine++;
  }

  art1.create=time(NULL);
  art1.modify=time(NULL);
  printArticle(&art1);
}

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
  /*-shared memory*/
  int idSms;
  key_t keySms;
  size_t sizeSms;
  /*-threads*/
  int tid;
  pthread_t idConnexion;
  /*-buffers*/

  /*-descripteurs*/
  int descBrPub;
  int descBrCv;

  int vRecv, vSend;

  /*generation de la cle key_t*/
  keySms=ftok("../README",420);

  /*calcul de la taille de la memoire partagee*/
  sizeSms=size_t(99*sizeof(article)+9702*sizeof(line));

  /*creation de la memoire partagee*/
  idSms=shmget(keySms,sizeSms,IPC_CREAT|0666);
  shmat(idSms,NULL,0666);
  cout<<"idSms="<<idSms<<endl;

  createArticle();
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


