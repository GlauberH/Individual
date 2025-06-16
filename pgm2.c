#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

typedef enum {
    TIPO_INT, TIPO_BOOL, TIPO_FLOAT, TIPO_STR
} TipoDado;

typedef union {
    int inteiro;
    bool booleano;
    double racional;
    char string[20];
} Valor;

typedef struct {
    long timestamp;
    TipoDado tipo;
    Valor valor;
} Registro;

time_t converter_para_timestamp(int dia, int mes, int ano, int hora, int min, int seg) 
{
    struct tm t;

    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia;
    t.tm_hour = hora;
    t.tm_min = min;
    t.tm_sec = seg;
    t.tm_isdst = -1;

    time_t timestamp = mktime(&t);
    if (timestamp == -1) {
        printf("Data inválida. Tente novamente.\n");
    } else {
        return timestamp;
    }
}

int validarData(int dia, int mes, int ano, int hora, int min, int seg) {
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1970 || 
        hora < 0 || hora > 23 || min < 0 || min > 59 || seg < 0 || seg > 59) {
        return 0;
    }
    
    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) {
        return 0;
    }
    
    if (mes == 2) {
        int bissexto = (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0));
        if ((bissexto && dia > 29) || (!bissexto && dia > 28)) {
            return 0;
        }
    }
    
    return 1;
}

time_t lerTimestamp() {
    int dia, mes, ano, hora, min, seg;

    while (1) {
        printf("Data e hora (dd mm aaaa hh mm ss): ");
        if (scanf("%d %d %d %d %d %d", &dia, &mes, &ano, &hora, &min, &seg) != 6) {
            while (getchar() != '\n');
            printf("Entrada inválida.\n");
            continue;
        }

        if (!validarData(dia, mes, ano, hora, min, seg)) {
            printf("Data inválida.\n");
            continue;
        }

        time_t ts = converter_para_timestamp(dia, mes, ano, hora, min, seg);
        if (ts != -1) {
            return ts;
        }
    }
}

TipoDado detectarTipo(const char* str) {
    if (!strcmp(str, "true") || !strcmp(str, "false")) {
        return TIPO_BOOL;
    }
    
    char *end;
    strtol(str, &end, 10);
    if (*end == '\0') {
        return TIPO_INT;
    }
    
    strtod(str, &end);
    if (*end == '\0') {
        return TIPO_FLOAT;
    }
    
    return TIPO_STR;
}

Valor converterValor(const char* str, TipoDado tipo) {
    Valor v;
    
    switch (tipo) {
        case TIPO_INT:
            v.inteiro = atoi(str);
            break;
        case TIPO_BOOL:
            v.booleano = !strcmp(str, "true");
            break;
        case TIPO_FLOAT:
            v.racional = atof(str);
            break;
        case TIPO_STR:
            strcpy(v.string, str);
            break;
    }
    
    return v;
}

// Busca binária

int buscarMaisProximo(Registro *dados, int n, long tsBuscado) {
    if (n == 0) return -1;
    
    int inicio = 0, fim = n - 1;
    int maisProx = 0;
    long menorDif = labs(dados[0].timestamp - tsBuscado);
    
    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        long difAtual = labs(dados[meio].timestamp - tsBuscado);
        
        if (difAtual < menorDif) {
            menorDif = difAtual;
            maisProx = meio;
        }
        
        if (dados[meio].timestamp == tsBuscado) {
            return meio;
        } else if (dados[meio].timestamp < tsBuscado) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    
    // Verifica vizinhos

    for (int i = -1; i <= 1; i++) {
        int idx = maisProx + i;
        if (idx >= 0 && idx < n) {
            long dif = labs(dados[idx].timestamp - tsBuscado);
            if (dif < menorDif) {
                menorDif = dif;
                maisProx = idx;
            }
        }
    }
    
    return maisProx;
}

void imprimirValor(Valor v, TipoDado tipo) {
    switch (tipo) {
        case TIPO_INT:
            printf("%d", v.inteiro);
            break;
        case TIPO_BOOL:
            printf("%s", v.booleano ? "true" : "false");
            break;
        case TIPO_FLOAT:
            printf("%.6f", v.racional);
            break;
        case TIPO_STR:
            printf("%s", v.string);
            break;
    }
}

int main(int argc, char *argv[]) {
    char nomeSens[20];
    long tsConsulta;
    
    if (argc == 3) {
        strcpy(nomeSens, argv[1]);
        tsConsulta = atol(argv[2]);
    } else if (argc == 1) {
        printf("Nome do sensor: ");
        scanf("%s", nomeSens);
        
        printf("Informe a data e hora:\n");
        tsConsulta = lerTimestamp();
    } else {
        printf("Uso: %s [sensor timestamp] ou %s (interativo)\n", argv[0], argv[0]);
        return 1;
    }
    
    char nomeArq[30];
    sprintf(nomeArq, "%s.txt", nomeSens);
    
    FILE *arq = fopen(nomeArq, "r");
    if (!arq) {
        printf("Erro: arquivo %s não encontrado\n", nomeArq);
        return 1;
    }
    
    // Conta registros

    int numReg = 0;
    long ts;
    char sensor[20], valor[20];
    
    while (fscanf(arq, "%ld %s %s", &ts, sensor, valor) == 3) {
        numReg++;
    }
    
    if (numReg == 0) {
        printf("Arquivo vazio\n");
        fclose(arq);
        return 1;
    }
    
    Registro *dados = malloc(numReg * sizeof(Registro));
    if (!dados) {
        printf("Erro de memória\n");
        fclose(arq);
        return 1;
    }
    
    // Lê dados
    rewind(arq);
    int i = 0;
    
    while (fscanf(arq, "%ld %s %s", &ts, sensor, valor) == 3) {
        dados[i].timestamp = ts;
        dados[i].tipo = detectarTipo(valor);
        dados[i].valor = converterValor(valor, dados[i].tipo);
        i++;
    }
    
    fclose(arq);
    
    printf("Arquivo %s: %d registros\n", nomeArq, numReg);
    printf("Buscando timestamp %ld...\n", tsConsulta);
    
    int indice = buscarMaisProximo(dados, numReg, tsConsulta);
    
    if (indice == -1) {
        printf("Erro na busca\n");
        free(dados);
        return 1;
    }
    
    Registro result = dados[indice];
    
    struct tm *tmConsult = localtime(&tsConsulta);
    struct tm *tmEncontr = localtime(&result.timestamp);
    
    char dataConsult[30], dataEncontr[30];
    strftime(dataConsult, sizeof(dataConsult), "%d/%m/%Y %H:%M:%S", tmConsult);
    strftime(dataEncontr, sizeof(dataEncontr), "%d/%m/%Y %H:%M:%S", tmEncontr);
    
    long dif = labs(result.timestamp - tsConsulta);
    
    printf("\n# RESULTADO\n");
    printf("Sensor: %s\n", nomeSens);
    printf("Consultado: %ld (%s)\n", tsConsulta, dataConsult);
    printf("Encontrado: %ld (%s)\n", result.timestamp, dataEncontr);
    printf("Diferença: %ld segundos\n", dif);
    printf("Valor: ");
    imprimirValor(result.valor, result.tipo);
    printf("\nTipo: ");
    
    char *tipos[] = {"inteiro", "booleano", "racional", "string"};
    printf("%s\n", tipos[result.tipo]);
    
    printf("Posição: %d\n", indice + 1);
    
    free(dados);

}