# Extended Calculator
This is an exam project I had to develop for the course "Formal languages and Compilers" at the Free University of Bozen.

## Repo structure
This repo is divided in three branches:
- main: which contains the "finished product", alongside with pdf-copies of the report which accomplanies (and documents) the code and the guidelines of the assignment.
- report: the branch that contains the latex code used for generating the aforementioned report (alongside with a pdf-copy of it)
- template: which contains the template provided by the professor and a pdf-copy of the guidelines provided by the professor

## How to run
The code was made to be run with the help of lex and yacc, therefore in order to run it one has to have installed both these tools alongside with a c-compiler.
Instructions on how to install these can be found at the following links:
- [flex](https://github.com/westes/flex) (although any distribution of lex should do the trick)
- [yacc](https://silcnitc.github.io/install.html)
- [gcc](https://gcc.gnu.org/install/binaries.html)

**Note**: it is way easier to run everything on unix-based systems where you can install the first two with just a line of code (the third should come shipped with the system). For Windows users I'd recommend setting up [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) (I did it like that)

The three lines of code in order to compile the sources are:
```
flex -l lexer.l
yacc parser.y
gcc y.tab.c -ll
```
If you'd like to see a more detailed report on yacc issues you can add `-Wcounterexamples` after the yacc line.
