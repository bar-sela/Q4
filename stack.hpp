//
// Created by barsela on 4/19/22.
//

#ifndef EX4_STACK_HPP
#define EX4_STACK_HPP


typedef struct Node {
    char str[1024];
    struct Node *next;
}node;

void push(char* str_input);
void pop();
char* top();


#endif //EX4_STACK_HPP