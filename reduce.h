//
// Created by carlos on 2/21/22.
//

#ifndef TIPO_REDUCE_H
#define TIPO_REDUCE_H
struct ir_node;

struct ir_node *substitute(const struct ir_node *into_expr, const char *name, struct ir_node *with_expr);

#define reduce_error(r, c, msg, ...) do{ printf("%lu:%lu: " msg "\n", (r), (c), ##__VA_ARGS__); exit(1); }while(0)
#endif //TIPO_REDUCE_H
