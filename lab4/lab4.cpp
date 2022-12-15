#include "lab4.hpp"

static int32_t g_index = 0;

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

enum class RPN_STATES
{
    S0,
    S1
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

int32_t EquationSolver::solve(const std::vector<uint32_t> &rpn)
{
    int32_t result{0};
    auto it = rpn.begin();
    std::vector<double> stack{0.};
    while (!(((*it) & 2 << 30) != 0 && ((*it) & 0xff) == ';'))
    {
        if (((*it) & 2 << 30) != 0)
        {
            const double op2 = *stack.rbegin();
            stack.pop_back();
            double op1 = *stack.rbegin();
            stack.pop_back();
            switch ((*it) & 0xff)
            {
            case '+':
                op1 += op2;
                break;
            case '-':
                op1 -= op2;
                break;
            case '*':
                op1 *= op2;
                break;
            case '/':
                op1 /= op2;
                break;
            default:
                break;
            }
            stack.push_back(op1);
        }
        else if (((*it) & 1 << 30) != 0)
        {
            int32_t val = m_indexes[(*it) & 0x3fffffff];
            stack.push_back(val);
        }
        else
        {
            stack.push_back((*it));
        }
        ++it;
    }
    result = *stack.rbegin();
    return result;
}

std::vector<uint32_t> getRPN(const std::vector<uint32_t> &srcTokens)
{
    static std::map<char, std::pair<int32_t, int32_t>> op2Priority{
        {'(', {3, -1}},
        {')', {0, NULL}},
        {'+', {1, 1}},
        {'-', {1, 1}},
        {'*', {2, 2}},
        {'/', {2, 2}},
        {';', {-1, NULL}},
        {'@', {NULL, -2}}};
    std::vector<uint32_t> retRPN{};
    std::vector<uint32_t> stack{static_cast<uint32_t>(2 << 30 | '@')};

    RPN_STATES currState = RPN_STATES::S0;

    auto it = srcTokens.begin();

    while (it != srcTokens.end())
    {
        switch (currState)
        {
        case RPN_STATES::S0:
            if (((*it) & (2 << 30)) != 0)
            {
                if (op2Priority.at((*it) & 0xff).first <= op2Priority.at((*stack.rbegin()) & 0xff).second)
                {
                    retRPN.push_back(*stack.rbegin());
                    stack.pop_back();
                    currState = RPN_STATES::S1;
                }
                else
                {
                    if (((*it) & 0xff) != ')' && ((*it) & 0xff) != ';')
                    {
                        stack.push_back(*it);
                        ++it;
                    }
                }
            }
            else
            {
                retRPN.push_back(*it);
                ++it;
            }
            break;
        case RPN_STATES::S1:
            if (((*it) & (2 << 30)) != 0)
            {
                if (op2Priority.at(*it).first <= op2Priority.at(*stack.rbegin()).second)
                {
                    retRPN.push_back(*stack.rbegin());
                    stack.pop_back();
                }
                else
                {
                    if (((*it) & 0xff) != ')' && ((*it) & 0xff) != ';')
                    {
                        stack.push_back(*it);
                        ++it;
                        currState = RPN_STATES::S0;
                    }
                    if (((*it) & 0xff) == ')')
                    {
                        stack.pop_back();
                        ++it;
                        currState = RPN_STATES::S0;
                    }
                    if (((*it) & 0xff) == ';')
                    {
                        retRPN.push_back(*it);
                        currState = RPN_STATES::S0;
                        ++it;
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    return retRPN;
}

std::pair<std::string, std::vector<uint32_t>> EquationSolver::stateMachineExecute(const char *text, uint32_t *startSymbol)
{
    STATES currState = STATES::INITIATION;
    char nextSymbol = {};
    std::string key{};
    std::string nextWord{};
    std::vector<uint32_t> tokens{};
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
                uint32_t newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
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
                uint32_t newToken = 1 << 30 | m_variables.at(nextWord);
                tokens.push_back(newToken);
                newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
                nextWord.clear();
            }
            else if (nextSymbol == ')')
            {
                currState = STATES::EQ_TRANS;
                uint32_t newToken = 1 << 30 | m_variables.at(nextWord);
                tokens.push_back(newToken);
                newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
                nextWord.clear();
            }
            else if (nextSymbol == ';')
            {
                currState = STATES::FINISHED;
                uint32_t newToken = 1 << 30 | m_variables.at(nextWord);
                tokens.push_back(newToken);
                newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
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
                uint32_t newToken = 0 | std::stoi(nextWord);
                tokens.push_back(newToken);
                newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
                nextWord.clear();
            }
            else if (nextSymbol == ')')
            {
                currState = STATES::EQ_TRANS;
                uint32_t newToken = 0 | std::stoi(nextWord);
                tokens.push_back(newToken);
                newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
                nextWord.clear();
            }
            else if (nextSymbol == ';')
            {
                currState = STATES::FINISHED;
                uint32_t newToken = 0 | std::stoi(nextWord);
                tokens.push_back(newToken);
                newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
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
                uint32_t newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
            }
            else if (nextSymbol == ')')
            {
                currState = STATES::EQ_TRANS;
                uint32_t newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
            }
            else if (nextSymbol == ';')
            {
                currState = STATES::FINISHED;
                uint32_t newToken = 2 << 30 | nextSymbol;
                tokens.push_back(newToken);
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
    return std::make_pair(key, getRPN(tokens));
}

EquationSolver::EquationSolver(const char *text) : m_tokens(), m_variables(), m_indexes()
{
    uint32_t ind = 0;
    // Read whole file
    while (text[ind] != '\0')
    {
        // Read line
        while (text[ind] != '\n' && text[ind] != '\0')
        {
            std::pair<std::string, std::vector<uint32_t>> res = stateMachineExecute(text, &ind);
            if (m_variables.find(res.first) == m_variables.end())
            {
                m_variables[res.first] = g_index;
                int32_t solved = solve(res.second);
                m_indexes[g_index++] = solved;
            }
            m_tokens.push_back(std::move(res));
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
