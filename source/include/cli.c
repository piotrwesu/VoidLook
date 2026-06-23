#include "cli.h"

#include <stdio.h>
#include <string.h>

static void print_line_width(const char *text, const int terminal_width);

void cli_print_apod(const char *url_photo, const char *title, const char *explanation)
{
    printf("HD Url: \t");
    if(url_photo)
        printf("%s\n", url_photo);

    printf("Title: \t\t");
    if(title)
        printf("%s\n", title);

    const int terminal_width = 79;

    if(strlen(explanation) <= terminal_width) {
        if(explanation) {
            printf("\x1b[90m");
            printf("\n%s\n", explanation);
            printf("\x1b[0m\n");
        }
    }
    else {
        printf("\n");
        printf("\x1b[90m");
        print_line_width(explanation, terminal_width);
        printf("\x1b[0m\n");
    }
};

static void print_line_width(const char *text, const int terminal_width)
{
    int start = 0;
    int text_length = strlen(text);

    while(start + terminal_width < text_length) {
        while(*(text+ start) == ' ') // we miss empty space on start line
            start++;

        int end = start + terminal_width;

        if(*(text + end) != ' ') {
            while(*(text + end) != ' ') // don't end line in half word
                end--;                          // go back
        }
        end++;

        printf("%.*s\n", end - start, text + start);
        start = end;
        }

        printf("%s\n\n", text + start);
}
