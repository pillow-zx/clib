#include <autofree.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
        autofree char *input = (char *)malloc(sizeof(char) * 100);
        scanf("%s", input);
        printf("%s\n", input);
}
