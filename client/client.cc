#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "sock.h"
#include "sockdist.h"

using namespace std;

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

  int boolConnect = connect(descBrCli,(struct sockaddr *)adrBrPub,lgAdrBrPub);
  
cout<<"res du connect: "<<boolConnect<<endl;

//Contenant d'envoi
char envoi[255]="";
char envoi1[255]="";
char envoi2[255]="";
char envoi3[255]="";


//Contenant de reception
char recu[255]="";
char recu1[255]="";
char recu2[255]="";
char recu3[255]="";


//on recoit les options
  int resR = recv(descBrCli,recu,sizeof(recu),0); 
//affiche reception
cout<<recu<<endl;
//prepare a input

char * input=new char[255];
char * input1=new char[255];
char * input2=new char[255];
char * input3=new char[255];

cin.getline(input,255);

//on envoit notre choix
if(strcmp(input,"1")==0)
	{
	strcat(envoi,input);
	int resS = send(descBrCli,envoi,strlen(envoi),0);

	//on recoit saisir titre
	int resR2 = recv(descBrCli,recu1,sizeof(recu1),0);
	cout<<recu1;
	//input titre
	cin.getline(input1,255);
	strcpy(envoi1,input1);
	//envoi titre
	int resS2 = send(descBrCli,envoi1,strlen(envoi1),0);

	//recoit saisir auteur
	int resR3 = recv(descBrCli,recu2,sizeof(recu2),0);
	cout<<recu2;
	//input auteur
	cin.getline(input2,255);
	strcpy(envoi2,input2);
	//envoi auteur
	int resS3 = send(descBrCli,envoi2,strlen(envoi2),0);


	//recoit saisir contenu
	int resR4 = recv(descBrCli,recu3,sizeof(recu3),0);
	cout<<recu3;
	//input contenu
	cin.getline(input3,255);
	strcpy(envoi3,input3);
	//envoi contenu
	int resS4 = send(descBrCli,envoi3,strlen(envoi3),0);
	}

  return 0;
}



