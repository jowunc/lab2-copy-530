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

    cmd_t *node;
    int count;
    int i;

    if(shell == NULL){
        return;
    }

    if(shell->head_node == NULL){
        return;
    }
    if (shell->total_cmd_t == 0){
        return;
    } 

    node = shell->head_node;
    count = (int)shell->total_cmd_t;

    if (node->argv == NULL || node->argv[0] == NULL){
        return;
    }

    if(count == 1){
        if(is_builtin(node)){
            builtin(node);
            return;
        }
    }

    int pipe_count = 0;
    if (count > 1) {
        pipe_count = count - 1;
    }

    int (*pipefds)[2] = NULL;
    pid_t *kids = NULL;
    int started = 0;

    kids = (pid_t*)malloc(sizeof(pid_t) * count);
    if (kids == NULL){
        perror("malloc");
        return;
    }

    if (pipe_count > 0){
        pipefds = (int (*)[2])malloc(sizeof(int[2]) * pipe_count);
        if (pipefds == NULL){
            perror("malloc");
            free(kids);
            return;
        }
        for (i = 0; i < pipe_count; i++){
            if(pipe(pipefds[i]) < 0){
                perror("pipe");
                for(int j = 0; j < i; j++){
                    close(pipefds[j][0]);
                    close(pipefds[j][1]);
                }
                free(pipefds);
                free(kids);
                return;
            }
        }
    }

    node = shell->head_node;
    int fork_failed = 0;

    for (i = 0; i < count; i++){
        pid_t pid = fork();

        if (pid < 0){
            perror("fork");
            fork_failed = 1;
            break;
        }
        if(pid == 0){
            if(pipe_count > 0){
                if(i > 0){
                    if(dup2(pipefds[i-1][0], STDIN_FILENO) < 0){
                        perror("dup2");
                        _exit(1);
                    }
                }
                if(i < count - 1){
                    if (dup2(pipefds[i][1], STDOUT_FILENO) < 0){
                        perror("dup2");
                        _exit(1);
                    }
                }
                for (int k = 0; k < pipe_count; k++){
                    close(pipefds[k][0]);
                    close(pipefds[k][1]);
                }
            }
            if(node->in_file != NULL){
                int infd = open(node->in_file, O_RDONLY);
                if(infd < 0){
                    perror("open");
                    _exit(1);
                }
                if (dup2(infd, STDIN_FILENO) < 0){
                    perror("dup2");
                    close(infd);
                    _exit(1);
                }
                close(infd);
            }
            if(node->out_file != NULL){
                int flags = O_WRONLY | O_CREAT;
                if (node->append){
                    flags = flags | O_APPEND;
                }else{
                    flags = flags | O_TRUNC;
                }

                int outfd = open(node->out_file, flags, 0644);
                if (outfd< 0){
                    perror("open");
                    _exit(1);
                }
                int target = STDOUT_FILENO;
                if (node->stderr){
                    target = STDERR_FILENO;
                }
                if (dup2(outfd, target) < 0) {
                     perror("dup2");
                     close(outfd);
                     _exit(1);
                }
                close(outfd);

        }   
            execvp(node->argv[0], node->argv);
            perror("execvp");
            _exit(1);

    }
        kids[started] = pid;
        started++;

        if(node->next_node != NULL){
            node = node->next_node;
        }
    

    }

    if(pipe_count > 0){
        for(i = 0; i < pipe_count; i++){
            close(pipefds[i][0]);
            close(pipefds[i][1]);
        }
    }

    for (i = 0; i < started; i++){
        waitpid(kids[i], NULL, 0);
    }

    if(pipefds != NULL){
        free(pipefds);
    }
    free(kids);

    if (fork_failed){
        return;
    }

}
// end execute() function

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
void parse_command(char* command) {
  if (shell == NULL) {
    shell = malloc(sizeof(input_struct));
    if (!shell) { fprintf(stderr, "malloc failed\n"); return; }
    shell->head_node = NULL;
    shell->total_cmd_t = 0;
    shell->user_input = NULL;
  }

  // initalize cmd struct
  cmd_t* cmd = malloc(sizeof(cmd_t));
  if (!cmd) { fprintf(stderr, "malloc failed\n"); return; }

  int cap = 64;
  cmd->argv = malloc(sizeof(char*) * (cap + 1));
  if (!cmd->argv) { fprintf(stderr, "malloc failed\n"); free(cmd); return; }

  cmd->argc = 0;
  cmd->argv[0] = NULL;
  cmd->in_file = NULL;
  cmd->out_file = NULL;
  cmd->append = false;
  cmd->stderr = false;
  cmd->next_node = NULL;

  bool next_is_infile = false;
  bool next_is_outfile = false;
  bool invalid = false;

  char* p = command;

  while (!invalid) {
    // skip whitespace
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p == '\0') break;

    if (((*p == '<') || (*p == '>') || (*p == '2' && *(p + 1) == '>')) && (next_is_infile || next_is_outfile)) {
      if (next_is_infile) fprintf(stderr, "Missing input filename\n");
      else fprintf(stderr, "Missing output filename\n");
      invalid = true;
      break;
    }

    if (*p == '2' && *(p + 1) == '>'){
        if (cmd->out_file != NULL){
            fprintf(stderr, "Mutltiple output redirections\n");
            invalid = true;
            break;
        }
        cmd->stderr = true;
        cmd->append = false;
        next_is_outfile = true;
        p += 2;
        continue;
    }

    // handle input redirection
    if (*p == '<') {
      // multiple input redirections not allowed
      if (cmd->in_file != NULL) {
        fprintf(stderr, "Multiple input redirections\n");
        invalid = true;
        break;
      }
      next_is_infile = true;
      p++; 
      continue;
    }

    // handle output redirection
    if (*p == '>') {
      // multiple output redirections not allowed
      if (cmd->out_file != NULL) {
        fprintf(stderr, "Multiple output redirections\n");
        invalid = true;
        break;
      }

      if (*(p + 1) == '>') {
        cmd->append = true;
        p += 2; 
      } else {
        cmd->append = false;
        p++; 
      }
      next_is_outfile = true;
      continue;
    }

    char* start = p;
    while (*p && !isspace((unsigned char)*p) && *p != '<' && *p != '>') p++;

    char saved = *p;
    *p = '\0';

    if (start[0] != '\0') {
      if (next_is_infile) {
        cmd->in_file = strdup(start);
        next_is_infile = false;
      } else if (next_is_outfile) {
        cmd->out_file = strdup(start);
        next_is_outfile = false;
      } else {
        if (cmd->argc < cap) cmd->argv[cmd->argc++] = strdup(start);
        else fprintf(stderr, "Too many arguments (max %d)\n", cap);
      }
    }

    *p = saved;
  }

  // operator at end with no filename
  if (!invalid && next_is_infile) {
    fprintf(stderr, "Missing input filename\n");
    invalid = true;
  }
  if (!invalid && next_is_outfile) {
    fprintf(stderr, "Missing output filename\n");
    invalid = true;
  }

  // null-terminate argv
  cmd->argv[cmd->argc] = NULL;

  // mark invalid/empty
  if (invalid || cmd->argc == 0) {
    cmd->argc = 0;
    cmd->argv[0] = NULL;
  }

  // append to linked list
  if (shell->head_node == NULL) {
    shell->head_node = cmd;
  } else {
    cmd_t* curr = shell->head_node;
    while (curr->next_node != NULL) curr = curr->next_node;
    curr->next_node = cmd;
  }
} //end parse_command() function

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
  if (shell == NULL) {
  shell = malloc(sizeof(input_struct));
  if (!shell) { fprintf(stderr, "malloc failed\n"); return 0; }
  shell->head_node = NULL;
  shell->total_cmd_t = 0;
  shell->user_input = NULL;
  } else {
    unallocate_resources();
  }
  shell->user_input = user_input;
  trim(user_input);
  if (user_input[0] == '\0'){
    //user input is empty
    shell->total_cmd_t = 0;
    shell->head_node = NULL;
    return 0;
  }
  else if (strchr(user_input, '|') == NULL){
    //user input is not empty and does not contain a pipe
    shell->total_cmd_t = 1;
    parse_command(user_input);
    return shell->total_cmd_t;
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
        trim(start);
        parse_command(start);
        start = p + 1;
      }
      p++;
    }
    trim(start);
    if (start[0] == '\0') {
    // empty command between pipes: treat whole input as invalid
    shell->total_cmd_t = 0;
    shell->head_node = NULL;
  return 0;
  }
    parse_command(start);
    shell->total_cmd_t = num_pipes + 1;
  }
  return shell->total_cmd_t;
  
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

  if (shell == NULL) return;

  cmd_t* curr = shell->head_node;

  while (curr != NULL){
    cmd_t* next = curr->next_node;
    if (curr->argv != NULL){
      for (int i = 0; i < curr->argc; i++){
        free(curr->argv[i]);
      }
      free(curr->argv);
    }
    free(curr->in_file);
    free(curr->out_file);
    free(curr);
    curr = next;
  }
  shell->head_node = NULL;
  shell->total_cmd_t = 0;
  shell->user_input = NULL;
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
