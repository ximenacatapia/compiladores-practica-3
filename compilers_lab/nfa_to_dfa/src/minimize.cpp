#include "dfa_min.hpp"
#include <deque>
#include <algorithm>
#include <queue>

namespace
{

    using Block = std::set<int>;

    DFA remove_unreachable(const DFA &dfa){
        std::set<int> reachable;
        std::queue<int> q;
        reachable.insert(dfa.start_state);
        q.push(dfa.start_state);
        while (!q.empty()){
            int u = q.front();
            q.pop();
            for (char c : dfa.alphabet)
            {
                auto it = dfa.transitions.find({u, c});
                if (it != dfa.transitions.end())
                {
                    int v = it->second;
                    if (reachable.insert(v).second)
                        q.push(v);
                }
            }
        }

        DFA out;
        out.alphabet = dfa.alphabet;
        out.start_state = dfa.start_state;
        out.states = reachable;

        for (int s : reachable)
            if (dfa.accept_states.count(s))
                out.accept_states.insert(s);

        for (const auto &entry : dfa.transitions){
            int from = entry.first.first;
            int to = entry.second;
            if (reachable.count(from) && reachable.count(to))
                out.transitions[entry.first] = to;
        }

        return out;
    }


    DFA complete_with_sink(const DFA &dfa, int &sink_id){

        sink_id = -1;
        bool needs_sink = false;
        for (int s : dfa.states){
            for (char c : dfa.alphabet){
                if (dfa.transitions.find({s, c}) == dfa.transitions.end()){
                    needs_sink = true;
                    break;
                }
            }
            if (needs_sink)
                break;
        }

        if (!needs_sink)
            return dfa;

        DFA out = dfa;
        sink_id = dfa.states.empty() ? 0 : (*dfa.states.rbegin() + 1);
        out.states.insert(sink_id);

        for (int s : out.states)
            for (char c : out.alphabet)
                if (out.transitions.find({s, c}) == out.transitions.end())
                    out.transitions[{s, c}] = sink_id;

        return out;
    }


    Block preimage(const DFA &dfa, const Block &A, char c){
        Block X;
        for (const auto &entry : dfa.transitions){
            if (entry.first.second == c && A.count(entry.second))
                X.insert(entry.first.first);
        }
        return X;
    }


    std::vector<Block> hopcroft(const DFA &dfa){
        const Block &F = dfa.accept_states;
        Block Q_minus_F;

        for (int s : dfa.states)
            if (!F.count(s))
                Q_minus_F.insert(s);

        std::vector<Block> P;
        std::deque<Block> W; 

        if (!F.empty())
            P.push_back(F);

        if (!Q_minus_F.empty())
            P.push_back(Q_minus_F);

        if (!F.empty() && !Q_minus_F.empty()){
            W.push_back(F);
            W.push_back(Q_minus_F);
        }

        while (!W.empty()){
            Block A = W.front();
            W.pop_front();

            for (char c : dfa.alphabet){
                Block X = preimage(dfa, A, c);
                if (X.empty())
                    continue;
                std::vector<Block> newP;
                newP.reserve(P.size() + 1);

                for (const Block &Y : P){
                    Block Y1, Y2;
                    for (int q : Y)
                        (X.count(q) ? Y1 : Y2).insert(q);
                    if (Y1.empty() || Y2.empty()){
                        newP.push_back(Y);
                        continue;
                    }

                    newP.push_back(Y1);
                    newP.push_back(Y2);
                    auto it = std::find(W.begin(), W.end(), Y);
                    if (it != W.end()){
                        *it = Y1;
                        W.insert(it + 1, Y2);
                    }
                    else
                    {
                        W.push_back(Y1.size() <= Y2.size() ? Y1 : Y2);
                    }
                }

                P.swap(newP);
            }
        }

        return P;
    }


    DFA build_minimal_from_partition(const DFA &dfa, const std::vector<Block> &P, int sink_id){
        std::map<int, int> block_of_state;

        for (std::size_t i = 0; i < P.size(); ++i)
            for (int s : P[i])
                block_of_state[s] = static_cast<int>(i);

        DFA out;
        out.alphabet = dfa.alphabet;
        out.start_state = block_of_state[dfa.start_state];

        for (std::size_t i = 0; i < P.size(); ++i){
            int id = static_cast<int>(i);
            out.states.insert(id);
            int representative = *P[i].begin();

            if (dfa.accept_states.count(representative))
                out.accept_states.insert(id);

            for (char c : dfa.alphabet){
                auto it = dfa.transitions.find({representative, c});
                if (it != dfa.transitions.end())
                    out.transitions[{id, c}] = block_of_state[it->second];
            }
        }

        if (sink_id != -1){
            int sink_block = block_of_state[sink_id];
            bool is_pure_sink = !out.accept_states.count(sink_block);

            if (is_pure_sink){
                for (char c : out.alphabet){
                    auto it = out.transitions.find({sink_block, c});
                    if (it == out.transitions.end() || it->second != sink_block){
                        is_pure_sink = false;
                        break;
                    }
                }
            }

            if (is_pure_sink){
                out.states.erase(sink_block);
                for (auto it = out.transitions.begin(); it != out.transitions.end();){
                    if (it->first.first == sink_block || it->second == sink_block)
                        it = out.transitions.erase(it);
                    else
                        ++it;
                }
            }
        }

        return out;
    }
}


DFA minimize_dfa(const DFA &dfa){
    DFA reachable = remove_unreachable(dfa);
    int sink_id = -1;
    DFA complete = complete_with_sink(reachable, sink_id);
    std::vector<Block> P = hopcroft(complete);
    return build_minimal_from_partition(complete, P, sink_id);
}