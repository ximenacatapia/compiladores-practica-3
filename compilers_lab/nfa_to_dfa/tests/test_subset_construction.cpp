#include "../src/regex.hpp"
#include "../src/nfa.hpp"
#include "../src/dfa.hpp"

#include <cassert>
#include <iostream>
#include <map>
#include <set>

void print_nfa(const Nfa &n)
{
    std::cout << "========== NFA ==========\n";

    std::cout << "Start state: " << n.start_state << '\n';
    std::cout << "Accept state: " << n.accept_state << "\n\n";

    for (int from = 0; from < n.num_states; ++from)
    {
        for (const auto &t : n.states[from].transitions)
        {
            std::cout << from << " --";

            if (t.symbol == EPSILON)
                std::cout << "epsilon";
            else
                std::cout << t.symbol;

            std::cout << "--> " << t.to << '\n';
        }
    }

    std::cout << '\n';
}

void print_dfa(const Dfa &d)
{
    std::cout << "========== DFA ==========\n";

    std::cout << "Start state: S" << d.start_state << '\n';

    std::cout << "Accept states: { ";
    for (int state : d.accept_states)
        std::cout << "S" << state << " ";
    std::cout << "}\n\n";

    for (int from = 0; from < d.num_states; ++from)
    {
        for (const auto &t : d.states[from].transitions)
        {
            std::cout
                << "S" << from
                << " --" << t.symbol
                << "--> S" << t.to
                << '\n';
        }
    }

    std::cout << '\n';
}

int transition_to(const Dfa &d, int from, char symbol)
{
    for (const auto &t : d.states[from].transitions)
    {
        if (t.symbol == symbol)
            return t.to;
    }

    return -1;
}

bool has_epsilon_transitions(const Dfa &d)
{
    for (const auto &state : d.states)
    {
        for (const auto &t : state.transitions)
        {
            if (t.symbol == EPSILON)
                return true;
        }
    }

    return false;
}

bool is_deterministic(const Dfa &d)
{
    for (const auto &state : d.states)
    {
        std::set<char> symbols;

        for (const auto &t : state.transitions)
        {
            if (symbols.find(t.symbol) != symbols.end())
                return false;

            symbols.insert(t.symbol);
        }
    }

    return true;
}

int main()
{
    Regex regex = parse_regex("ab|c*");

    Nfa nfa = regex_to_nfa(regex);

    std::cout << "\nRegex: ab|c*\n\n";

    print_nfa(nfa);

    Dfa dfa = nfa_to_dfa(nfa);

    print_dfa(dfa);

    /*
     * DFA esperado según la construcción manual:
     *
     * S0 --a--> S1
     * S0 --c--> S2
     * S1 --b--> S3
     * S2 --c--> S2
     *
     * Estados finales:
     * S0, S2, S3
     */

    assert(dfa.num_states == 4);

    assert(dfa.start_state == 0);

    assert(transition_to(dfa, 0, 'a') == 1);
    assert(transition_to(dfa, 0, 'b') == -1);
    assert(transition_to(dfa, 0, 'c') == 2);

    assert(transition_to(dfa, 1, 'a') == -1);
    assert(transition_to(dfa, 1, 'b') == 3);
    assert(transition_to(dfa, 1, 'c') == -1);

    assert(transition_to(dfa, 2, 'a') == -1);
    assert(transition_to(dfa, 2, 'b') == -1);
    assert(transition_to(dfa, 2, 'c') == 2);

    assert(transition_to(dfa, 3, 'a') == -1);
    assert(transition_to(dfa, 3, 'b') == -1);
    assert(transition_to(dfa, 3, 'c') == -1);

    assert(dfa.accept_states == std::set<int>({0, 2, 3}));

    assert(!has_epsilon_transitions(dfa));

    assert(is_deterministic(dfa));

    std::cout << "========== VALIDATION ==========\n";
    std::cout << "[OK] Expected number of DFA states\n";
    std::cout << "[OK] Expected initial state\n";
    std::cout << "[OK] Expected transitions\n";
    std::cout << "[OK] Expected accepting states\n";
    std::cout << "[OK] No epsilon transitions in DFA\n";
    std::cout << "[OK] DFA is deterministic\n";
    std::cout << "\nSubset construction test passed.\n";

    free_nfa(nfa);

    return 0;
}