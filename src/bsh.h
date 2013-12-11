#ifndef BSH_H
#define BSH_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BUFF_SIZE	32
#define BUFF_MAX	256

void bsh_read_line(char *line);
char ** explode (char sep, char *str, int *pcount);
char *trim ( char *s );

#ifdef	__cplusplus
}
#endif

#endif


