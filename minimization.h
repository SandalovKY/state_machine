#pragma once

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <iostream>
#include <bitset>
#include <set>
#include <algorithm>

namespace state_machine
{
class Impl
{
private:
    int m_num{};
    int m_ind{};
    int m_p{};
    bool m_pw{};
    bool m_inf{};

    static int countSetBits(int num);

public:
    Impl(int num, bool inf)
    :   m_num(num),
        m_ind(countSetBits(num)),
        m_p(0),
        m_pw(false),
        m_inf(inf){}
    // Due to semantic of the creation the new implicant, its m_pw value should be false
    Impl(const Impl& other)
    :   m_num(other.m_num),
        m_ind(other.m_ind),
        m_p(other.m_p),
        m_pw(false),
        m_inf(other.m_inf){}
    bool operator==(Impl&) const;
    static std::shared_ptr<Impl> patch(Impl&, Impl&);
    int getNum() const { return m_num; }
    int getInd() const { return m_ind; }
    int getPatch() const { return m_p; }
    bool isPatched() const { return m_pw; }
    bool isInf() const { return m_inf; }
};


class DNF
{
private:
    std::vector<Impl> m_data;

    std::set<int> removeRedundancy(
        std::vector<std::set<int> >,
        std::vector<std::set<int> >);
public:
    DNF(std::string);
    void minimize();
    void print(std::ostream&) const;
};


class Reader
{
public:
    static DNF readDNF(std::string);
};
} // namespace state_machine