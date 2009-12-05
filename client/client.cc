#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../lib/sock.h"
#include "../lib/sockdist.h"

using namespace std;

//fonctions utilitaires
void initTab(char t[],int size);

//fonction gestion du menu
int menuAccount(int* descBrCli);
void menuGroup(int* descBrCli);
void menuArticle(int* descBrCli);
void menuHome(int* descBrCli);

//fonctions gestion des comptes
int authentification(int *descBrCli);
void createAccount(int *descBrCli);
void signupNotification(int *descBrCli);
void modifyAccount(int *descBrCli);
int deleteAccount(int *descBrCli);

////fonctions gestion des groupes
//void createGroup(int *descBrCv);
//void modifyGroup(int *descBrCv, group *grp);
//void deleteGroup(int *descBrCv, group *grp);

//fonctions gestion des articles
void createArticle(int *descBrCli);
void modifyArticle(int *descBrCli);
void deleteArticle(int *descBrCli);
void printArticle(int *descBrCli);


void initTab(char t[],int size){
  for(int i=0;i<size;i++){
    t[i]='\0';
  }
}

int menuAccount(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  int idAuth=1;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //reception de la liste des options
  initTab(buffer,sBuffer);
  vRecv = recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--send");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'option choisie
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  switch(buffer[0]){
  case '1':
    signupNotification(descBrCli);
    goto debut_menu;
    break;
  case '2':
    modifyAccount(descBrCli);
    goto debut_menu;
    break;
  case '3':
    if(deleteAccount(descBrCli)){
      idAuth=0;
      break;
    }else{
      goto debut_menu;
      break;
    }
  case '4':
    break;
  default:
    cout<<"Cle invalide"<<endl;
    goto debut_menu;
  }
  return idAuth;
}

void menuGroup(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //reception de la liste des options
  initTab(buffer,sBuffer);
  vRecv = recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--send");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'option choisie
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  switch(buffer[0]){
  case '1':
    createAccount(descBrCli);
    goto debut_menu;
  case '2':
    createAccount(descBrCli);
    goto debut_menu;
  case '4':
    break;
  default:
    cout<<"Cle invalide ololz"<<endl;
    goto debut_menu;
  }
}

void menuArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //reception de la liste des options
  initTab(buffer,sBuffer);
  vRecv = recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--send");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'option choisie
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  switch(buffer[0]){
  case '1':
    createAccount(descBrCli);
    goto debut_menu;
  case '2':
    createArticle(descBrCli);
    goto debut_menu;
  case '4':
    break;
  default:
    cout<<"Cle invalide"<<endl;
    goto debut_menu;
  }
}

void menuHome(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //variable d'authentification
  int idAuth=0;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //reception de la liste des options
  initTab(buffer,sBuffer);
  vRecv = recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--send");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'option choisie
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  if(idAuth==0){
    switch(buffer[0]){
    case '1':
      idAuth=authentification(descBrCli);
      goto debut_menu;
      break;
    case '2':
      createAccount(descBrCli);
      goto debut_menu;
      break;
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
      idAuth=menuAccount(descBrCli);
      goto debut_menu;
      break;
    case '2':
      //menuGroup(descBrCli);
      goto debut_menu;
      break;
    case '3':
      menuArticle(descBrCli);
      goto debut_menu;
      break;
    case '4':
      idAuth=0;
      cout<<"Logged out."<<endl;
      goto debut_menu;
      break;
    default:
      cout<<"Cle invalide"<<endl;
      goto debut_menu;
      break;
    }//*/
  }
}

int authentification(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //variables
  int idAuth=0;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //saisie et envoie des identifiants
  while((strcmp(buffer,"#done")!=0) and (strcmp(buffer,"#fail")!=0)){
    initTab(buffer,sBuffer);
    vRecv=recv(*descBrCli,buffer,sBuffer,0);
    if(vRecv==-1){
      perror("--receive");
      exit(1);
    }

    if((strcmp(buffer,"#done")!=0) and (strcmp(buffer,"#fail")!=0)){
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

  if(strcmp(buffer,"#done")==0){
    idAuth=1;
  }

  return idAuth;
}

void createAccount(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //envoie demande creation compte
  initTab(buffer,sBuffer);
  strcpy(buffer,"account create");
  //vSend=send(*descBrCli,buffer,strlen(buffer),0);

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
}

void signupNotification(int* descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception de l'etat de notification
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer<<endl;
}

void modifyAccount(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception demande de saisie du passwd
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du passwd saisi
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de fin de procedure
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
}

int deleteAccount(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //variable
  int deleted=0;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception demande de confirmation
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie resultat demande de confirmation
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de fin de procedure
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  if(strcmp(buffer,"#done")==0){
    deleted=1;
  }

  return deleted;
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
  if(vConnect==-1){
    perror("--connect");
    exit(1);
  }

  menuHome(&descBrCli);
}
