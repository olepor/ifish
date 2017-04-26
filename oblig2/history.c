#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/**
 * datablokk_id peker kan ha flere blokker. Så må ha peker til den første blokken
 * også gå videre fra der.
 */
typedef struct metadatablokk {
  int id;
  int length;
  int data_index[15];
  struct metadatablokk* next;
} metadatablokk;

char* bitmap;
char* datablocks;
int ncmd;


metadatablokk* head = NULL;
metadatablokk* current = NULL;

#ifdef DEBUG
void print_bitmap() {
  printf("BITMAP: \n\n");
  int i;
  int j;
  for (i = 0; i<8; i++) {
    for (j = 7; j>=0; j--) {
      printf("%d", (*(bitmap+i) >> j) & 1);
    }
    if (i>0 && (i+1)%4 == 0) {
      printf("\n");
    }
  }
}
void print_datablocks() {
  printf("DATABLOCKS: \n\n");
  int i;
  while(i < 64) {
    printf("#%8.8s#", datablocks+(8*i++));
    if (i>0 && i%4 == 0) {
      printf("\n");
    }
  }
}
#endif

void history_init() {
  bitmap = (char*)malloc((8+64*8)*sizeof(char));
  datablocks = bitmap + 8;
}

void free_history() {
  free(bitmap);
  metadatablokk* tmp = head;
  while (tmp != NULL) {
    metadatablokk* next = tmp->next;
    free(tmp);
    tmp = next;
  }
}


void free_block(metadatablokk* n) {
  int i;
  for (i = 0; i<15; i++) {
    int index = n->data_index[i];
    if (index > -1) {
      int row = (index/8);
      int place = 7-(index % 8);

      *(bitmap + row) &= ~(1 << place);
      bzero(datablocks+(8*index), 8);
    } else {
      break;
    }
  }
  //free(n);
}

/**
 * FIFO
 * Finn ledige blokker i bitmap.
 * Sett riktig dritt i bitmap.
 * cmd skal lagres i data_index?
 */

char* get_metadatablokk_text(metadatablokk* n){
  char* text = malloc(sizeof(char)*n->length + 1);
  *text = '\0';
  int i, l=0;
  for(i=0; i<15; i++){
    int index = n->data_index[i];
    if(index > -1){
      int mv = (n->length - l < 8) ? n->length - l : 8 ;
      strncat(text, datablocks+(8*index), mv);
      l += mv;
    }else{
      return text;
    }
  }
  printf("Error: couldn't get text for one history element.");
  free(text);
  exit(1);
}

void new_metadatablokk(char* cmd, int num) {
  metadatablokk* n = (metadatablokk*) malloc(sizeof(metadatablokk));
  ncmd++;
  
  if (head == NULL) {
    head = n;
    current = n;
  } else {
    current->next = n;
    current = n;
  }
  n->next = NULL;
  n->id = num;
  n->length = strlen(cmd);

  int d = 0;
  while(1) {
    int i, j, index=0;
    for (i = 0; i<8; i++) {
      char* byte = bitmap+i;
      for (j = 7; j>=0; j--) {
	int bit = (*byte >> j) & 1;
	if (bit == 0) {
	  *byte |= 1<<j;
	  char* block = datablocks+(8*index);
	  int len = strlen(cmd);
	  int blen = len<8 ? len : 8;
	  strncpy(block, cmd, blen);
	  n->data_index[d++] = index;
	  if (len <= 8) {
	    while (d < 15) n->data_index[d++] = -1;
#ifdef DEBUG
	    print_bitmap();
	    print_datablocks();
#endif
	    return;
	  }
	  cmd += 8;
	}
	index++;
      }
    }
    metadatablokk* tmp = head;
    head = head->next;
    free_block(tmp);
  }
}


metadatablokk* get_metadatablokk(int i) {
  metadatablokk* tmp = head;

  while(tmp != NULL) {
    if (tmp->id == i) {
      return tmp;
    }
    tmp = tmp->next;
  }
  return NULL;
}

int delete_metadatablokk(int i) {
  metadatablokk* tmp = head;
  metadatablokk* prev = NULL;

  if (head == NULL) {
    printf("No history to delete.\n");
  }
  while (tmp != NULL) {
    if (tmp->id == i) {
      if (tmp == head) {
	head = head->next;
      } else if (tmp == current) {
	current = prev;
	if (prev != NULL) {
	  prev->next = NULL;
	}
      } else {
	prev->next = tmp->next;
      }
      free_block(tmp);
      return 1;
    }
    prev = tmp;
    tmp = tmp->next;
  }
  return 0;
}

void print_history() {
  metadatablokk* tmp = head;

  while(tmp != NULL) {
    char* cmd = get_metadatablokk_text(tmp);
    printf("%d: %s\n", tmp->id, cmd);
    tmp = tmp->next;
  }
}

char** builtin_cmd(char** param) {
  if (param[0] == NULL) {
    return NULL;
  }
  if (strcmp(param[0], "h") == 0) {
    printf("History list of the last %d commands:\n", ncmd);
    print_history();
    return NULL;
  }
  if (strcmp(param[0], "h") == 0 && strcmp(param[1], "-d") == 0) {
    if (param[2] == NULL) {
      printf("Expected index parameter.\n");
    } else {
      int i = atoi(param[2]);
      if (delete_metadatablokk(i) != 1) {
	printf("Failed to delete command.\n");
      }
    }
    return NULL;
  }
  if (strcmp(param[0], "h") == 0) {
    if (param[1] == NULL) {
      printf("Expected i\n");
    } else {
      int i = atoi(param[1]);
      metadatablokk* n = get_metadatablokk(i);
      if (n != NULL) {
	char* tmp = get_metadatablokk_text(n);
	char** t = NULL;
	t[0] = tmp;
	t[1] = '\0';
	return t;
      } else {
	return NULL;
	printf("Could not find command in history.\n");
      }
    }
  }
  return param;
}
