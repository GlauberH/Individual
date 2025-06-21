// Programa 03: Geração de Dados
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_SENS 20
#define LEIT_PSENSOR 2000
#define MAX_NAMES 20
#define MAX_STRING_VALUE 16

typedef enum {
    TIPO_INTEIRO, // CONJ_Z
    TIPO_BOOLEANO, // BINARIO  
    TIPO_RACIONAL, // CONJ_Q
    TIPO_STRING // TEXTO
} TipoDado;

// Sensor

typedef struct {
    char nome[MAX_NAMES];
    TipoDado tipo;
} Sens;

// Leitura

typedef struct {
    time_t timestamp;
    char sensor[MAX_NAMES];
    char valor[MAX_STRING_VALUE + 1];
} Leitura;

// Array global para evitar estouro de pilha
static Leitura leituras[MAX_SENS * LEIT_PSENSOR];

// Validar data

int validar_data(int dia, int mes, int ano) {
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1970) {
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

int validar_hora(int hora, int min, int seg) {
    return (hora >= 0 && hora <= 23 && min >= 0 && min <= 59 && seg >= 0 && seg <= 59);
}

time_t converter_para_timestamp(int dia, int mes, int ano, int hora, int min, int seg) {
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
        return -1;
    } else {
        return timestamp;
    }
}

time_t gerar_timestamp_aleatorio(struct tm *inicial, struct tm *final) {
    time_t timestamp_inicial, timestamp_final;
        
    timestamp_inicial = mktime(inicial);
    if (timestamp_inicial == -1) {
        return -1;
    }

    timestamp_final = mktime(final);
    if (timestamp_final == -1) {
        return -1;
    }

    time_t timestamp_aleatorio = timestamp_inicial + rand() % (timestamp_final - timestamp_inicial + 1);
    
    return timestamp_aleatorio;
}

// Converte string de tipo pra enum

TipoDado converter_tipo_string(const char* tipo_str) {
    if (strcmp(tipo_str, "CONJ_Z") == 0) {
        return TIPO_INTEIRO;
    } else if (strcmp(tipo_str, "BINARIO") == 0) {
        return TIPO_BOOLEANO;
    } else if (strcmp(tipo_str, "CONJ_Q") == 0) {
        return TIPO_RACIONAL;
    } else if (strcmp(tipo_str, "TEXTO") == 0) {
        return TIPO_STRING;
    } else {
        return -1;
    }
}

int validar_tipo(const char* tipo_str) {
    return (strcmp(tipo_str, "CONJ_Z") == 0 || 
            strcmp(tipo_str, "BINARIO") == 0 || 
            strcmp(tipo_str, "CONJ_Q") == 0 || 
            strcmp(tipo_str, "TEXTO") == 0);
}

// Valor aleatório

void gerarValor(TipoDado tipo, char *valor) {
    switch (tipo) {
        case TIPO_INTEIRO:
            sprintf(valor, "%d", rand() % 1000);
            break;
        case TIPO_BOOLEANO:
            strcpy(valor, (rand() % 2) ? "true" : "false");
            break;
        case TIPO_RACIONAL:
            sprintf(valor, "%.6f", (rand() / (double)RAND_MAX) * 100.0);
            break;
        case TIPO_STRING: {
            const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            int len = 3 + rand() % 14;
            if (len > MAX_STRING_VALUE) len = MAX_STRING_VALUE;
            for (int i = 0; i < len; i++) {
                valor[i] = chars[rand() % 26];
            }
            valor[len] = '\0';
            break;
        }
    }
}

// Converte tipo pra string legível

const char* tipo_para_str(TipoDado tipo) {
    switch (tipo) {
        case TIPO_INTEIRO: return "CONJ_Z";
        case TIPO_BOOLEANO: return "BINARIO";
        case TIPO_RACIONAL: return "CONJ_Q";
        case TIPO_STRING: return "TEXTO";
        default: return "DESCONHECIDO";
    }
}

int comparar_leituras_dec(const void *a, const void *b) {
    const Leitura *la = (const Leitura *)a;
    const Leitura *lb = (const Leitura *)b;
    
    if (la->timestamp > lb->timestamp) return -1;
    if (la->timestamp < lb->timestamp) return 1;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 6) {
        printf("Uso: %s <data_inicio> <hora_inicio> <data_fim> <hora_fim> <sensores> [seed]\n", argv[0]);
        printf("Exemplo: %s \"01/01/2024\" \"00:00:00\" \"31/12/2024\" \"23:59:59\" \"TEMP:CONJ_Z,PRESS:CONJ_Q,VIBR:BINARIO,NOME:TEXTO\"\n", argv[0]);
        printf("Tipos válidos: CONJ_Z (inteiro), CONJ_Q (float), TEXTO (string), BINARIO (booleano)\n");
        printf("A linha de comado de execução não é válida.\n");
        return 1;
    }
    
    int dia_ini, mes_ini, ano_ini, hora_ini, min_ini, seg_ini;
    int dia_fim, mes_fim, ano_fim, hora_fim, min_fim, seg_fim;
    unsigned seed = time(NULL);
    
    if (sscanf(argv[1], "%d/%d/%d", &dia_ini, &mes_ini, &ano_ini) != 3) {
        printf("Erro: Data inicial inválida. Use formato DD/MM/AAAA\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    if (sscanf(argv[2], "%d:%d:%d", &hora_ini, &min_ini, &seg_ini) != 3) {
        printf("Erro: Hora inicial inválida. Use formato HH:MM:SS\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    if (sscanf(argv[3], "%d/%d/%d", &dia_fim, &mes_fim, &ano_fim) != 3) {
        printf("Erro: Data final inválida. Use formato DD/MM/AAAA\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    if (sscanf(argv[4], "%d:%d:%d", &hora_fim, &min_fim, &seg_fim) != 3) {
        printf("Erro: Hora final inválida. Use formato HH:MM:SS\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    // Validar Data e Hora

    if (!validar_data(dia_ini, mes_ini, ano_ini) || !validar_hora(hora_ini, min_ini, seg_ini)) {
        printf("Erro: Campos de data e hora inicial fora do intervalo de valores válidos.\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    if (!validar_data(dia_fim, mes_fim, ano_fim) || !validar_hora(hora_fim, min_fim, seg_fim)) {
        printf("Erro: Campos de data e hora final fora do intervalo de valores válidos.\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    // Verifica se a data final é depois da inicial

    time_t ts_ini = converter_para_timestamp(dia_ini, mes_ini, ano_ini, hora_ini, min_ini, seg_ini);
    time_t ts_fim = converter_para_timestamp(dia_fim, mes_fim, ano_fim, hora_fim, min_fim, seg_fim);
    
    if (ts_ini == -1 || ts_fim == -1) {
        printf("Erro: Campos de data e hora fora do intervalo de valores válidos.\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    if (ts_ini >= ts_fim) {
        printf("Erro: Data inicial deve ser anterior à data final.\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    char sens_str[1000];
    strncpy(sens_str, argv[5], sizeof(sens_str) - 1);
    sens_str[sizeof(sens_str) - 1] = '\0';
    
    Sens sens[MAX_SENS];
    int num_sens = 0;
    
    char *pedaco = strtok(sens_str, ",");
    while (pedaco != NULL && num_sens < MAX_SENS) {
        char *dois_pts = strchr(pedaco, ':');
        if (dois_pts == NULL) {
            printf("Erro: Sensor mal formatado: %s. Use formato NOME:TIPO\n", pedaco);
            printf("A linha de comando de execução não é válida.\n");
            return 1;
        }
        
        *dois_pts = '\0';
        char *nome = pedaco;
        char *tipo_str = dois_pts + 1;
        
        if (strlen(nome) == 0 || strlen(nome) >= MAX_NAMES) {
            printf("Erro: Nome do sensor inválido: %s\n", nome);
            printf("A linha de comando de execução não é válida.\n");
            return 1;
        }
        
        if (!validar_tipo(tipo_str)) {
            printf("Erro: Tipo do sensor passado no argumento diferente de CONJ_Z, CONJ_Q, TEXTO e BINARIO.\n");
            printf("Sensor: %s, Tipo inválido: %s\n", nome, tipo_str);
            printf("A linha de comando de execução não é válida.\n");
            return 1;
        }
        
        strcpy(sens[num_sens].nome, nome);
        sens[num_sens].tipo = converter_tipo_string(tipo_str);
        num_sens++;
        
        pedaco = strtok(NULL, ",");
    }
    
    if (num_sens == 0) {
        printf("Erro: Nenhum sensor válido encontrado.\n");
        printf("A linha de comando de execução não é válida.\n");
        return 1;
    }
    
    if (argc >= 7) {
        seed = (unsigned)atoi(argv[6]);
    }
    srand(seed);
    
    printf("# GERANDO DADOS DOS SENSORES\n");
    printf("Período: %02d/%02d/%04d %02d:%02d:%02d até %02d/%02d/%04d %02d:%02d:%02d\n",
           dia_ini, mes_ini, ano_ini, hora_ini, min_ini, seg_ini,
           dia_fim, mes_fim, ano_fim, hora_fim, min_fim, seg_fim);
    printf("Sensores: %d | Seed: %u\n", num_sens, seed);
    printf("Leituras por sensor: %d\n\n", LEIT_PSENSOR);
    
    struct tm tm_inicial = {0}, tm_final = {0};
    
    tm_inicial.tm_year = ano_ini - 1900;
    tm_inicial.tm_mon = mes_ini - 1;
    tm_inicial.tm_mday = dia_ini;
    tm_inicial.tm_hour = hora_ini;
    tm_inicial.tm_min = min_ini;
    tm_inicial.tm_sec = seg_ini;
    tm_inicial.tm_isdst = -1;
    
    tm_final.tm_year = ano_fim - 1900;
    tm_final.tm_mon = mes_fim - 1;
    tm_final.tm_mday = dia_fim;
    tm_final.tm_hour = hora_fim;
    tm_final.tm_min = min_fim;
    tm_final.tm_sec = seg_fim;
    tm_final.tm_isdst = -1;
    
    size_t total_leit = num_sens * LEIT_PSENSOR;
    
    size_t idx = 0;
    for (int i = 0; i < num_sens; i++) {
        printf("Gerando dados para sensor %s [%s]...\n", 
               sens[i].nome, tipo_para_str(sens[i].tipo));
        
        for (int j = 0; j < LEIT_PSENSOR; j++) {
            time_t ts = gerar_timestamp_aleatorio(&tm_inicial, &tm_final);
            if (ts == -1) {
                printf("Erro ao gerar timestamp.\n");
                return 1;
            }
            
            leituras[idx].timestamp = ts;
            strncpy(leituras[idx].sensor, sens[i].nome, MAX_NAMES - 1);
            leituras[idx].sensor[MAX_NAMES - 1] = '\0';
            
            gerarValor(sens[i].tipo, leituras[idx].valor);
            
            idx++;
        }
    }
    
    printf("\nOrdenando %zu leituras em ordem DECRESCENTE por timestamp...\n", total_leit);
    qsort(leituras, total_leit, sizeof(Leitura), comparar_leituras_dec);
    
    // Salva no arquivo
    FILE *arquivo = fopen("dados.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo dados.txt\n");
        return 1;
    }
    
    printf("Salvando arquivos dados.txt...\n");
    
    for (size_t i = 0; i < total_leit; i++) {
        fprintf(arquivo, "%ld %s %s\n",
                (long)leituras[i].timestamp,
                leituras[i].sensor,
                leituras[i].valor);
    }
    
    fclose(arquivo);
    
    // Relatório final

    printf("\n# ARQUIVO GERADO COM SUCESSO\n");
    printf("Arquivo: dados.txt\n");
    printf("Total de leituras: %zu\n", total_leit);
    printf("Ordenação: decrscente por timestamp\n");
    printf("Formato: <TIMESTAMP> <ID_SENSOR> <VALOR>\n\n");
    
    printf("Resumo dos sensores:\n");
    for (int i = 0; i < num_sens; i++) {
        printf("- %s (%s): %d leituras\n", 
               sens[i].nome, tipo_para_str(sens[i].tipo), LEIT_PSENSOR);
    }
    
}
