#ifndef DFA_MIN_HPP
#define DFA_MIN_HPP

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "dfa.hpp"

struct DFA
{
    std::set<int> states;
    std::set<char> alphabet;
    int start_state = -1;
    std::set<int> accept_states;
    std::map<std::pair<int, char>, int> transitions;
};

DFA to_template_dfa(const Dfa &d);

void print_dfa(const DFA &dfa);
void print_dfa_min(const DFA &dfa_min);
bool test_string(const DFA &dfa, const std::string &input);
int run_test_suite(const DFA &dfa,
                   const std::vector<std::string> &accept_tests,
                   const std::vector<std::string> &reject_tests);

DFA minimize_dfa(const DFA &dfa);

#endif