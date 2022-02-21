//
// Created by carlos on 2/21/22.
//

#ifndef TIPO_REDUCE_H
#define TIPO_REDUCE_H
struct node;

struct node *deep_copy(const struct node *node);
struct node *substitute(const struct node *into_expr, const char *name, struct node *with_expr);

#define reduce_error(t, msg, ...) do{ printf("%lu:%lu: " msg "\n", (t).row, (t).col, ##__VA_ARGS__); exit(1); }while(0)
#endif //TIPO_REDUCE_H
