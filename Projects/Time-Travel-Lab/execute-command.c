// UCLA CS 111 Lab 1 command execution

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "command-internals.h"
#include <string.h>
#include "command.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>

int execute_simple_command(command_t command)
{
  int status;
  pid_t pid = fork();
  if (pid == 0){
       if (command->input == NULL && command->output != NULL) {
          int fd = open(command->output, O_TRUNC | O_CREAT | O_WRONLY, 0644);
          if (fd < 0)
             exit(1);
         dup2(fd, 1);
         close(fd);
         }
   else  if (command->input != NULL && command->output != NULL)
           {
            int df = open(command->input, O_RDONLY);
            int fd = open(command->output, O_TRUNC | O_CREAT | O_WRONLY, 0644);
            if ((df < 0) || (fd < 0))
                exit(1);
            dup2(df, 0);
            dup2(fd, 1);
            close(fd);
            close(df);
          } 
    else if (command->input != NULL && command->output == NULL) {
           int df = open(command->input, O_RDONLY);
           if (df < 0)
              exit(1);
           dup2(df, 0);
           close(df);
     }
     execvp(command->u.word[0], command->u.word);
     _Exit(-1);
  }
  waitpid(pid, &status, 0);
  return WEXITSTATUS(status);
}



int execute_pipe_command(command_t command)
{
   command_t left = command->u.command[0];
   command_t right = command->u.command[1];
   int pipefd[2];
   pid_t pid;
   pid_t pid2;
   pipe(pipefd);
   pid = fork();
   int status; 
   if (pid == 0) {
      dup2(pipefd[1], 1);
      close(pipefd[0]);
      status = execute_command(left, 0);
      exit(status);
    }
   else {
      dup2(pipefd[0], 0);
      close(pipefd[1]);
      status = execute_command(right, 0);
    }
 return status;
}


int execute_or_command(command_t command)
{
   int status;
   command_t left = command->u.command[0];
   command_t right = command->u.command[1];
   status = execute_command(left, 0);
   if (status != 0)
      {
      status = execute_command(right, 0);
      }
  return status;
}

int execute_and_command(command_t command)
{
   int status;
   command_t left = command->u.command[0];
   command_t right = command->u.command[1];
   status = execute_command(left, 0);
   if (status == 0)
      {
      status = execute_command(right, 0);
      }
  return status;
}

int execute_sequence_command(command_t command)
{
   int status;
   command_t left = command->u.command[0];
   command_t right = command->u.command[1];
   status = execute_command(left, 0);
   status = execute_command(right, 0);
   return status;
}





int
command_status (command_t c)
{
  return c->status;
}
int
execute_command (command_t c, bool a)
{
  switch(c->type){
    case AND_COMMAND:
      return execute_and_command(c);
      break;
    case OR_COMMAND:
      return execute_or_command(c);
      break;
    case SEQUENCE_COMMAND:
      return execute_sequence_command(c);
      break;
    case PIPE_COMMAND:
      return execute_pipe_command(c);
      break;
    case SIMPLE_COMMAND:
      return execute_simple_command(c);
      break;
    case SUBSHELL_COMMAND:
      return execute_command(c->u.subshell_command, 0);
      break;
  }
}
