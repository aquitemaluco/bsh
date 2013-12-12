#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#include "bsh.h"

typedef int PIPE_T[2];

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	int i, count = 0;
	
	char *line;
	char **progs;

	line  = (char *) malloc(sizeof(char));
	printf("Bem bindo ao mini-shell.\n");
	do {
		printf("cmd> ");
		bsh_read_line(line);
		if (! strcmp("exit", line)) exit(0); //Saida a pedido do usuario 
		
		progs = explode('|', line, &count);

		PIPE_T ppipes[count];
		for (i=0; i < count; i++) {
			pipe(ppipes[i]);
		}

		for (i=0; i < (count+1); i++) {
			pid = fork();
			if (pid < 0) exit(1); //condicao de erro aborta execucao!
			else if(pid == 0) {
				/** Processo filho */
				char **pfile;
				int fcount = 0, parcount = 0;
				int fd, fopened;
				
				//Tubos e conexoes "Tigre"/"Amanco"! :)
				if (count) {
					if (i == (count+1)) {
						close(ppipes[i][1]); //stdout ultimo processo
					} else {
						dup2(ppipes[i][1], 1); //stdout
					}
					
					if (i == 0) {
						close(ppipes[i][0]); //stdin primeiro processo
					} else {
						dup2(ppipes[i-1][0], 0); //stdin
					}
					
					int n;
					for (n=0; n<count; n++) {
						//Fecha copia do pipe do processo filho
						close(ppipes[n][0]);
						close(ppipes[n][1]);
					}
				}
				
				//Verifica se tem redirecionamento para arquivo.
				pfile = explode('>', trim(progs[i]), &fcount);
				if (fcount) {
					//Abre arquivo e redireciona saida padrao
					if ((fd = open(trim(pfile[1]), O_CREAT|O_WRONLY|O_TRUNC,
							0666)) == -1) {
						perror("Error na abertura de saida") ;
						exit(1) ;
					}
					
					fopened = 1;
					dup2(fd, 1); // 1 - saida padrao
				} else {
					pfile = explode('<', trim(progs[i]), &fcount);
					if (fcount) {
						//Abre arquivo e redireciona entrada padrao
						int fd;
						if ((fd = open(trim(pfile[1]), O_CREAT|O_RDONLY,
								0666)) == -1) {
							perror("Error na abertura de entrada") ;
							exit(1) ;
						}
						
						fopened = 1;
						dup2(fd, 0); // 0 - entrada padrao
					} else {
						pfile[0] = progs[i];
					}
				}
				
				char **plist = explode(' ', trim(pfile[0]), &parcount);
				execv(plist[0], plist);
				fflush(stdout);
				
				//Limpeza
				free(pfile);
				free(plist);
				if (fopened) {
					close(fd);
				}
				
				//Saida normal do processo filho!
				exit(0);
			}
			wait(&status);
			close(ppipes[i][1]);
		}
	} while(1);
}

void bsh_read_line(char *line)
{
	char buff[BUFF_SIZE];

	line[0] = '\0';
	int sairloop = 0;
	while((! sairloop) && fgets(buff, BUFF_SIZE, stdin)) {
		if (buff[strlen(buff)-1] == '\n') {
			sairloop = 1;
		}

		line = (char *) realloc(line, sizeof(char)*(strlen(line) + BUFF_SIZE));
		strcat(line, buff);
	}
	line[strlen(line)-1] = '\0';
}

char ** explode (char sep, char *str, int *pcount)
{
 	char **arr_str = (char**) malloc(0);
 	int count = 0;
 	char *cp = str;
 	char *apos = str;

 	while ((cp = strchr(cp, sep)) != NULL) {
		count++;

		arr_str = (char **)realloc(arr_str, count*sizeof(char*));
		arr_str[count-1] = (char *)realloc(arr_str[count-1], cp - apos);
		strncpy(arr_str[count-1], apos, cp - apos);

		cp +=sizeof(char);
		apos = cp;
 	}

 	arr_str = (char **)realloc(arr_str, (count+1)*sizeof(char*));
 	arr_str[count] = (char *)realloc(arr_str[count], &str[strlen(str)] - apos);
 	strncpy(arr_str[count], apos, &str[strlen(str)] - apos);

	*pcount = count;
 	return arr_str;
}

char *trim (char *s)
{
	int i = 0;
	int j = strlen ( s ) - 1;
	int k = 0;
	while ( isspace ( s[i] ) && s[i] != '\0' )
		i++;
	
	while ( isspace ( s[j] ) && j >= 0 )
		j--;

	while ( i <= j )
		s[k++] = s[i++];

	s[k] = '\0';

	return s;
}
