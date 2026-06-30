# Trabalho Prático 2 — Gerenciador de Memória Virtual

Simulador, em **C (C11)**, da tradução de endereços lógicos para físicos em um
espaço de endereçamento virtual de 2¹⁶ = 65.536 bytes. O simulador usa um
**TLB** e uma **tabela de páginas**, implementa **paginação por demanda** a
partir de um `BACKING_STORE.bin`, faz **substituição de páginas por LRU
aproximado** (*aging*) e **substituição de TLB por FIFO**, e ao final relata as
estatísticas de desempenho.

> 📄 **Relatório técnico completo:** [`report/relatorio.md`](report/relatorio.md)

## Parâmetros da simulação (`include/config.h`)

| Parâmetro | Valor |
|---|---|
| Tamanho da página / quadro | 256 bytes |
| Entradas na tabela de páginas | 256 |
| Número de quadros | 128 |
| Memória física | 32.768 bytes |
| Entradas no TLB | 16 |
| Espaço virtual | 65.536 bytes |

## Estrutura

```text
vm_manager/
├── Makefile
├── README.md
├── include/            # config.h, tlb.h, page_table.h, memory.h, statistics.h
├── src/                # main.c, tlb.c, page_table.c, memory.c, statistics.c
├── data/               # generate_data.py (gera os dados de teste)
└── report/             # relatorio.md + resultados/ (evidências de teste)
```

## 1. Gerar os arquivos de entrada

```bash
cd data
python3 generate_data.py   # SEED=42 (determinístico)
cd ..
```

Gera `BACKING_STORE.bin`, `addresses_random.txt` e `addresses_location.txt`.

## 2. Compilar

```bash
make            # gcc -Wall -Wextra -O2 -std=c11 (sem warnings)
```

## 3. Executar

```bash
./vm < data/addresses_random.txt     # acesso aleatório
./vm < data/addresses_location.txt   # acesso com localidade de referência
```

Para cada endereço é impresso o endereço lógico, o físico e o byte armazenado;
ao final são exibidas a taxa de faltas de página e a taxa de acerto do TLB.

## Resultados (reproduzíveis, SEED=42)

| Cenário | Faltas de página | Taxa de faltas | Acerto do TLB |
|---|---|---|---|
| Aleatório (`addresses_random.txt`) | 4.958 | 0,496 | 0,065 |
| Localidade (`addresses_location.txt`) | 1.164 | 0,116 | 0,792 |

**Validação funcional:** 20.000 traduções verificadas (2 cenários × 10.000),
**0 divergências** — ver [`report/resultados/validacao.txt`](report/resultados/validacao.txt).

## Uso de IA

O desenvolvimento seguiu a metodologia **Spec-Driven Development (SDD)**; os
*prompts* e a documentação do uso de Inteligência Artificial estão na Seção 8 do
[relatório](report/relatorio.md).
