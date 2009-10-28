#ifndef _ARTICLIB_H
#define _ARTICLIB_H

struct line {
  char character[255];
  struct line *next;
};

struct article {
  char title[255];
  char autor[255];
  struct tm *create;
  struct tm *modify;
  struct line *content;
};

#endif
