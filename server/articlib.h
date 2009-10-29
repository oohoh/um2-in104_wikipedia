#ifndef _ARTICLIB_H
#define _ARTICLIB_H

struct line {
  char text[255];
  struct line *next;
};

struct article {
  char title[255];
  char author[255];
  time_t create;
  time_t modify;
  struct line *content;
};

#endif
