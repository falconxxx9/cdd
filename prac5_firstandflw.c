
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_PROD 50
#define MAX_LEN 50

char lhs[MAX_PROD];
char rhs[MAX_PROD][MAX_LEN];
int n = 0;

int firstSet[128][128];
int followSet[128][128];
int used[128];

// Check whether a symbol is a non-terminal
int isNT(char c)
{
    return isupper((unsigned char)c);
}

// Read grammar productions
void readGrammar()
{
    char left[20], right[MAX_LEN];

    printf("Enter each production as: LHS RHS\n");
    printf("Example: E TX\n");
    printf("Enter e for epsilon and # # to finish.\n\n");

    while (n < MAX_PROD &&
           scanf("%19s %49s", left, right) == 2)
    {
        if (strcmp(left, "#") == 0 &&
            strcmp(right, "#") == 0)
            break;

        if (strlen(left) != 1 ||
            !isNT(left[0]) || right[0] == '\0')
        {
            printf("Invalid production.\n");
            continue;
        }

        lhs[n] = left[0];
        strcpy(rhs[n], right);

        used[(unsigned char)left[0]] = 1;

        for (int i = 0; right[i] != '\0'; i++)
        {
            if (right[i] != 'e')
                used[(unsigned char)right[i]] = 1;
        }

        n++;
    }
}

// Add symbol to FIRST set
int addFirst(char A, char symbol)
{
    unsigned char a = (unsigned char)A;
    unsigned char s = (unsigned char)symbol;

    if (!firstSet[a][s])
    {
        firstSet[a][s] = 1;
        return 1;
    }

    return 0;
}

// Calculate FIRST sets
void calculateFirst()
{
    int changed = 1;

    while (changed)
    {
        changed = 0;

        for (int i = 0; i < n; i++)
        {
            char A = lhs[i];
            char *r = rhs[i];

            // Production A -> e
            if (strcmp(r, "e") == 0)
            {
                changed |= addFirst(A, 'e');
                continue;
            }

            int allNullable = 1;

            for (int j = 0; r[j] != '\0'; j++)
            {
                char X = r[j];

                // If X is a terminal
                if (!isNT(X))
                {
                    changed |= addFirst(A, X);
                    allNullable = 0;
                    break;
                }

                // Add FIRST(X) except epsilon
                for (int t = 0; t < 128; t++)
                {
                    if (t != 'e' &&
                        firstSet[(unsigned char)X][t])
                    {
                        changed |= addFirst(A, (char)t);
                    }
                }

                // Check whether X can produce epsilon
                if (!firstSet[(unsigned char)X]
                             [(unsigned char)'e'])
                {
                    allNullable = 0;
                    break;
                }
            }

            // If all symbols can produce epsilon
            if (allNullable)
                changed |= addFirst(A, 'e');
        }
    }
}

// Calculate FOLLOW sets
void calculateFollow()
{
    int changed = 1;
    char start = lhs[0];

    // Add $ to FOLLOW of start symbol
    followSet[(unsigned char)start]['$'] = 1;

    while (changed)
    {
        changed = 0;

        for (int i = 0; i < n; i++)
        {
            char A = lhs[i];
            char *r = rhs[i];

            if (strcmp(r, "e") == 0)
                continue;

            for (int j = 0; r[j] != '\0'; j++)
            {
                char B = r[j];

                if (!isNT(B))
                    continue;

                int nullable = 1;

                // Examine symbols after B
                for (int k = j + 1; r[k] != '\0'; k++)
                {
                    char X = r[k];

                    // If X is a terminal
                    if (!isNT(X))
                    {
                        if (X != 'e' &&
                            !followSet[(unsigned char)B]
                                      [(unsigned char)X])
                        {
                            followSet[(unsigned char)B]
                                     [(unsigned char)X] = 1;
                            changed = 1;
                        }

                        nullable = 0;
                        break;
                    }

                    // Add FIRST(X) except epsilon
                    for (int t = 0; t < 128; t++)
                    {
                        if (t != 'e' &&
                            firstSet[(unsigned char)X][t] &&
                            !followSet[(unsigned char)B][t])
                        {
                            followSet[(unsigned char)B][t] = 1;
                            changed = 1;
                        }
                    }

                    // Check whether X can produce epsilon
                    if (!firstSet[(unsigned char)X]
                                 [(unsigned char)'e'])
                    {
                        nullable = 0;
                        break;
                    }
                }

                // Add FOLLOW(A) to FOLLOW(B)
                if (nullable)
                {
                    for (int t = 0; t < 128; t++)
                    {
                        if (followSet[(unsigned char)A][t] &&
                            !followSet[(unsigned char)B][t])
                        {
                            followSet[(unsigned char)B][t] = 1;
                            changed = 1;
                        }
                    }
                }
            }
        }
    }
}

// Display FIRST and FOLLOW sets
void printSets()
{
    printf("\nFIRST sets:\n");

    for (int A = 0; A < 128; A++)
    {
        if (used[A] && isNT((char)A))
        {
            printf("FIRST(%c) = { ", A);

            for (int t = 0; t < 128; t++)
            {
                if (firstSet[A][t])
                    printf("%c ", t);
            }

            printf("}\n");
        }
    }

    printf("\nFOLLOW sets:\n");

    for (int A = 0; A < 128; A++)
    {
        if (used[A] && isNT((char)A))
        {
            printf("FOLLOW(%c) = { ", A);

            for (int t = 0; t < 128; t++)
            {
                if (followSet[A][t])
                    printf("%c ", t);
            }

            printf("}\n");
        }
    }
}

int main()
{
    readGrammar();

    if (n == 0)
    {
        printf("No productions entered.\n");
        return 1;
    }

    calculateFirst();
    calculateFollow();
    printSets();

    return 0;
}
