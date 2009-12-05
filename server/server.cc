#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <utility>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../lib/sock.h"
#include "../lib/sockdist.h"
#include "../lib/wikilib.h"

using namespace std;

//identifiant de la memoire partage
int shmid;

//fonctions utilitaires
void initTab(char t[],int size);
void initShmwiki(shmwiki wiki);

//fonction gestion du menu
void menuHome(int* descBrCv);

//fonctions gestion des comptes
int authentification(int *descBrCv);
account createAccount(int *descBrCv);
void modifyAccount(int *descBrCv, account *acc);
void deleteAccount(int *descBrCv, account *acc);

////fonctions gestion des groupes
//void createGroup(int *descBrCv);
//void modifyGroup(int *descBrCv, group *grp);
//void deleteGroup(int *descBrCv, group *grp);

//fonctions gestion des articles
article createArticle(int *descBrCv);
void modifyArticle(int *descBrCv, article *art);
void deleteArticle(int *descBrCv, article *art);
void printArticle(int *descBrCv, article *art);

void initShmwiki(){
  int i;
  shmwiki *p_wiki;
  p_wiki=(shmwiki *)shmat(shmid,NULL,0666);

  //initialisation des comptes
  for(i=0;i<25;i++){
    initTab(p_wiki->acc_list[i].login, sizeof(p_wiki->acc_list[i].login));
    initTab(p_wiki->acc_list[i].passwd, sizeof(p_wiki->acc_list[i].passwd));
  }
  strcpy(p_wiki->acc_list[0].login,"admin");
  strcpy(p_wiki->acc_list[0].passwd,"admin");

  //test
  /*for(i=0;i<25;i++){
    if(strcmp(p_wiki->acc_list[i].login,"")>0){
      cout<<i<<"-login="<<p_wiki->acc_list[i].login<<"\tpasswd="<<p_wiki->acc_list[i].passwd<<endl;
    }
  }//*/

  //initialisation des autres groupes
  for(i=0;i<15;i++){
    int j;
    initTab(p_wiki->grp_list[i].name, sizeof(p_wiki->grp_list[i].name));
    for(j=0;j<25;j++){
      p_wiki->grp_list[i].user[j]=-1;
    }
    for(j=0;j<50;j++){
      p_wiki->grp_list[i].article[j]=-1;
    }
  }
  strcpy(p_wiki->grp_list[0].name,"admin");
  p_wiki->grp_list[0].user[0]=0;
  strcpy(p_wiki->grp_list[1].name,"public");
  strcpy(p_wiki->grp_list[2].name,"newsletter");

  //test
  /*for(i=0;i<15;i++){
    int j;
    if(strcmp(p_wiki->grp_list[i].name,"")>0){
      cout<<"$"<<i<<" --group:"<<p_wiki->grp_list[i].name<<endl;
      for(j=0;j<25;j++){
	cout<<"user#"<<j<<":"<<p_wiki->grp_list[i].user[j]<<endl;
      }
      for(j=0;j<50;j++){
	cout<<"article#"<<j<<":"<<p_wiki->grp_list[i].article[j]<<endl;
      }
    }
  }//*/
  

  //initialisation des articles
  for(i=0;i<50;i++){
    initTab(p_wiki->art_list[i].title, sizeof(p_wiki->art_list[i].title));
    initTab(p_wiki->art_list[i].author, sizeof(p_wiki->art_list[i].author));
    p_wiki->art_list[i].create=0;
    p_wiki->art_list[i].modify=0;
    initTab(p_wiki->art_list[i].content, sizeof(p_wiki->art_list[i].content));
  }

  //test
  /*for(i=0;i<50;i++){
    if(strcmp(p_wiki->art_list[i].title,"")>0){
      cout<<i<<"-title="<<p_wiki->art_list[i].title<<endl;
    }
  }//*/
}

void initTab(char t[],int size){
  for(int i=0;i<size;i++){
    t[i]='\0';
  }
}

void menuAccount(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //envoi de la liste des options:
  initTab(buffer,sBuffer);
  strcat(buffer,"Menu - Compte\n1- S'authentifier\n2- Creer un compte\n3- Supprimer un compte\n4- Retour a l'accueil\n#Choix> ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'option choisie
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  switch(buffer[0]){
  case '1':
    createAccount(descBrCv);
    goto debut_menu;
  case '2':
    createAccount(descBrCv);
    goto debut_menu;
  case '4':
    break;
  default:
    goto debut_menu;
  }
}

void menuGroup(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //envoi de la liste des options:
  initTab(buffer,sBuffer);
  strcat(buffer,"Menu - Groupe\n1- Creer un groupe\n2- Modifier un groupe\n3- Supprimer un groupe\n4- Retour a l'accueil\n#Choix> ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'option choisie
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  switch(buffer[0]){
  case '1':
    createAccount(descBrCv);
    goto debut_menu;
  case '2':
    createArticle(descBrCv);
    goto debut_menu;
  case '4':
    break;
  default:
    cout<<"Cle invalide"<<endl;
    goto debut_menu;
  }
}

void menuArticle(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //envoi de la liste des options:
  initTab(buffer,sBuffer);
  strcat(buffer,"Menu - Article\n1- Liste des articles\n2- Creer un article\n3- Modifier un article\n4- Supprimer un article\n4- Retour a l'accueil\n#Choix> ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'option choisie
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  switch(buffer[0]){
  case '1':
    createArticle(descBrCv);
    goto debut_menu;
  case '2':
    createArticle(descBrCv);
    goto debut_menu;
  case '4':
    break;
  default:
    cout<<"Cle invalide"<<endl;
    goto debut_menu;
  }
}

void menuHome(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //recepteur de la valeur du shmdt
  shmwiki *p_wiki;
  int vShmdt;

  //variable d'authentification
  int idAuth=-1;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //envoi de la liste des options:
  initTab(buffer,sBuffer);
  if(idAuth==-1){
    strcat(buffer,"\nAccueil - Public\n1- S'authentifier\n2- S'inscrire\n3- Quitter\n#Choix> ");
  }else{
    strcat(buffer,"\nAccueil - ");
    p_wiki=(shmwiki *)shmat(shmid,NULL,0666);
    strcat(buffer,p_wiki->acc_list[idAuth].login);
    vShmdt=shmdt((void *)p_wiki);
    if(vSend==-1){
      perror("--send");
      exit(1);
    }
    strcat(buffer,"\n1- Mon compte\n2- Groupes\n3- Articles\n4- Deconnection\n#Choix> ");
  }
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'option choisie
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  if(idAuth==-1){
    switch(buffer[0]){
    case '1':
      idAuth=authentification(descBrCv);
      goto debut_menu;
      break;
    case '2':
      createAccount(descBrCv);
      goto debut_menu;
    case '3':
      break;
    default:
      cout<<"Cle invalide"<<endl;
      goto debut_menu;
      break;
    }
  }else{
    switch(buffer[0]){
    case '1':
      goto debut_menu;
      break;
    case '2':
      //menuGroup(descBrCv);
      goto debut_menu;
      break;
    case '3':
      menuArticle(descBrCv);
      goto debut_menu;
      break;
    case '4':
      idAuth=-1;
      goto debut_menu;
      break;
    default:
      cout<<"Cle invalide"<<endl;
      goto debut_menu;
      break;
    }//*/
  }
}


int authentification(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
  int vShmdt;

  //variables
  int i;
  int idAuth=-1;

  //definition de l'article
  account acc;

  //buffer
  char buffer[255];

  //on envoit saisie du login
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"\nAuthentification\nSaisir le login: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du login
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(acc.login,buffer);

  //envoi saisir passwd
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"Saisir le passwd: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du passwd
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(acc.passwd,buffer);


  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"#fail");
  shmwiki* p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
  for(i=0;i<25;i++){
    if(strcmp(p_shmwiki->acc_list[i].login,acc.login)==0){
      if(strcmp(p_shmwiki->acc_list[i].passwd,acc.passwd)==0){
	idAuth=i;
	strcpy(buffer,"#done");
	break;
      }
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  return idAuth;
}

account createAccount(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
  int i;

  //definition de l'article
  account acc;

  //buffer
  char buffer[255];

  //on envoit saisie du login
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"\nCreation d'un compte utilisateur\nSaisir le login: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du login
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(acc.login,buffer);

  //envoi saisir passwd
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"Saisir le passwd: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du passwd
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  strcpy(acc.passwd,buffer);

  shmwiki* p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
  for(i=0;i<25;i++){
    if(strcmp(p_shmwiki->acc_list[i].login,"")==0){
      p_shmwiki->acc_list[i]=acc;
      break;
    }
  }

  //envoi du message de fin de procedure
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"#done");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  return acc;
}

void printArticle(int *descBrCv,article *art){
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


void modifyArticle(int *descBrCv,article *art){
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


article createArticle(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //definition de l'article
  article art;

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
  return art;
}

void *actConnectClient (void *par){
  //initialisation du thread
  int descBrCv=*(int*)par;
  pthread_t idThread=pthread_self();
  cout<<"activite: "<<idThread<<" dans proc: "<<getpid()<<endl;

  menuHome(&descBrCv);
  /*//recepteur de la valeur des send et recv
  int vSend, vRecv;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //Envoi de liste d'options:
  initTab(buffer,sBuffer);
  strcat(buffer,"press 1 to -create account\npress 2 to - create article\npress 2 to - list current articles");
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

  if(strcmp(buffer,"account create")==0){
    createAccount(&descBrCv);
  }*/

  pthread_exit(NULL);
}


int main(int argc, char *argv[]){
  /*declaration des variables*/
  /*-shared memory*/
  int shmid;
  key_t shmkey;
  size_t shmsize;
  shmwiki *p_wiki;
  /*-threads*/
  int tid;
  pthread_t idConnexion;
  /*-buffers*/

  /*-descripteurs*/
  int descBrPub;
  int descBrCv;

  int vRecv, vSend;

  /*generation de la cle key_t*/
  shmkey=ftok("../README",423);

  /*calcul de la taille de la memoire partagee*/
  shmsize=size_t(sizeof(shmwiki));

  /*creation de la memoire partagee*/
  shmid=shmget(shmkey,shmsize,IPC_CREAT|0666);
  cout<<"shmid="<<shmid<<endl;
  initShmwiki();  

//p_wiki=(shmwiki *)shmat(shmid,NULL,0666);

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
