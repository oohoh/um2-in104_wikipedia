#ifndef _WIKILIB_H
#define _WIKILIB_H

typedef struct t_account {
  char login[31];
  char passwd[31];
} account;

typedef struct t_group {
  char name[31];
  int user[25];
  int article[50];
} group;

typedef struct t_article {
  char title[255];
  char author[255];
  time_t create;
  time_t modify;
  char content[255];
} article;

typedef struct t_shmwiki {
  account acc_list[25];
  group grp_list[15];
  article art_list[50];
} shmwiki;

#endif
