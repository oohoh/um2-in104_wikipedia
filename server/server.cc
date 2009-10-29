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
  cout<<endl<<endl;
  cout<<"Titre : "<<art->title<<endl;
  cout<<"Auteur : "<<art->author<<endl;
  cout<<"Contenu :"<<art->content<<endl;
  cout<<"Date de Creation : "<<ctime(&art->create);
  cout<<"Date de dernier modif : "<<ctime(&art->modify);
 
}



void createArticle(int * descBrCv){
  //contenant de reception	

char recu1[255]="";
char recu2[255]="";
char recu3[255]="";

//contenant de reponse

char reponse1[255]="";
char reponse2[255]="";
char reponse3[255]="";



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
	printArticle(&art1);	
	
	

}


void *actConnectClient (void * par){
  int *descBrCv=(int *)par;
  pthread_t idThread=pthread_self();
  cout<<"activite: "<<idThread<<" dans proc: "<<getpid()<<endl;

char recu[255]="";
char reponse[255]="";

//Envoi de liste d'options:
strcat(reponse,"press 1 to - create article\npress 2 to - list current articles");

int resS = send(*descBrCv,reponse,strlen(reponse),0);

//on recoit l'option choisit
int resR = recv(*descBrCv,recu,sizeof(recu),0);


  if(strcmp(recu,"1")==0)
	{
	createArticle(descBrCv);
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


