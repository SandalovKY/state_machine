#include "minimization.h"

namespace state_machine
{
int Impl::countSetBits(int num)
{
    int cnt{ 0 };
    while (num)
    {
        num &= (num - 1);
        ++cnt;
    }
    return cnt;
}

bool Impl::operator==(Impl& other) const
{
    return m_num == other.m_num && m_p == other.m_p;
}

std::shared_ptr<Impl> Impl::patch(Impl& lh, Impl& rh)
{
    if (lh == rh)
    {
        rh.m_p = -1;
        rh.m_pw = true;
        return nullptr;
    }
    if (lh.m_num < rh.m_num &&
        lh.m_p == rh.m_p &&
        rh.m_ind - lh.m_ind == 1 &&
        countSetBits(rh.m_num - lh.m_num) == 1)
    {
        lh.m_pw = true;
        rh.m_pw = true;
        Impl* nImplP = new Impl(lh);
        nImplP->m_p |= rh.m_num - lh.m_num;
        return std::shared_ptr<Impl>(nImplP);
    }
    return nullptr;
}

std::set<int> DNF::removeRedundancy(
    std::vector<std::set<int> > coverage,
    std::vector<std::set<int> > coverageOnBars)
{
    std::set<int> usedCols {};
    for (int i = 0; i < coverageOnBars.size(); ++i)
    {
        usedCols.insert(i);
    }
    std::set<int> ansver{};

    while (usedCols.size() != 0)
    {
        auto r = std::min_element(usedCols.begin(), usedCols.end(), [&coverageOnBars](const auto& a, const auto& b){
            return coverageOnBars[a].size() < coverageOnBars[b].size();
        });
        auto result = std::max_element(coverageOnBars[*r].begin(), coverageOnBars[*r].end(), [&coverage](int a, int b){
            return coverage[a].size() > coverage[b].size();
        });
        ansver.insert(*result);
        std::set<int> res{};
        std::set_difference(usedCols.begin(), usedCols.end(), coverage[*result].begin(), coverage[*result].end(), std::inserter(res, res.begin()));
        for (auto& el: coverage[*result])
        {
            coverageOnBars[el].erase(*result);
        }
        usedCols = res;
    }

    return ansver;
}

DNF::DNF(std::string sourceDNF)
{
    for (int pos = 0; pos < sourceDNF.size(); ++pos)
    {
        bool isInf = sourceDNF[pos] == '-';
        if (sourceDNF[pos] == '1' or isInf)
        {
            m_data.emplace_back(Impl(pos, isInf));
        }
    }
}

void DNF::minimize()
{
    std::vector<Impl> src{};
    for (const auto& el: m_data)
    {
        if (!el.isInf())
        {
            src.push_back(el);
        }
    }
    // First stage: getting TDNF (Terminated Disjunctive Normal FOrm)
    bool canMinimize{ true };
    while (canMinimize)
    {
        canMinimize = false;
        std::vector<Impl> newGenImpl{};
        for (int outerInd = 0; outerInd < m_data.size() - 1; ++outerInd)
        {
            for (int innerInd = outerInd + 1; innerInd < m_data.size(); ++innerInd)
            {
                Impl& implI = m_data[outerInd];
                Impl& implJ = m_data[innerInd];
                std::shared_ptr<Impl> newImpl = Impl::patch(implI, implJ);
                if (newImpl != nullptr)
                {
                    canMinimize |= true;
                    newGenImpl.push_back(*newImpl);
                }
            }
        }
        for (int ind = 0; ind < m_data.size(); ++ind)
        {
            if (!m_data[ind].isPatched())
            {
                newGenImpl.push_back(m_data[ind]);
            }
        }
        m_data = newGenImpl;
    }

    // Second stage: minimization of TDNF, getting MDNF (minimum)

    std::vector<std::set<int> > coverage{ m_data.size() };
    std::vector<std::set<int> > coverageOnBars{ src.size() };

    for (int ind = 0; ind < m_data.size(); ++ind)
    {
        std::set<int> implCov{};
        for (int jnd = 0; jnd < src.size(); ++jnd)
        {
            if ((src[jnd].getNum() & ~m_data[ind].getPatch()) == m_data[ind].getNum())
            {
                implCov.insert(jnd);
                coverageOnBars[jnd].insert(ind);
            }
        }
        coverage[ind] = std::move(implCov);
    }

    std::set<int> ansver = removeRedundancy(coverage, coverageOnBars);
    std::vector<Impl> new_data{};
    for (auto& el: ansver)
    {
        new_data.push_back(m_data[el]);
    }
    m_data = new_data;
}

void DNF::print(std::ostream& outStream) const
{
    for (const auto& impl: m_data)
    {
        std::bitset<4> patch(impl.getPatch());
        std::bitset<4> num(impl.getNum());
        for (int ind = patch.size() - 1; ind >= 0; --ind)
        {
            if (patch[ind] == 1)
            {
                outStream << '-';
            }
            else
            {
                outStream << num[ind];
            }
        }
        outStream << std::endl;
    }
}


DNF Reader::readDNF(std::string path)
{
    std::string retString{};
    std::ifstream inputFile{ path };
    if (inputFile.is_open())
    {
        getline(inputFile, retString);
        inputFile.close();
    }
    return DNF{ retString };
}
}

// int main(int argc, char* argv[])
// {
//     if (argc < 2)
//     {
//         std::cout << "USE: lab1 (input_file_name)\n";
//         return 0;
//     }
//     state_machine::DNF sourceDNF =  state_machine::Reader::readDNF(argv[1]);
//     sourceDNF.minimize();
//     std::ofstream out{ "mdnf.txt" };
//     sourceDNF.print(out);
//     out.close();
//     return 0;
// }