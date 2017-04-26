#ifndef HISTORY_H
#define HISTORY_H


typedef struct metadatablokk {
  char* command;
  int id;
  int length;
  int data_index[15];
  struct metadatablokk* next;
} metadatablokk;

void print_bitmap();
void print_datablocks();
void history_init();
void free_history(metadatablokk*);
void print_history();
char* get_metadatablokk_cmd(metadatablokk*);
void free_block(metadatablokk*);
metadatablokk* new_metadatablokk(char*, int);
metadatablokk* get_metadatablokk(int);
int delete_metadatablokk(int);
char** builtin_cmd(char**);

#endif
