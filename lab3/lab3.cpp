#include "lab3.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>

#define MAX_LINE_SIZE 1000

FSMReader::FSMReader(const std::string& path)
{
    std::ifstream file;
    file.open(path, std::ios::in);

    std::string inputLine;
    inputLine.reserve(MAX_LINE_SIZE);

    std::vector<std::string> *table = nullptr;

    uint32_t numStates{ 0 };
    uint32_t numIns{ 0 };

    if (file.is_open())
    {
        std::getline(file, inputLine, '\n');
        std::stringstream sstream_init(inputLine);
        sstream_init >> numStates >> numIns;
        this->outs.resize(numStates);
        this->states.resize(numStates);

        while (std::getline(file, inputLine, '\n'))
        {
            if (inputLine.find("#") != std::string::npos)
            {
                std::stringstream sstream(inputLine);
                std::vector<std::string> parsedLine(std::istream_iterator<std::string>{sstream},
                                                    std::istream_iterator<std::string>());
                if (parsedLine.size() != 2)
                {
                    std::cerr << "Incompatible format\n";
                    return;
                }
                if (parsedLine[1] == "states") table = &(this->states);
                else if (parsedLine[1] == "outs") table = &(this->outs); 
            }
            else if (table != nullptr)
            {
                std::stringstream sstream(inputLine);
                std::vector<char> parsedLine(std::istream_iterator<char>{sstream},
                                                    std::istream_iterator<char>());

                if (parsedLine.size() != numStates)
                {
                    std::cerr << "Incompatible format\n";
                    return;
                }
                for (int i = 0; i < parsedLine.size(); ++i)
                {
                    if ((*table)[i].size() == numIns)
                    {
                        std::cerr << "Incompatible format\n";
                        return;
                    }
                    (*table)[i].push_back(parsedLine[i]);
                }
            }
            else
            {
                std::cerr << "Incompatible format\n";
                return;
            }

        }
    }
}

bool compareStates(const std::string& st1, const std::string& st2)
{
    bool areConsistent = true;

    if (st1.size() != st2.size()) return false;

    for (int i = 0; i < st1.size(); ++i)
    {
        areConsistent &= st1[i] == '-' || st2[i] == '-' || st1[i] == st2[i];
    }

    return areConsistent;
}

std::set<SCouple> getNextLevelPairs(const std::string& st1, const std::string& st2, uint8_t ist1, uint8_t ist2)
{
    std::set<SCouple> retVector{};
    if (st1.size() != st2.size()) return retVector;

    for (int i = 0; i < st1.size(); ++i)
    {
        if (st1[i] != '-' && st2[i] != '-' && st1[i] != st2[i])
        {
            uint8_t first = st1[i] - '0' > st2[i] - '0' ? (st2[i] - '0') - 1 : (st1[i] - '0') - 1;
            uint8_t second = st1[i] - '0' < st2[i] - '0' ? (st2[i] - '0') - 1 : (st1[i] - '0') - 1;
            if (std::make_pair(ist1, ist2) != std::make_pair(first, second)) retVector.insert({first, second});
        }
    }
    return retVector;
}

PreCompTable::PreCompTable(table_t states, table_t outs)
{
    numStates = states.size();
    for (int i = 0; i < states.size() - 1; ++i)
    {
        for (int j = i + 1; j < states.size(); ++j)
        {
            std::vector<SCouple> el;
            if (compareStates(outs[i], outs[j]))
            {
                this->compTable[{i, j}] = getNextLevelPairs(states[i], states[j], i, j);
            }
            else
            {
                this->compTable[{i, j}] = std::set<SCouple>{ {-1, -1} };
                this->incomSt.push_back({i, j});
            }
        }
    }
}

FCompTable::FCompTable(std::map<SCouple, std::set<SCouple>>& preCompTable, std::vector<SCouple>& incompStates, size_t numStates)
{
    std::vector<SCouple> newIncomp(incompStates);
    while (newIncomp.size())
    {
        SCouple inc = *(--newIncomp.end());
        newIncomp.pop_back();
        for (const auto& el: preCompTable)
        {
            if (el.second.find(inc) != el.second.end())
            {
                newIncomp.push_back(el.first);
                incompStates.push_back(el.first);
            }
        }
    }

    for (auto& incompPair: incompStates)
    {
        auto foundIncomp = preCompTable.find(incompPair);
        if (foundIncomp != preCompTable.end())
        {
            foundIncomp->second = std::set<SCouple>{SCouple{-1, -1}};
        }
    }

    fCompTable.resize(numStates);

    for (auto& compPair: preCompTable)
    {
        if (compPair.second != std::set<SCouple>{SCouple{-1, -1}})
        {
            fCompTable[compPair.first.first].push_back(compPair.first.second);
        }
    }
}

SMaxComp::SMaxComp(const std::vector<std::vector<uint32_t> >& fCompTable)
{
    numPrevStates = fCompTable.size();
    for (int32_t i = fCompTable.size() - 1; i >= 0; --i)
    {
        if(fCompTable[i].size() == 0)
        {
            maxComps.insert({static_cast<uint32_t>(i)});
        }
        else
        {
            std::set<uint32_t> addends{};
            for (const auto& st: fCompTable[i])
            {
                addends.insert(st);
            }
            std::set<std::set<uint32_t> > newInserters{};
            for (auto& maxSet: maxComps)
            {
                std::set<uint32_t> intersection;
                std::set_intersection(maxSet.begin(),
                    maxSet.end(), addends.begin(), addends.end(),
                    std::inserter(intersection, intersection.begin()));
                intersection.insert(i);
                newInserters.insert(intersection);
            }
            for (const auto& insrtr: newInserters)
            {
                bool needToInsert{true};
                std::vector<std::set<uint32_t>> instrs2Remove{};
                for (const auto& ready: maxComps)
                {
                    std::set<uint32_t> intersection;
                    std::set_intersection(insrtr.begin(),
                        insrtr.end(), ready.begin(), ready.end(),
                        std::inserter(intersection, intersection.begin()));
                    if (intersection == ready)
                    {
                        instrs2Remove.push_back(ready);
                    }
                    else if (intersection == insrtr)
                    {
                        needToInsert = false;
                    }
                }
                for (const auto& ins2rem: instrs2Remove)
                {
                    maxComps.erase(ins2rem);
                }
                if (needToInsert)
                {
                    maxComps.insert(insrtr);
                }
            }
        }
    }
    for (const auto& set: maxComps)
    {
        maxCompsVec.push_back(set);
    }
}

std::vector<std::set<uint32_t>> SMaxComp::getSortedMaxComps()
{
    std::vector<std::set<uint32_t>> retSortedVector{};
    std::vector<std::pair<uint32_t, std::vector<uint32_t>>> prevStates{numPrevStates};

    for (int ind = 0; ind < prevStates.size(); ++ind)
    {
        for (int ind_inn = 0; ind_inn < maxCompsVec.size(); ++ind_inn)
        {
            if (maxCompsVec[ind_inn].find(ind) != maxCompsVec[ind_inn].end())
            {
                prevStates[ind].first = ind;
                prevStates[ind].second.push_back(ind_inn);
            }
        }
    }
    while (retSortedVector.size() < maxCompsVec.size())
    {
        std::sort(prevStates.begin(), prevStates.end(), [](const auto& a, const auto& b){
            return a.second.size() < b.second.size();
        });
        auto it = prevStates.begin();
        while (it->second.size() == 0) ++it;
        std::sort(it->second.begin(), it->second.end(), [this](const auto& a, const auto& b){
            return this->maxCompsVec[a].size() > this->maxCompsVec[b].size();
        });
        retSortedVector.push_back(maxCompsVec[*(it->second.begin())]);
        uint32_t findInd = *(it->second.begin());

        // prevStates.erase(prevStates.begin());
        for (auto& el: prevStates)
        {
            auto it = std::find_if(el.second.begin(), el.second.end(), [&findInd](uint32_t a){
                return a == findInd;
            });
            if (it != el.second.end())
            {
                el.second.erase(it);
            }
        }
    }
    return retSortedVector;
}

std::vector<std::set<uint32_t>> getNewStates(const std::set<uint32_t>& maxSet, const table_t& st1)
{
    std::vector<std::set<uint32_t>> retVector{};
    if (st1.begin()->size() != st1.rbegin()->size()) return retVector;

    for (int i = 0; i < st1.begin()->size(); ++i)
    {
        std::set<uint32_t> resSet{};
        for (auto& state: maxSet)
        {
            if (st1[state][i] != '-') resSet.insert(st1[state][i] - '0' - 1);
        }
        retVector.push_back(std::move(resSet));
    }
    return retVector;
}
std::vector<char> getNewOuts(const std::set<uint32_t>& maxSet, const table_t& st1)
{
    std::vector<char> retVector{};
    if (st1.begin()->size() != st1.rbegin()->size()) return retVector;

    for (int i = 0; i < st1.begin()->size(); ++i)
    {
        char retSym = '-';
        for (auto& state: maxSet)
        {
            if (st1[state][i] != '-') {
                retSym = st1[state][i];
                break;
            }
        }
        retVector.push_back(retSym);
    }
    return retVector;
}

void getMinStateMachine(const std::vector<std::set<uint32_t> >& maxCompSets, const table_t& states, const table_t& outs)
{
    int newStateIndex = 0;
    std::vector<std::vector<char>> newOuts(maxCompSets.size());
    std::vector<std::vector<char>> newStates{(maxCompSets.size())};
    std::vector<std::set<uint32_t>> newStatesVec{};
    newStatesVec.push_back(*maxCompSets.begin());
    std::vector<std::pair<uint32_t, std::set<uint32_t>>> unresolvedStates{{newStateIndex++, *maxCompSets.begin()}};
    int cntr = 0;
    while (!unresolvedStates.empty() && cntr < 15) {
        std::set<uint32_t> nextUnresolved = unresolvedStates.rbegin()->second;
        std::vector<std::set<uint32_t>> next = getNewStates(nextUnresolved, states);
        uint32_t ind = unresolvedStates.rbegin()->first;
        unresolvedStates.pop_back();
        newOuts[ind] = getNewOuts(nextUnresolved, outs);

        for (const auto& state: next) {
            if (state.empty()) {
                newStates[ind].push_back('-');
                continue;
            }
            bool stateResolved{false};
            if (std::includes(nextUnresolved.begin(), nextUnresolved.end(), state.begin(), state.end())) {
                stateResolved = true;
                newStates[ind].push_back(ind + '0' + 1);
            }
            if (!stateResolved)
            for (int i = 0; i < newStatesVec.size(); ++i) {
                if (std::includes(newStatesVec[i].begin(), newStatesVec[i].end(), state.begin(), state.end())) {
                    stateResolved = true;
                    newStates[ind].push_back(i + '0' + 1);
                    break;
                }
            }
            if (!stateResolved) {
                for (const auto& otherState: maxCompSets) {
                    if (std::includes(otherState.begin(), otherState.end(), state.begin(), state.end())) {
                        unresolvedStates.push_back({newStateIndex++, otherState});
                        newStates[ind].push_back(newStateIndex + '0');
                        newStatesVec.push_back(otherState);
                        break;
                    }
                }
            }
        }
        ++cntr;
    }
    std::cout << "New states\n";
    for (int i = 0; i < newStatesVec.size(); ++i) {
        std::cout << i+1 << ":\n";
        for (auto& el: newStates[i]) {
            std::cout << el << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "New outs\n";
    for (int i = 0; i < newStatesVec.size(); ++i) {
        std::cout << i+1 << ":\n";
        for (auto& el: newOuts[i]) {
            std::cout << el << ' ';
        }
        std::cout << std::endl;
    }
}
