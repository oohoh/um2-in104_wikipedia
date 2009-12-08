#ifndef _WIKILIB_H
#define _WIKILIB_H

typedef struct t_account { //structure compte
  char login[31]; //login du compte
  char passwd[31]; //mot de passe du compte
  int active; //boolean d'activite du compte (fonction ban)
} account;

typedef struct t_group { //structure groupe
  char name[31]; //nom du groupe
  int user[25]; //tableau d'identifiant des utilisateurs
  int article[50]; //tableau d'identifiant des articles
} group;

typedef struct t_article { //structure article
  char title[255]; //titre de l'article
  char author[31]; //nom de l'auteur
  time_t create; //date de creation
  time_t modify; //date de modification
  char content[255]; //contenu de l'article
} article;

typedef struct t_shmwiki { //structure memoire partagee
  account acc_list[25]; //liste de comptes
  group grp_list[16]; //liste de groupes
  article art_list[50]; //liste d'articles
} shmwiki;

#endif
