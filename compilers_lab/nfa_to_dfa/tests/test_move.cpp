#include "nfa.hpp"
#include <cassert>
#include <iostream>
#include <set>

void test_move()
{
    Nfa n;
    int q0 = n.add_state(); // 0, estado origen
    int q1 = n.add_state(); // 1
    int q2 = n.add_state(); // 2
    int q3 = n.add_state(); // 3

    n.add_transition(q0, q1, 'b'); // q0 ->b q1
    n.add_transition(q0, q2, 'b'); // q0 ->b q2
    n.add_transition(q0, q3, 'a'); // q0 ->a q3

    // Caso del manual: Move({q0}, 'b') debe dar exactamente {q1, q2}.
    // q3 queda fuera porque se alcanza con 'a'
    std::set<int> resultado = move(n, {q0}, 'b');
    std::set<int> esperado = {q1, q2};
    assert(resultado == esperado);

    // Símbolo que ningún estado consume, resultado vacío.
    std::set<int> vacio = move(n, {q0}, 'z');
    assert(vacio.empty());

    // Move sobre un conjunto de varios estados sin transiciones: vacío.
    std::set<int> desde_dos = move(n, {q1, q2}, 'a');
    assert(desde_dos.empty());

    std::cout << "test_move OK\n";
}

int main()
{
    test_move();
    return 0;
}