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
void broadcastSender(char msg[255]);
int memberOf(int idAuth, int idGrp);
int groupExist(char name[31]);
int accExist(char login[31]);
int inGroup(int idArt);

//fonction gestion du menu
int menuAccount(int *descBrCv, int idAuth);
void menuGroup(int *descBrCv, int idAuth);
void menuArticle(int *descBrCv, int idAuth);
void menuHome(int *descBrCv, int idAuth);

//fonctions gestion des comptes
int authentification(int *descBrCv);
void listAccount(int *descBrCv);
void banAccount(int *descBrCv);
void createAccount(int *descBrCv);
void signupNotification(int *descBrCv, int idAuth);
void modifyAccount(int *descBrCv, int idAuth);
int deleteAccount(int *descBrCv, int idAuth);

//fonctions gestion des groupes
void listGroup(int* descBrCv);
void createGroup(int *descBrCv, int idAuth);
void joinGroup(int *descBrCv, int idAuth);
void leaveGroup(int *descBrCv, int idAuth);
void deleteGroup(int *descBrCv, int idAuth);

//fonctions gestion des articles
void listArticle(int* descBrCv, int idAuth);
void printArticle(int *descBrCv, int idAuth);
void createArticle(int *descBrCv, int idAuth);
void modifyArticle(int *descBrCv, int idAuth);
void deleteArticle(int *descBrCv, int idAuth);

void initTab(char t[],int size){
  for(int i=0;i<size;i++){
    t[i]='\0';
  }
}

void initShmwiki(){
  int i;
  int vShmdt;
  shmwiki *p_shmwiki;
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);

  //initialisation des comptes
  for(i=0;i<25;i++){
    initTab(p_shmwiki->acc_list[i].login, sizeof(p_shmwiki->acc_list[i].login));
    initTab(p_shmwiki->acc_list[i].passwd, sizeof(p_shmwiki->acc_list[i].passwd));
    p_shmwiki->acc_list[i].active=0;
  }
  strcpy(p_shmwiki->acc_list[0].login,"admin");
  strcpy(p_shmwiki->acc_list[0].passwd,"admin");
  p_shmwiki->acc_list[0].active=1;

  //initialisation des autres groupes
  for(i=0;i<16;i++){
    int j;
    initTab(p_shmwiki->grp_list[i].name, sizeof(p_shmwiki->grp_list[i].name));
    for(j=0;j<25;j++){
      p_shmwiki->grp_list[i].user[j]=-1;
    }
    for(j=0;j<50;j++){
      p_shmwiki->grp_list[i].article[j]=-1;
    }
  }
  strcpy(p_shmwiki->grp_list[0].name,"admin");
  p_shmwiki->grp_list[0].user[0]=0;
  strcpy(p_shmwiki->grp_list[1].name,"public");
  strcpy(p_shmwiki->grp_list[15].name,"notification");

  //initialisation des articles
  for(i=0;i<50;i++){
    initTab(p_shmwiki->art_list[i].title, sizeof(p_shmwiki->art_list[i].title));
    initTab(p_shmwiki->art_list[i].author, sizeof(p_shmwiki->art_list[i].author));
    p_shmwiki->art_list[i].create=0;
    p_shmwiki->art_list[i].modify=0;
    initTab(p_shmwiki->art_list[i].content, sizeof(p_shmwiki->art_list[i].content));
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }
}

void broadcastSender(char msg[255]){
  int sock;                         /* Socket */
  struct sockaddr_in broadcastAddr; /* Broadcast address */
  char *broadcastIP;                /* IP broadcast address */
  unsigned short broadcastPort;     /* Server port */
  char sendString[255];                 /* String to broadcast */
  int broadcastPermission;          /* Socket opt to set permission to broadcast */
  unsigned int sendStringLen;       /* Length of string to broadcast */

  broadcastIP = (char*)"localhost";            /* First arg:  broadcast IP address */ 
  broadcastPort = 21345;    /* Second arg:  broadcast port */
  strcpy(sendString,msg);

  /* Create socket for sending/receiving datagrams */
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    printf("socket() failed");

  /* Set socket to allow broadcast */
  broadcastPermission = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
		 sizeof(broadcastPermission)) < 0)
    printf("setsockopt() failed");

  /* Construct local address structure */
  memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
  broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
  broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
  broadcastAddr.sin_port = htons(broadcastPort);         /* Broadcast port */

  sendStringLen = strlen(sendString);  /* Find length of sendString */
  /* Broadcast sendString in datagram to clients every 3 seconds*/
  if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *) 
	     &broadcastAddr, sizeof(broadcastAddr)) != sendStringLen)
    printf("sendto() sent a different number of bytes than expected");

  sleep(1);   /* Avoids flooding the network */
}

int memberOf(int idAuth, int idGrp){
  //variables
  int i;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);

  for(i=0;i<25;i++){
    if(p_shmwiki->grp_list[idGrp].user[i]==idAuth){
      vShmdt=shmdt((void *)p_shmwiki);
      if(vShmdt==-1){
	perror("--shmdt");
	exit(1);
      }
      return i;
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  return -1;
}

int groupExist(char name[31]){
  //variables
  int i;
  int k=-1;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);

  for(i=0;i<16;i++){
    if(strcmp(p_shmwiki->grp_list[i].name,name)==0){
      k=i;
      break;
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  return k;
}

int inGroup(int idArt){
  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //variables
  int i;

  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);

  for(i=0;i<16;i++){
    if(strcmp(p_shmwiki->grp_list[i].name,"")!=0){
      int j;
      for(j=0;j<50;j++){
	if(p_shmwiki->grp_list[i].article[j]==idArt){
	  vShmdt=shmdt((void *)p_shmwiki);
	  if(vShmdt==-1){
	    perror("--shmdt");
	    exit(1);
	  }
	  return i;
	}
      }
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  return -1;
}

int accExist(char login[31]){
  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //variables
  int i;

  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);

  for(i=0;i<25;i++){
    if(strcmp(p_shmwiki->acc_list[i].login,login)==0){
      vShmdt=shmdt((void *)p_shmwiki);
      if(vShmdt==-1){
	perror("--shmdt");
	exit(1);
      }
      return i;
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }
  return -1;
}

int menuAccount(int* descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //envoi de la liste des options:
  initTab(buffer,sBuffer);
  strcat(buffer,"\nCompte - ");
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
  strcat(buffer,p_shmwiki->acc_list[idAuth].login);
  vShmdt=shmdt((void *)p_shmwiki);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
  if(idAuth==0){
    strcat(buffer,"\n1- (De)Inscription Notification\n2- Modifier le mot de passe\n3- Bannir un utilisateur\n4- Retour a l'accueil\n#Choix> ");
    vSend=send(*descBrCv,buffer,strlen(buffer),0);
    if(vSend==-1){
      perror("--send");
      exit(1);
    }
  }else{
    strcat(buffer,"\n1- (De)Inscription Notification\n2- Modifier le mot de passe\n3- Supprimer le compte\n4- Retour a l'accueil\n#Choix> ");
    vSend=send(*descBrCv,buffer,strlen(buffer),0);
    if(vSend==-1){
      perror("--send");
      exit(1);
    }
  }

  //reception de l'option choisie
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  
  if(idAuth==0){
    switch(buffer[0]){
    case '1':
      signupNotification(descBrCv, idAuth);
      goto debut_menu;
      break;
    case '2':
      modifyAccount(descBrCv, idAuth);
      goto debut_menu;
      break;
    case '3':
      banAccount(descBrCv);
      goto debut_menu;
      break;
    case '4':
      break;
    default:
      goto debut_menu;
    }
  }else{
    switch(buffer[0]){
    case '1':
      signupNotification(descBrCv, idAuth);
      goto debut_menu;
      break;
    case '2':
      modifyAccount(descBrCv, idAuth);
      goto debut_menu;
      break;
    case '3':
      if(deleteAccount(descBrCv, idAuth)){
	idAuth=-1;
	break;
      }else{
	goto debut_menu;
	break;
      }
    case '4':
      break;
    default:
      goto debut_menu;
    }
  }
  return idAuth;
}

void menuGroup(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  //envoie de la liste des groupes
  listGroup(descBrCv);

  //envoi de la liste des options:
  initTab(buffer,sBuffer);
  strcat(buffer,"\nAccueil - ");
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
  strcat(buffer,p_shmwiki->acc_list[idAuth].login);
  vShmdt=shmdt((void *)p_shmwiki);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
  strcat(buffer,"\n1- Creer un groupe\n2- Joindre un groupe\n3- Quitter un groupe\n4- Supprimer un groupe\n5- Retour a l'accueil\n#Choix> ");
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
    createGroup(descBrCv, idAuth);
    goto debut_menu;
    break;
  case '2':
    joinGroup(descBrCv, idAuth);
    goto debut_menu;
    break;
  case '3':
    leaveGroup(descBrCv, idAuth);
    goto debut_menu;
    break;
  case '4':
    deleteGroup(descBrCv, idAuth);
    goto debut_menu;
    break;
  case '5':
    break;
  default:
    goto debut_menu;
  }
}

void menuArticle(int* descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //creation du buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

 debut_menu:

  listArticle(descBrCv, idAuth);

  //envoi de la liste des options:
  initTab(buffer,sBuffer);
  strcat(buffer,"\nAccueil - ");
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
  strcat(buffer,p_shmwiki->acc_list[idAuth].login);
  vShmdt=shmdt((void *)p_shmwiki);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
  strcat(buffer,"\n1- Consulter un article\n2- Creer un article\n3- Modifier un article\n4- Supprimer un article\n5- Retour a l'accueil\n#Choix> ");
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
    printArticle(descBrCv, idAuth);
    goto debut_menu;
    break;
  case '2':
    createArticle(descBrCv, idAuth);
    goto debut_menu;
    break;
  case '3':
    modifyArticle(descBrCv, idAuth);
    goto debut_menu;
    break;
  case '4':
    deleteArticle(descBrCv, idAuth);
    goto debut_menu;
    break;
  case '5':
    break;
  default:
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
      goto debut_menu;
      break;
    }
  }else{
    switch(buffer[0]){
    case '1':
      idAuth=menuAccount(descBrCv, idAuth);
      goto debut_menu;
      break;
    case '2':
      menuGroup(descBrCv, idAuth);
      goto debut_menu;
      break;
    case '3':
      menuArticle(descBrCv, idAuth);
      goto debut_menu;
      break;
    case '4':
      idAuth=-1;
      goto debut_menu;
      break;
    default:
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

  shmwiki* p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
  for(i=0;i<25;i++){
    if(strcmp(p_shmwiki->acc_list[i].login,acc.login)==0){
      if((strcmp(p_shmwiki->acc_list[i].passwd,acc.passwd)==0) && (p_shmwiki->acc_list[i].active==1)){
	idAuth=i;
	break;
      }
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  char iChar[2];
  initTab(iChar,sizeof(iChar));
  sprintf(iChar, "%d\0", idAuth);
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,iChar);
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de synchronisation
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"0");
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
  for(i=0;i<25;i++){
    if(p_shmwiki->grp_list[15].user[i]==idAuth){
      strcpy(buffer,"1");
      break;
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  //envoie de l'etat de la notification
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de synchronisation
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  return idAuth;
}

void listAccount(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
  int i, idAcc;
  char iChar[2];

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[1023];
  int sBuffer=sizeof(buffer);

  initTab(buffer,sBuffer);
  strcat(buffer,"Liste des comptes:\n");
  int n=0;
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
  for(i=0;i<25;i++){
    if(strcmp(p_shmwiki->acc_list[i].login,"")!=0){
      strcat(buffer,"<");
      initTab(iChar,sizeof(iChar));
      initTab(iChar,sizeof(iChar));
      sprintf(iChar, "%d\0", i);
      strcat(buffer,iChar);
      strcat(buffer,">");
      strcat(buffer,p_shmwiki->acc_list[i].login);
      if(p_shmwiki->acc_list[i].active==1){
	strcat(buffer," [actif]    ");
      }else{
	strcat(buffer," [banni]    ");
      }
      n++;
      if(n==3){
	strcat(buffer,"\n");
	n=0;
      }
    }
  }
  strcat(buffer,"\n");

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  //envoi de la liste des comptes
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
}

void banAccount(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
  int i, idAcc;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[255];

  listAccount(descBrCv);

  //on envoit saisie de l'ID du compte
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"\nBan de compte\nSaisir l'ID: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'ID du compte
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  idAcc=atoi(buffer);

  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
  if(p_shmwiki->acc_list[idAcc].active==1){
    p_shmwiki->acc_list[idAcc].active=0;
  }else{
    p_shmwiki->acc_list[idAcc].active=1;
  }
  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }
}

void createAccount(int *descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
  int i, k;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

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
  acc.active=1;

  initTab(buffer,sizeof(buffer));
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
  if(accExist(acc.login)!=-1){
    strcpy(buffer,"Compte deja existant\n\n");
  }else{
    for(i=0;i<25;i++){
      if(strcmp(p_shmwiki->acc_list[i].login,"")==0){
	k=i;
	p_shmwiki->acc_list[i]=acc;
	strcpy(buffer,"Compte cree\n");
	break;
      }
    }
  }
  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  //envoi du message de fin de procedure
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de synchronisation
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
}

void signupNotification(int* descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //variables
  int i;
  int reg=1;

  //buffer
  char buffer[255];
  initTab(buffer,sizeof(buffer));

  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
  for(i=0;i<25;i++){
    if(p_shmwiki->grp_list[15].user[i]==idAuth){
      p_shmwiki->grp_list[15].user[i]=-1;
      strcpy(buffer,"0");
      reg=0;
    }
  }

  if(reg==1){
    for(i=0;i<25;i++){
      if(p_shmwiki->grp_list[15].user[i]==-1){
	p_shmwiki->grp_list[15].user[i]=idAuth;
	strcpy(buffer,"1");
      }
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  //on envoit l'etat de la notification
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de synchronisation
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
}

void modifyAccount(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
  int i;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //envoi saisir passwd
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nChangement de mot de passe\nSaisir le passwd: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du passwd
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
  strcpy(p_shmwiki->acc_list[idAuth].passwd,buffer);
  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }
}

int deleteAccount(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //variables
  int i;
  int deleted=0;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //envoi demande de confirmation
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nSuppression de compte\nEtes vous sur de vouloir supprimer votre compte?\nO:Oui\nn:Non\n#Choix> ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception resultat demande de confirmation
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  if((strcmp(buffer,"O")==0) or (strcmp(buffer,"o")==0)){
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
    initTab(p_shmwiki->acc_list[idAuth].login, sizeof(p_shmwiki->acc_list[idAuth].login));
    initTab(p_shmwiki->acc_list[idAuth].passwd, sizeof(p_shmwiki->acc_list[idAuth].passwd));
    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
    deleted=1;
  }

  //envoi du message de fin de procedure
  initTab(buffer,sBuffer);
  if(deleted==1){
    strcpy(buffer,"#done");
  }else{
    strcpy(buffer,"#fail");
  }
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de synchronisation
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  return deleted;
}

void listGroup(int* descBrCv){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //variables
  int i;
  char iChar[2];
  int n=0;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  initTab(buffer,sBuffer);
  strcat(buffer,"\nListe des Groupes:\n");
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
  for(i=1;i<15;i++){
    if(strcmp(p_shmwiki->grp_list[i].name,"")!=0){
      sprintf(iChar, "%d\0", i);
      strcat(buffer,"#");
      strcat(buffer,iChar);
      strcat(buffer,":");
      strcat(buffer,p_shmwiki->grp_list[i].name);
      strcat(buffer,"    ");
      n++;
    }

    if(n==3){
      strcat(buffer,"\n");
      n=0;
    }
  }
  strcat(buffer,"\n");
  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  //on envoit l'etat de la notification
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
}

void createGroup(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;
  int i;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);
  char msgBroadcast[255];
  int sMsgBroadcast=sizeof(msgBroadcast);

  //on envoit saisie du nom de groupe
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nCreation d'un groupe\nSaisir le nom: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du nom de groupe
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  if(groupExist(buffer)==-1){
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
    for(i=0;i<16;i++){
      if(strcmp(p_shmwiki->grp_list[i].name,"")==0){
	strcpy(p_shmwiki->grp_list[i].name,buffer);
	p_shmwiki->grp_list[i].user[0]=idAuth;
	initTab(buffer,sBuffer);
	strcpy(buffer,"Groupe cree");
	initTab(msgBroadcast,sMsgBroadcast);
	strcat(msgBroadcast,"[annonce] Groupe \"");
	strcat(msgBroadcast,p_shmwiki->grp_list[i].name);
	strcat(msgBroadcast,"\" cree.\n");
	broadcastSender(msgBroadcast);
	break;
      }
    }
    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
  }else{
    initTab(buffer,sBuffer);
    strcpy(buffer,"Ce groupe existe deja");
  }

  //envoi du message de fin de procedure
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
}

void joinGroup(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //variables
  int i, idGrp;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //envoi saisie ID groupe
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nJoindre un groupe\nSaisir l'ID du groupe: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'ID
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  idGrp=atoi(buffer);

  initTab(buffer,sBuffer);
  if(memberOf(idAuth, idGrp)!=-1){
    strcpy(buffer,"Vous etes deja inscris dans ce groupe");
  }else{
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
    for(i=0;i<25;i++){
      if(p_shmwiki->grp_list[idGrp].user[i]==-1){
	p_shmwiki->grp_list[idGrp].user[i]=idAuth;
      }
    }
    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
    strcpy(buffer,"Inscription reussi");
  }

  //envoi du message de fin de procedure
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
}

void leaveGroup(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //variables
  int i, idGrp;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  //envoi saisie ID groupe
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nQuitter un groupe\nSaisir l'ID du groupe: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'ID
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  idGrp=atoi(buffer);

  initTab(buffer,sBuffer);
  if(memberOf(idAuth, idGrp)==-1){
    strcpy(buffer,"Vous n'etes pas inscris dans ce groupe");
  }else{
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);
    for(i=0;i<25;i++){
      if(p_shmwiki->grp_list[idGrp].user[i]==idAuth){
	p_shmwiki->grp_list[idGrp].user[i]=-1;
      }
    }
    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
    strcpy(buffer,"Desinscription reussi");
  }

  //envoi du message de fin de procedure
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
}

void deleteGroup(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //variables
  int i, idGrp;
  int deleted=0;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);
  char msgBroadcast[255];
  int sMsgBroadcast=sizeof(msgBroadcast);

  //envoi demande d'ID du groupe
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nSuppression de groupe\nEntrez l'ID du groupe:");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception resultat demande de confirmation
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

  idGrp=atoi(buffer);

  initTab(buffer,sBuffer);
  if(memberOf(idAuth, idGrp)!=-1){
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0);

    initTab(msgBroadcast,sMsgBroadcast);
    strcat(msgBroadcast,"[annonce] Groupe \"");
    strcat(msgBroadcast,p_shmwiki->grp_list[idGrp].name);
    strcat(msgBroadcast,"\" supprime.\n");
    broadcastSender(msgBroadcast);

    initTab(p_shmwiki->grp_list[idGrp].name, sizeof(p_shmwiki->grp_list[idGrp].name));
    for(i=0;i<25;i++){
      p_shmwiki->grp_list[idGrp].user[i]=-1;
    }
    for(i=0;i<50;i++){
      if(p_shmwiki->grp_list[idGrp].article[i]!=-1){
	initTab(msgBroadcast,sMsgBroadcast);
	strcat(msgBroadcast,"[annonce] Article ");
	strcat(msgBroadcast,p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].title);
	strcat(msgBroadcast," supprime.\n");
	broadcastSender(msgBroadcast);
	initTab(p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].title, sizeof(p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].title));
	initTab(p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].author, sizeof(p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].author));
	p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].create=0;
	p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].modify=0;
	initTab(p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].content, sizeof(p_shmwiki->art_list[p_shmwiki->grp_list[idGrp].article[i]].content));
	p_shmwiki->grp_list[idGrp].article[i]=-1;
      }
    }
    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
    strcpy(buffer,"Groupe supprime");
  }else{
    strcpy(buffer,"Vous n'avez pas les droits requis");
  }

  //envoi du resultat
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
}

void listArticle(int* descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //variables
  int i;
  char iChar[2];

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);

  initTab(buffer,sBuffer);
  strcat(buffer,"Liste des articles:\n");
  p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
  for(i=1;i<15;i++){
    if((strcmp(p_shmwiki->grp_list[i].name,"")!=0) && (memberOf(idAuth,i)!=-1)){
      strcat(buffer,"<");
      initTab(iChar,sizeof(iChar));
      sprintf(iChar, "%d\0", i);
      strcat(buffer,iChar);
      strcat(buffer,":");
      strcat(buffer,p_shmwiki->grp_list[i].name);
      strcat(buffer,">\n");
      int j;
      int n=0;
      for(j=0;j<50;j++){
	if(p_shmwiki->grp_list[i].article[j]!=-1){
	  initTab(iChar,sizeof(iChar));
	  sprintf(iChar, "%d\0", p_shmwiki->grp_list[i].article[j]);
	  strcat(buffer,"#");
	  strcat(buffer,iChar);
	  strcat(buffer,":");
	  strcat(buffer,p_shmwiki->art_list[p_shmwiki->grp_list[i].article[j]].title);
	  strcat(buffer,"    ");
	  n++;
	  if(n==2){
	    strcat(buffer,"\n");
	    n=0;
	  }
	}
      }
      strcat(buffer,"\n");
    }
  }

  vShmdt=shmdt((void *)p_shmwiki);
  if(vShmdt==-1){
    perror("--shmdt");
    exit(1);
  }

  //on envoit la liste des articles
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
}

void printArticle(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //variables
  int i, idArt, idGrp;

  //buffer
  char buffer[1023];
  int sBuffer=sizeof(buffer);

  //envoi saisie ID article
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nConsulter un article\nSaisir l'ID de l'article: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'ID
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  idArt=atoi(buffer);

  initTab(buffer,sBuffer);
  if(memberOf(idAuth,inGroup(idArt))==-1){
    strcpy(buffer,"vous n'avez pas les droits requis");
  }else{
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);

    //ajout du titre
    strcat(buffer,"\nTitre : ");
    strcat(buffer,p_shmwiki->art_list[idArt].title);
    strcat(buffer,"\n");

    //ajout de l'auteur
    strcat(buffer,"Auteur : ");
    strcat(buffer,p_shmwiki->art_list[idArt].author);
    strcat(buffer,"\n");

    //ajout de la date de creation
    strcat(buffer,"Creation : ");
    strcat(buffer,ctime(&p_shmwiki->art_list[idArt].create));

    //ajout de la date de derniere modification
    strcat(buffer,"Modification : ");
    strcat(buffer,ctime(&p_shmwiki->art_list[idArt].modify));

    //ajout du contenu
    strcat(buffer,"Contenu : ");
    strcat(buffer,p_shmwiki->art_list[idArt].content);
    strcat(buffer,"\n\n");

    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
  }

  //envoie de l'article
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
}

void createArticle(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //variables
  int i, idGrp;
  char temp_title[255];
  char temp_content[255];

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);
  char msgBroadcast[255];
  int sMsgBroadcast=sizeof(msgBroadcast);

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
  strcpy(temp_title,buffer);

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
  strcpy(temp_content,buffer);

  //envoi saisir groupe
  initTab(buffer,sizeof(buffer));
  strcpy(buffer,"Saisir groupe associe (vide pour acces publique): ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }
	
  //recoit groupe
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  if(strcmp(buffer,"")==0){
    idGrp=1;
  }else{
    idGrp=atoi(buffer);
  }

  initTab(buffer,sizeof(buffer));
  if(memberOf(idAuth,idGrp)==-1){
    strcpy(buffer,"vous n'avez pas les droits requis\n\n");
  }else{
    initTab(msgBroadcast,sMsgBroadcast);
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
    strcat(msgBroadcast,"[annonce] Article \"");
    strcat(msgBroadcast,temp_title);
    strcat(msgBroadcast,"\" ajoute dans le groupe \"");
    strcat(msgBroadcast,p_shmwiki->grp_list[idGrp].name);
    strcat(msgBroadcast,"\".\n");
    broadcastSender(msgBroadcast);
    int i=0;
    int wrote=0;
    while(i<50 && wrote==0){
      if(strcmp(p_shmwiki->art_list[i].title,"")==0){
	//ajout du titre
	strcpy(p_shmwiki->art_list[i].title,temp_title);

	//ajout de l'auteur
	strcpy(p_shmwiki->art_list[i].author,p_shmwiki->acc_list[idAuth].login);

	//ajout de la date de creation
	p_shmwiki->art_list[i].create=time(NULL);

	//ajout de la date de derniere modification
	p_shmwiki->art_list[i].modify=time(NULL);

	//ajout du contenu
	strcpy(p_shmwiki->art_list[i].content,temp_content);

	int j=0;
	while(j<50 && wrote==0){
	  if(p_shmwiki->grp_list[idGrp].article[j]==-1){
	    p_shmwiki->grp_list[idGrp].article[j]=i;
	    wrote=1;
	  }
	  j++;
	}
	strcpy(buffer,"article cree\n\n");
      }
      i++;
    }
    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
  }

  //envoi du message de fin de procedure
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
}

void modifyArticle(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //variables
  int i, idArt, idGrp;
  char temp_title[255];
  char temp_content[255];

  //buffer
  char buffer[1023];
  int sBuffer=sizeof(buffer);
  char msgBroadcast[255];
  int sMsgBroadcast=sizeof(msgBroadcast);

  //envoi saisie ID article
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nModifier un article\nSaisir l'ID de l'article: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'ID
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  idArt=atoi(buffer);

  initTab(buffer,sBuffer);
  if(memberOf(idAuth,inGroup(idArt))==-1){
    strcpy(buffer,"vous n'avez pas les droits requis");
  }else{
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);

    //ajout du titre
    strcat(buffer,"Titre : ");
    strcat(buffer,p_shmwiki->art_list[idArt].title);
    strcat(buffer,"\n");

    //ajout de l'auteur
    strcat(buffer,"Auteur : ");
    strcat(buffer,p_shmwiki->art_list[idArt].author);
    strcat(buffer,"\n");

    //ajout de la date de creation
    strcat(buffer,"Creation : ");
    strcat(buffer,ctime(&p_shmwiki->art_list[idArt].create));

    //ajout de la date de derniere modification
    strcat(buffer,"Modification : ");
    strcat(buffer,ctime(&p_shmwiki->art_list[idArt].modify));

    //ajout du contenu
    strcat(buffer,"Contenu : ");
    strcat(buffer,p_shmwiki->art_list[idArt].content);
    strcat(buffer,"\n\n");

    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
  }

  //envoie de l'article
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception du message de synchronisation
  initTab(buffer,sizeof(buffer));
  vRecv=recv(*descBrCv,buffer,sizeof(buffer),0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }

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
  strcpy(temp_title,buffer);

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
  strcpy(temp_content,buffer);

  initTab(buffer,sizeof(buffer));

  if(memberOf(idAuth,(idGrp=inGroup(idArt)))==-1){
    strcpy(buffer,"vous n'avez pas les droits requis\n\n");
  }else{
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);

    strcat(msgBroadcast,"[annonce] Article \"");
    strcat(msgBroadcast,p_shmwiki->art_list[idArt].title);
    strcat(msgBroadcast,"\" modifie en article \"");
    strcat(msgBroadcast,temp_title);
    strcat(msgBroadcast,"\".\n");
    broadcastSender(msgBroadcast);

    //modification du titre
    strcpy(p_shmwiki->art_list[idArt].title,temp_title);

    //modification de la date de derniere modification
    p_shmwiki->art_list[idArt].modify=time(NULL);

    //modification du contenu
    strcpy(p_shmwiki->art_list[idArt].content,temp_content);

    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }

    strcpy(buffer,"article modifie\n\n");
  }

  //envoi du message de fin de procedure
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
}

void deleteArticle(int *descBrCv, int idAuth){
  //recepteur de la valeur des send et recv
  int vSend, vRecv;

  //shmem
  shmwiki *p_shmwiki;
  int vShmdt;

  //variables
  int i, idArt, idGrp;

  //buffer
  char buffer[255];
  int sBuffer=sizeof(buffer);
  char msgBroadcast[255];
  int sMsgBroadcast=sizeof(msgBroadcast);

  //envoi saisie ID article
  initTab(buffer,sBuffer);
  strcpy(buffer,"\nSupprimer un article\nSaisir l'ID de l'article: ");
  vSend=send(*descBrCv,buffer,strlen(buffer),0);
  if(vSend==-1){
    perror("--send");
    exit(1);
  }

  //reception de l'ID
  initTab(buffer,sBuffer);
  vRecv=recv(*descBrCv,buffer,sBuffer,0);
  if(vRecv==-1){
    perror("--receive");
    exit(1);
  }
  idArt=atoi(buffer);

  initTab(buffer,sBuffer);
  if(memberOf(idAuth,(idGrp=inGroup(idArt)))==-1){
    strcpy(buffer,"vous n'avez pas les droits requis\n\n");
  }else{
    p_shmwiki=(shmwiki *)shmat(shmid,NULL,0666);
    initTab(msgBroadcast,sMsgBroadcast);
    strcat(msgBroadcast,"[annonce] Article \"");
    strcat(msgBroadcast,p_shmwiki->art_list[idArt].title);
    strcat(msgBroadcast,"\" supprime.\n");
    broadcastSender(msgBroadcast);

    for(i=0;i<50;i++){
      if(p_shmwiki->grp_list[idGrp].article[i]==idArt){
	p_shmwiki->grp_list[idGrp].article[i]=-1;
      }
    }
    initTab(p_shmwiki->art_list[idArt].title, sizeof(p_shmwiki->art_list[idArt].title));
    initTab(p_shmwiki->art_list[idArt].author, sizeof(p_shmwiki->art_list[idArt].author));
    p_shmwiki->art_list[idArt].create=0;
    p_shmwiki->art_list[idArt].modify=0;
    initTab(p_shmwiki->art_list[idArt].content, sizeof(p_shmwiki->art_list[idArt].content));

    vShmdt=shmdt((void *)p_shmwiki);
    if(vShmdt==-1){
      perror("--shmdt");
      exit(1);
    }
    strcpy(buffer,"article supprime\n\n");
  }

  //envoie du resultat de la suppression d'article
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
}


void *actConnectClient (void *par){
  //initialisation du thread
  int descBrCv=*(int*)par;
  pthread_t idThread=pthread_self();

  menuHome(&descBrCv);

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
  initShmwiki();

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
