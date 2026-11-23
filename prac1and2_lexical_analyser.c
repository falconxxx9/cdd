
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
bool isDelimiter(char ch)
{
    return (ch == ' ' || ch == '+' || ch == '-' ||
            ch == '*' || ch == '/' || ch == ',' ||
            ch == ';' || ch == '>' || ch == '<' ||
            ch == '=' || ch == '(' || ch == ')' ||
            ch == '[' || ch == ']' || ch == '{' ||
            ch == '}');
}
bool isOperator(char ch)
{
    return (ch == '+' || ch == '-' || ch == '*' ||
            ch == '/' || ch == '>' || ch == '<' ||
            ch == '=');
}
bool isKeyword(char *str)
{
    char *keywords[] = {
        "if", "else", "while", "do", "break",
        "continue", "int", "double", "float",
        "return", "char", "case", "sizeof",
        "long", "short", "typedef", "switch",
        "unsigned", "void", "static", "struct",
        "goto"
    };

    for (int i = 0; i < 22; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
            return true;
    }
    return false;
}
bool validIdentifier(char *str)
{
    if (str[0] == '\0' ||
        (!isalpha((unsigned char)str[0]) && str[0] != '_'))
        return false;

    for (int i = 1; str[i] != '\0'; i++)
    {
        if (!isalnum((unsigned char)str[i]) && str[i] != '_')
            return false;
    }

    return true;
}
bool isInteger(char *str)
{
    int i = 0;
    if (str[0] == '-')
        i++;
    if (str[i] == '\0')
        return false;
    for (; str[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)str[i]))
            return false;
    }
    return true;
}
bool isRealNumber(char *str)
{
    int i = 0, decimal = 0;
    if (str[0] == '-')
        i++;
    if (str[i] == '\0')
        return false;
    for (; str[i] != '\0'; i++)
    {
        if (str[i] == '.')
            decimal++;
        else if (!isdigit((unsigned char)str[i]))
            return false;
    }
    return decimal == 1;
}
void parse(char *str)
{
    char token[100];
    int i = 0, j = 0;

    while (str[i] != '\0')
    {
        if (isDelimiter(str[i]))
        {
            if (j > 0)
            {
                token[j] = '\0';

                if (isKeyword(token))
                    printf("%s IS A KEYWORD\n", token);
                else if (isInteger(token))
                    printf("%s IS AN INTEGER\n", token);
                else if (isRealNumber(token))
                    printf("%s IS A REAL NUMBER\n", token);
                else if (validIdentifier(token))
                    printf("%s IS A VALID IDENTIFIER\n", token);
                else
                    printf("%s IS NOT A VALID IDENTIFIER\n", token);

                j = 0;
            }

            if (isOperator(str[i]))
                printf("%c IS AN OPERATOR\n", str[i]);
        }
        else
        {
            token[j++] = str[i];
        }

        i++;
    }

    if (j > 0)
    {
        token[j] = '\0';

        if (isKeyword(token))
            printf("%s IS A KEYWORD\n", token);
        else if (isInteger(token))
            printf("%s IS AN INTEGER\n", token);
        else if (isRealNumber(token))
            printf("%s IS A REAL NUMBER\n", token);
        else if (validIdentifier(token))
            printf("%s IS A VALID IDENTIFIER\n", token);
        else
            printf("%s IS NOT A VALID IDENTIFIER\n", token);
    }
}

int main()
{
    char str[100] = "int 1a = b + 1c;";

    printf("--- Lexical Analyzer Output ---\n");
    parse(str);

    return 0;
}
