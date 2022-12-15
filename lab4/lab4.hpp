#include <map>
#include <string>
#include <vector>
#include <iostream>

class EquationSolver
{
private:
    std::vector<std::pair<std::string, std::vector<uint32_t>>> m_tokens;
    std::map<std::string, uint32_t> m_variables;
    std::map<uint32_t, int32_t> m_indexes;

    int32_t solve(const std::vector<uint32_t> &);
    std::pair<std::string, std::vector<uint32_t>> stateMachineExecute(const char *text, uint32_t *startSymbol);

public:
    EquationSolver(const char *text);
    void printTokensMap(std::ostream &ostream);
};