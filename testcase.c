#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

// ----------------------------------
// You may add helper functions here
// -----------------------------------






// End helper function definitions


typedef int (*function_ptr)();

typedef struct {
    char *name;
    function_ptr fn;
} fn_table_entry_t;


// ----------------------------------
// Define test cases here.
// Three examples have been provided.
// ----------------------------------


int tc1() {
    int num_cmds = 0;
    int res = PASS;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "ls -l", CHUNK - 1);
    num_cmds = parse_input( user_input );
    if (num_cmds != 1 ) res = FAIL;
    unallocate_resources();
    free(user_input);
    return res;
}

int tc2() {
    int res = PASS;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "ls -l | grep shell", CHUNK - 1);
    parse_input( user_input );
    if (shell->total_cmd_t != 2 ) res = FAIL;
    unallocate_resources();
    free(user_input);
    return res;
}

int tc3() {
    int res = PASS;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "ls -l | grep shell", CHUNK - 1);
    parse_input( user_input );
    if (shell->head_node->in_file != NULL ) res = FAIL;
    unallocate_resources();
    free(user_input);
    return res;
}

int tc4() {
    int num_cmds = 0;
    int res = PASS;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "   ls -l   ", CHUNK - 1);
    num_cmds = parse_input(user_input);
    if (num_cmds != 1 ) res = FAIL;
    unallocate_resources();
    free(user_input);
    return res;
}

int tc5() {
    int res = PASS;
    int num_cmds = 0;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "echo hi", CHUNK - 1);
    num_cmds = parse_input(user_input);
    if (num_cmds != 1) res = FAIL;
    if (shell == NULL || shell->head_node == NULL || shell->head_node->argv == NULL) {
        res = FAIL;
    } else {
        if (strcmp(shell->head_node->argv[0], "echo") != 0) res = FAIL;
        if (strcmp(shell->head_node->argv[1], "hi") != 0) res = FAIL;
        if (shell->head_node->argv[2] != NULL) res = FAIL; // NULL-termination
    }
    unallocate_resources();
    free(user_input);
    return res;
}

int tc6() {
    int res = PASS;
    int num_cmds = 0;

    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "cat < in.txt | grep shell", CHUNK - 1);

    num_cmds = parse_input(user_input);

    if (num_cmds != 2) res = FAIL;

    if (shell == NULL || shell->head_node == NULL || shell->head_node->next_node == NULL) {
        res = FAIL;
    } else {
        cmd_t* cmd1 = shell->head_node;
        cmd_t* cmd2 = shell->head_node->next_node;

        if (cmd1->in_file == NULL || strcmp(cmd1->in_file, "in.txt") != 0) res = FAIL;
        if (cmd2->in_file != NULL) res = FAIL;
    } // <-- MISSING BRACE WAS HERE

    unallocate_resources();
    free(user_input);
    return res;
}

int tc7() {
    int res = PASS;
    int num_cmds = 0;

    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "ls -l | grep shell > out.txt", CHUNK - 1);

    num_cmds = parse_input(user_input);

    if (num_cmds != 2) res = FAIL;

    if (shell == NULL || shell->head_node == NULL || shell->head_node->next_node == NULL) {
        res = FAIL;
    } else {
        cmd_t* cmd1 = shell->head_node;
        cmd_t* cmd2 = shell->head_node->next_node;

        if (cmd2->out_file == NULL || strcmp(cmd2->out_file, "out.txt") != 0) res = FAIL;
        if (cmd1->out_file != NULL) res = FAIL;
    } 

    unallocate_resources();
    free(user_input);
    return res;
}

int tc8() {
    int res = PASS;
    int num_cmds = 0;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "grep shell < in.txt > out.txt", CHUNK - 1);
    num_cmds = parse_input( user_input );
    if (num_cmds != 1) res = FAIL;
    if (shell == NULL || shell->head_node == NULL){
        res = FAIL;
    } else {
    if (shell->head_node->in_file == NULL || strcmp(shell->head_node->in_file, "in.txt") != 0) res = FAIL;
    if (shell->head_node->out_file == NULL || strcmp(shell->head_node->out_file, "out.txt") != 0) res = FAIL;
    }
    unallocate_resources();
    free(user_input);
    return res;
}

int tc9() {
    int res = PASS;
    int num_cmds = 0;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "ls >> out.txt", CHUNK - 1);
    num_cmds = parse_input( user_input );
    if (num_cmds != 1) res = FAIL;
    if (shell == NULL || shell->head_node == NULL){
        res = FAIL;
    } else {
    if (shell->head_node->out_file == NULL || strcmp(shell->head_node->out_file, "out.txt") != 0) res = FAIL;
    if (shell->head_node->append != true) res = FAIL;
    }
    unallocate_resources();
    free(user_input);
    return res;
}

int tc10() {
    int res = PASS;
    int num_cmds = 0;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "ls | wc -l >> counts.txt", CHUNK - 1);
    num_cmds = parse_input( user_input );
    if (num_cmds != 2) res = FAIL;
   if (shell == NULL || shell->head_node == NULL || shell->head_node->next_node == NULL) {
        res = FAIL;
    } else {
        if (shell->head_node->next_node->append != true) res = FAIL;
    }
    unallocate_resources();
    free(user_input);
    return res;
}

int tc11() {
    int res = PASS;
    int num_cmds = 0;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "|", CHUNK - 1);
    num_cmds = parse_input( user_input );
    if (num_cmds != 0) res = FAIL;
    if (shell == NULL ) res = FAIL;
    else {
        if (shell->total_cmd_t != 0) res = FAIL;
        if (shell->head_node != NULL) res = FAIL;
    }
    unallocate_resources();
    free(user_input);
    return res;
}

int tc12() {
    int res = PASS;
    int num_cmds = 0;
    char* user_input = (char*)calloc(CHUNK, sizeof(char));
    strncpy(user_input, "cat <", CHUNK - 1);
    num_cmds = parse_input( user_input );
    if (num_cmds != 1) res = FAIL;
    if (shell == NULL || shell->head_node == NULL) res = FAIL;
    else {
        cmd_t* cmd = shell->head_node;
        if (cmd->argc != 0) res = FAIL;
        if (cmd->argv == NULL || cmd->argv[0] != NULL) res = FAIL;
    }
    unallocate_resources();
    free(user_input);
    return res;
}





// End test case function definitions

// ----------------------------------
// Every new test case function needs 
// to be defined the function table.
// Update as needed.
// ----------------------------------

fn_table_entry_t fn_table[] = {
    {"tc1", tc1},
    {"tc2", tc2},
    {"tc3", tc3},
    {"tc4", tc4},
    {"tc5", tc5},
    {"tc6", tc6},
    {"tc7", tc7},
    {"tc8", tc8},
    {"tc9", tc9},
    {"tc10", tc10},
    {"tc11", tc11},
    {"tc12", tc12},
    {NULL, NULL} // mark the end
};

// End function table definitions

function_ptr lookup_function(const char *fn_name) {
    if (!fn_name) {
        return NULL;
    }
    for (int i = 0; fn_table[i].name != NULL; ++i) {
        if (!strcmp(fn_name, fn_table[i].name)) {
            return fn_table[i].fn;
        }
    }
    return NULL; // testcase not found
}

int main( int argc, char** argv ) {

    int rv = FAIL;
    
    if ( argc != 2 ) {
        printf("------------------------\n");
        printf("Test case program\n");
        printf("------------------------\n");
        printf("Usage: ./testcase <testcase name>\n\n");
        printf("Example: ./testcase tc1\n\n");
        return 0;
    }

    function_ptr func = lookup_function( argv[1] );
    if (func != NULL) {
        rv = func();
    } else {
        printf("Testcase (%s) not defined!\n", argv[1] );
    }

    return rv;
}