
#include "bx.h"
#include "bx.cpp"

#include <stdio.h>

struct node {
     node* previous;
     node* next;
};

entry_point int main(int argc, char** argv) {
     
     printf("compiler: %s\n", COMPILER_NAME);
     printf("platform: %s\n", PLATFORM_NAME);
     printf("architecture: %s\n", ARCHITECTURE_NAME);
     
     node* first = 0;
     node* last = 0;
     
     node test0 = {};
     node test1 = {};
     node test2 = {};
     node test3 = {};
     
     doubly_push_front(first, last, &test0);
     doubly_push_front(first, last, &test1);
     doubly_push_front(first, last, &test2);
     doubly_push_front(first, last, &test3);
     
     return 0;
}