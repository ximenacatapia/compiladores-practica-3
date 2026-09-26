#ifndef DFA_HPP
#define DFA_HPP

#include <set>
#include <string>
#include <vector>

#include "nfa.hpp"

struct DfaTransition
{
    char symbol;
    int to;
};

struct DfaState
{
    std::vector<DfaTransition> transitions;
};

struct Dfa
{
    int num_states = 0;
    std::vector<DfaState> states;
    int start_state = -1;
    std::set<int> accept_states;

    int add_state();
    void add_transition(int from, int to, char symbol);
};

Dfa nfa_to_dfa(const Nfa &n);
bool match_dfa(const Dfa &d, const std::string &input);

#endif
