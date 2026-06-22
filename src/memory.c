#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "config.h"
#include "page_table.h"
#include "tlb.h"

static signed char physical_memory[NUM_FRAMES][FRAME_SIZE];

/*
 * Indica qual página está carregada em cada quadro.
 * Valor -1 indica quadro livre.
 */
static int frame_to_page[NUM_FRAMES];

static FILE *backing = NULL;

void memory_init(FILE *backing_store)
{
    backing = backing_store;

    for (int i = 0; i < NUM_FRAMES; i++) {
        frame_to_page[i] = -1;

        for (int j = 0; j < FRAME_SIZE; j++) {
            physical_memory[i][j] = 0;
        }
    }
}

static int find_free_frame(void)
{
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (frame_to_page[i] == -1) {
            return i;
        }
    }

    return -1;
}

int handle_page_fault(int page)
{
    /*
     * TODO:
     * 1. Procurar quadro livre.
     * 2. Se não houver quadro livre, selecionar página vítima.
     * 3. Invalidar página vítima na tabela de páginas.
     * 4. Remover página vítima do TLB.
     * 5. Ler a página correta do BACKING_STORE.bin.
     * 6. Atualizar frame_to_page.
     * 7. Atualizar tabela de páginas.
     * 8. Retornar número do frame.
     */

    if (page < 0 || page >= PAGE_TABLE_SIZE) {
        fprintf(stderr, "Erro: pagina invalida %d\n", page);
        exit(1);
    }

    int frame = find_free_frame();

    if (frame == -1) {
        int victim_page = select_victim_page();

        /*
         * TODO:
         * Obter o quadro da página vítima.
         * Invalidar tabela e TLB.
         */

        if (victim_page < 0) {
            fprintf(stderr, "Erro interno: nao foi possivel selecionar pagina vitima.\n");
            exit(1);
        }

        frame = page_table_get_frame(victim_page);

        if (frame < 0 || frame >= NUM_FRAMES) {
            fprintf(stderr, "Erro interno: quadro invalido para pagina vitima %d.\n", victim_page);
            exit(1);
        }

        page_table_invalidate(victim_page);
        tlb_remove(victim_page);
        frame_to_page[frame] = -1;
    }

    /*
     * TODO:
     * Fazer fseek para page * PAGE_SIZE.
     * Fazer fread de PAGE_SIZE bytes para physical_memory[frame].
     */

    if (backing == NULL) {
        fprintf(stderr, "Erro interno: BACKING_STORE nao inicializado.\n");
        exit(1);
    }

    if (fseek(backing, page * PAGE_SIZE, SEEK_SET) != 0) {
        fprintf(stderr, "Erro: falha no fseek para pagina %d.\n", page);
        exit(1);
    }

    size_t bytes_read = fread(physical_memory[frame], sizeof(signed char), PAGE_SIZE, backing);
    if (bytes_read != PAGE_SIZE) {
        fprintf(stderr, "Erro: falha ao ler pagina %d do BACKING_STORE.\n", page);
        exit(1);
    }

    frame_to_page[frame] = page;
    page_table_update(page, frame);

    return frame;
}

int select_victim_page(void)
{
    /*
     * TODO:
     * Selecionar a página válida com menor aging_counter.
     * Em caso de empate, qualquer critério consistente pode ser usado.
     */

    int victim_page = -1;
    unsigned char min_aging = 0xFF;

    for (int page = 0; page < PAGE_TABLE_SIZE; page++) {
        if (!page_table_is_valid(page)) {
            continue;
        }

        unsigned char aging = page_table_get_aging_counter(page);

        if (victim_page == -1 || aging < min_aging) {
            victim_page = page;
            min_aging = aging;
        }
    }

    return victim_page;
}

signed char read_memory(int frame, int offset)
{
    /*
     * TODO:
     * Retornar o byte armazenado em physical_memory[frame][offset].
     */

    if (frame < 0 || frame >= NUM_FRAMES || offset < 0 || offset >= FRAME_SIZE) {
        return 0;
    }

    return physical_memory[frame][offset];
}

int get_page_loaded_in_frame(int frame)
{
    if (frame < 0 || frame >= NUM_FRAMES) {
        return -1;
    }

    return frame_to_page[frame];
}
