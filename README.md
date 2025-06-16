# Atividade-Individual

# Sistema de Processamento de Dados de Sensores

Este projeto implementa um sistema para processamento e consulta de dados coletados de sensores.

## Visão Geral

O sistema permite:
- Geração de dados simulados de sensores
- Processamento e organização de dados por sensor
- Consulta temporal eficiente com busca por proximidade

## Estrutura dos Dados

### Tipos de Dados Suportados
- *INTEIRO*: Números inteiros
- *BOOLEANO*: Valores true/false
- *RACIONAL*: Números decimais
- *STRING*: Texto de até 20 caracteres

### Formato de Arquivo

timestamp sensor valor
timestamp sensor valor


*Exemplo:*

1704067200 TEMP 23.5
1704067260 HUMID true
1704067320 MOTION 0

## Programas

### 1. pgm3.c - Gerador de Dados
Gera dados sintéticos para teste do sistema.

*Uso:*
bash
./pgm3 "DD/MM/AAAA" "HH:MM:SS" "DD/MM/AAAA" "HH:MM:SS" "SENSORES" [seed]

*Exemplo:*
bash
./pgm3 "01/01/2024" "00:00:00" "31/01/2024" "23:59:59" "TEMP:2,HUMID:1,MOTION:0"


*Tipos de Sensores:*
- *0*: INTEIRO (valores 0-999)
- *1*: BOOLEANO (true/false)
- *2*: RACIONAL (valores decimais 0-100)
- *3*: STRING (texto aleatório)

*Saída:* Arquivo `dados.txt` com 2000 leituras por sensor.

### 2. pgm1.c - Processador de Dados
Processa arquivo de dados brutos e organiza por sensor.

*Uso:*
bash
./pgm1 [arquivo_entrada]


*Funcionalidades:*
- Detecção automática de tipos de dados
- Validação de consistência por sensor
- Ordenação cronológica dos dados
- Geração de arquivos individuais por sensor

*Saída:* Arquivos `{sensor}.txt` para cada sensor encontrado.

### 3. pgm2.c - Consultor Temporal
Realiza consultas temporais em dados de sensores específicos.

*Modo Interativo:*
bash
./pgm2


*Modo Linha de Comando:*
bash
./pgm2 [nome_sensor] [timestamp]


*Funcionalidades:*
- Busca binária otimizada
- Encontra o registro mais próximo ao timestamp solicitado
- Validação completa de datas
- Interface interativa para entrada de dados

## Como Usar

### 1. Compilação
bash
gcc -o pgm3 pgm3.c
gcc -o pgm1 pgm1.c
gcc -o pgm2 pgm2.c


### 2. Fluxo de Trabalho
1. *Gerar dados de teste:*
   bash
   ./pgm3 "01/01/2024" "00:00:00" "01/02/2024" "23:59:59" "TEMP:2,PRESS:0,STATUS:1"
   

2. *Processar dados:*
   bash
   ./pgm1 dados.txt
   

3. *Consultar dados:*
   bash
   ./pgm2 TEMP
   

## Algoritmos Implementados

### Detecção de Tipos
O sistema identifica automaticamente o tipo baseado no valor:
- Strings "true"/"false" -> BOOLEANO
- Números inteiros -> INTEIRO
- Números decimais -> RACIONAL
- Outros valores -> STRING

### Busca Binária com Proximidade
- Busca binária no array ordenado por timestamp
- Algoritmo de proximidade

### Ordenação
- Bubble sort

## Validações

### Datas e Horários
- Validação de dias por mês
- Verificação de anos bissextos
- Horários válidos (0-23:0-59:0-59)
- Ordem cronológica (data inicial < data final)

### Dados
- Consistência de tipos por sensor
- Verificação de limites de arrays
- Tratamento de erros de arquivo e memória

## Limitações

- Máximo de 50.000 leituras
- Máximo de 50 sensores
- Processamento em memória
- Strings limitadas a 20 caracteres
- Timestamps limitados ao range do time_t

## Exemplo de Saída

### Processamento (pgm1):

Total de leituras: 6000
Sensores encontrados: 3
TEMP: 2000 leituras (racional)
HUMID: 2000 leituras (booleano)
MOTION: 2000 leituras (inteiro)


### Consulta (pgm2):

# RESULTADO
Sensor: TEMP
Consultado: 1704067200 (01/01/2024 00:00:00)
Encontrado: 1704067205 (01/01/2024 00:00:05)
Diferença: 5 segundos
Valor: 23.450000
Tipo: racional
Posição: 15


## Observações

- Código desenvolvido em C
- Utiliza bibliotecas padrão (stdio.h, stdlib.h, time.h, etc.)
- Gerenciamento de memória dinâmica para otimização

---
*Projeto desenvolvido para a disciplina de Estruturas de Dados*
