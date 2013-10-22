// UCLA CS 111 Lab 1 command reading
#include <ctype.h>
#include "command.h"
#include "command-internals.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <error.h>
#include<string.h>




void freeTree(struct command* t)  
{
    if (t == NULL)
       return;
    if(t->type == SIMPLE_COMMAND)
    {
        if(t->input != NULL){
            free(t->input);
        }
        if(t->output != NULL){
            free(t->output);
        }
        if(t->u.word != NULL) {
            int i;
            for(i = 0; t->u.word[i] != NULL; i++){
                free(t->u.word[i]);
            }
            free(t->u.word);
        }
        free(t);   
       
        return;
    }
    if(t->type == SUBSHELL_COMMAND){
      freeTree(t->u.subshell_command);
    }
    else{
      freeTree(t->u.command[0]);
      freeTree(t->u.command[1]);
    }
    free(t);
}





typedef struct {
  char** command;
  int command_size;
}Complete_command;



int complete_command_array_size = 7;
int num_complete_command = 0;
Complete_command* a;
int lines = 1;


bool isNorm(int c) {
  if(isalnum(c) || c == '!' || c == '%' ||
       c == '+' || c == ',' || c == '-' ||
       c == '.' || c == '/' || c == ':' ||
       c == '@' || c == '^' || c == '_' )
    {
    return true;
    }
  else
    return false;
}

void printIt (enum command_type a)
{
   if (a == AND_COMMAND)
      printf("[&&] ");
   if (a == OR_COMMAND)
      printf("[||] ");
   if (a == PIPE_COMMAND)
      printf("[|] ");
   if (a == SEQUENCE_COMMAND)
      printf("[;] ");
}

enum command_type id_type(char** command) {
  int i;
  for(i=0; (*command)[i] != '\0'; i++) {
    char c = (*command)[i];
    if(isNorm((int) c)){
      return SIMPLE_COMMAND;
    }
    else if(c == '&') {
      return AND_COMMAND;
    }
    else if (c == ';') {
      return SEQUENCE_COMMAND;
    }
    else if(c == '|') {
      if((*command)[i+1] == '|')
        return OR_COMMAND;
      else
        return PIPE_COMMAND;
    }
  }
}





bool lowerPrecedence(enum command_type a, enum command_type b) {
   if ((a == PIPE_COMMAND) || (b == SEQUENCE_COMMAND))
      return true;
   if (((b == AND_COMMAND) || (b == OR_COMMAND)) && (a != SEQUENCE_COMMAND))
      return true;
   return false;
}

bool isOperator(enum command_type a){
  return  ((a == AND_COMMAND) || (a == SEQUENCE_COMMAND) || (a == OR_COMMAND) ||  (a == PIPE_COMMAND));
}


int findLeastRightPrecedence(Complete_command *c, int start, int end)
{
   int i;
   int indexLRP = -1;
   int num_leftPar = 0;
   int num_rightPar = 0;
   bool isInPar = false;
   enum command_type LRP;
   for (i = start; i < end; i++)
       {
          if (strcmp(c->command[i], "(") == 0){
               num_leftPar ++;
               isInPar = true;
             }
          if (strcmp(c->command[i], ")") == 0){
               num_rightPar ++;
               if (num_leftPar == num_rightPar)
                  isInPar = false;
               continue;
             }
          if (isOperator(id_type(&(c->command[i]))) && (!isInPar)) {
               if (indexLRP == -1){
                   LRP = id_type(&(c->command[i]));
                   indexLRP = i;
                 }
               else {
                    if (lowerPrecedence(LRP, id_type(&(c->command[i])))) {
                        LRP = id_type(&(c->command[i]));
                        indexLRP = i;
                      }
                    }
             }
       }
    return indexLRP;
 }


int numRedirect (char** command)
{
  int total = 0;
  char* i;
  for (i = *command; *i != '\0'; i++)
      {
         if ((*i == '>') || (*i == '<'))
            total ++;
      }
  return total;
}

void free_space(Complete_command** arr, int num_complete_command) {

  int i;
  int j;
  for( i=0; i < num_complete_command; i++){
    int word_size = (*arr)[i].command_size;
    for( j=0; j < word_size; j++){
      free((*arr)[i].command[j]);
    }
    free((*arr)[i].command);
  }
  free((*arr));
}



void errorAndLeave(int lines, Complete_command** arr, int num_complete_command)
{
  fprintf(stderr,"%d:", lines);
  free_space(arr, num_complete_command);
  exit(1);
}

char** parseTemp(char* temp){

  int num_words = 0;
  int words_array_size = 2;
  int num_char = 0;
  int char_array_size = 5;


  char** words = malloc(words_array_size*sizeof(char*));
  words[num_words] = malloc(char_array_size*sizeof(char));

  int i;
  for(i = 0; i < strlen(temp); i++){
    switch(temp[i]){
      case ' ':
        if((i+1) == strlen(temp))
          continue;
        if(num_words == words_array_size - 1){
          words_array_size += 2;
          words = realloc(words, words_array_size*sizeof(char*));
        }
        words[num_words][num_char] = '\0';
        num_words++;
        num_char = 0;
        char_array_size = 5;
        words[num_words] = malloc(char_array_size*sizeof(char));
        break;
      default:
        if(num_char == char_array_size - 1){
          char_array_size += 5;
          words[num_words] = realloc(words[num_words], char_array_size*sizeof(char));
        }
        words[num_words][num_char] = temp[i];
        num_char++;
        break;
     }
  }
  words_array_size++;
  words = realloc(words, words_array_size*sizeof(char*));
  words[num_words][num_char] = '\0';
  words[++num_words] = NULL;
  return words;
}


struct command* buildRedirect(char** c){
  char* i;
  bool isInput = false;
  bool firstSeen = false;
  bool outSeen = false;
  int num_temp = 0;
  int num_temp_size = 5;
  char* temp = malloc(num_temp_size*sizeof(char)); 
  struct command* comm = malloc(1*sizeof(struct command));
  comm->type = SIMPLE_COMMAND;
  comm->input = NULL;
  comm->output = NULL;
  
  for(i = *c; *i != '\0'; i++) {
    if ((*i == ' ') && (num_temp == 0))
       continue;
    if(isNorm(*i) || *i == ' ') {
       if((num_temp == num_temp_size - 1)) {
         num_temp_size += 5;
         temp = realloc(temp, num_temp_size*sizeof(char));
       }
       temp[num_temp] = *i;
       num_temp++;
    }
    if(*i == '<') {
      if(outSeen || firstSeen){
         errorAndLeave(lines, &a, num_complete_command);
      }
      if (temp[num_temp - 1] == ' ') 
         temp[num_temp - 1] = '\0';
      else
          temp[num_temp] = '\0';
      //comm->u.word = malloc(2*sizeof(char*));
      //comm->u.word[0] = temp;
      //comm->u.word[1] = NULL;
      comm->u.word = parseTemp(temp);
      num_temp = 0;
      num_temp_size = 5;
      char* temp1 = temp;
      temp = malloc(num_temp_size*sizeof(char));
      free(temp1);
      isInput = true;
      firstSeen = true;
    }
    else if(*i == '>'){
      if(outSeen){
        errorAndLeave(lines, &a, num_complete_command);
      }
      if (temp[num_temp - 1] == ' ')
         temp[num_temp - 1] = '\0';
      else
         temp[num_temp] = '\0';
      if(firstSeen){
        comm->input = temp;
      }
      else{
        comm->u.word = parseTemp(temp);
        //comm->u.word = malloc(2*sizeof(char*));
        //comm->u.word[0] = temp;
        //comm->u.word[1] = NULL;
      }
      outSeen = true;
      isInput = false;
      num_temp = 0;
      num_temp_size = 5;
      if(!firstSeen){
        char* temp1 = temp;
        temp = malloc(num_temp_size*sizeof(char));
        free(temp1);
      }
      else
        temp = malloc(num_temp_size*sizeof(char));
    }
  } 
 
 if(num_temp == num_temp_size){
    num_temp_size++;
    temp = realloc(temp,num_temp_size*sizeof(char));
  }

if (temp[num_temp - 1] == ' ')
    temp[num_temp - 1] = '\0';
else
    temp[num_temp] = '\0';
  
  if(isInput) {
    comm->input = temp;
    comm->output = NULL;
    comm->status = 123;
  }
  else if(firstSeen && !isInput){
    comm->output = temp;
    comm->status = 100;
  }
  else{
    comm->output = temp;
    comm->input = NULL;
    comm->status = 321;
  }
  return comm;
}

void parseSimpleCommand(struct command* node, Complete_command *c, int n){

  int num_words = 0;
  int words_array_size = 2;
  int num_chars = 0;
  int char_array_size = 5;
  node->u.word = malloc(words_array_size*sizeof(char*));
  node->u.word[num_words] = malloc(char_array_size*sizeof(char));

  int i;
  for(i=0; i < strlen(c->command[n]); i++){
    switch(((c->command[n])[i])) {

     case ' ':
        if(num_words == words_array_size - 1){
          words_array_size += 2;
          node->u.word = realloc(node->u.word, words_array_size*sizeof(char*));
        }
          (node->u.word[num_words])[num_chars] = '\0';
          num_words++;
          num_chars = 0;
          char_array_size = 5;
          node->u.word[num_words] = malloc(char_array_size*sizeof(char));
        break;

      default:
        if(num_chars == char_array_size - 1){
          char_array_size += 5;
          node->u.word[num_words] = realloc(node->u.word[num_words], char_array_size*sizeof(char));
        }
        (node->u.word[num_words])[num_chars] = ((c->command[n])[i]);
        num_chars++;
        break;

    }
  }
  words_array_size++;
  node->u.word = realloc(node->u.word, words_array_size*sizeof(char*));
  (node->u.word[num_words])[num_chars] = '\0';
  node->u.word[++num_words] = NULL;
}


struct command* makeTree(Complete_command *c, int start, int end)
{
   int LRP;
   if (start == end) {
      if (numRedirect(&((*c).command[start])) > 0) {
         if (numRedirect(&((*c).command[start])) > 2)
            errorAndLeave(lines, &a, num_complete_command);
         else
            return buildRedirect(&((*c).command[start]));
      }
      struct command* node;
      node = malloc(1*sizeof(struct command));
      node->type = SIMPLE_COMMAND;
      parseSimpleCommand(node, c, start);
      //node->u.word = malloc(2*sizeof(char*));
      //node->u.word[0] = malloc((strlen((*c).command[start])+1)*sizeof(char));
      //strcpy((node->u.word[0]), ((*c).command[start]));
      //node->u.word[1] = NULL;
      node->output = NULL;
      node->input = NULL;
      return node;
      }
   LRP = findLeastRightPrecedence(c, start, end);

   if (LRP == -1) {
      struct command *node = malloc(1*sizeof(struct command));
      node->type = SUBSHELL_COMMAND;
      if (start - end == 1) {
         struct command* node1;
         node1 = malloc(1*sizeof(struct command));
         node1->type = SIMPLE_COMMAND;
         node1->u.word = malloc(2*sizeof(char*));
         node1->u.word[0] = "";
         node1->u.word[1] = NULL;
         node->output = NULL;
         node->input = NULL;
         node->u.subshell_command = node1;
         return node;
       }
      else {
      node->u.subshell_command = makeTree(c, start + 1, end - 1);
      return node;
      }
   }
   struct command* node;
   node = malloc(1*sizeof(struct command));
   node->type = id_type(&(c->command[LRP]));
   node->u.command[0] = makeTree(c, start, LRP - 1);
   node->u.command[1] = makeTree(c, LRP + 1, end);
   node->output = NULL;
   node->input = NULL;
   return node;
}     

void InOrder (struct command** curr)
{
  if((*curr)->type == SIMPLE_COMMAND) {
    if ((*curr)->status == 123) {
       printf("[%s<", (*curr)->u.word[0]);
       printf("%s] ", (*curr)->input);
       return;
     }
    if ((*curr)->status == 321) {
       printf("[%s>", (*curr)->u.word[0]);
       printf("%s] ", (*curr)->output);
       return;
     }
    if ((*curr)->status == 100) {
       printf("[%s<%s>%s] ", (*curr)->u.word[0], (*curr)->input, (*curr)->output);
       return;
     }
     printf("[%s] ", (*curr)->u.word[0]);
     return;
  }
  
  if ((*curr)->type == SUBSHELL_COMMAND) {
     printf("[( ");
     InOrder(&(*curr)->u.subshell_command);
     printf(")] ");
     return;
     }

  InOrder(&(*curr)->u.command[0]);
  printIt((*curr)->type);
  InOrder(&(*curr)->u.command[1]);
   
}


                 






int stringLength(char* arr) {
  int length = 0;
  char* i;
  for(i=arr; *i != '\0'; i++) {
    length++;
  }
  return length;
}

void copy_command(char*** arr, char** command, int num_command,int num_words) {
  ((*arr)[num_words]) = malloc(num_command*sizeof(char)); 
  int i;
  for(i=0; i < num_command; i++){
    (((*arr)[num_words])[i]) = ((*command)[i]);
  }
}

void put_command(char*** arr, char** command, int num_words, int* num_command, int* command_array_size) {
  if(*num_command == *command_array_size){
    (*command_array_size)++;
    (*command) = realloc((*command), (*command_array_size)*sizeof(char));
  }
  (*command)[*num_command] = '\0';
  (*num_command)++;
  ((*arr)[num_words]) = *command;
  *command_array_size = 100;
  *command = malloc((*command_array_size)*sizeof(char));

}


void put_words(Complete_command** arr, char*** words, int* num_words, int* num_complete_command, int* words_array_size){

  ((*arr)[*num_complete_command]).command = *words;
  ((*arr)[*num_complete_command]).command_size = *num_words;
  *words_array_size = 1;
  *words = malloc((*words_array_size)*sizeof(char*));
  (*num_complete_command)++;
  *num_words = 0; 

}


void free_words(char*** arr, int num_words) { 
  int i;
  for(i=0; i < num_words; i++) {
    free((*arr)[i]);
  }
  free((*arr));
}


void reset_command(char** command, int* num_command, int* num_words, int currentByte){
  
  *num_command = 0;
  (*num_words)++;
  ((*command)[*num_command]) = currentByte;
  (*num_command)++;

}

void store_word(Complete_command** arr, Complete_command comm, int num_complete) {

  ((*arr)[num_complete]).command = malloc(comm.command_size*sizeof(char*));
  char** temp = ((*arr[num_complete])).command;
  ((*arr[num_complete])).command_size = comm.command_size;

  int i;
  for(i = 0; i < comm.command_size; i++){
    copy_command(&temp, &(comm.command[i]), stringLength(comm.command[i]), i);

  }
}



typedef struct command_stream{
 struct command** root;
  int num_root; 
  int index;
}command_stream;



command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
int num_words = 0;
int num_command = 0;
int command_array_size = 100;
int complete_command_array_size = 7;
int words_array_size = 1;
char** words = malloc(words_array_size*sizeof(char*));
char* command = malloc(command_array_size*sizeof(char));
int leftParenthesis = 0;
int rightParenthesis = 0;
int prevByte = ')';
//command_stream *tree = NULL:
command_stream_t tree = NULL;
a = malloc(complete_command_array_size*sizeof(Complete_command));

bool comment_complete_command = false;
bool singleAnd = false;
bool space_seen = false;
bool backslashNewline = false;
bool justNewline = false;
bool prevNecessarySlash = false;
int currentByte;
/*
for (currentByte = get_next_byte(get_next_byte_argument);
                        currentByte != EOF; ) {
   putchar(currentByte);
  }

 while ((currentByte = get_next_byte(get_next_byte_argument)) != EOF)
    putchar(currentByte);
*/

   while ((currentByte = get_next_byte(get_next_byte_argument)) != EOF)  {
      if (rightParenthesis > leftParenthesis)
        errorAndLeave(lines, &a, num_complete_command);
      prevByte = currentByte; 
      switch (currentByte) {
          case '`':
              errorAndLeave(lines, &a, num_complete_command);
          case ';':
          if ((num_command == 0) && (num_words == 0))
             errorAndLeave(lines, &a, num_complete_command);
           if (num_command > 0){
               if ((command[num_command - 1] == '>') || (command[num_command - 1] == '<') || (command[num_command - 1] == '&') || (command[num_command - 1] == '|')) {
                 errorAndLeave(lines, &a, num_complete_command);
                    }
                }
           if ((num_words > 0) && (num_command == 0)) {
                  if ((strcmp(words[num_words -1],"||") == 0) || (strcmp(words[num_words -1],"(") == 0) || (strcmp(words[num_words -1],"|") == 0) || (strcmp(words[num_words -1],"&&") == 0) || (strcmp(words[num_words -1],";") == 0)){
                       errorAndLeave(lines, &a, num_complete_command);
                    }
               } 
               if (num_command > 0)
                {
                   if (words_array_size == num_words){
                       words_array_size += 5;
                       words = realloc(words, (words_array_size*sizeof(char*)));
                      }
                   put_command(&words, &command, num_words, &num_command, &command_array_size);
                   num_command = 0;
                   num_words ++;
                }
                   if(command_array_size == num_command){
                       command_array_size += 5;
                       command = realloc(command, (command_array_size*sizeof(char)));
                     }
                
                   command[num_command] = currentByte;
                   num_command ++;
                   if (words_array_size == num_words){
                       words_array_size += 5;
                       words = realloc(words, (words_array_size*sizeof(char*)));
                     }
                   put_command(&words, &command, num_words, &num_command, &command_array_size);
                   num_command = 0;
                   num_words ++;
                   space_seen = false;
                   break;
          case '&':
               singleAnd = true;
               if (justNewline)                 {

                    errorAndLeave(lines, &a, num_complete_command);
               }    
               if (backslashNewline)
                 {
                   command[num_command] = currentByte;
                   num_command ++;
                   backslashNewline = false;
                   break;
                 }
              if (((num_command > 0) && isNorm(command[num_command - 1])) || ((num_command == 0) && (num_words > 0) && strcmp(words[num_words - 1], ")") == 0)) 
                 {
                   if (num_command > 0) { 
                    if (words_array_size == num_words)
                       {
                          words_array_size += 5;
                          words = realloc(words, (words_array_size*sizeof(char*)));
                       }
                     put_command(&words, &command, num_words, &num_command, &command_array_size);
                     num_command = 0;
                     num_words++;
                  }
                     space_seen = false;
                     command[num_command] = currentByte;
                     num_command++;

                     break;
                 }
               if ((num_command > 0) && (command[num_command - 1] == '&'))
                  {
                     singleAnd = false;
                     if(command_array_size == num_command){
                       command_array_size += 5;
                       command = realloc(command, (command_array_size*sizeof(char)));
                     }

                     if (words_array_size == num_words){
                          words_array_size += 5;
                          words = realloc(words, (words_array_size*sizeof(char*)));
                     }

                     command[num_command] = currentByte;
                     num_command++;
                     put_command(&words, &command, num_words, &num_command, &command_array_size);
                     space_seen = false;

                     num_command = 0;
                     num_words++;
                     space_seen = false;

                     break;
                
                }
             if ((num_command > 0) && (num_words > 0))
             {
                   
                   if ((strcmp(words[num_words -1],"||") == 0) || (command[num_command - 1] == '>') || (command[num_command - 1] == '<') || (command[num_command - 1] == '|') || (strcmp(words[num_words -1],"|") == 0) || (strcmp(words[num_words -1],"&&") == 0))
                  {
                  
                      errorAndLeave(lines, &a, num_complete_command);
                  }
             }
             errorAndLeave(lines, &a, num_complete_command);
          case '|':  
               if (justNewline)
               {

                  errorAndLeave(lines, &a, num_complete_command);
               }
              if (backslashNewline)
                 {
                   command[num_command] = currentByte;
                   num_command ++;
                   backslashNewline = false;
                   break;
                 }
              if (((num_command > 0) && isNorm(command[num_command - 1])) || ((num_command == 0) && (num_words > 0) && strcmp(words[num_words - 1], ")") == 0))  
                 {  
                    if (num_command > 0){
                       if (words_array_size == num_words)                            {
                             words_array_size += 5;
                             words = realloc(words, (words_array_size*sizeof(char*)));
                          }
                        put_command(&words, &command, num_words, &num_command, &command_array_size);
                        num_command = 0;
                        num_words++;
                 }
                     space_seen = false;
                     command[num_command] = currentByte;
                     num_command++;

                     break;
                  }
               if ((num_command > 0) && command[num_command - 1] == '|')
                  {  
		     if(command_array_size == num_command){  
                       command_array_size += 5;
                       command = realloc(command, (command_array_size*sizeof(char)));
                     }

                     if (words_array_size == num_words){  
                          words_array_size += 5;
                          words = realloc(words, (words_array_size*sizeof(char*)));
                     }
                     
                     command[num_command] = currentByte;
                     num_command++;
                     put_command(&words, &command, num_words, &num_command, &command_array_size);

                     num_command = 0;
                     num_words++;
                     space_seen = false;

                     break;
                  }
               if ((num_command > 0) && (num_words > 0))
               {
                   if ((strcmp(words[num_words -1],"||") == 0) || (command[num_command - 1] == '>') || (command[num_command - 1] == '<') ||  (command[num_command - 1] == '&') || (strcmp(words[num_words -1],"|") == 0) || (strcmp(words[num_words -1],"&&") == 0))
                  {
                    errorAndLeave(lines, &a, num_complete_command);
      
                  }
               }
                errorAndLeave(lines, &a, num_complete_command);
          case ' ':
             if (num_command > 0)
                space_seen = true;
             break;
          case ')':
                rightParenthesis ++;
                if ( (num_command == 0) && (num_words > 0) && (strcmp(words[num_words - 1],"(") == 0) )
                  errorAndLeave(lines, &a, num_complete_command);
                if ((num_command == 0) && (num_words > 0) && (strcmp(words[num_words - 1], ";") == 0)) {
                    free(words[num_words - 1]);
                    num_words --;
                    }
                if (num_command > 0)
                {
                   if (words_array_size == num_words){
                       words_array_size += 5;
                       words = realloc(words, (words_array_size*sizeof(char*)));
                      }
                      put_command(&words, &command, num_words, &num_command, &command_array_size);
                      num_command = 0;
                      num_words ++;
                }
                   if(command_array_size == num_command){
                       command_array_size += 5;
                       command = realloc(command, (command_array_size*sizeof(char)));
                     }
                   command[num_command] = currentByte;
                   num_command ++;
                   if (words_array_size == num_words){
                       words_array_size += 5;
                       words = realloc(words, (words_array_size*sizeof(char*)));
                     }
                   put_command(&words, &command, num_words, &num_command, &command_array_size);
                   num_command = 0;
                   num_words ++;
                   space_seen = false;
                   break;
          case '(':
                leftParenthesis ++;
                if ((num_words > 0) && (num_command == 0)) {

                    if ((strcmp(words[num_words - 1], "||") != 0) && (strcmp(words[num_words - 1], "&&") != 0) && (strcmp(words[num_words - 1], "|") != 0) && (strcmp(words[num_words - 1], ";") != 0)  && (strcmp(words[num_words - 1], "(") != 0)) {
                      errorAndLeave(lines, &a, num_complete_command);
                      }
                    }
                if (num_command > 0)
                {
                   if (words_array_size == num_words){
                       words_array_size += 5;
                       words = realloc(words, (words_array_size*sizeof(char*)));
                      }
                 
                      put_command(&words, &command, num_words, &num_command, &command_array_size);
                      num_command = 0;
                      num_words ++;
                
                if (num_words > 0) {
                    if ((strcmp(words[num_words - 1], "||") != 0) && (strcmp(words[num_words - 1], "&&") != 0) && (strcmp(words[num_words - 1], "|") != 0) && (strcmp(words[num_words - 1], ";") != 0)  && (strcmp(words[num_words - 1], "(") != 0)) {
                      errorAndLeave(lines, &a, num_complete_command);
                      }
                    }
                 }
               
                   if(command_array_size == num_command){
                       command_array_size += 5;
                       command = realloc(command, (command_array_size*sizeof(char)));
                     }
                   command[num_command] = currentByte;
                   num_command ++;
                   if (words_array_size == num_words){
                       words_array_size += 5;
                       words = realloc(words, (words_array_size*sizeof(char*)));
                     }
                   put_command(&words, &command, num_words, &num_command, &command_array_size);
                   num_command = 0;
                   num_words ++;
                   space_seen = false;
                   break; 
          case '<':
             if ((num_command > 0) && (command[num_command - 1] == '|'))
                {
                   errorAndLeave(lines, &a, num_complete_command);
                }
             if (num_command == 0)
                {  
                    errorAndLeave(lines,&a, num_complete_command);
                }
              if(command_array_size == num_command)
                {
                  command_array_size += 5;
                  command = realloc(command, (command_array_size*sizeof(char)));
                }
              else if (space_seen && (command_array_size == num_command - 1))
             {
                command_array_size ++;
                command = realloc(command, (command_array_size*sizeof(char)));
             }
              if (space_seen)
               {
                command[num_command] = ' ';
                command[num_command + 1] = currentByte;
                num_command += 2;
                space_seen = false;
                break;
               }
              command[num_command] = currentByte;
              num_command++;
              break; 
          case '>':
             if ((num_command > 0) && (command[num_command - 1] == '|'))
               {
                errorAndLeave(lines, &a, num_complete_command);
               }
             if (num_command == 0)
               {
                errorAndLeave(lines, &a, num_complete_command);
               }
             if(command_array_size == num_command)
                {
                  command_array_size += 5;
                  command = realloc(command, (command_array_size*sizeof(char)));
                }
             else if (space_seen && (command_array_size == num_command - 1))
             {
                command_array_size ++;
                command = realloc(command, (command_array_size*sizeof(char)));
             }
             if (space_seen)
             {
                command[num_command] = ' ';
                command[num_command + 1] = currentByte;
                num_command += 2;
                space_seen = false;
                break;
             }
              command[num_command] = currentByte;
              num_command++;
              break;
          case '\\':
                if(command_array_size == num_command){
                   command_array_size += 5;
                   command = realloc(command, (command_array_size*sizeof(char)));
                   }

                if (words_array_size == num_words){
                    words_array_size += 5;
                    words = realloc(words, (words_array_size*sizeof(char*)));
                    }

                 put_command(&words, &command, num_words, &num_command, &command_array_size);
                 space_seen = false;

                 num_command = 0;
                 prevNecessarySlash = true;
                 num_words++;
                 space_seen = false;
                 break;
          case '\n':
             lines ++;
             if (!prevNecessarySlash)
                 justNewline = true;
                if ((((num_command > 0) && isNorm(command[num_command - 1]) || command[num_command - 1] == ')') || ((num_words > 0) && (strcmp(words[num_words - 1], ";") == 0  ||  strcmp(words[num_words - 1], ")") == 0) )) || comment_complete_command)
                {
                if ((num_command == 0) && (num_words > 0) && (strcmp(words[num_words - 1], ";") == 0)) {
                    free(words[num_words - 1]);
                    num_words --;
                }          
                  if (num_command > 0) {
                     if (words_array_size == num_words){
                       words_array_size += 5;
                       words = realloc(words, (words_array_size*sizeof(char*)));
                       }
                      put_command(&words, &command, num_words, &num_command, &command_array_size);
                      num_command = 0;
                      num_words++;
                      space_seen = false;
                  }
                
                   if (leftParenthesis == rightParenthesis) {
                      if(num_complete_command == complete_command_array_size){
                        complete_command_array_size += 5;
                        a = realloc(a, complete_command_array_size*sizeof(Complete_command));
                      }
                      put_words(&a, &words, &num_words, &num_complete_command, &words_array_size);
                      comment_complete_command = false;
                      leftParenthesis = rightParenthesis = 0;

                   } 
                   break;
                 
                }
             if ((num_words > 0) && (num_command > 0))
             {
               if ((strcmp(words[num_words - 1],"||") == 0) || (command[num_command - 1] == '|') || (strcmp(words[num_words - 1],"|") == 0) || (strcmp(words[num_words - 1],"&&" == 0)))
                  {
                   if (num_command != 0)
                      {
                         if (words_array_size == num_words)  
                         {
                            words_array_size += 5;
                            words = realloc(words, (words_array_size*sizeof(char*)));
                          }
                         put_command(&words, &command, num_words, &num_command, &command_array_size);
                         num_words ++;
                         num_command = 0;
  
                    }
                   break;
                }
              }
             if (prevNecessarySlash)
                {
                 backslashNewline = true;
                 prevNecessarySlash = false;
                 break;
          
                }
             if ((command[num_command - 1] == '<') || (command[num_command - 1] == '>'))
                {

                  errorAndLeave(lines, &a, num_complete_command);
                }
             break;
                case '#':
                  if  ((num_words == 0 && num_command == 0)  || space_seen || ((num_words > 0) && ((strcmp(words[num_words -1],"||") == 0) || (strcmp(words[num_words -1],")") == 0) || (strcmp(words[num_words -1],"&&") == 0) || (strcmp(words[num_words -1],"|") == 0))))
                  {
                   if (num_command > 0) {
                     if (words_array_size == num_words)
                         {
                            words_array_size += 5;
                            words = realloc(words, (words_array_size*sizeof(char*)));
                          }
                         if (isNorm(command[num_command - 1]))
                            comment_complete_command = true;
                         put_command(&words, &command, num_words, &num_command, &command_array_size);
                         num_words ++;
                         num_command = 0;
                     }
                     space_seen = false;
                     while ((currentByte) != '\n')
                     {
                         currentByte = get_next_byte(get_next_byte_argument);
                     }
                   }
                   else 
                   {
                      if (command_array_size == num_command){
                      command_array_size += 5;
                      command = realloc(command, (command_array_size*sizeof(char)));
                      }
                      command[num_command] = currentByte;
                      num_command++;
                    }
                  break;
          default:
             if (singleAnd)
                errorAndLeave(lines, &a, num_complete_command);
             justNewline = false;
             if (command_array_size == num_command){
                command_array_size += 5;
                command = realloc(command, (command_array_size*sizeof(char)));
             }
             if (space_seen && (command_array_size == num_command - 1))
             {
                command_array_size ++;
                command = realloc(command, (command_array_size*sizeof(char)));
             }
             if ((num_command > 0) && (command[num_command - 1] == '|'))
                {
                  if (words_array_size == num_words)  
                       {
                          words_array_size += 5;
                          words = realloc(words, (words_array_size*sizeof(char*)));
                       }
 
                  put_command(&words, &command, num_words, &num_command, &command_array_size);
                  space_seen = false;
                  num_words ++;
                  num_command = 0;
                  space_seen = false;

                }
                  
             if (space_seen)
             {
                command[num_command] = ' ';
                command[num_command + 1] = currentByte;
                num_command += 2;
                space_seen = false;
                break;
             }
             command[num_command] = currentByte;
             num_command++;
             break;
         }
       
     }

     if(command_array_size == num_command){
        command_array_size += 5;
        command = realloc(command, (command_array_size*sizeof(char)));
        }
     if (words_array_size == num_words){
          words_array_size += 5;
          words = realloc(words, (words_array_size*sizeof(char*)));
         }
     if ((num_command == 0) && (num_words > 0) && (strcmp(words[num_words - 1], ";") == 0)) {
           free(words[num_words - 1]);
           num_words --;
       }
     if ( (num_words > 0) && ( (strcmp(words[num_words - 1], "||") == 0) || (strcmp(words[num_words - 1], "|") == 0) || (strcmp(words[num_words - 1], "&&") == 0))) {
           errorAndLeave(lines, &a, num_complete_command);
         }    
  if ((num_command > 0) || (num_words > 0) ){
      if (num_command > 0)
          put_command(&words, &command, num_words, &num_command, &command_array_size);
        if(num_complete_command == complete_command_array_size){
          complete_command_array_size += 5;
          a = realloc(a, complete_command_array_size*sizeof(Complete_command));
                   }
        put_words(&a, &words, &num_words, &num_complete_command, &words_array_size);
   }
     if (leftParenthesis != rightParenthesis)
         errorAndLeave(lines, &a, num_complete_command);
     leftParenthesis = rightParenthesis = 0;

int i;
int j;

for (j = 0; j < num_complete_command; j++){
  for (i = 0; i < a[j].command_size ; i++)
    {
//     printf("[%s] ", a[j].command[i]);
    }
//   printf("\n");
  }

if (num_complete_command > 0) {
  tree = malloc(1*sizeof(command_stream));
  tree->root = malloc(num_complete_command*sizeof(struct command*));
  tree->num_root = num_complete_command;
  tree->index = 0;
}

int k;
for (k = 0; k < num_complete_command; k++)
   {
    struct command *brett = makeTree(&(a[k]), 0, a[k].command_size - 1);
    tree->root[k] = brett;
 //    InOrder(&brett);
 //    printf("\n");
   }
 free_space(&a, num_complete_command);

return tree;
}




command_t
read_command_stream (command_stream_t s)
{
  if(s == NULL){
    return NULL;
  }
  if(s->num_root == s->index){
    int i;
    for(i = 0; i < s->num_root; i++)
      freeTree(s->root[i]);
    free(s->root);
    free(s);
    return NULL;
  } 
  command_t result = s->root[s->index];
  (s->index)++;
  return result;
}
