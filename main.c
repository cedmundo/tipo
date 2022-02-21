#include <stdio.h>
#include "parse.h"

int main() {
    const char *src = "f = x => x";
    struct token token = first_token(src);
    struct token current = next_token(token);
    struct ast_node *node = parse(&current);
    print_node(node, 0);
    return 0;
}
