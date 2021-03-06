/* main rpn parser and lexical analysis, part of the RPN calculator */
#include <rational>
#include <string>

enum token
    {
              t_type,           /* operator or token type */
    Rational: t_value,          /* value, if t_type is "Number" */
              t_word[20],       /* raw string */
    }

const Number    = '0'
const EndOfExpr = '#'

rpncalc(const string[])
    {
    new index
    new field[token]
    for ( ;; )
        {
        field = gettoken(string, index)
        switch (field[t_type])
            {
            case Number:
                push field[t_value]
            case '+':
                push pop() + pop()
            case '-':
                push - pop() + pop()
            case '*':
                push pop() * pop()
            case '/', ':':
                push 1.0 / pop() * pop()
            case EndOfExpr:
                break   /* exit "for" loop */
            default:
                printf "Unknown operator '%s'\n", field[t_word]
            }
        }
    printf "Result = %r\n", pop()
    if (clearstack())
        print "Stack not empty\n", red
    }

gettoken(const string[], &index)
    {
    /* first get the next "word" from the string */
    new word[20]
    word = strtok(string, index)

    /* then parse it */
    new field[token]
    field[t_word] = word
    if (strlen(word) == 0)
        {
        field[t_type] = EndOfExpr /* special "stop" symbol */
        field[t_value] = 0
        }
    else if ('0' <= word[0] <= '9')
        {
        field[t_type] = Number
        field[t_value] = rval(word)
        }
    else
        {
        field[t_type] = word[0]
        field[t_value] = 0
        }

    return field
    }
