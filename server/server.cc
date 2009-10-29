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


/*void printArticle(struct article *art){
  cout<<endl<<endl;
  cout<<"Title : "<<art->title<<endl;
  cout<<"Author : "<<art->author<<endl;
  cout<<"Create : "<<ctime(&art->create);
  cout<<"Modify : "<<ctime(&art->modify);
  cout<<"Content :"<<art->content<<endl;
}
*/


/*void createArticle(){
  struct article art1;
  cout<<"title:";
  cin.getline(art1.title,255);
  cout<<"author:";
  cin.getline(art1.author,255);
  cout<<"content:";
  cin.getline(art1.content,255);
  art1.create=time(NULL);
  art1.modify=time(NULL);
  printArticle(&art1);
}
*/

void *actConnectClient (void * par){
  int *descBrCv=(int *)par;
  pthread_t idThread=pthread_self();
  cout<<"activite: "<<idThread<<" dans proc: "<<getpid()<<endl;


//contenant de reception	
char recu[255]="";
char recu1[255]="";
char recu2[255]="";
char recu3[255]="";

//contenant de reponse
char reponse[255]="";
char reponse1[255]="";
char reponse2[255]="";
char reponse3[255]="";
//Envoi de liste d'options:
strcat(reponse,"press 1 to - create article");

int resS = send(*descBrCv,reponse,strlen(reponse),0);

//on recupere le choix



  //on recoit l'option choisit
  int resR = recv(*descBrCv,recu,sizeof(recu),0);


  if(strcmp(recu,"1")==0)
	{
	struct article art1;
	strcpy(reponse1,"Creation d'un Article\nSaisir le titre de l'article: ");
	
	//on envoit saisir titre
  	int resS2 = send(*descBrCv,reponse1,strlen(reponse1),0);
	
	//date de creation	
	art1.create=time(NULL);
	//date de modif
        art1.modify=time(NULL);
	
	//recoit titre
	int resR2 = recv(*descBrCv,recu1,sizeof(recu1),0);
	strcpy(art1.title,recu1);
	
	//envoi saisir auteur
	
	strcpy(reponse2,"Saisir auteur: ");
	int resS3 = send(*descBrCv,reponse2,strlen(reponse2),0);
	
	//recoit auteur
	int resR3 = recv(*descBrCv,recu2,sizeof(recu2),0);
	strcpy(art1.author,recu2);


	//envoi saisir contenu
	strcpy(reponse3,"Saisir contenu: ");
	int resS4 = send(*descBrCv,reponse3,strlen(reponse3),0);
	
	//recoit contenu
	int resR4 = recv(*descBrCv,recu3,sizeof(recu3),0);
	strcpy(art1.content,recu3);

	cout<<"Article cree: "<<endl;
	cout<<"Titre: "<<art1.title<<endl;
	cout<<"Auteur: "<<art1.author<<endl;
	cout<<"Contenu: "<<art1.content<<endl;
	cout<<"Date Creation: "<<ctime(&art1.create);
	cout<<"Date de dernier modif: "<<ctime(&art1.modify);
	
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

  //createArticle();

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
}


