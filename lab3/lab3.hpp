#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>
#include <inttypes.h>
#include <algorithm>

using table_t = std::vector<std::string>;

class FSMReader
{
private:
    table_t states;
    table_t outs;
public:
    FSMReader(const std::string&);
    table_t& getStates() { return states; }
    table_t& getOuts() { return outs; }
};

typedef std::pair<int32_t, int32_t> SCouple;

class CompTableBase
{
private:
public:
    CompTableBase() = default;
};

class PreCompTable: public CompTableBase
{
private:
    std::map<SCouple, std::set<SCouple> > compTable;
    std::vector<SCouple> incomSt;
    size_t numStates{0};
public:
    PreCompTable(table_t states, table_t outs);

    std::map<SCouple, std::set<SCouple> >& getPreCompTable()
    {
        return this->compTable;
    }
    std::vector<SCouple>& getIncompTable()
    {
        return this->incomSt;
    }
    size_t getNumStates()
    {
        return this->numStates;
    }
};

class FCompTable: public CompTableBase
{
private:
    std::vector<std::vector<uint32_t> > fCompTable;
public:
    FCompTable(std::map<SCouple, std::set<SCouple>>& preCompTable, std::vector<SCouple>& incompStates, size_t numStates);
    std::vector<std::vector<uint32_t> > getFCompTable()
    {
        return this->fCompTable;
    }
};

class SMaxComp
{
private:
    std::set<std::set<uint32_t> > maxComps;
    std::vector<std::set<uint32_t>> maxCompsVec;
    uint32_t numPrevStates;
public:
    SMaxComp(const std::vector<std::vector<uint32_t> >& fCompTable);
    std::vector<std::set<uint32_t>> getSortedMaxComps();
    std::vector<std::set<uint32_t> > getMaxComps()
    {
        return this->maxCompsVec;
    }
};

void getMinStateMachine(const std::vector<std::set<uint32_t> >&, const table_t&, const table_t&);
