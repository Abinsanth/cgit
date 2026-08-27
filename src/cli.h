#ifndef CGIT_CLI_H
#define CGIT_CLI_H

int cli_run(int argc, char *argv[]);
int cli_version(void);
int cli_help(void);
int cli_error(const char *message);
int cli_init(void);
int cli_add(const char *path);

#endif