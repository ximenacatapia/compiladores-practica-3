#include "regex.hpp"
#include "nfa.hpp"
#include "dfa.hpp"
#include "dfa_min.hpp"

#include <cassert>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

void strip_crlf(std::string &s)
{
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n'))
    {
        s.pop_back();
    }
}

bool basic_regex_check(const std::string &re, std::string &error)
{
    if (re.empty())
    {
        error = "la expresion regular esta vacia";
        return false;
    }
    int depth = 0;
    for (char c : re)
    {
        if (c == '(')
            depth++;
        else if (c == ')' && --depth < 0)
        {
            error = "hay un ')' sin su '(' correspondiente";
            return false;
        }
    }
    if (depth != 0)
    {
        error = "hay parentesis sin cerrar";
        return false;
    }
    return true;
}

void print_postfix(const Regex &r)
{
    for (const auto &t : r.items)
    {
        std::cout << t.value;
    }
    std::cout << "\n";
}

void test_strings_stdin(const std::string &regex_str)
{
    Regex r = parse_regex(regex_str);
    Nfa n = regex_to_nfa(r);

    std::string line;
    while (std::getline(std::cin, line))
    {
        strip_crlf(line);
        bool result = match_nfa(n, line);
        std::cout << (result ? '1' : '0');
    }
    std::cout << "\n";

    free_nfa(n);
}

int serialize_nfa_from_regex(const std::string &regex_str, const std::string &output_path)
{
    Regex r = parse_regex(regex_str);
    Nfa n = regex_to_nfa(r);

    bool ok = save_nfa(n, output_path);
    free_nfa(n);

    if (!ok)
    {
        std::cerr << "Error: No se pudo serializar el NFA en '" << output_path << "'.\n";
        return 1;
    }

    return 0;
}

// Modo -l (carga): reconstruye un NFA previamente serializado con -o
// leyéndolo desde disco, y valida contra él las cadenas que llegan por
// stdin (una por línea), imprimiendo 1/0 por cada una.
int load_and_test(const std::string &input_path)
{
    Nfa n;
    if (!load_nfa(input_path, n))
    {
        std::cerr << "Error: No se pudo cargar el NFA desde '" << input_path << "'.\n";
        return 1;
    }

    std::string line;
    while (std::getline(std::cin, line))
    {
        strip_crlf(line);
        bool result = match_nfa(n, line);
        std::cout << (result ? '1' : '0');
    }
    std::cout << "\n";

    free_nfa(n);
    return 0;
}

// Modo -m: regex -> NFA -> DFA -> DFA mínimo.
// Después de la regex, cada línea de stdin es un caso de prueba:
//   +cadena  -> debe ser aceptada
//   -cadena  -> debe ser rechazada
//   #...     -> comentario (se ignora)
// Un "+" o "-" solo representa la cadena vacía (ε).
int minimize_and_test(const std::string &regex_str)
{
    std::vector<std::string> accept_strings;
    std::vector<std::string> reject_strings;
    std::string line;
    int line_no = 1;
    while (std::getline(std::cin, line))
    {
        line_no++;
        strip_crlf(line);
        if (line.empty() || line[0] == '#')
            continue;
        if (line[0] == '+')
            accept_strings.push_back(line.substr(1));
        else if (line[0] == '-')
            reject_strings.push_back(line.substr(1));
        else
        {
            std::cerr << "Error en la linea " << line_no
                      << ": debe empezar con '+', '-' o '#'.\n";
            return 1;
        }
    }

    Regex r = parse_regex(regex_str);
    Nfa n = regex_to_nfa(r);
    Dfa d = nfa_to_dfa(n);
    DFA dfa_original = to_template_dfa(d);

    std::cout << "Expresion regular: " << regex_str << "\n\n";
    print_dfa(dfa_original);
    std::cout << "\n";

    DFA dfa_min = minimize_dfa(dfa_original);
    print_dfa_min(dfa_min);

    assert(dfa_original.states.size() >= dfa_min.states.size());
    std::cout << "\nComprobacion de estados correcta: "
              << dfa_original.states.size() << " >= "
              << dfa_min.states.size() << "\n";

    // Chequeo extra, el DFA original y el minimizado deben dar lo mismo
    int mismatches = 0;
    for (const auto *group : {&accept_strings, &reject_strings})
        for (const auto &s : *group)
            if (test_string(dfa_original, s) != test_string(dfa_min, s))
            {
                std::cout << "DISCREPANCIA original vs minimizado en \"" << s << "\"\n";
                mismatches++;
            }
    if (mismatches == 0)
        std::cout << "Original y minimizado coinciden en todas las cadenas.\n";

    int failures = run_test_suite(dfa_min, accept_strings, reject_strings);

    free_nfa(n);
    return (failures == 0 && mismatches == 0) ? 0 : 1;
}

int main(int argc, char *argv[])
{
    int opt;
    std::string output_file;
    std::string input_file;
    int mode = 0;

    while ((opt = getopt(argc, argv, "rtmo:l:")) != -1)
    {
        switch (opt)
        {
        case 'r':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t, -m, -o o -l.\n";
                return 1;
            }
            mode = 'r';
            break;
        case 't':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t, -m, -o o -l.\n";
                return 1;
            }
            mode = 't';
            break;
        case 'm':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t, -m, -o o -l.\n";
                return 1;
            }
            mode = 'm';
            break;
        case 'o':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t, -m, -o o -l.\n";
                return 1;
            }
            mode = 'o';
            output_file = optarg;
            break;
        case 'l':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t, -m, -o o -l.\n";
                return 1;
            }
            mode = 'l';
            input_file = optarg;
            break;

        default:
            std::cerr << "Usage: " << argv[0] << " -r | -t | -m | -o <archivo.nfa> | -l <archivo.nfa>\n";
            return 1;
        }
    }

    if (mode == 0)
    {
        std::cerr << "Usage: " << argv[0] << " -r | -t | -m | -o <archivo.nfa> | -l <archivo.nfa>\n";
        return 1;
    }

    // El modo -l carga el NFA desde disco
    if (mode == 'l')
    {
        return load_and_test(input_file);
    }

    std::string regex_str;
    if (!std::getline(std::cin, regex_str))
    {
        return 1;
    }
    strip_crlf(regex_str);

    std::string error;
    if (!basic_regex_check(regex_str, error))
    {
        std::cerr << "Error: " << error << ".\n";
        return 1;
    }

    if (mode == 'r')
    {
        print_postfix(parse_regex(regex_str));
        return 0;
    }

    if (mode == 't')
    {
        test_strings_stdin(regex_str);
        return 0;
    }

    if (mode == 'm')
    {
        return minimize_and_test(regex_str);
    }

    return serialize_nfa_from_regex(regex_str, output_file);
}
