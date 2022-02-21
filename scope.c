//
// Created by carlos on 2/20/22.
//
// NOLINTBEGIN(misc-no-recursion)
#include "scope.h"
#include <stdlib.h>
#include <memory.h>

struct scope *new_scope() {
    return calloc(1, sizeof(struct scope));
}

void scope_free(struct scope *scope) {
    if (scope->bind_head != NULL) {
        struct bind *current = scope->bind_head;
        struct bind *tmp;
        while (current != NULL) {
            tmp = current->next;
            free(current);
            current = tmp;
        }
    }

    if (scope->children_head != NULL) {
        struct scope *current = scope->children_head;
        struct scope *tmp;

        while (current != NULL) {
            tmp = current->sibling;
            scope_free(current);
            current = tmp;
        }
    }
}

void scope_add_child(struct scope *parent, struct scope *child) {
    if (parent->children_head == NULL) {
        parent->children_head = child;
    }

    if (parent->children_tail != NULL) {
        parent->children_tail->sibling = child;
    }

    parent->children_tail = child;
}

struct scope *fork_scope(struct scope *parent) {
    struct scope *scope = new_scope();
    scope->parent = parent;
    scope_add_child(parent, scope);
    return scope;
}

struct bind *get_bind(struct scope *scope, struct token lookup_name, bool recursive) {
    if (scope->bind_head == NULL) {
        return NULL;
    }

    struct bind *cur = scope->bind_head;
    while (cur != NULL) {
        struct token cur_name = cur->name;
        if (memcmp(cur_name.buf, lookup_name.buf, cur_name.len) == 0 && cur_name.len == lookup_name.len) {
            return cur;
        }

        cur = cur->next;
    }

    if (scope->parent != NULL && recursive) {
        return get_bind(scope->parent, lookup_name, recursive);
    }

    return NULL;
}

struct scope *def_bind(struct scope *scope, enum bind_type typ, struct token name, struct ast_node *value) {
    struct bind *already_defined = get_bind(scope, name, false);
    if (already_defined != NULL) {
        struct scope *forked = fork_scope(scope);
        return def_bind(forked, typ, name, value);
    }

    struct bind *bind = calloc(1, sizeof(struct bind));
    bind->typ = typ;
    bind->name = name;
    bind->value = value;

    if (scope->bind_head == NULL) {
        scope->bind_head = bind;
    }

    if (scope->bind_tail != NULL) {
        scope->bind_tail->next = bind;
    }

    scope->bind_tail = bind;
    return scope;
}
// NOLINTEND(misc-no-recursion)