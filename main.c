#include <stdio.h>
#include "parse.h"
#include "ir.h"

void a() {
    const char *src = "a => b => c => d => e => f => w"; // a => b => c => d => e
    struct token token = first_token(src);
    struct token current = next_token(token);
    struct ast_node *node = parse(&current);
    print_ast_node(node, 0);
}

int main() {
    a();

    const char *src = "a b => w"; // a => b => c => d => e
    struct token token = first_token(src);
    struct token current = next_token(token);

    printf("==== AST ====\n");
    struct ast_node *node = parse(&current);
    print_ast_node(node, 0);

    printf("==== IR NORMALIZED ====\n");
    struct ir_node *normalized = new_ir_node_from_ast_node(node);
    print_ir_node(normalized, 0);

    return 0;
}
