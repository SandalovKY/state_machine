#include "lab3.hpp"
#include <cstdlib>
#include <iostream>
#include <algorithm>

int main(void)
{
    FSMReader reader("input.txt");
    table_t& states = reader.getStates();
    table_t& outs = reader.getOuts();


    std::cout << "Source transitions and outs tables:\n";
    uint32_t cntr = 0;
    for (auto& state: states)
    {
        std::cout << cntr << ": " << state << std::endl;
    }
    std::cout << std::endl;
    cntr = 0;
    for (auto& out: outs)
    {
        std::cout << cntr << ": " << out << std::endl;
    }

    PreCompTable tbl(states, outs);

    std::map<SCouple, std::set<SCouple> >& preCompTable = tbl.getPreCompTable();
    std::vector<SCouple> incTbl = tbl.getIncompTable();

    for (auto& el: preCompTable)
    {
        std::cout << '[' << el.first.first << ',' << el.first.second << "] :\n";
        for (auto& vecEl: el.second)
        {
            std::cout << vecEl.first << '\t' << vecEl.second << '\n';
        }
    }

    FCompTable tbl1(preCompTable, incTbl, tbl.getNumStates());

    std::cout << "iNC TABLE:\n";

    for (auto& el: incTbl)
    {
        std::cout << el.first << '\t' << el.second << '\n';
    }
    SMaxComp maxComp(tbl1.getFCompTable());

    std::vector<std::set<uint32_t>> resMaxComps = maxComp.getSortedMaxComps();

    std::cout << "Res max comps\n";
    for (auto& comp: resMaxComps)
    {
        for (auto& state: comp)
        {
            std::cout << state << ' ';
        }
        std::cout << std::endl;
    }

    getMinStateMachine(resMaxComps, states, outs);

    return EXIT_SUCCESS;
}