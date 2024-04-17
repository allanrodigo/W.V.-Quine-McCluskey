#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "term.h"        // Assegure que o arquivo de cabeçalho contém as definições corretas
#include "expression.h"  // Assegure que o arquivo de cabeçalho contém as funções de impressão corretas

int MAX_VARIABLES = 8;

// Função para preencher a representação binária e contar os '1's
void fill_binary_representation(Term *term) {
    term->count_one = 0;
    for (int i = 0; i < 8; i++) {
        if ((term->num >> i) & 1) {
            term->count_one++;
        }
    }
}

// Função para combinar dois termos se eles diferirem por exatamente um bit, considerando máscaras
Term combine_terms(Term a, Term b) {
    Term result;
    result.num = a.num & b.num;
    result.mask = (a.num ^ b.num) | a.mask | b.mask;
    result.used = false;
    return result;
}

// Verifica se dois termos podem ser combinados
bool can_combine(Term a, Term b) {
    unsigned int diff = (a.num ^ b.num) & ~(a.mask | b.mask);
    return diff != 0 && (diff & (diff - 1)) == 0;
}

void remove_duplicates(Term *terms, int *num_terms) {
    int newCount = 0;  // Novo contador para o número de termos após remover duplicatas
    for (int i = 0; i < *num_terms; i++) {
        bool isDuplicate = false;
        for (int j = 0; j < newCount; j++) {
            if (terms[i].num == terms[j].num && terms[i].mask == terms[j].mask) {
                isDuplicate = true;
                break;
            }
        }
        if (!isDuplicate) {
            terms[newCount++] = terms[i];  // Adiciona este termo ao array se não for duplicado
        }
    }
    *num_terms = newCount;  // Atualiza o contador de termos para refletir o número de termos únicos
}

// Função para gerar o circuito minimizado
void generate_min_circuit(int v[], int size) {
    Term terms[MAX_VARIABLES];
    int num_terms = size;

    // Inicializa os termos
    for (int i = 0; i < size; i++) {
        terms[i].num = v[i];
        terms[i].mask = 0;
        terms[i].used = false;
        fill_binary_representation(&terms[i]);
    }

    bool progress;
    do {
        progress = false;
        for (int i = 0; i < num_terms - 1; i++) {
            for (int j = i + 1; j < num_terms; j++) {
                if (!terms[i].used && !terms[j].used && can_combine(terms[i], terms[j])) {
                    Term newTerm = combine_terms(terms[i], terms[j]);
                    terms[i] = newTerm; // Substitui o termo i pelo novo termo combinado
                    terms[j].used = true; // Marca o termo j como usado
                    progress = true;
                }
            }
        }
        // Remove termos usados
        int offset = 0;
        for (int i = 0; i < num_terms; i++) {
            if (!terms[i].used) {
                terms[offset++] = terms[i];
            }
        }
        num_terms = offset;
    } while (progress);

    // Remover duplicatas
    remove_duplicates(terms, &num_terms);

    // Imprimir termos resultantes
    for (int i = 0; i < num_terms; i++) {
        printf("Termo Final: ");
        print_binary(terms[i].num);
        printf("\n");
    }
    free(terms); // Liberar memória alocada
}