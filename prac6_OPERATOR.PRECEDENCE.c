
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 50

char lhs[MAX], rhs[MAX][MAX];
int n = 0;
int firstVT[128][128], lastVT[128][128];
char op[128][128];//op['+']['*'] = '<';
int conflict[128][128], used[128];

int isNT(char c) {
    return isupper((unsigned char)c);
}

void inputGrammar(void) {
    char left[20], right[MAX];
    int i;

    printf("Enter productions as LHS RHS (example: E E+T)\n");
    printf("Enter # # to finish.\n");

    while (n < MAX && scanf("%19s %49s", left, right) == 2) {
        if (strcmp(left, "#") == 0 && strcmp(right, "#") == 0)
            break;

        if (strlen(left) != 1 || !isNT(left[0]) ||
            strcmp(right, "e") == 0 || strcmp(right, "epsilon") == 0) {
            printf("Invalid input. Use one uppercase LHS and no epsilon.\n");
            continue;
        }

        lhs[n] = left[0];
        strcpy(rhs[n], right);
        used[(unsigned char)left[0]] = 1;

        for (i = 0; right[i] != '\0'; i++)
            used[(unsigned char)right[i]] = 1;

        n++;
    }
}

/* FIRSTVT and LASTVT are repeated until no new symbols are added.
 FIRSTVT(A): the set of terminals that can occur at the beginning
 of strings derived from non-terminal A, according to the operator-precedence rules.
 LASTVT(A): the set of terminals that can occur at the end of strings derived from
 non-terminal A, according to those rules.
 E -> E+T
E -> T
T -> i
The terminal + appears immediately after a non-terminal in E -> E+T, so it can be
added to FIRSTVT(E) using the rules implemented by this program.
 Also, the terminals found in FIRSTVT(T) are added to FIRSTVT(E) through the production E -> T.*/
void findFirstLastVT(void) {
    int change = 1, i, j, len;
    char A, B, *r;

    while (change) {
        change = 0;

        for (i = 0; i < n; i++) {
            A = lhs[i];
            r = rhs[i];
            len = (int)strlen(r);

            /* FIRSTVT(A) */
            /*If the first character on
            the right-hand side is a terminal,
             add it to FIRSTVT of the left-hand-side non-terminal.
             F -> i
             Since i is a terminal, the program adds it to FIRSTVT(F).*/
            if (!isNT(r[0])) {
                if (!firstVT[(unsigned char)A][(unsigned char)r[0]]) {
                    firstVT[(unsigned char)A][(unsigned char)r[0]] = 1;
                    change = 1;
                }
            } else {
                B = r[0];//first character is a non-terminal, so the program stores it in B.
                //E -> E+T The first character is a non-terminal and the second is a terminal. Therefore, + is added to FIRSTVT(E).
                if (r[1] && !isNT(r[1]) &&//whether the second character exists and is a terminal that has not already been added.
                    !firstVT[(unsigned char)A][(unsigned char)r[1]]) {
                    firstVT[(unsigned char)A][(unsigned char)r[1]] = 1;
                    change = 1;
                }
       /*E -> T
        T -> i
       Once i is in FIRSTVT(T), the production E -> T allows the program to add i to FIRSTVT(E).
This is why the loop repeats: the set for T may not have been complete when the program first processed E -> T. */
                for (j = 0; j < 128; j++)
                    if (firstVT[(unsigned char)B][j] &&
                        !firstVT[(unsigned char)A][j]) {
                        firstVT[(unsigned char)A][j] = 1;
                        change = 1;
                    }
            }

            /* LASTVT(A) */
            if (!isNT(r[len - 1])) {
                if (!lastVT[(unsigned char)A][(unsigned char)r[len - 1]]) {
                    lastVT[(unsigned char)A][(unsigned char)r[len - 1]] = 1;
                    change = 1;
                }
            } else {
                B = r[len - 1];
                if (len > 1 && !isNT(r[len - 2]) &&
                    !lastVT[(unsigned char)A][(unsigned char)r[len - 2]]) {
                    lastVT[(unsigned char)A][(unsigned char)r[len - 2]] = 1;
                    change = 1;
                }

                for (j = 0; j < 128; j++)
                    if (lastVT[(unsigned char)B][j] &&
                        !lastVT[(unsigned char)A][j]) {
                        lastVT[(unsigned char)A][j] = 1;
                        change = 1;
                    }
            }
        }
    }
}

void addRelation(char a, char b, char symbol) {
    unsigned char x = (unsigned char)a, y = (unsigned char)b;

    if (op[x][y] == '\0')
        op[x][y] = symbol;
    else if (op[x][y] != symbol)
        conflict[x][y] = 1;
}

void makeOPM(char start) {
    int i, j, t;
    char *r;
    int len;

    for (i = 0; i < n; i++) {
        r = rhs[i];
        len = (int)strlen(r);

        for (j = 0; j < len - 1; j++) {
            /* Terminal next to terminal: = , A -> a+.. a = +*/
            if (!isNT(r[j]) && !isNT(r[j + 1]))
                addRelation(r[j], r[j + 1], '=');

            /* Terminal, non-terminal, terminal: = , F -> (E) ..( = )*/
            if (j + 2 < len && !isNT(r[j]) &&
                isNT(r[j + 1]) && !isNT(r[j + 2]))
                addRelation(r[j], r[j + 2], '=');

            /* Terminal before non-terminal: < FIRSTVT , A -> aB checks whether
            character t belongs to FIRSTVT of the non-terminal that follows a, and whether t is a terminal.
            For example, if + belongs to FIRSTVT(B), the program records:*/
            if (!isNT(r[j]) && isNT(r[j + 1]))
                for (t = 0; t < 128; t++)
                    if (firstVT[(unsigned char)r[j + 1]][t] && !isNT((char)t))
                        addRelation(r[j], (char)t, '<');

            /* Non-terminal before terminal: LASTVT > terminal , A -> B+
            The loop searches for all terminals in LASTVT(B).
            Suppose i belongs to LASTVT(B). The program records:i > +*/
            if (isNT(r[j]) && !isNT(r[j + 1]))
                for (t = 0; t < 128; t++)
                    if (lastVT[(unsigned char)r[j]][t] && !isNT((char)t))
                        addRelation((char)t, r[j + 1], '>');
        }
    }

    /* End marker $ */
    used[(unsigned char)'$'] = 1;
    for (t = 0; t < 128; t++) {
        if (firstVT[(unsigned char)start][t] && !isNT((char)t))
            addRelation('$', (char)t, '<');//for every terminal in FIRSTVT of the start symbol, record $ < terminal.
        if (lastVT[(unsigned char)start][t] && !isNT((char)t))
            addRelation((char)t, '$', '>');//for every terminal in LASTVT of the start symbol, record terminal > $.
    }
}

void printSets(void) {
    int a, t;

    printf("\nFIRSTVT:\n");
    for (a = 0; a < 128; a++)
        if (used[a] && isNT((char)a)) {
            printf("FIRSTVT(%c) = { ", a);
            for (t = 0; t < 128; t++)
                if (firstVT[a][t] && !isNT((char)t)) printf("%c ", t);
            printf("}\n");
        }

    printf("\nLASTVT:\n");
    for (a = 0; a < 128; a++)
        if (used[a] && isNT((char)a)) {
            printf("LASTVT(%c) = { ", a);
            for (t = 0; t < 128; t++)
                if (lastVT[a][t] && !isNT((char)t)) printf("%c ", t);
            printf("}\n");
        }
}

void printOPM(void) {
    int a, b, bad = 0;

    printf("\nOperator Precedence Matrix:\n    ");
    for (b = 0; b < 128; b++)
        if (used[b] && !isNT((char)b)) printf("%c   ", b);
    printf("\n");

    for (a = 0; a < 128; a++)
        if (used[a] && !isNT((char)a)) {
            printf("%c   ", a);
            for (b = 0; b < 128; b++)
                if (used[b] && !isNT((char)b)) {
                    if (conflict[a][b]) {
                        printf("!   ");
                        bad = 1;
                    } else if (op[a][b])
                        printf("%c   ", op[a][b]);
                    else
                        printf("-   ");
                }
            printf("\n");
        }

    if (bad)
        printf("\n! means conflicting relations: this is not a valid OPG.\n");
}

int main(void) {
    int i, j;

    inputGrammar();
    if (n == 0) {
        printf("No productions entered.\n");
        return 1;
    }

    /* Check the basic OPG restriction: no adjacent non-terminals. */
    for (i = 0; i < n; i++)
        for (j = 0; rhs[i][j] && rhs[i][j + 1]; j++)
            if (isNT(rhs[i][j]) && isNT(rhs[i][j + 1])) {
                printf("Invalid for this program: adjacent non-terminals in %c -> %s\n",
                       lhs[i], rhs[i]);
                return 1;
            }

    printf("\nProductions:\n");
    for (i = 0; i < n; i++)
        printf("%c -> %s\n", lhs[i], rhs[i]);

    findFirstLastVT();
    printSets();
    makeOPM(lhs[0]);
    printOPM();

    return 0;
}
