//Arthur Roldan Slikta - 10353847
//Felipe Haddad - 10437372

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* =========================================================================
   1. DEFINIÇÃO DOS ÁTOMOS (TOKENS) DA LINGUAGEM PORTUGOL
   ========================================================================= */
typedef enum {
    // Palavras Reservadas
    ATOMO_ALGORITMO,
    ATOMO_CARACTERE,
    ATOMO_DIV,
    ATOMO_E,
    ATOMO_ENQUANTO,
    ATOMO_ENTAO,
    ATOMO_ESCREVA,
    ATOMO_FACA,
    ATOMO_FALSO,
    ATOMO_FIM,
    ATOMO_FUNCAO,
    ATOMO_IDENTIFICADOR,
    ATOMO_INICIO,
    ATOMO_INTEIRO,
    ATOMO_LEIA,
    ATOMO_LOGICO,
    ATOMO_MOD,
    ATOMO_OU,
    ATOMO_PROCEDIMENTO,
    ATOMO_SE,
    ATOMO_SENAO,
    ATOMO_VAR,
    ATOMO_VERDADEIRO,

    // Constantes
    ATOMO_CONSTINT,
    ATOMO_CONSTCHAR,

    // Símbolos e Operadores Delimitadores
    ATOMO_PONTO_VIRGULA,  // ';'
    ATOMO_PONTO,          // '.'
    ATOMO_VIRGULA,        // ','
    ATOMO_DOIS_PONTOS,    // ':'
    ATOMO_ATRIBUICAO,     // ':='
    ATOMO_ABRE_PAR,       // '('
    ATOMO_FECHA_PAR,      // ')'

    // Operadores Relacionais
    ATOMO_OP_DIFERENTE,   // '<>'
    ATOMO_OP_MENOR_IGUAL, // '<='
    ATOMO_OP_MAIOR_IGUAL, // '>='
    ATOMO_OP_MENOR,       // '<'
    ATOMO_OP_MAIOR,       // '>'
    ATOMO_OP_IGUAL,       // '='

    // Operadores Aritméticos / Lógicos adicionais
    ATOMO_OP_SOMA,        // '+'
    ATOMO_OP_SUB,         // '-'
    ATOMO_OP_MULT,        // '*'
    ATOMO_NAO,            // nao

    // Especiais
    ATOMO_COMENTARIO,
    ATOMO_ERRO,
    ATOMO_FIM_ARQUIVO
} TAtomo;

/* =========================================================================
   2. ESTRUTURA TInfoAtomo
   ========================================================================= */
typedef struct {
    TAtomo atomo;
    int linha;
    union {
        int numero;     // atributo para constint
        char id[16];    // atributo para identificador (máx 15 chars + \0)
        char ch;        // atributo para constchar
    } atributo;
} TInfoAtomo;

/* =========================================================================
   3. VARIÁVEIS GLOBAIS
   ========================================================================= */
FILE *arquivo_fonte = NULL;
int linha_atual = 1;
TInfoAtomo lookahead;
int total_linhas_analisadas = 0;

/* =========================================================================
   4. PROTÓTIPOS DAS FUNÇÕES
   ========================================================================= */
TInfoAtomo obter_atomo(void);
void consome(TAtomo atomo_esperado);
const char* nome_atomo(TAtomo atomo);
void imprime_info_atomo(TInfoAtomo info);

// Funções do ASDR para a Gramática EBNF de Portugol
void programa(void);
void bloco(void);
void declaracao_variaveis(void);
void lista_variaveis(void);
void declaracao_de_rotinas(void);
void declaracao_de_funcao(void);
void declaracao_de_procedimento(void);
void tipo(void);
void parametros_formais(void);
void parametro_formal(void);
void comando_composto(void);
void comando(void);
void comando_entrada(void);
void comando_saida(void);
void comando_condicional(void);
void comando_repeticao(void);
void lista_expressao(void);
void expressao(void);
void operador_relacional(void);
void expressao_simples(void);
void operador_adicao(void);
void termo(void);
void operador_multiplicacao(void);
void fator(void);

/* =========================================================================
   5. AUXILIARES E IMPRESSÃO DE MENSAGENS DE ERRO
   ========================================================================= */
const char* nome_atomo(TAtomo atomo) {
    switch (atomo) {
        case ATOMO_ALGORITMO: return "algoritmo";
        case ATOMO_CARACTERE: return "caractere";
        case ATOMO_DIV: return "div";
        case ATOMO_E: return "e";
        case ATOMO_ENQUANTO: return "enquanto";
        case ATOMO_ENTAO: return "entao";
        case ATOMO_ESCREVA: return "escreva";
        case ATOMO_FACA: return "faca";
        case ATOMO_FALSO: return "falso";
        case ATOMO_FIM: return "fim";
        case ATOMO_FUNCAO: return "funcao";
        case ATOMO_INICIO: return "inicio";
        case ATOMO_INTEIRO: return "inteiro";
        case ATOMO_LEIA: return "leia";
        case ATOMO_LOGICO: return "logico";
        case ATOMO_MOD: return "mod";
        case ATOMO_OU: return "ou";
        case ATOMO_PROCEDIMENTO: return "procedimento";
        case ATOMO_SE: return "se";
        case ATOMO_SENAO: return "senao";
        case ATOMO_VAR: return "var";
        case ATOMO_VERDADEIRO: return "verdadeiro";
        case ATOMO_IDENTIFICADOR: return "identificador";
        case ATOMO_CONSTINT: return "constint";
        case ATOMO_CONSTCHAR: return "constchar";
        case ATOMO_PONTO_VIRGULA: return ";";
        case ATOMO_PONTO: return ".";
        case ATOMO_VIRGULA: return ",";
        case ATOMO_DOIS_PONTOS: return ":";
        case ATOMO_ATRIBUICAO: return ":=";
        case ATOMO_ABRE_PAR: return "(";
        case ATOMO_FECHA_PAR: return ")";
        case ATOMO_OP_DIFERENTE: return "<>";
        case ATOMO_OP_MENOR_IGUAL: return "<=";
        case ATOMO_OP_MAIOR_IGUAL: return ">=";
        case ATOMO_OP_MENOR: return "<";
        case ATOMO_OP_MAIOR: return ">";
        case ATOMO_OP_IGUAL: return "=";
        case ATOMO_OP_SOMA: return "+";
        case ATOMO_OP_SUB: return "-";
        case ATOMO_OP_MULT: return "*";
        case ATOMO_NAO: return "nao";
        case ATOMO_COMENTARIO: return "comentario";
        case ATOMO_ERRO: return "erro";
        case ATOMO_FIM_ARQUIVO: return "fim de arquivo";
        default: return "desconhecido";
    }
}

void imprime_info_atomo(TInfoAtomo info) {
    if (info.atomo == ATOMO_COMENTARIO) {
        printf("# %2d:comentario\n", info.linha);
    } else if (info.atomo == ATOMO_IDENTIFICADOR) {
        printf("# %2d:identificador: %s\n", info.linha, info.atributo.id);
    } else if (info.atomo == ATOMO_CONSTINT) {
        printf("# %2d:constint: %d\n", info.linha, info.atributo.numero);
    } else if (info.atomo == ATOMO_CONSTCHAR) {
        printf("# %2d:constchar: %c\n", info.linha, info.atributo.ch);
    } else if (info.atomo == ATOMO_PONTO_VIRGULA) {
        printf("# %2d:ponto_virgula\n", info.linha);
    } else if (info.atomo == ATOMO_PONTO) {
        printf("# %2d:ponto\n", info.linha);
    } else if (info.atomo == ATOMO_VIRGULA) {
        printf("# %2d:virgula\n", info.linha);
    } else if (info.atomo == ATOMO_DOIS_PONTOS) {
        printf("# %2d:dois_pontos\n", info.linha);
    } else if (info.atomo == ATOMO_ATRIBUICAO) {
        printf("# %2d:atribuicao\n", info.linha);
    } else if (info.atomo == ATOMO_ABRE_PAR) {
        printf("# %2d:abre_par\n", info.linha);
    } else if (info.atomo == ATOMO_FECHA_PAR) {
        printf("# %2d:fecha_par\n", info.linha);
    } else {
        printf("# %2d:%s\n", info.linha, nome_atomo(info.atomo));
    }
}

void erro_lexico(const char *msg) {
    printf("# %2d:erro lexico, %s\n", linha_atual, msg);
    if (arquivo_fonte) fclose(arquivo_fonte);
    exit(1);
}

void erro_sintatico(TAtomo atomo_esperado, TInfoAtomo atomo_encontrado) {
    printf("# %2d:erro sintatico, esperado [%s] encontrado [%s]\n",
           atomo_encontrado.linha,
           nome_atomo(atomo_esperado),
           nome_atomo(atomo_encontrado.atomo));
    if (arquivo_fonte) fclose(arquivo_fonte);
    exit(1);
}

/* =========================================================================
   6. ANALISADOR LÉXICO (obter_atomo)
   ========================================================================= */
TInfoAtomo obter_atomo(void) {
    TInfoAtomo info;
    int c;

    while (1) {
        c = fgetc(arquivo_fonte);

        if (c == EOF) {
            info.atomo = ATOMO_FIM_ARQUIVO;
            info.linha = linha_atual;
            return info;
        }

        // Trata delimitadores (espaço, tabulação, nova linha, retorno)
        if (c == ' ' || c == '\t' || c == '\r') {
            continue;
        }
        if (c == '\n') {
            linha_atual++;
            continue;
        }

        info.linha = linha_atual;

        // Comentários de múltiplas linhas: {- ... -}
        if (c == '{') {
            int proximo = fgetc(arquivo_fonte);
            if (proximo == '-') {
                while (1) {
                    int ch = fgetc(arquivo_fonte);
                    if (ch == EOF) {
                        erro_lexico("comentario nao fechado ate o fim do arquivo");
                    }
                    if (ch == '\n') {
                        linha_atual++;
                    }
                    if (ch == '-') {
                        int ch2 = fgetc(arquivo_fonte);
                        if (ch2 == '}') {
                            info.atomo = ATOMO_COMENTARIO;
                            return info;
                        } else {
                            ungetc(ch2, arquivo_fonte);
                        }
                    }
                }
            } else {
                ungetc(proximo, arquivo_fonte);
                erro_lexico("caractere '{' invalido (esperado comentario '{-')");
            }
        }

        // Identificadores e Palavras Reservadas
        if (isalpha(c)) {
            char lexema[100];
            int idx = 0;
            lexema[idx++] = (char)c;

            while (1) {
                int ch = fgetc(arquivo_fonte);
                if (isalnum(ch) || ch == '_') {
                    if (idx < 99) lexema[idx++] = (char)ch;
                } else {
                    ungetc(ch, arquivo_fonte);
                    break;
                }
            }
            lexema[idx] = '\0';

            // Verifica tamanho máximo de 15 caracteres para identificador
            if (idx > 15) {
                erro_lexico("identificador com mais de 15 caracteres");
            }

            // Converter para minúsculo para comparar palavra reservada (case-insensitive)
            char lexema_lower[100];
            for (int i = 0; i <= idx; i++) {
                lexema_lower[i] = (char)tolower(lexema[i]);
            }

            if (strcmp(lexema_lower, "algoritmo") == 0) info.atomo = ATOMO_ALGORITMO;
            else if (strcmp(lexema_lower, "caractere") == 0) info.atomo = ATOMO_CARACTERE;
            else if (strcmp(lexema_lower, "div") == 0) info.atomo = ATOMO_DIV;
            else if (strcmp(lexema_lower, "e") == 0) info.atomo = ATOMO_E;
            else if (strcmp(lexema_lower, "enquanto") == 0) info.atomo = ATOMO_ENQUANTO;
            else if (strcmp(lexema_lower, "entao") == 0) info.atomo = ATOMO_ENTAO;
            else if (strcmp(lexema_lower, "escreva") == 0) info.atomo = ATOMO_ESCREVA;
            else if (strcmp(lexema_lower, "faca") == 0) info.atomo = ATOMO_FACA;
            else if (strcmp(lexema_lower, "falso") == 0) info.atomo = ATOMO_FALSO;
            else if (strcmp(lexema_lower, "fim") == 0) info.atomo = ATOMO_FIM;
            else if (strcmp(lexema_lower, "funcao") == 0) info.atomo = ATOMO_FUNCAO;
            else if (strcmp(lexema_lower, "inicio") == 0) info.atomo = ATOMO_INICIO;
            else if (strcmp(lexema_lower, "inteiro") == 0) info.atomo = ATOMO_INTEIRO;
            else if (strcmp(lexema_lower, "leia") == 0) info.atomo = ATOMO_LEIA;
            else if (strcmp(lexema_lower, "logico") == 0) info.atomo = ATOMO_LOGICO;
            else if (strcmp(lexema_lower, "mod") == 0) info.atomo = ATOMO_MOD;
            else if (strcmp(lexema_lower, "ou") == 0) info.atomo = ATOMO_OU;
            else if (strcmp(lexema_lower, "procedimento") == 0) info.atomo = ATOMO_PROCEDIMENTO;
            else if (strcmp(lexema_lower, "se") == 0) info.atomo = ATOMO_SE;
            else if (strcmp(lexema_lower, "senao") == 0) info.atomo = ATOMO_SENAO;
            else if (strcmp(lexema_lower, "var") == 0) info.atomo = ATOMO_VAR;
            else if (strcmp(lexema_lower, "verdadeiro") == 0) info.atomo = ATOMO_VERDADEIRO;
            else if (strcmp(lexema_lower, "nao") == 0) info.atomo = ATOMO_NAO;
            else {
                info.atomo = ATOMO_IDENTIFICADOR;
                strcpy(info.atributo.id, lexema);
            }
            return info;
        }

        // Constantes Inteiras: digito+((E(+|ε)digito+)|ε)
        if (isdigit(c)) {
            char num_str[100];
            int idx = 0;
            num_str[idx++] = (char)c;

            while (1) {
                int ch = fgetc(arquivo_fonte);
                if (isdigit(ch)) {
                    if (idx < 99) num_str[idx++] = (char)ch;
                } else {
                    ungetc(ch, arquivo_fonte);
                    break;
                }
            }

            // Notação exponencial 'E' ou 'e'
            int ch_exp = fgetc(arquivo_fonte);
            if (ch_exp == 'E' || ch_exp == 'e') {
                num_str[idx++] = 'E';
                int ch_sinal = fgetc(arquivo_fonte);
                if (ch_sinal == '+') {
                    num_str[idx++] = '+';
                } else if (isdigit(ch_sinal)) {
                    num_str[idx++] = (char)ch_sinal;
                } else {
                    ungetc(ch_sinal, arquivo_fonte);
                }

                while (1) {
                    int ch = fgetc(arquivo_fonte);
                    if (isdigit(ch)) {
                        if (idx < 99) num_str[idx++] = (char)ch;
                    } else {
                        ungetc(ch, arquivo_fonte);
                        break;
                    }
                }
            } else {
                ungetc(ch_exp, arquivo_fonte);
            }
            num_str[idx] = '\0';

            if (strchr(num_str, 'E')) {
                double val = atof(num_str);
                info.atributo.numero = (int)val;
            } else {
                info.atributo.numero = atoi(num_str);
            }
            info.atomo = ATOMO_CONSTINT;
            return info;
        }

        // Constante Caractere: 'c'
        if (c == '\'') {
            int char_val = fgetc(arquivo_fonte);
            if (char_val == EOF || char_val == '\n') {
                erro_lexico("constante caractere mal formatada");
            }
            int fecho = fgetc(arquivo_fonte);
            if (fecho != '\'') {
                erro_lexico("esperado ' para fechar constante caractere");
            }
            info.atomo = ATOMO_CONSTCHAR;
            info.atributo.ch = (char)char_val;
            return info;
        }

        // Operadores e Símbolos Especiais
        if (c == ';') { info.atomo = ATOMO_PONTO_VIRGULA; return info; }
        if (c == '.') { info.atomo = ATOMO_PONTO; return info; }
        if (c == ',') { info.atomo = ATOMO_VIRGULA; return info; }
        if (c == '(') { info.atomo = ATOMO_ABRE_PAR; return info; }
        if (c == ')') { info.atomo = ATOMO_FECHA_PAR; return info; }
        if (c == '+') { info.atomo = ATOMO_OP_SOMA; return info; }
        if (c == '-') { info.atomo = ATOMO_OP_SUB; return info; }
        if (c == '*') { info.atomo = ATOMO_OP_MULT; return info; }
        if (c == '=') { info.atomo = ATOMO_OP_IGUAL; return info; }

        if (c == ':') {
            int prox = fgetc(arquivo_fonte);
            if (prox == '=') {
                info.atomo = ATOMO_ATRIBUICAO;
            } else {
                ungetc(prox, arquivo_fonte);
                info.atomo = ATOMO_DOIS_PONTOS;
            }
            return info;
        }

        if (c == '<') {
            int prox = fgetc(arquivo_fonte);
            if (prox == '>') {
                info.atomo = ATOMO_OP_DIFERENTE;
            } else if (prox == '=') {
                info.atomo = ATOMO_OP_MENOR_IGUAL;
            } else {
                ungetc(prox, arquivo_fonte);
                info.atomo = ATOMO_OP_MENOR;
            }
            return info;
        }

        if (c == '>') {
            int prox = fgetc(arquivo_fonte);
            if (prox == '=') {
                info.atomo = ATOMO_OP_MAIOR_IGUAL;
            } else {
                ungetc(prox, arquivo_fonte);
                info.atomo = ATOMO_OP_MAIOR;
            }
            return info;
        }

        erro_lexico("caractere invalido ou desconhecido");
    }
}

/* =========================================================================
   7. ANALISADOR SINTÁTICO (consome e ASDR)
   ========================================================================= */
void consome(TAtomo atomo_esperado) {
    if (lookahead.atomo == atomo_esperado) {
        imprime_info_atomo(lookahead);
        lookahead = obter_atomo();
        // Repassa e imprime comentários continuamente
        while (lookahead.atomo == ATOMO_COMENTARIO) {
            imprime_info_atomo(lookahead);
            lookahead = obter_atomo();
        }
    } else {
        erro_sintatico(atomo_esperado, lookahead);
    }
}

// <programa> ::= algoritmo identificador ';' <bloco> '.'
void programa(void) {
    consome(ATOMO_ALGORITMO);
    consome(ATOMO_IDENTIFICADOR);
    consome(ATOMO_PONTO_VIRGULA);
    bloco();
    consome(ATOMO_PONTO);
}

// <bloco> ::= <declaração_variáveis> <declaração_de_rotinas> <comando_composto>
void bloco(void) {
    declaracao_variaveis();
    declaracao_de_rotinas();
    comando_composto();
}

// <declaração_variáveis> ::= [ var <lista_variaveis> ';' { <lista_variaveis> ';' } ]
void declaracao_variaveis(void) {
    if (lookahead.atomo == ATOMO_VAR) {
        consome(ATOMO_VAR);
        lista_variaveis();
        consome(ATOMO_PONTO_VIRGULA);
        while (lookahead.atomo == ATOMO_IDENTIFICADOR) {
            lista_variaveis();
            consome(ATOMO_PONTO_VIRGULA);
        }
    }
}

// <lista_variaveis> ::= identificador { ',' identificador } ':' <tipo>
void lista_variaveis(void) {
    consome(ATOMO_IDENTIFICADOR);
    while (lookahead.atomo == ATOMO_VIRGULA) {
        consome(ATOMO_VIRGULA);
        consome(ATOMO_IDENTIFICADOR);
    }
    consome(ATOMO_DOIS_PONTOS);
    tipo();
}

// <tipo> ::= caractere | inteiro | logico
void tipo(void) {
    if (lookahead.atomo == ATOMO_CARACTERE) {
        consome(ATOMO_CARACTERE);
    } else if (lookahead.atomo == ATOMO_INTEIRO) {
        consome(ATOMO_INTEIRO);
    } else if (lookahead.atomo == ATOMO_LOGICO) {
        consome(ATOMO_LOGICO);
    } else {
        erro_sintatico(ATOMO_INTEIRO, lookahead);
    }
}

// <declaracao_de_rotinas> ::= { <declaração_de_função> | <declaração_de_procedimento> }
void declaracao_de_rotinas(void) {
    while (lookahead.atomo == ATOMO_FUNCAO || lookahead.atomo == ATOMO_PROCEDIMENTO) {
        if (lookahead.atomo == ATOMO_FUNCAO) {
            declaracao_de_funcao();
        } else {
            declaracao_de_procedimento();
        }
    }
}

// <declaração_de_função> ::= funcao <tipo> identificador <parametros_formais> <declaracao_variaveis> <comando_composto>
void declaracao_de_funcao(void) {
    consome(ATOMO_FUNCAO);
    tipo();
    consome(ATOMO_IDENTIFICADOR);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// <declaracao_de_procedimento> ::= procedimento identificador <parametros_formais> <declaracao_variaveis> <comando_composto>
void declaracao_de_procedimento(void) {
    consome(ATOMO_PROCEDIMENTO);
    consome(ATOMO_IDENTIFICADOR);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// <parâmetros_formais> ::= '(' [ <parâmetro_formal> { ';' <parâmetro_formal> } ] ')'
void parametros_formais(void) {
    consome(ATOMO_ABRE_PAR);
    if (lookahead.atomo == ATOMO_VAR || lookahead.atomo == ATOMO_IDENTIFICADOR) {
        parametro_formal();
        while (lookahead.atomo == ATOMO_PONTO_VIRGULA) {
            consome(ATOMO_PONTO_VIRGULA);
            parametro_formal();
        }
    }
    consome(ATOMO_FECHA_PAR);
}

// <parâmetro_formal> ::= [var] <lista_variaveis>
void parametro_formal(void) {
    if (lookahead.atomo == ATOMO_VAR) {
        consome(ATOMO_VAR);
    }
    lista_variaveis();
}

// <comando_composto> ::= inicio <comando> { ';' <comando> } fim
void comando_composto(void) {
    consome(ATOMO_INICIO);
    comando();
    while (lookahead.atomo == ATOMO_PONTO_VIRGULA) {
        consome(ATOMO_PONTO_VIRGULA);
        comando();
    }
    consome(ATOMO_FIM);
}

// <comando> ::= <comando_atribuição_ou_chamada> | <comando_entrada> | <comando_saida> | <comando_condicional> | <comando_repeticao> | <comando_composto>
void comando(void) {
    if (lookahead.atomo == ATOMO_IDENTIFICADOR) {
        consome(ATOMO_IDENTIFICADOR);
        if (lookahead.atomo == ATOMO_ATRIBUICAO) {
            // <comando_atribuição>
            consome(ATOMO_ATRIBUICAO);
            expressao();
        } else if (lookahead.atomo == ATOMO_ABRE_PAR) {
            // <chamada_procedimento> com parâmetros
            consome(ATOMO_ABRE_PAR);
            lista_expressao();
            consome(ATOMO_FECHA_PAR);
        } else {
            // <chamada_procedimento> sem parâmetros
            // Nada mais a consumir
        }
    } else if (lookahead.atomo == ATOMO_LEIA) {
        comando_entrada();
    } else if (lookahead.atomo == ATOMO_ESCREVA) {
        comando_saida();
    } else if (lookahead.atomo == ATOMO_SE) {
        comando_condicional();
    } else if (lookahead.atomo == ATOMO_ENQUANTO) {
        comando_repeticao();
    } else if (lookahead.atomo == ATOMO_INICIO) {
        comando_composto();
    } else {
        erro_sintatico(ATOMO_IDENTIFICADOR, lookahead);
    }
}

// <comando_entrada> ::= leia '(' identificador { ',' identificador } ')'
void comando_entrada(void) {
    consome(ATOMO_LEIA);
    consome(ATOMO_ABRE_PAR);
    consome(ATOMO_IDENTIFICADOR);
    while (lookahead.atomo == ATOMO_VIRGULA) {
        consome(ATOMO_VIRGULA);
        consome(ATOMO_IDENTIFICADOR);
    }
    consome(ATOMO_FECHA_PAR);
}

// <comando_saida> ::= escreva '(' <lista_expressão> ')'
void comando_saida(void) {
    consome(ATOMO_ESCREVA);
    consome(ATOMO_ABRE_PAR);
    lista_expressao();
    consome(ATOMO_FECHA_PAR);
}

// <comando_condicional> ::= se <expressão> entao <comando> [ senao <comando> ]
void comando_condicional(void) {
    consome(ATOMO_SE);
    expressao();
    consome(ATOMO_ENTAO);
    comando();
    if (lookahead.atomo == ATOMO_SENAO) {
        consome(ATOMO_SENAO);
        comando();
    }
}

// <comando_repeticao> ::= enquanto <expressão> faca <comando>
void comando_repeticao(void) {
    consome(ATOMO_ENQUANTO);
    expressao();
    consome(ATOMO_FACA);
    comando();
}

// <lista_expressão> ::= <expressão> { ',' <expressão> }
void lista_expressao(void) {
    expressao();
    while (lookahead.atomo == ATOMO_VIRGULA) {
        consome(ATOMO_VIRGULA);
        expressao();
    }
}

// <expressão> ::= <expressão_simples> [ <operador_relacional> <expressão_simples> ]
void expressao(void) {
    expressao_simples();
    if (lookahead.atomo == ATOMO_OP_DIFERENTE || lookahead.atomo == ATOMO_OP_MENOR_IGUAL ||
        lookahead.atomo == ATOMO_OP_MAIOR_IGUAL || lookahead.atomo == ATOMO_OP_MENOR ||
        lookahead.atomo == ATOMO_OP_MAIOR || lookahead.atomo == ATOMO_OP_IGUAL) {
        operador_relacional();
        expressao_simples();
    }
}

void operador_relacional(void) {
    if (lookahead.atomo == ATOMO_OP_DIFERENTE || lookahead.atomo == ATOMO_OP_MENOR_IGUAL ||
        lookahead.atomo == ATOMO_OP_MAIOR_IGUAL || lookahead.atomo == ATOMO_OP_MENOR ||
        lookahead.atomo == ATOMO_OP_MAIOR || lookahead.atomo == ATOMO_OP_IGUAL) {
        consome(lookahead.atomo);
    } else {
        erro_sintatico(ATOMO_OP_IGUAL, lookahead);
    }
}

// <expressão_simples> ::= <termo> { <operador_adição> <termo> }
void expressao_simples(void) {
    termo();
    while (lookahead.atomo == ATOMO_OP_SOMA || lookahead.atomo == ATOMO_OP_SUB ||
           lookahead.atomo == ATOMO_MOD || lookahead.atomo == ATOMO_OU) {
        operador_adicao();
        termo();
    }
}

void operador_adicao(void) {
    if (lookahead.atomo == ATOMO_OP_SOMA || lookahead.atomo == ATOMO_OP_SUB ||
        lookahead.atomo == ATOMO_MOD || lookahead.atomo == ATOMO_OU) {
        consome(lookahead.atomo);
    } else {
        erro_sintatico(ATOMO_OP_SOMA, lookahead);
    }
}

// <termo> ::= <fator> { <operador_multiplicação> <fator> }
void termo(void) {
    fator();
    while (lookahead.atomo == ATOMO_OP_MULT || lookahead.atomo == ATOMO_DIV ||
           lookahead.atomo == ATOMO_E) {
        operador_multiplicacao();
        fator();
    }
}

void operador_multiplicacao(void) {
    if (lookahead.atomo == ATOMO_OP_MULT || lookahead.atomo == ATOMO_DIV ||
        lookahead.atomo == ATOMO_E) {
        consome(lookahead.atomo);
    } else {
        erro_sintatico(ATOMO_OP_MULT, lookahead);
    }
}

// <fator> ::= identificador [ '(' <lista_expressão> ')' ] | constint | constchar | '(' <expressão> ')' | ( '+' | '-' | nao ) <fator> | verdadeiro | falso
void fator(void) {
    if (lookahead.atomo == ATOMO_IDENTIFICADOR) {
        consome(ATOMO_IDENTIFICADOR);
        if (lookahead.atomo == ATOMO_ABRE_PAR) {
            consome(ATOMO_ABRE_PAR);
            lista_expressao();
            consome(ATOMO_FECHA_PAR);
        }
    } else if (lookahead.atomo == ATOMO_CONSTINT) {
        consome(ATOMO_CONSTINT);
    } else if (lookahead.atomo == ATOMO_CONSTCHAR) {
        consome(ATOMO_CONSTCHAR);
    } else if (lookahead.atomo == ATOMO_ABRE_PAR) {
        consome(ATOMO_ABRE_PAR);
        expressao();
        consome(ATOMO_FECHA_PAR);
    } else if (lookahead.atomo == ATOMO_OP_SOMA || lookahead.atomo == ATOMO_OP_SUB || lookahead.atomo == ATOMO_NAO) {
        consome(lookahead.atomo);
        fator();
    } else if (lookahead.atomo == ATOMO_VERDADEIRO) {
        consome(ATOMO_VERDADEIRO);
    } else if (lookahead.atomo == ATOMO_FALSO) {
        consome(ATOMO_FALSO);
    } else {
        erro_sintatico(ATOMO_IDENTIFICADOR, lookahead);
    }
}

/* =========================================================================
   8. FUNÇÃO PRINCIPAL (MAIN)
   ========================================================================= */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    arquivo_fonte = fopen(argv[1], "r");
    if (!arquivo_fonte) {
        printf("Erro ao abrir arquivo fonte: %s\n", argv[1]);
        return 1;
    }

    // Inicializa o lookahead com o primeiro átomo
    lookahead = obter_atomo();
    while (lookahead.atomo == ATOMO_COMENTARIO) {
        imprime_info_atomo(lookahead);
        lookahead = obter_atomo();
    }

    // Executa a análise sintática
    programa();

    // Salva o total de linhas antes de fechar
    total_linhas_analisadas = linha_atual;

    if (arquivo_fonte) fclose(arquivo_fonte);

    printf("%d linhas analisadas, programa sintaticamente correto\n", total_linhas_analisadas);

    return 0;
}
