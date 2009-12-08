#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../lib/sock.h"
#include "../lib/sockdist.h"

using namespace std;

//donnees statique
int DISPLAYNOTIFICATION=0;//boolean de notification

//fonctions utilitaires
void initTab(char t[],int size);

//fonction gestion du menu
int menuAccount(int* descBrCli);
void menuGroup(int* descBrCli);
void menuArticle(int* descBrCli);
void menuHome(int* descBrCli);

//fonctions gestion des comptes
int authentification(int *descBrCli);
void listAccount(int *descBrCli);
void banAccount(int *descBrCli);
void createAccount(int *descBrCli);
void signupNotification(int *descBrCli);
void modifyAccount(int *descBrCli);
int deleteAccount(int *descBrCli);

////fonctions gestion des groupes
void listGroup(int *descBrCli);
void createGroup(int *descBrCli);
void joinGroup(int *descBrCli);
void leaveGroup(int *descBrCli);
void deleteGroup(int *descBrCli);

//fonctions gestion des articles
void listArticle(int *descBrCli);
void pubPrintArticle(int *descBrCli);
void printArticle(int *descBrCli);
void createArticle(int *descBrCli);
void modifyArticle(int *descBrCli);
void deleteArticle(int *descBrCli);


void initTab(char t[],int size){
  for(int i=0;i<size;i++){
    t[i]='\0';
  }
}

int menuAccount(int *descBrCli, int idAuth){
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

  if(idAuth==0){
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
      banAccount(descBrCli);
      goto debut_menu;
      break;
    case '4':
      break;
    default:
      cout<<"Cle invalide"<<endl;
      goto debut_menu;
    }
  }else{
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
	idAuth=-1;
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
  }
  return idAuth;
}

void menuGroup(int *descBrCli, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //reception de la liste des groupes
  listGroup(descBrCli);

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
    createGroup(descBrCli);
    goto debut_menu;
    break;
  case '2':
    joinGroup(descBrCli);
    goto debut_menu;
    break;
  case '3':
    leaveGroup(descBrCli);
    goto debut_menu;
    break;
  case '4':
    deleteGroup(descBrCli);
    goto debut_menu;
    break;
  case '5':
    break;
  default:
    cout<<"Cle invalide"<<endl;
    goto debut_menu;
  }
}

void menuArticle(int *descBrCli, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  listArticle(descBrCli);

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
    printArticle(descBrCli);
    goto debut_menu;
    break;
  case '2':
    createArticle(descBrCli);
    goto debut_menu;
    break;
  case '3':
    modifyArticle(descBrCli);
    goto debut_menu;
    break;
  case '4':
    deleteArticle(descBrCli);
    goto debut_menu;
    break;
  case '5':
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
  int idAuth=-1;

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

  if(idAuth==-1){
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
      pubPrintArticle(descBrCli);
      goto debut_menu;
      break;
    case '4':
      break;
    default:
      cout<<"Cle invalide"<<endl;
      goto debut_menu;
      break;
    }
  }else{
    switch(buffer[0]){
    case '1':
      idAuth=menuAccount(descBrCli, idAuth);
      goto debut_menu;
      break;
    case '2':
      menuGroup(descBrCli, idAuth);
      goto debut_menu;
      break;
    case '3':
      menuArticle(descBrCli, idAuth);
      goto debut_menu;
      break;
    case '4':
      idAuth=-1;
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

  //reception demande de login
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du login
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception demande de passwd
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du passwd
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception resultat de la procedure
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  idAuth=atoi(buffer);

  //envoie du message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'etat de la notification
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  DISPLAYNOTIFICATION=atoi(buffer);

  //envoie du message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  if(idAuth==-1){
    cout<<"Echec de l'authentification"<<endl;
  }else{
    cout<<"Authentification reussi"<<endl;
  }

  return idAuth;
}

void listAccount(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[1023];
  int sBuffer=sizeof(buffer);

  //reception de la liste des groupes
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void banAccount(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  listAccount(descBrCli);
  cout<<"listAccount done"<<endl;
  //reception de la saisie de l'ID
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'ID
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void createAccount(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception demande de login
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du login
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception demande de passwd
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du passwd
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du resultat de la procedure
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
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
  DISPLAYNOTIFICATION=atoi(buffer);

  if(DISPLAYNOTIFICATION==1){
    cout<<"Notification: ON"<<endl;
  }else{
    cout<<"Notification: OFF"<<endl;
  }

  //envoie du message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
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

  //envoie du message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  return deleted;
}

void listGroup(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception de la liste des groupes
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcat(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void createGroup(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception demande de nom
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du nom de groupe
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du resultat
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer<<endl;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcat(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void joinGroup(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception demande d'ID du groupe
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'ID du groupe
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du resultat
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer<<endl;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcat(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void leaveGroup(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception demande d'ID du groupe
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'ID du groupe
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du resultat
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer<<endl;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcat(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void deleteGroup(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
   
  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception demande d'ID du groupe
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'ID du groupe
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du resultat
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer<<endl;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcat(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void listArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception de la liste des articles
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcat(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void pubPrintArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[1023];
  int sBuffer=sizeof(buffer);

  //reception de la liste des articles
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcat(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de la demande de saisie de l'ID article
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'ID article
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'article
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void printArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[1023];
  int sBuffer=sizeof(buffer);

  //reception de la demande de saisie de l'ID article
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'ID article
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'article
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void createArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //reception de la demande de saisie du titre
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de du titre
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de la demande de saisie du contenu
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du contenu
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de la demande de saisie du groupe
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du groupe
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du resultat de la procedure
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void modifyArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[1023];
  int sBuffer=sizeof(buffer);

  //reception de la demande de saisie de l'ID
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'ID de l'article
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'article actuel
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#done");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de la demande de saisie du titre
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de du titre
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de la demande de saisie du contenu
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du contenu
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du resultat de la procedure
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie du message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void deleteArticle(int *descBrCli){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);


  //reception de la demande de saisie de l'ID article
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie de l'ID article
  initTab(buffer,sBuffer);
  cin.getline(buffer,sBuffer);
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du resultat de la procedure
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCli,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  cout<<buffer;

  //envoie message de synchronisation
  initTab(buffer,sBuffer);
  strcpy(buffer,"#sync");
  vSend=send(*descBrCli,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
}

void *actBroadcastReceiver(void *par){
  //variables
  int sock;
  struct sockaddr_in broadcastAddr;
  unsigned short broadcastPort;
  char recvString[255];
  int recvStringLen;

  broadcastPort = 21345;

  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    printf("socket() failed");

  //construction de la structure bind
  memset(&broadcastAddr, 0, sizeof(broadcastAddr));
  broadcastAddr.sin_family = AF_INET;
  broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  broadcastAddr.sin_port = htons(broadcastPort);

  //rattacher au port de broadcast
  if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
    printf("bind() failed\n");
  while(true){
    /* Receive a single datagram from the server */
    if ((recvStringLen = recvfrom(sock, recvString, 255, 0, NULL, 0)) < 0)
      printf("recvfrom() failed");

    recvString[recvStringLen] = '\0';
    if(DISPLAYNOTIFICATION==1){
      printf("%s", recvString);
    }
  }
    
  close(sock);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    if (argc < 3){
      fprintf(stderr,"Usage:  %s <address> <port>\n", argv[0]);
      exit(1);
    }

  //creation du thread broadcast
  pthread_t idBroadcastReceiver;
  if(pthread_create(&idBroadcastReceiver,NULL,actBroadcastReceiver,NULL)!=0){
    perror("--creation thread");
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
