#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
Answers can be either in Portuguese or in English.
Respostas podem sem tanto em português como em inglês.
*/

/*
1. Term of commitment

The group members declare that all code developed for this project is their own.
The group members declare that they have not copied material from the Internet
  nor obtained code from third parties.

2. Group members and allocation of effort

Fill in the lines below with the name and email of the group members.
Replace XX with the contribution of each group member in the development of the work.

Mateus Faria Zaparoli Monteiro mateuszaparoli7@gmail.com 50%
Ricardo Shen ricardoshen88@gmail.com 50%

3. Solutions

Briefly describe the solutions implemented for this project and justify their choices.

Task 1 Solution:
    Here, we implemented a simple for command, if the fork fails it returns a number less than 0,
    so we print an error message and return -1, as it was asked in the task.
    Otherwise, we return the child process id as it was asked on the task too.

Task 2 Solution:
    Here, to execute simple commands, we choose to use the execvp function,
    wich change what the current process do so that it executes the command passed as parameter.
    As it mention in the execvp manual, if it returns, it means that an error ocurred,
    and if that happens we print an error message and exit with -1 as it was asked in the task.

Task 3 Solution:
    Here, we have to change the IO of the process to complete the task.
    So, first of all, we open the file passed as parameter using the mode that was given,
    than if the execution fails we print an error message and exit with -1.
    If not, we close the file descriptor that we want to redirect,
    after that we duplicate the file descriptor so that it takes the place of the one we just closed.
    Doing that, we do not use the input/output that was there before, but the one we opened.
    Finally, if the duplication fails we print an error message and exit with -1,
    otherwise we close the file descriptor that we opened and the task is complete.

Task 4 Solution:
    Here, we implemented pipe functionality for inter-process communication. We create a pipe
    with pipe() system call, getting read (p[0]) and write (p[1]) file descriptors. Then we
    fork two children: the first redirects stdout to the write end and runs the left command,
    the second redirects stdin to the read end and runs the right command. The parent closes
    both pipe ends and waits for both children to complete.

Task 5 Solution:
    The correction and explanation for this task are found inside the main function

4. Bibliographic references

Add the bibliographic references here.
Slide: Aula 05 - Processos no Unix. Chamadas de sistemas - SlidesArquivo
Fork functions: https://www.ibm.com/docs/en/zos/2.5.0?topic=functions-fork-create-new-process
Execvp manual: https://www.ionos.com/pt-br/digitalguide/sites-de-internet/desenvolvimento-web/execvp/
Exec manual: exec(3)-Library Functions Manual
Open command: https://br-c.org/doku.php?id=open
Dup command: https://man7.org/linux/man-pages/man2/dup.2.html
*/

/****************************************************************
 * Simplified xv6 Shell
 *
 * This code was adapted from the UNIX xv6 code and material from
 * the MIT Operating Systems course (6.828).
 ***************************************************************/

#define MAXARGS 10

/* Every command has a type. After identifying the command's type,
    the code converts a *cmd into the specific command type. */
struct cmd {
  int type; /* ' ' (exec)
               '|' (pipe)
               '<' or '>' (redirection) */
};

struct execcmd {
  int type;             // ' ' (exec)
  char *argv[MAXARGS];  // Arguments for the command to be executed
};

struct redircmd {
  int type;         // < or > (redirection)
  struct cmd *cmd;  // The command to execute (e.g., an execcmd)
  char *file;       // The input or output file
  int mode;         // The mode in which the file should be opened
  int fd;           // The file descriptor number to be used
};

struct pipecmd {
  int type;           // | (pipe)
  struct cmd *left;   // Left side of the pipe
  struct cmd *right;  // Right side of the pipe
};

int fork1(void);                                        // Fork but exit if an error occurs
struct cmd *parsecmd(char *);                           // Process the command line
void handle_simple_cmd(struct execcmd *ecmd);           // Handle simple commands
void handle_redirection(struct redircmd *rcmd);         // Handle redirection
void handle_pipe(struct pipecmd *pcmd, int *p, int r);  // Handle pipes

/* Execute the command cmd. It never returns. */
void runcmd(struct cmd *cmd) {
  int p[2], r;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if (cmd == 0)
    exit(0);

  switch (cmd->type) {
    default:
      fprintf(stderr, "Unknown command type\n");
      exit(-1);

    case ' ':
      ecmd = (struct execcmd *)cmd;
      if (ecmd->argv[0] == 0)
        exit(0);
      handle_simple_cmd(ecmd);
      break;

    case '>':
    case '<':
      rcmd = (struct redircmd *)cmd;
      handle_redirection(rcmd);
      runcmd(rcmd->cmd);
      break;

    case '|':
      pcmd = (struct pipecmd *)cmd;
      handle_pipe(pcmd, p, r);
      break;
  }
  exit(0);
}

int fork1(void) {
  /* Task 1: Implement the fork1 function.
  The function is supposed to create a new process using the `fork()` system call.
  It should print a message if the fork fails, otherwise return the process ID of the child process (or -1 if the fork fails).
  */
  int child_pid;

  if ((child_pid = fork()) < 0) {
    fprintf(stderr, "Fork creation failed\n");
    return -1;
  } else {
    return child_pid;
  }
  /* END OF TASK 1 */
}

void handle_simple_cmd(struct execcmd *ecmd) {
  /* Task 2: Implement the code below to execute simple commands. */
  execvp(ecmd->argv[0], ecmd->argv);
  fprintf(stderr, "Handle command: %s failed \n", ecmd->argv[0]);
  exit(-1);
  /* END OF TASK 2 */
}

void handle_redirection(struct redircmd *rcmd) {
  /* Task 3: Implement the code below to handle input/output redirection. */
  int fileDescriptor;

  fileDescriptor = open(rcmd->file, rcmd->mode, 0644);

  if (fileDescriptor < 0) {
    fprintf(stderr, "Error opening file: %s\n", rcmd->file);
    exit(-1);
  }

  close(rcmd->fd);

  if (dup(fileDescriptor) < 0) {
    fprintf(stderr, "Error duplicating file descriptor\n");
    exit(-1);
  }

  close(fileDescriptor);
  /* END OF TASK 3 */
}

void handle_pipe(struct pipecmd *pcmd, int *p, int r) {
  /* Task 4: Implement the code below to handle pipes. */
  if (pipe(p) < 0) {
    fprintf(stderr, "pipe failed\n");
    exit(-1);
  }

  if (fork1() == 0) {
    close(p[0]);
    dup2(p[1], 1);
    close(p[1]);
    runcmd(pcmd->left);
  }

  if (fork1() == 0) {
    close(p[1]);
    dup2(p[0], 0);
    close(p[0]);
    runcmd(pcmd->right);
  }

  close(p[0]);
  close(p[1]);
  wait(NULL);
  wait(NULL);
  /* END OF TASK 4 */
}

int getcmd(char *buf, int nbuf) {
  if (isatty(fileno(stdin)))
    fprintf(stdout, "$ ");
  memset(buf, 0, nbuf);
  fgets(buf, nbuf, stdin);
  if (buf[0] == 0)  // EOF
    return -1;
  return 0;
}

int main(void) {
  static char buf[100];
  int r;

  // Read and execute commands.
  while (getcmd(buf, sizeof(buf)) >= 0) {
    /* Task 5: Explain the purpose of the if statement below and correct the error message.
    Why is the current error message incorrect? Justify the new message. */
    /* Answer:
        The if condition below handles the cd command, used to change directory.
        When the input starts with "cd", chdir() is executed, changing the current directory.
        Therefore, the message "process does not exist" is wrong, because chdir() is related
        to directories and not processes.
     */
    if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
      buf[strlen(buf) - 1] = 0;
      if (chdir(buf + 3) < 0)
        fprintf(stderr, "directory does not exist\n");
      continue;
    }
    /* END OF TASK 5 */

    if (fork1() == 0)
      runcmd(parsecmd(buf));
    wait(&r);
  }
  exit(0);
}

/****************************************************************
 * Helper functions for creating command structures
 ***************************************************************/

struct cmd *
execcmd(void) {
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ' ';
  return (struct cmd *)cmd;
}

struct cmd *
redircmd(struct cmd *subcmd, char *file, int type) {
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->mode = (type == '<') ? O_RDONLY : O_WRONLY | O_CREAT | O_TRUNC;
  cmd->fd = (type == '<') ? 0 : 1;
  return (struct cmd *)cmd;
}

struct cmd *
pipecmd(struct cmd *left, struct cmd *right) {
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = '|';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd *)cmd;
}

/****************************************************************
 * Command Line Processing
 ***************************************************************/

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

int gettoken(char **ps, char *es, char **q, char **eq) {
  char *s;
  int ret;

  s = *ps;
  while (s < es && strchr(whitespace, *s))
    s++;
  if (q)
    *q = s;
  ret = *s;
  switch (*s) {
    case 0:
      break;
    case '|':
    case '<':
      s++;
      break;
    case '>':
      s++;
      break;
    default:
      ret = 'a';
      while (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
        s++;
      break;
  }
  if (eq)
    *eq = s;

  while (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int peek(char **ps, char *es, char *toks) {
  char *s = *ps;
  while (s < es && strchr(whitespace, *s)) s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char **, char *);
struct cmd *parsepipe(char **, char *);
struct cmd *parseexec(char **, char *);

/* Copy characters from the input buffer, starting from s to es.
 * Place a null terminator at the end to create a valid string. */
char *mkcopy(char *s, char *es) {
  int n = es - s;
  char *c = malloc(n + 1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd *
parsecmd(char *s) {
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if (s != es) {
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd *
parseline(char **ps, char *es) {
  struct cmd *cmd;
  cmd = parsepipe(ps, es);
  return cmd;
}

struct cmd *
parsepipe(char **ps, char *es) {
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd *
parseredirs(struct cmd *cmd, char **ps, char *es) {
  int tok;
  char *q, *eq;

  while (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    if (gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(stderr, "missing file for redirection\n");
      exit(-1);
    }
    switch (tok) {
      case '<':
        cmd = redircmd(cmd, mkcopy(q, eq), '<');
        break;
      case '>':
        cmd = redircmd(cmd, mkcopy(q, eq), '>');
        break;
    }
  }
  return cmd;
}

struct cmd *
parseexec(char **ps, char *es) {
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  ret = execcmd();
  cmd = (struct execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while (!peek(ps, es, "|")) {
    if ((tok = gettoken(ps, es, &q, &eq)) == 0)
      break;
    if (tok != 'a') {
      fprintf(stderr, "syntax error\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);
    argc++;
    if (argc >= MAXARGS) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  return ret;
}