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


typedef struct command_stream{
 struct command** root;
  int num_root;
  int index;
}command_stream;

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
  if (!a) {
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
}

typedef struct graphNode{  
command_t command;
struct graphNode* before[50];
int before_index;
pid_t pid;
} graphNode;


typedef struct outputAndnode {   
char* word;
struct graphNode* t;
int which_command;
} outputAndnode;

outputAndnode outputs[1000];
int output_count = 0;


struct queueNode
 {
    graphNode * data;    
    struct queueNode* next;
 };

struct queueNode* Dependency_front = NULL;
struct queueNode* Dependency_rear = NULL; 
struct queueNode* NoDependency_front = NULL;
struct queueNode* NoDependency_rear = NULL;

struct queueNode* delQueue(struct queueNode** front, struct queueNode** rear)
{
   if(*rear == NULL){
     printf("Can't delete cause queue is empty\n");
     return;
   }

   if(*front == *rear){
     *front = NULL;
   }

   struct queueNode* temp = *rear;
   *rear = (*rear)->next;

   return temp;
}

int queueSize(struct queueNode* front){
  int count = 0;
  if(front == NULL)
    return count;

  while(front != NULL){
    front = front->next;
    count++;
  }
  return count;
}

void push(struct queueNode** front, struct queueNode** rear, struct graphNode* value)
{
     struct queueNode *temp;
     temp=(struct queueNode *)malloc(sizeof(struct queueNode));
     temp->data=value;
     if (*front == NULL)
     {
           *front=temp;
           (*front)->next=NULL;
           *rear=*front;
     }
     else
     {
           (*front)->next=temp;
           (*front)=temp;
           (*front)->next=NULL;
     }
}


void print_queue(struct queueNode* rear){
  if(rear == NULL){
    printf("Can't print cause empty\n");
    return;
  }


  while(rear != NULL){
  InOrder(&(rear->data->command));
  printf("\n");
    rear = rear->next;
  }

}







bool findIO (command_t command, struct graphNode* n, const int which_c)
{
  bool dependent = false;
  bool subshell = false;
  if (command->type == SIMPLE_COMMAND)
     {
     int k;
     int j = 0;
        while(command->u.word[j] != NULL) {
          for (k = 0; k < output_count; k++) {
                if (strcmp(outputs[k].word, command->u.word[j]) == 0){
                    dependent = true;
                    n->before[n->before_index] = outputs[k].t;
                    n->before_index++;
                   }
              }
         j++;
        }
     if (command->output != NULL) {
         outputs[output_count].word = command->output;
         outputs[output_count].t = n;
         outputs[output_count].which_command = which_c;
         output_count++;
        }
    
     if (command->input != NULL) {
         int i;
         for (i = 0; i < output_count; i++) {
             if (strcmp(outputs[i].word, command->input) == 0 && outputs[i].which_command != which_c) {
                dependent = true;
                n->before[n->before_index] = outputs[i].t;
                n->before_index ++;
                }
             }
        }
 
     return dependent;
     } 

   if (command->type == SUBSHELL_COMMAND)
     return findIO(command->u.subshell_command, n, which_c);
   bool left = findIO(command->u.command[0], n, which_c);
   bool right = findIO(command->u.command[1], n, which_c);

   return left || right;
}




void build_parallel(command_stream_t stream)
{
  int i = 0;
  for (; i < stream->num_root; i++) 
      {
      graphNode* n = malloc(1*sizeof(graphNode));
      n->command = stream->root[i];
      n->before_index = 0;
      n->pid = -200;
      if (findIO(n->command, n, i))
         push(&Dependency_front, &Dependency_rear, n);  
      else
        push(&NoDependency_front, &NoDependency_rear, n);       
      }
     int j = 0;

}

void execute_parallel()
{
 int ndpid_array_size = 10;
 int num_ndpid = 0;
 int* ndpid_array = malloc(ndpid_array_size*sizeof(int));
 pid_t pid;




  while (queueSize(NoDependency_rear) > 0) 
    {
       struct queueNode* ptr= delQueue(&NoDependency_front, &NoDependency_rear);
       graphNode* temp = ptr->data;
       pid = fork();
       if (pid == 0)
          {
          execute_command(temp->command, 0);
          exit(1); 
          }
       else if (pid > 0){
          if (num_ndpid == ndpid_array_size){
            ndpid_array_size += 10;
            ndpid_array = realloc(ndpid_array, ndpid_array_size*sizeof(int));
          }
          ndpid_array[num_ndpid] = pid;
          temp->pid = pid;
          num_ndpid++;
          }
     }
  if(queueSize(Dependency_rear) == 0){
    int i;
    for(i = 0; i < num_ndpid; i++){
      waitpid(ndpid_array[i], 0, 0);
    }
    free(ndpid_array);
    return;
  }
 

int dpid_array_size = 10;
int num_dpid = 0;
int* dpid_array = malloc(dpid_array_size*sizeof(int));
bool isInvalid = false;


  while (queueSize(Dependency_rear) > 0)
        {
           struct queueNode* ptr = delQueue(&Dependency_front, &Dependency_rear);
           graphNode* temp = ptr->data;
           int i = 0;
           for (; i < temp->before_index; i++)
               {
                  if (temp->before[i]->pid == -200)
                     {
                     push(&Dependency_front, &Dependency_rear, temp);
                     isInvalid = true;
                     break;                      
                     }

                  if (waitpid(temp->before[i]->pid, 0, WNOHANG) == 0){
                     push(&Dependency_front, &Dependency_rear, temp);
                     isInvalid = true;
                     break;
                  }

               }
           if (isInvalid){
              isInvalid = false; 
              continue;
           }
           pid = fork();
           if (pid == 0)
              {
                execute_command(temp->command, 0);
                exit(1); 
              }
           else if (pid > 0){
                if (num_dpid == dpid_array_size){
                   dpid_array_size += 10;
                   dpid_array = realloc(dpid_array, dpid_array_size*sizeof(int));
                }
              dpid_array[num_dpid] = pid;
              temp->pid = pid;
              num_dpid++;
              }   
        }

    int j = 0;
    for(; j < num_dpid; j++){
      waitpid(dpid_array[j], 0, 0);
    }
    free(dpid_array);
  
    int q;
    for(q = 0; q < num_ndpid; q++){
      waitpid(ndpid_array[q], 0, 0);
    }
    free(ndpid_array);


}




