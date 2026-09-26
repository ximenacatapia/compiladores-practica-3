#include "dfa_min.hpp"

#include <iostream>

DFA to_template_dfa(const Dfa &d)
{
    DFA out;
    out.start_state = d.start_state;
    out.accept_states = d.accept_states;

    for (int q = 0; q < d.num_states; ++q)
    {
        out.states.insert(q);
        for (const DfaTransition &t : d.states[q].transitions)
        {
            out.transitions[{q, t.symbol}] = t.to;
            out.alphabet.insert(t.symbol);
        }
    }
    return out;
}

namespace
{
    void print_table(const DFA &dfa)
    {
        std::cout << "Numero de estados: " << dfa.states.size() << "\n";
        std::cout << "Alfabeto: ";
        for (char c : dfa.alphabet)
            std::cout << c << " ";
        std::cout << "\nEstado Inicial: " << dfa.start_state << "\n";
        std::cout << "Estados de Aceptacion: ";
        for (int f : dfa.accept_states)
            std::cout << f << " ";
        std::cout << "\nTransiciones:\n";
        for (const auto &entry : dfa.transitions)
        {
            std::cout << "  d(" << entry.first.first << ", '"
                      << entry.first.second << "') -> " << entry.second << "\n";
        }
    }
}

void print_dfa(const DFA &dfa)
{
    std::cout << "--- Tabla de Transiciones (DFA Original) ---\n";
    print_table(dfa);
}

void print_dfa_min(const DFA &dfa_min)
{
    std::cout << "--- Tabla de Transiciones (DFA Minimizado) ---\n";
    print_table(dfa_min);
}

bool test_string(const DFA &dfa, const std::string &input)
{
    int current = dfa.start_state;
    for (char c : input)
    {
        auto it = dfa.transitions.find({current, c});
        if (it == dfa.transitions.end())
            return false;
        current = it->second;
    }
    return dfa.accept_states.count(current) > 0;
}

int run_test_suite(const DFA &dfa,
                   const std::vector<std::string> &accept_tests,
                   const std::vector<std::string> &reject_tests)
{
    int passed = 0;
    int total = static_cast<int>(accept_tests.size() + reject_tests.size());

    std::cout << "\n[Corriendo Casos de Aceptacion]\n";
    for (const auto &s : accept_tests)
    {
        bool res = test_string(dfa, s);
        std::cout << "Cadena \"" << s << "\": " << (res ? "PASS" : "FAIL") << "\n";
        if (res)
            passed++;
    }

    std::cout << "\n[Corriendo Casos de Rechazo]\n";
    for (const auto &s : reject_tests)
    {
        bool res = !test_string(dfa, s);
        std::cout << "Cadena \"" << s << "\": " << (res ? "PASS" : "FAIL") << "\n";
        if (res)
            passed++;
    }

    std::cout << "\nResultado: " << passed << "/" << total << " pruebas superadas.\n";
    return total - passed;
}