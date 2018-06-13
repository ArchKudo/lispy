#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

int main()
{
    char *input = NULL;

    printf("LISPY v0.0.2\n");
    printf("Enter CTRL+C or CTRL+D on an empty line to exit\n");
    while (1)
    {
        input = readline("lispy> ");

        if (input != NULL)
        {
            add_history(input);
        }
        else // Handle EOF
        {
            printf("Bye!");
            break;
        }
        // Echo it back
        printf("%s\n", input);
    }
    free(input);
    return 0;
}
