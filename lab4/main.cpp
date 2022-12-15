#include "lab4.hpp"
#include "iostream"

int main(void)
{
    const char *test = "x = 12 - 3;\ny = x * 2 - 3;\nk8 = y * ( 33 - x / 3 ) + 24;";
    EquationSolver solver(test);
    solver.printTokensMap(std::cout);
    return 0;
}
