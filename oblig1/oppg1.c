#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>


typedef struct node {
  char* data;
  struct node* neste;
} Node;

Node* head = NULL;
Node* current = NULL;
int totalNodes = 0;

/*
 * Creates new node in allocated memory.
 * Line fed into the function is copied into the allocated spot in memory.
 * Next pointer is set to point to NULL, because there are no next element yet.
 */
Node* new_node(char* c) {
  Node* n = malloc(sizeof(Node));
  n->data = malloc(strlen(c));
  strcpy(n->data, c);
  n->neste = NULL;
  return n;
  
}

/*
 * Sets head and current pointer to point at the new node.
 * totalNodes are use for the random generating in random line function.
 * If head dosent point to NULL, that means there is already a node in the linked
 * list. The next pointer in current points to a new node and sets the current
 * pointer to the new node.
 */
int add_node(char* c) {
  if (head == NULL) {
    head = current = new_node(c);
    totalNodes++;
    return 0;
  } else {
    current->neste = new_node(c);
    current = current->neste;
    totalNodes++;
    return 0;
  }
}

/*
 * Iterates through the linked list and prints the string pointed on by data.
 */
int print() {
  printf("Print text:\n");
  Node* n = head;
  while (n != NULL) {
    printf(" %s", n->data);
    n = n->neste;
  }
  return 0;
}

/*
 * Iterates through the linked list and frees all allocated memory.
 */
int freeAllocated() {
  Node* n = head;
  Node* tmp = n;
  while (n != NULL) {

    free(n->data);
    free(n->neste);
    n->data = NULL;
    n->neste = NULL;
    n = tmp;
  }
  free(tmp->data);
  free(tmp->neste);
  head = NULL;
  current = NULL;
  return 0;
}

/*
 * Prints a random line in the linked list.
 * A random number between 0 and totalNodes is generated and used to find
 * a random line. 
 */
int print_random_line() {
  int i = 0;
  Node* n = head;
  srand(time(NULL));
  int random = rand() % totalNodes;
  for (i = 0; i < totalNodes; i++) {
    if (i == random) {
      printf("Print a random line\n");
      printf(" %s", n->data);
      return 0;
    } else {
      n = n->neste;
    }
  }
  return 0;
}

/*
 * This is used to check if a character is a vowel.
 * Currently dosent work with æøå.
 */
int vowelChecker(const char c) {
  int size = 10;
  int i;
  for (i = 0; i < size; i++) {
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u'
	|| c == 'y') {//|| c == 'æ' || c == 'ø' || c == 'å') { // æ ø å 
      return 1;
    }
  }
  return 0;
}

/*
 * This fuction replaces all vowels with vowels.
 * line_pointer points to the saved string n->data. The for loop iterates
 * through the string and checks every character for vowel, if it is a vowel
 * it is set to another vowel. n->data is printed because line_pointer only
 * points to where n->data is saved and changed.
 */
int replace() {
  printf("Replace vowels ...\n");
  char kaigen[] = "aeiouy";
  int len = 6;
  int i;
  for (i = 0; i<len; i++) {
    printf(" ... with vowel %c\n", kaigen[i]);
    Node* n = head;
    while (n != NULL) {
      char* line_pointer = n->data;
      for (; *line_pointer != '\0'; line_pointer++) {
	if (vowelChecker(*line_pointer) == 1) {
	  *line_pointer = kaigen[i];
	}
      }
      printf(" %s", n->data);
      n = n->neste;
    }
  }
  return 0;
}

/*
 * This function removes vowels.
 * A tmp array is created to store the removed string.
 * Iterates through the line to check if a character is a vowel, if it is a vowel
 * an index is skipped in the tmp array. The last character gets set to '\0'.
 */
int vowelRemover() {
  printf("Remove vowels:\n");
  
  Node* n = head;

  while(n != NULL) {
    int len = strlen(n->data);
    char tmp[len];
    char* line_pointer = n->data;
    int j = 0;
    for (; *line_pointer != '\0'; line_pointer++) {
      if (vowelChecker(*line_pointer) == 0) {
	tmp[j++] = *line_pointer;
      }
    }
    tmp[j] = '\0';
    printf(" %s", tmp);
    n = n->neste;
  }
  return 0;
}

/*
 * Number of characters is 149 with this method.
 * The increased number of characters is caused,
 * by æ, ø and å.
 */
int len() {
  Node* n = head;
  int len = 0;
  while (n != NULL) {
    len += strlen(n->data);
    n = n->neste;
  }
  printf(" The text is %d characters long\n", len);
  return 0;
}

/*
 * This function reads the file and uses other fuctions to add the lines read
 * into a linked list.
 */
int filereader(char* filename) {
  FILE* file = fopen(filename, "r");
  
  if (file == NULL) {
    printf("Error while opening file.\n");
    exit(1);
  }

  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  
  while ((read = getline(&line, &len, file) != -1)) {
    add_node(line);
  }
  
  free(line);  
  fclose(file);
  return 0;
}

/*
 * Main
 * Functions are called on here.
 */
int main(int argc, char* argv[]) {
  
  if(argc == 1) {
    printf("Ingen argument.\n");
    exit(1);
  }
    
  if (filereader(argv[2]) != 0) {
    printf("File reader error\n");
  }
    
    
  
  if (strcmp(argv[1], "print") == 0) {
    if (print() != 0) {
      printf("Error in print()\n");
      return 1;
    } else {
      exit(0);
    }
  } else if (strcmp(argv[1], "random") == 0) {
    if (print_random_line() != 0) {
      printf("Error in random()\n");
      return 1;
    } else {
      exit(0);
    }
  } else if (strcmp(argv[1], "replace") == 0) {
    if (replace() == 0) {
      exit(0);
    } else {
      printf("Error in replace\n");
      exit(1);
    }
  } else if (strcmp(argv[1], "remove") == 0) {
    if (vowelRemover() == 0) {
      exit(0);
    } else {
      printf("Error in remove()\n");
      exit(1);
    }
  } else if (strcmp(argv[1], "len") == 0) {
    if (len() != 0) {
      printf("Eror in len fuction\n");
    } else {
      exit(0); }
  } else {
    printf("Uglydig kommando.\n");
  }
  if (freeAllocated() == 0) {
    printf("Great sucsess in freeAllocated()\n");
  } else { printf("Error in freeAllocated()\n");
  }
  freeAllocated();
  exit(0);
}
