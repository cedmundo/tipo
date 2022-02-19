#include <stdio.h>
#include "parse.h"

int main() {
    const char *src = "λx = λy = e";
    struct token token = first_token(src);
    struct token current = next_token(token);
    struct node *node = parse(&current);
    print_node(node, 0);
    return 0;
}
