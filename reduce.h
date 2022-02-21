//
// Created by carlos on 2/21/22.
//

#ifndef TIPO_REDUCE_H
#define TIPO_REDUCE_H
struct ast_node;

struct ast_node *deep_copy(const struct ast_node *node);
struct ast_node *substitute(const struct ast_node *into_expr, const char *name, struct ast_node *with_expr);

#define reduce_error(t, msg, ...) do{ printf("%lu:%lu: " msg "\n", (t).row, (t).col, ##__VA_ARGS__); exit(1); }while(0)
#endif //TIPO_REDUCE_H
