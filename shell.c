// I pledge the COMP530 honor code.

// ----------------------------------------------
// These are the only libraries that can be
// used. Under no circumstances can additional
// libraries be included
#include "shell.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

input_struct* shell = NULL;

// ----------------------------------------
// Functions that CANNOT be modified
// ----------------------------------------
// ------------------------------------
// Removes leading and trailing whitespace
// from a string.
//
// This function is useful when parsing
// and cleaning user input.
void trim(char* str) {

    char* p = str;       
    char* start = str;    /* first non-ws */
    char* last = NULL;    /* last non-ws */
    size_t len = 0;

    /* find first non-whitespace */
    while (*p && isspace((unsigned char)*p))
        p++;
    start = p;

    /* find last non-whitespace */
    while (*p) {
        if (!isspace((unsigned char)*p))
            last = p;
        p++;
    }

    if (last) {
        len = (size_t)(last - start + 1);
        /* remove white-space */
        if (start != str)
            memmove(str, start, len);

        str[len] = '\0';
    } else {
        /* string was empty or all whitespace */
        str[0] = '\0';
    }

} // end trim function


// ------------------------------------
// Executes a built-in command.
//
// This function handles built-in commands only,
// specifically `cd` and `exit`. These commands
// are executed directly by the shell process
// rather than by creating a child process.
//
// See the "Shell" section in the README.
void builtin( cmd_t* cmd ) {

  if ( strcmp(cmd->argv[0], "exit") == 0) {
    unallocate_resources();
    exit(0);
  } else if ( strcmp( cmd->argv[0], "cd") == 0 ) {
    if (cmd->argc == 1)
      chdir(getenv("HOME"));  // cd with no arguments
    else if ( cmd->argc == 2)
      chdir(cmd->argv[1]);  // cd with 1 arg
    else
      fprintf(stderr, "cd: Too many arguments\n");
  }

} // end builtin() function

// ------------------------------------
// Determines whether a command is a built-in.
//
// This function checks whether the command
// specified by cmd_t->argv[0] is a built-in
// command. Only `cd` and `exit` are treated
// as built-ins.
//
// Returns:
//   true  if cmd_t->argv[0] is "cd" or "exit"
//   false otherwise
bool is_builtin(cmd_t* cmd) {
    
    if (strcmp(cmd->argv[0], "cd") == 0 || strcmp(cmd->argv[0], "exit") == 0)
        return true;
    return false;

} // end is_builtin() function


// ----------------------------------------
// Functions that CAN be modified
// ----------------------------------------

// --------------------------------------
// Executes the parsed user input after the
// input_struct and associated cmd_t structures
// have been constructed by parse_input() and
// parse_command().
//
// This function:
//   - Sets up pipes if needed.
//   - Executes built-in commands directly in
//     the shell process.
//   - Otherwise, creates one child process per
//     command using fork().
//   - In each child, configures any required
//     redirection and pipe endpoints, closes
//     unused file descriptors, then calls
//     execvp() to execute the program.
//   - In the parent, closes unused file
//     descriptors and waits for all child
//     processes to complete.
//
// See the "Shell" section in the README for
// additional details.
void execute() {

  // TODO: your solution






} // end execute() function

// --------------------------------------
// Called by parse_input() to further process
// an individual command string.
//
// This function parses the command string and
// constructs a cmd_t structure by identifying
// and setting the following fields:
//   - argv
//   - argc
//   - in_file
//   - out_file
//   - append
//   - stderr (honor section only)
//
// After the cmd_t structure is initialized,
// it is appended to the end of the linked list
// of commands.
//
// See the "Shell → Internal Representation"
// section in the README for examples.
void parse_command( char* command ) {
<<<<<<< HEAD
  //initialize cmd struct
  cmd_t* cmd = malloc(sizeof(cmd_t));
  cmd->argc = 0;
  cmd->in_file = NULL;
  cmd->out_file = NULL;
  cmd->append = false;
  cmd->stderr = false;
  cmd->next_node = NULL;
  char *p = command;
  char *start = p;
  bool next_is_infile = false;
  bool next_is_outfile = false;
  while(*p != '\0'){
    //loop till null terminator (as appended by parse_input())
    if (*p == ' '){
      *p = '\0'; 
      //check for redirection
      if (next_is_infile){
        cmd->in_file = strdup(start);
        next_is_infile = false;
      }
      else if (*start == '<'){
        next_is_infile = true;
      }
      else if (next_is_outfile){
        cmd->out_file = strdup(start);
        next_is_outfile = false;
      }
      else if (strncmp(start, ">>", 2) == 0){
        next_is_outfile = true;
        cmd->append = true;
      }
      else if (*start == '>'){
        next_is_outfile = true;
      }
      else {
        cmd -> argv[cmd->argc++] = strdup(start);
      }
      start = p + 1;   
    }
    p++;
  }
  //process the last token
  if (next_is_infile){
    cmd->in_file = strdup(start);
  }
  else if (next_is_outfile){
    cmd->out_file = strdup(start);
  }
  else{
    cmd ->argv[cmd->argc++] = strdup(start);
  }
  //null terminate argv
  cmd->argv[cmd->argc] = NULL;
  //append cmd to linked list
  if (shell->head_node == NULL){
    shell->head_node = cmd;
  }
  else{
    cmd_t *curr = shell->head_node;
    while (curr->next_node != NULL){
      curr = curr->next_node;
    }
    curr->next_node = cmd;
  }
=======

  // TODO: your solution




  





>>>>>>> f21755db606868e5c582bea9cd451c60829f3c3b
} // end parse_command function

// --------------------------------------
// Identifies and separates each command in the
// user_input string and builds the input_struct.
//
// If the input is not empty and does not contain
// a pipe symbol (`|`), then there is only one
// command. If one or more pipes are present,
// the input is split into multiple command
// strings (the number of commands equals the
// number of pipes plus one).
//
// For each identified command string, this
// function calls parse_command() to further
// process the command.
//
// Returns: The total number of commands (i.e., 
// shell_struct->total_cmd_t).
//
// See the "Shell → Internal Representation"
// section in the README for examples.
int parse_input( char* user_input ) {
<<<<<<< HEAD
  shell->user_input = user_input;
  trim(user_input);
  if (user_input[0] == '\0'){
    //user input is empty
    shell->total_cmd_t = 0;
    shell->head_node = NULL;
  }
  else if (strchr(user_input, '|') == NULL){
    //user input is not empty and does not contain a pipe
    shell->total_cmd_t = 1;
    parse_command(user_input);
  }
  else{
    //user input is not empty and there one or more pipes
    char *p = user_input;
    char *start = p;
    int num_pipes = 0;
    while (*p != '\0'){
      if (*p == '|'){
        num_pipes ++;
        *p = '\0';
        parse_command(start);
        start = p + 1;
      }
      p++;
    }
    parse_command(start);
    shell->total_cmd_t = num_pipes + 1;
  }
  return shell->total_cmd_t;
=======

  // TODO: your solution




  
  return 0; // this is just a place holder
>>>>>>> f21755db606868e5c582bea9cd451c60829f3c3b
  
} // end parse_input() function

// --------------------------------------
// Frees all dynamically allocated memory associated
// with the input_struct and its linked cmd_t structures.
//
// This includes any heap-allocated fields such as:
// - argv
// - in_file
// - out_file
// - each cmd_t node in the linked list
// - user_input
// - the input_struct itself
//
// See the "Shell → Internal Representation"
// section in the README for additional details.
void unallocate_resources() {

  // TODO: your solution






} // end unallocate_resources function


// --------------------------------------
// This function is provided for debugging
// purposes.
//
// Example code is included, but you are
// encouraged to modify it as needed to help
// you and your team debug and inspect the
// internal representation of the parsed
// user input.
void debug() {

  if ( shell != NULL ) {
    printf(" --------- DEBUG --------------\n");
    printf("User input (%s)\n", shell->user_input );
    printf("Total # of commands (%d)\n", shell->total_cmd_t );

    cmd_t* node = shell->head_node;
    while ( node != NULL ) {
      for ( int i=0; i<node->argc; i++ ) {
        printf( "argv[%d]=%s\n", i, node->argv[i] );
      }
      if ( node->in_file != NULL ) printf("IN_FILE (%s)\n", node->in_file );
      if ( node->out_file != NULL ) printf("OUT_FILE (%s)\n", node->out_file );
      if ( node->append ) printf("OUT FILE APPEND\n");
      if ( node->stderr ) printf("OUT FILE STDERR\n");
      node = node->next_node;
    }

  }

} // end debug function

