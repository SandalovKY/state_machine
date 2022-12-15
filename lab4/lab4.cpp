#include "lab4.hpp"

enum class STATES
{
    INITIATION,
    ID_KEY,
    EQUATION,
    ID_VALUED,
    NUMBER,
    EQ_TRANS,
    EQ_END,
    FINISHED,
    INVALID
};

bool isLetter(char sym)
{
    return (sym >= 97 && sym <= 122);
}

bool isDigit(char sym)
{
    return (sym >= 48 && sym <= 57);
}

bool isOperation(char sym)
{
    return (sym == '-' || sym == '+' || sym == '*' || sym == '/');
}

std::pair<std::string, std::vector<std::string>> stateMachineExecute(const char *text, uint32_t *startSymbol)
{
    STATES currState = STATES::INITIATION;
    char nextSymbol = {};
    std::string key{};
    std::string nextWord{};
    std::vector<std::string> tokens{};
    while (currState != STATES::FINISHED && currState != STATES::INVALID && nextSymbol != '\n')
    {
        while (text[*startSymbol] == ' ')
            ++*startSymbol;
        nextSymbol = text[*startSymbol];
        switch (currState)
        {
        case STATES::INITIATION:
            if (isLetter(nextSymbol))
            {
                currState = STATES::ID_KEY;
                key.push_back(nextSymbol);
            }
            else
            {
                currState = STATES::INVALID;
            }
            break;
        case STATES::ID_KEY:
            if (isLetter(nextSymbol) || isDigit(nextSymbol))
            {
                key.push_back(nextSymbol);
            }
            else if (nextSymbol == '=')
            {
                currState = STATES::EQUATION;
                nextWord.clear();
            }
            else
            {
                currState = STATES::INVALID;
            }
            break;
        case STATES::EQUATION:
            if (nextSymbol == '(')
            {
                tokens.push_back("(");
            }
            else if (isLetter(nextSymbol))
            {
                currState = STATES::ID_VALUED;
                nextWord.push_back(nextSymbol);
            }
            else if (isDigit(nextSymbol))
            {
                currState = STATES::NUMBER;
                nextWord.push_back(nextSymbol);
            }
            else
            {
                currState = STATES::INVALID;
            }
            break;
        case STATES::ID_VALUED:
            if (isLetter(nextSymbol) || isDigit(nextSymbol))
            {
                nextWord.push_back(nextSymbol);
            }
            if (isOperation(nextSymbol))
            {
                currState = STATES::EQUATION;
                tokens.push_back(nextWord);
                tokens.push_back({nextSymbol});
                nextWord.clear();
            }
            else if (nextSymbol == ')')
            {
                currState = STATES::EQ_TRANS;
                tokens.push_back(nextWord);
                tokens.push_back({nextSymbol});
                nextWord.clear();
            }
            else if (nextSymbol == ';')
            {
                currState = STATES::FINISHED;
                tokens.push_back(nextWord);
                tokens.push_back({nextSymbol});
                nextWord.clear();
            }
            else
            {
                currState = STATES::INVALID;
            }
            break;
        case STATES::NUMBER:
            if (isDigit(nextSymbol))
            {
                nextWord.push_back(nextSymbol);
            }
            else if (isOperation(nextSymbol))
            {
                currState = STATES::EQUATION;
                tokens.push_back(nextWord);
                tokens.push_back({nextSymbol});
                nextWord.clear();
            }
            else if (nextSymbol == ')')
            {
                currState = STATES::EQ_TRANS;
                tokens.push_back(nextWord);
                tokens.push_back({nextSymbol});
                nextWord.clear();
            }
            else if (nextSymbol == ';')
            {
                currState = STATES::FINISHED;
                tokens.push_back(nextWord);
                tokens.push_back({nextSymbol});
                nextWord.clear();
            }
            else
            {
                currState = STATES::INVALID;
            }
            break;
        case STATES::EQ_TRANS:
            if (isOperation(nextSymbol))
            {
                currState = STATES::EQUATION;
                tokens.push_back({nextSymbol});
            }
            else if (nextSymbol == ')')
            {
                currState = STATES::EQ_TRANS;
                tokens.push_back({nextSymbol});
            }
            else if (nextSymbol == ';')
            {
                currState = STATES::FINISHED;
                tokens.push_back({nextSymbol});
            }
            else
            {
                currState = STATES::INVALID;
            }
            break;
        case STATES::EQ_END:
            currState = STATES::FINISHED;
            break;
        default:
            break;
        }
        ++*startSymbol;
    }
    return std::make_pair(key, tokens);
}

std::vector<std::string> getRPN(const std::vector<std::string> &srcTokens)
{
    static std::map<std::string, std::pair<int32_t, int32_t>> op2Priority{
        {"(", {3, -1}},
        {")", {0, NULL}},
        {"+", {1, 1}},
        {"-", {1, 1}},
        {"*", {2, 2}},
        {"/", {2, 2}},
        {";", {2, NULL}},
        {"@", {NULL, -2}}};
    std::vector<std::string> retRPN{};
    std::vector<std::string> stack{"@"};

    return retRPN;
}

EquationSolver::EquationSolver(const char *text) : m_tokens()
{
    uint32_t ind = 0;
    // Read whole file
    while (text[ind] != '\0')
    {
        // Read line
        while (text[ind] != '\n' && text[ind] != '\0')
        {
            m_tokens.insert(stateMachineExecute(text, &ind));
            ++ind;
        }
    }
}
void EquationSolver::printTokensMap(std::ostream &ostream)
{
    for (auto &el : m_tokens)
    {
        ostream << el.first << ": ";
        for (auto &token : el.second)
        {
            ostream << token << ' ';
        }
        ostream << '\n';
    }
}
