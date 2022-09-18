#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>

class Impl
{
private:
    int m_num{};
    int m_ind{};
    int m_p{};
    bool m_pw{};

    static int countSetBits(int num)
    {
        int cnt{ 0 };
        while (num)
        {
            num &= (num - 1);
            ++cnt;
        }
        return cnt;
    }

public:
    Impl(int num) : m_num(num), m_ind(countSetBits(num)), m_p(0), m_pw(false) {}
    Impl(const Impl& other) : m_num(other.m_num), m_ind(other.m_ind), m_p(other.m_p), m_pw(false) {}
    bool operator==(Impl& other)
    {
        return m_num == other.m_num && m_p == other.m_p;
    }
    static std::shared_ptr<Impl> patch(Impl& lh, Impl& rh)
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
    int getNum() const { return m_num; }
    int getInd() const { return m_ind; }
    int getPatch() const { return m_p; }
    bool isPatched() const { return m_pw; } 
};


class DNF
{
private:
    std::vector<Impl> m_data;
public:
    DNF(std::string sourceDNF)
    {
        for (int pos = 0; pos < sourceDNF.size(); ++pos)
        {
            if (sourceDNF[pos] == '1' or sourceDNF[pos] == '-')
            {
                m_data.emplace_back(Impl(pos));
            }
        }
    }
    void minimize()
    {
        std::vector<Impl> src = m_data;
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

        std::vector<std::pair<int, int>>

        for (const auto& nt : m_data)
        {
            for (const auto& nt : src)
            {

            }
        }
    }
    void print(std::ostream& outStream) const
    {
        for (const auto& impl: m_data)
        {
            outStream << std::bitset<4>(impl.getNum()) << '\t' << std::bitset<4>(impl.getPatch()) << '\t' << impl.getInd() << std::endl;
        }
    }
    Impl& getImpl(int);
};


class Reader
{
public:
    static DNF readDNF(std::string path)
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
};


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "USE: lab1 (input_file_name)\n";
        return 0;
    }
    DNF sourceDNF =  Reader::readDNF(argv[1]);
    sourceDNF.print(std::cout);
    sourceDNF.minimize();
    std::cout << "\n-------------------------------------\n";
    sourceDNF.print(std::cout);
    return 0;
}