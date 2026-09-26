
#include "../src/regex.hpp"
#include "../src/nfa.hpp"
#include "../src/dfa.hpp"

#include <cassert>
#include <iostream>

void test_epsilon_closure_empty(){
    Nfa n;
    int q0 = n.add_state();
    int q1 = n.add_state();
    int q2 = n.add_state();

    n.add_transition(q0, q1, 'a');
    n.add_transition(q1, q2, 'b');
    n.add_transition(q2, q0, 'c');

    std::set<int> esperado = {};
    std::set<int> resultado_q0 = {};

    epsilon_closure(n,resultado_q0);
    assert(resultado_q0 == esperado);
}

void test_epsilon_closure_with_no_epsilon_transitions(){
    Nfa n;
    int q0 = n.add_state();
    int q1 = n.add_state();
    int q2 = n.add_state();

    n.add_transition(q0, q1, 'a');
    n.add_transition(q1, q2, 'b');
    n.add_transition(q2, q0, 'c');

    std::set<int> esperado = {q0};
    std::set<int> resultado_q0 = {q0};

    epsilon_closure(n,resultado_q0);
    assert(resultado_q0 == esperado);
}

void test_epsilon_closure_cycle(){
    Nfa n;
    int q0 = n.add_state();
    int q1 = n.add_state();
    int q2 = n.add_state();

    n.add_transition(q0, q1, EPSILON);
    n.add_transition(q1, q2, EPSILON);
    n.add_transition(q2, q0, EPSILON);

    std::set<int> esperado = {q0, q1, q2};

    std::set<int> resultado_q0 = {q0};
    epsilon_closure(n,resultado_q0);

    assert(resultado_q0 == esperado);

    std::set<int> resultado_q1 = {q1};
    epsilon_closure(n,resultado_q1);
    assert(resultado_q1 == esperado);

    std::set<int> resultado_q2 = {q2};
    epsilon_closure(n,resultado_q2);
    assert(resultado_q2 == esperado);

}

void test_epsilon_closure_loop(){
    Nfa n;
    int q0 = n.add_state();
    n.add_transition(q0, q0, EPSILON);

    std::set<int> esperado = {q0};
    std::set<int> resultado_q0 = {q0};

    epsilon_closure(n,resultado_q0);
    assert(resultado_q0 == esperado);
}

void test_epsilon_closure_multiple_initials(){
    Nfa n;
    int q0 = n.add_state();
    int q1 = n.add_state();
    int q2 = n.add_state();
    int q3 = n.add_state();

    n.add_transition(q0, q1, EPSILON); 
    n.add_transition(q2, q3, EPSILON); 

    std::set<int> esperado = {q0, q1, q2, q3};
    std::set<int> resultado = {q0, q2};

    epsilon_closure(n, resultado);
    assert(resultado == esperado);
}

void test_epsilon_closure_hybrid(){
    Nfa n;
    int q0 = n.add_state();
    int q1 = n.add_state();
    int q2 = n.add_state();
    int q3 = n.add_state();

    n.add_transition(q0, q1, EPSILON); 
    n.add_transition(q1, q2, 'a');
    n.add_transition(q2, q3, EPSILON); 

    std::set<int> esperado = {q0, q1};
    std::set<int> resultado = {q0};

    epsilon_closure(n, resultado);
    assert(resultado == esperado);
}

int main() {   
    std::cout << "========== VALIDATION ==========\n";

    test_epsilon_closure_empty();
    std::cout << "[OK] test_epsilon_closure_empty\n";

    test_epsilon_closure_with_no_epsilon_transitions();
    std::cout << "[OK] test_epsilon_closure_with_no_epsilon_transitions\n";

    test_epsilon_closure_cycle();
    std::cout << "[OK] test_epsilon_closure_cycle\n";

    test_epsilon_closure_loop();
    std::cout << "[OK] test_epsilon_closure_loop\n";

    test_epsilon_closure_multiple_initials();
    std::cout << "[OK] test_epsilon_closure_multiple_initials\n";

    test_epsilon_closure_hybrid();
    std::cout << "[OK] est_epsilon_closure_hybrid\n";

    std::cout << "\nEpsilon closure test passed.\n";


    return 0;
}