#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "safefork.h"
#include "history.h"

#define USER getenv("USER")
#define PATH getenv("PATH")
#define BUFFERSIZE 128
int i = 0;
int j = 0;
int run_in_background = 0;
int cmd_counter = 0;

/**
 * Promt funksjon
 * Denne funksjonen printer ut en kommando promt sammen med antall kommandoer.
 */
void promt() {
  printf("%s@ifish %d> ", USER, cmd_counter);
  cmd_counter++;
}

/**
 * Denne funksjonen splitter linja som blir tastet inn i kommando løkka.
 * Den splitter først for newline for å fjerne \n på slutten av strengen som blir inkludert
 * med fgets. Deretter splitter jeg med mellomrom og legger den splitta linja inn i param
 * array og returnerer peker til param array.
 */
char** strsplit(char* in) {
  char* line = in;
  char* nl_removed = strtok(line, "\n");
  char* string;
  char** param = malloc(20*sizeof(char*));
  i = 0;
  run_in_background = 0;
  for (string = strtok(nl_removed, " "); string != NULL; string = strtok(NULL, " ")) {
    if (strcmp(string, "&") != 0) {
      param[i++] = string;
    } else {
      run_in_background = 1;
      break;
    }
  }
  param[i] = NULL;
  return param;
}

/**
 * Denne funksjonen splitter PATH.
 * Bruker samme metode som i strsplit funksjonen. Jeg har mallocert med 20 i path_plitted array,
 * det er mer enn nødvendig, fordi PATH har 8 forskjellige path. Funksjonen returnerer peker til
 * path_splitted array som inneholder PATH ferdig splittet.
 */
char** pathSplit() {
  char* path = strdup(getenv("PATH"));
  char* string;
  char** path_splitted = malloc(20*sizeof(char*));
  j = 0;
  for (string = strtok(path, ":"); string != NULL; string = strtok(NULL, ":")) {
    path_splitted[j++] = string;
  }
  path_splitted[j] = NULL;
  free(path);
  return path_splitted;
}

/**
 * Denne funksjonen returnerer en path hvis kommando finnes, og NULL hvis den ikke finnes.
 * Funksjonen konstruerer en path som er "path_splitted[k]/param[0]". Så sjekker den om
 * de kommandoen på den pathen er en vanlig fil og kan kjørbar. Hvis kommandoen finnes,
 * og er kjørbar returnerer den path til den kjørbare kommandoen. Hvis den ikke finnes,
 * eller er kjørbar returnerer den NULL.
 */
char* ex_path(char** param) {
  char** path_splitted = pathSplit();
  char* full_path = NULL;
  struct stat sb;
  int k = 0;

  if (path_splitted == NULL) {
    printf("exit\n");
    exit(1);
  }
  if (param == NULL) {
    return NULL;
  }
  for (k = 0; k<j; k++) {
    full_path = malloc(strlen(path_splitted[k]) + strlen(param[0]) + 2);
    full_path[0] = 0;
    strcat(full_path, path_splitted[k]);
    strcat(full_path, "/");
    strcat(full_path, param[0]);

    if (stat(full_path, &sb) < 0 || !S_ISREG(sb.st_mode)) {
      free(full_path);
      full_path = NULL;
      continue;
    }

    if (access(full_path, F_OK|X_OK) != 0) {
      free(full_path);
      full_path = NULL;
      continue;
    }
    break;
  }
  if (path_splitted != NULL) {
    free(path_splitted);
  }
  return full_path;
}

/**
 * Denne funksjonen kjører kommandoer.
 * Funksjonen henter riktig path fra ex_path funksjonen hvis den finnes. Hvis den ikke finnes skal det
 * skrives en feilmelding. Denne feilmeldingen blir printet ut to ganger, jeg vet ikke hvorfor.
 * Hvis kommandoen finnes starter den en process med execve som kjører kommandoen.
 */
int startcmd(char** param) {
  pid_t pid;
  int status;
  char* path = ex_path(param);


  if (path == NULL) {
    printf("ifish: command not found\n");
    free(path);
    return 1;
  }
  
  pid = safefork();
  if (pid == -1) {
    free(path);
    return 1;
  } else if (pid > 0) {

    if(run_in_background == 1) {
      printf("Child process pid: %i\n", pid);
    } else {
      waitpid(pid, &status, 0);
    }
    free(path);
    return 1;
  }
  else {
    execve(path, param, __environ);
    free(path);
  }
  free(path);
  return 0;
}

/**
 * Main
 * En while løkke som kjører promt og fgets. Param henter splitta parameter fra strsplit funksjon,
 * og sender den inn i startcmd funksjon.
 */
int main(int argc, char* argv[]) {
  char buffer[BUFFERSIZE];
  char** param;
  int j = 0;
  history_init();
  while (promt(), fgets(buffer, BUFFERSIZE, stdin)) {
    if ((strcmp(buffer, "quit\n") == 0) || (strcmp(buffer, "exit\n") == 0)) {
      exit(0);
    }

    param = strsplit(buffer);
    char* tmp_s = buffer;
    char* f = strtok(tmp_s, "\n");
    if (f == NULL) {
      continue;
    }
    char* cmd_s = malloc(sizeof(char)*strlen(f)+1);
    strcpy(cmd_s, f);

    new_metadatablokk(buffer, cmd_counter);
    char** tmp = builtin_cmd(param);
    param = tmp;

    if (i != 0) {
#ifdef DEBUG
      printf("%d\n", i);
#endif
      for (j = 0; j<i; j++) {
#ifdef DEBUG

	//printf("%s\n", param[j]); //dette forårsaker seg fault i DEBUG
#endif

      }
      startcmd(param);
      free(param);
      free(cmd_s);
    }
  }
  return 0;
}
