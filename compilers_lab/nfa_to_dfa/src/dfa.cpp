#include "dfa.hpp"

#include <map>
#include <queue>

int Dfa::add_state()
{
    states.push_back(DfaState{});
    return num_states++;
}

void Dfa::add_transition(int from, int to, char symbol)
{
    states[from].transitions.push_back({symbol, to});
}

namespace
{
    std::set<char> compute_alphabet(const Nfa &n)
    {
        std::set<char> alphabet;
        for (const auto &state : n.states)
            for (const auto &t : state.transitions)
                if (t.symbol != EPSILON)
                    alphabet.insert(t.symbol);
        return alphabet;
    }
}

Dfa nfa_to_dfa(const Nfa &n)
{
    Dfa d;
    std::set<char> alphabet = compute_alphabet(n);

    std::set<int> s0 = {n.start_state};
    epsilon_closure(n, s0);

    std::map<std::set<int>, int> id_de;
    std::vector<std::set<int>> subconjunto_de;

    int s0_id = d.add_state();
    id_de[s0] = s0_id;
    subconjunto_de.push_back(s0);
    d.start_state = s0_id;

    std::queue<int> cola;
    cola.push(s0_id);

    while (!cola.empty())
    {
        int u_id = cola.front();
        cola.pop();
        std::set<int> U = subconjunto_de[u_id];

        for (char a : alphabet)
        {
            std::set<int> V = move(n, U, a);
            epsilon_closure(n, V);

            if (V.empty())
                continue;

            auto it = id_de.find(V);
            int v_id;
            if (it == id_de.end())
            {
                v_id = d.add_state();
                id_de[V] = v_id;
                subconjunto_de.push_back(V);
                cola.push(v_id);
            }
            else
            {
                v_id = it->second;
            }

            d.add_transition(u_id, v_id, a);
        }
    }

    for (int i = 0; i < static_cast<int>(subconjunto_de.size()); ++i)
    {
        if (subconjunto_de[i].find(n.accept_state) != subconjunto_de[i].end())
            d.accept_states.insert(i);
    }

    return d;
}

bool match_dfa(const Dfa &d, const std::string &input)
{
    int current = d.start_state;

    for (char c : input)
    {
        bool found = false;
        for (const auto &t : d.states[current].transitions)
        {
            if (t.symbol == c)
            {
                current = t.to;
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }

    return d.accept_states.find(current) != d.accept_states.end();
}
