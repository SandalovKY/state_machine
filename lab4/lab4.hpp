#include <map>
#include <string>
#include <vector>
#include <iostream>

class EquationSolver
{
private:
    std::map<std::string, std::vector<std::string>> m_tokens;

public:
    EquationSolver(const char *text);
    void printTokensMap(std::ostream &ostream);
};