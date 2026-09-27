# Compiladores-practica-3


## Compilar

```bash
cd compilers_lab/nfa_to_dfa
cmake -S . -B build
cmake --build build
```


Para ejecutar
```bash
    ./build/regex_to_nfa -m < casos/a_b_pares_internos.txt 
    ./build/regex_to_nfa -m < casos/par_de_unos.txt 
    ./build/regex_to_nfa -m < casos/subcadena_01.txt 
    ./build/regex_to_nfa -m < casos/sufijo_abb.txt 
```


