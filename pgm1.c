#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEIT 50000
#define MAX_SENS 50

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
    char sensor[20];
    TipoDado tipo;
    Valor valor;
} Leitura;

typedef struct {
    char nome[20];
    TipoDado tipo;
    int count;
} InfoSensor;

Leitura dados[MAX_LEIT];
InfoSensor sensores[MAX_SENS];
int numLeit = 0, numSens = 0;
int linhaAtual = 0;

TipoDado detectarTipo(char* str) {
    if (!strcmp(str, "true") || !strcmp(str, "false")) 
        return TIPO_BOOL;
    
    char *end;
    strtol(str, &end, 10);
    if (*end == '\0') return TIPO_INT;
    
    strtod(str, &end);
    if (*end == '\0') return TIPO_FLOAT;
    
    return TIPO_STR;
}

Valor convValor(char* str, TipoDado t) {
    Valor v;
    switch(t) {
        case TIPO_INT: v.inteiro = atoi(str); break;
        case TIPO_BOOL: v.booleano = !strcmp(str, "true"); break;
        case TIPO_FLOAT: v.racional = atof(str); break;
        case TIPO_STR: 
            strncpy(v.string, str, sizeof(v.string) - 1);
            v.string[sizeof(v.string) - 1] = '\0';
            break;
    }
    return v;
}

// Ordenação Decrescente

void ordenarDadosDescres(Leitura *arr, int n) {
    for(int i = 0; i < n-1; i++) {
        bool trocou = false;
        for(int j = 0; j < n-i-1; j++) {
            if(arr[j].timestamp < arr[j+1].timestamp) {
                Leitura tmp = arr[j];
                arr[j] = arr[j+1]; 
                arr[j+1] = tmp;
                trocou = true;
            }
        }
        if(!trocou) break;
    }
}

void escreverValor(FILE *f, Valor v, TipoDado t) {
    switch(t) {
        case TIPO_INT: fprintf(f, "%d", v.inteiro); break;
        case TIPO_BOOL: fprintf(f, "%s", v.booleano ? "true" : "false"); break;
        case TIPO_FLOAT: fprintf(f, "%.6f", v.racional); break;
        case TIPO_STR: fprintf(f, "%s", v.string); break;
    }
}

bool valLinha(const char* linha, char* timestampStr, char* sensorTemp, char* valorStr) {
    char linhaCopia[256];
    strncpy(linhaCopia, linha, sizeof(linhaCopia) - 1);
    linhaCopia[sizeof(linhaCopia) - 1] = '\0';
    
    int len = strlen(linhaCopia);
    if (len > 0 && linhaCopia[len-1] == '\n') {
        linhaCopia[len-1] = '\0';
    }
    
    if (strlen(linhaCopia) == 0) {
        return false;
    }
    
    int campos = sscanf(linhaCopia, "%s %s %s", timestampStr, sensorTemp, valorStr);
    
    if (campos != 3) {
        return false;
    }
    
    char *end;
    long ts = strtol(timestampStr, &end, 10);
    if (*end != '\0' || ts <= 0) {
        return false;
    }
    
    if (strlen(sensorTemp) == 0 || strlen(sensorTemp) >= 20) {
        return false;
    }
    
    if (strlen(valorStr) == 0) {
        return false;
    }
    
    return true;
}

int main(int argc, char **argv) {
    char nomeArq[256];
    
    if(argc == 2) {
        strcpy(nomeArq, argv[1]);
    } else {
        printf("Uso: %s <nome_arquivo>\n", argv[0]);
        printf("Digite o nome do arquivo: ");
        if (scanf("%255s", nomeArq) != 1) {
            printf("Erro ao ler nome do arquivo\n");
            return 1;
        }
    }
    
    FILE *f = fopen(nomeArq, "r");
    if(!f) {
        printf("Erro ao abrir %s\n", nomeArq);
        return 1;
    }

    char linha[256], timestampStr[32], sensorTemp[20], valorStr[20];
    linhaAtual = 0;
    int linhasInvalidas = 0;
    
    while(fgets(linha, sizeof(linha), f)) {
        linhaAtual++;
        
        if (!valLinha(linha, timestampStr, sensorTemp, valorStr)) {
            printf("AVISO: Linha %d inválida: %s", linhaAtual, linha);
            linhasInvalidas++;
            continue;
        }
        
        if(numLeit >= MAX_LEIT) {
            printf("Limite de leituras atingido: %d\n", MAX_LEIT);
            break;
        }
        
        dados[numLeit].timestamp = atol(timestampStr);
        strncpy(dados[numLeit].sensor, sensorTemp, sizeof(dados[numLeit].sensor) - 1);
        dados[numLeit].sensor[sizeof(dados[numLeit].sensor) - 1] = '\0';
        
        TipoDado tipoDetect = detectarTipo(valorStr);
        
        int indSensor = -1;
        for(int i = 0; i < numSens; i++) {
            if(!strcmp(sensores[i].nome, sensorTemp)) {
                indSensor = i;
                if(sensores[i].tipo != tipoDetect) {
                    tipoDetect = sensores[i].tipo;
                }
                break;
            }
        }
        
        // Novo sensor

        if(indSensor == -1) {
            if(numSens >= MAX_SENS) {
                printf("Limite de sensores atingido: %d\n", MAX_SENS);
                break;
            }
            strncpy(sensores[numSens].nome, sensorTemp, sizeof(sensores[numSens].nome) - 1);
            sensores[numSens].nome[sizeof(sensores[numSens].nome) - 1] = '\0';
            sensores[numSens].tipo = tipoDetect;
            sensores[numSens].count = 0;
            indSensor = numSens++;
        }
        
        dados[numLeit].tipo = tipoDetect;
        dados[numLeit].valor = convValor(valorStr, tipoDetect);
        sensores[indSensor].count++;
        numLeit++;
    }
    fclose(f);
    
    printf("Total de leituras válidas: %d\n", numLeit);
    printf("Total de linhas inválidas: %d\n", linhasInvalidas);
    printf("Sensores encontrados: %d\n", numSens);
    
    for(int i = 0; i < numSens; i++) {
        char *nomesTipo[] = {"inteiro", "booleano", "racional", "string"};
        printf("%s: %d leituras (%s)\n", sensores[i].nome, sensores[i].count, 
               nomesTipo[sensores[i].tipo]);
    }
    
    // Arquivos por sensor (Ordem Decrecente)

    for(int s = 0; s < numSens; s++) {
        char nomeArqSens[30];
        snprintf(nomeArqSens, sizeof(nomeArqSens), "%s.txt", sensores[s].nome);
        
        Leitura *dadosSensor = malloc(sensores[s].count * sizeof(Leitura));
        if (!dadosSensor) {
            printf("Erro de alocação de memória para sensor %s\n", sensores[s].nome);
            continue;
        }
        
        int countSensor = 0;
        
        for(int i = 0; i < numLeit; i++) {
            if(!strcmp(dados[i].sensor, sensores[s].nome)) {
                dadosSensor[countSensor++] = dados[i];
            }
        }

        ordenarDadosDescres(dadosSensor, countSensor);
        
        FILE *arq = fopen(nomeArqSens, "w");
        if(arq) {
            for(int i = 0; i < countSensor; i++) {
                fprintf(arq, "%ld %s ", dadosSensor[i].timestamp, dadosSensor[i].sensor);
                escreverValor(arq, dadosSensor[i].valor, dadosSensor[i].tipo);
                fprintf(arq, "\n");
            }
            fclose(arq);
            printf("Arquivo %s criado com %d leituras (ordem DECRESCENTE).\n", nomeArqSens, countSensor);
        } else {
            printf("Erro ao criar arquivo %s\n", nomeArqSens);
        }
        free(dadosSensor);
    }
    
}
