//
// Created by carlos on 2/20/22.
//

#ifndef TIPO_SCOPE_H
#define TIPO_SCOPE_H
#include "lex.h"
struct node;

enum bind_type {
    BIND_ARG,
    BIND_VALUE,
    BIND_INTRINSIC,
};

struct bind {
    struct bind *next;
    struct token name;
    struct node *value;
    enum bind_type typ;
};

struct scope {
    struct scope *parent;
    struct scope *sibling;
    struct scope *children_head;
    struct scope *children_tail;
    struct bind *bind_head;
    struct bind *bind_tail;
};

struct scope *new_scope();
void scope_free(struct scope *scope);
void scope_add_child(struct scope *parent, struct scope *child);

struct scope *fork_scope(struct scope *parent);
struct scope *def_bind(struct scope *scope, enum bind_type typ, struct token name, struct node *value);
struct bind *get_bind(struct scope *scope, struct token lookup_name, bool recursive);

#endif //TIPO_SCOPE_H
