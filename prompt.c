#include <stdio.h>
#include <string.h>

static char input[2048];

int main(int argc, char const *argv[])
{
    printf("LISPY v0.0.1\n");
    printf("Enter CTRL+C or exit to exit\n");

    while (1)
    {
        printf("lispy> ");
        scanf("[^\n", input);

        // Echo it back
        printf("%s", input);

        if (strcmp(input, "exit\n"))
        {
            printf("Bye!\n");
            break;
        }
    }
    return 0;
}
