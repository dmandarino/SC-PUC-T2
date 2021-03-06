//
//  SCT2.c
//
//
//  Created by Douglas Mandarino on 11/18/15.
//
//

#include "SCT2.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define TIMESLICE 10

typedef struct tarefa{
    char tag[5];
    int time;
    struct tarefa * prox;
} Tarefa;


typedef struct processo{
    int tempo_permanencia;
    int num_processo;
    int mem_requerida;
    int entradas;
    int alocado;
    Tarefa * t;
    
    struct processo * prox;
} Processo;


//variaveis globais
Processo * Execucao;
Processo * Bloqueados;
Processo * Prontos;

int part1[8];
int part2[4];
int part3[2];
int part4[1];
int part5[1];

int tamPart1 = sizeof(part1)/sizeof(int);
int tamPart2 = sizeof(part2)/sizeof(int);
int tamPart3 = sizeof(part3)/sizeof(int);
int tamPart4 = sizeof(part4)/sizeof(int);
int tamPart5 = sizeof(part5)/sizeof(int);

int vetorPart[5];


int qtd_processos = 0;
int TEMPO_TOTAL = 0;
int next= 0; // variavel para a politica next fit;

Processo * Cria_Processo_Lista(Processo * p);
Tarefa * Cria_Tarefa_Lista(Tarefa * Task, char * tag, int time);
void Inicializa_Memoria (void);
int Perpara_FirstFit(Processo * p);
int Prepara_NextFit(Processo* p);
int Prepara_BestFit(Processo * p);
int Prepara_WorstFit(Processo * p);
void Tempo_Processos(Processo * p);
void Reduz_Tempo_Execucao(Processo * p);
void Reduz_Tempo_Bloqueio(Processo * p);
void Bloq_Para_Prontos(Processo * p1, Processo * p2, int num_proc);
void Prontos_Para_Bloq(Processo * p1, Processo * p2, int num_proc);
void Execucao_Para_Prontos(Processo * p1, Processo * p2, int num_proc);
void Prontos_Para_Execucao(Processo * p1, Processo * p2, int num_proc);
void Desaloca_Processo(Processo * p);
void Remove_Tarefa_Lista(Processo * p);
void Verifica_Tarefa_Lista( Processo * p);
void Chama_Politica_Escolhida(int politic, Processo* pAux, Processo* Prontos);
void BubbleSort_Maior_Menor(int vetor[], int tamanho);
void BubbleSort_Menor_Maior(int n, int *v);

int main(void)
{
    int i=0, politic;
    Processo * pAux;
    Prontos = NULL; Execucao = NULL; Bloqueados = NULL;
    Prontos = Cria_Processo_Lista(Prontos);
    pAux = Prontos;
    Inicializa_Memoria();
    
    vetorPart[0] = tamPart1;
    vetorPart[1] = tamPart2;
    vetorPart[2] = tamPart3;
    vetorPart[3] = tamPart4;
    vetorPart[4] = tamPart5;
    
    printf("\n\n===========================================================================================\n\n");
    printf("Escolha uma das Politicas de Alocacao de Memoria:\n\n\t1-FIRST FIT\n\t2-NEXT FIT\n\t3-BEST FIT\n\t4-WORST FIT\n\n");
    
    printf("\tDigite a Politica Escolhida: ");
    scanf("%d", &politic);
    printf("\n\n================================COMECANDO OS PROCESSOS=====================================");
    
    // Tomada de Decisao A partir da Politica Escolhida
    while(qtd_processos != 0){
        if(TEMPO_TOTAL% 5 == 0){
            printf("\n\n");
            
            
            printf("=============================Uso de Processos na Memoria:=================================\n\n\tMemoria: ");
            for( i = 0; i < 8; i++)  printf("[%d]", part1[i]);
            printf("--");
            for( i = 0; i < 4; i++)  printf("[%d]", part2[i]);
            printf("--");
            for( i = 0; i < 2; i++)  printf("[%d]", part3[i]);
            printf("--");
            printf("[%d]", part4[0]);
            printf("--");
            printf("[%d]", part5[0]);
            
            printf("\n\n");
            
            
            printf("\n--Processos em Execucao\n\n");
            Tempo_Processos( Execucao);
            printf("\n--Processos Prontos\n\n");
            Tempo_Processos(Prontos);
            printf("\n--Processos Bloqueados\n\n");
            Tempo_Processos( Bloqueados);
        }
        
        Chama_Politica_Escolhida(politic, pAux, Prontos);
        
        Reduz_Tempo_Execucao( Execucao);
        Reduz_Tempo_Bloqueio( Bloqueados);
        Verifica_Tarefa_Lista(Prontos);
        
        TEMPO_TOTAL++;
        pAux = Prontos;
        if(Prontos == NULL && Execucao == NULL && Bloqueados == NULL){
            printf("\n\n===========================================================================================");
            printf("\n\nTodos os processos terminaram.\n\n");
            printf("TEMPO TOTAL:%d\n\n", TEMPO_TOTAL);
            printf("===========================================================================================\n\n");
            break;
        }
    }
    return 0;
}


void Chama_Politica_Escolhida(int politic, Processo* pAux, Processo* Prontos)
{
    if(pAux != NULL && strcmp(pAux->t->tag, "exec") == 0){
        switch (politic)
        {
            case 1:
                while(Perpara_FirstFit(Prontos)== -1)
                    Desaloca_Processo( Execucao);
                break;
                
            case 2:
                while(Prepara_NextFit(Prontos)== -1){
                    if(Prepara_NextFit(Prontos)!=-1)
                        break;
                    Desaloca_Processo( Execucao);
                }
                break;
                
            case 3:
                while(Prepara_BestFit(Prontos)== -1)
                    Desaloca_Processo( Execucao);
                break;
                
            default:
                while(Prepara_WorstFit(Prontos)== -1)
                    Desaloca_Processo( Execucao);
        }
    } else if(pAux != NULL && strcmp(pAux->t->tag, "io")==0)
        Prontos_Para_Bloq( Bloqueados, Prontos, pAux->num_processo);
    
    pAux = Prontos;
    
}


Processo * Cria_Processo_Lista(Processo * p)
{
    int proc, mem, ent, time;
    Processo * novo;
    char tag[5];
    FILE * f;
    f = fopen("arquivo.txt", "r");
    if(f == NULL)
        exit(0);
    
    int qtdProcessosTxt;
    
    fscanf(f, "%d", &qtdProcessosTxt);
    
    qtd_processos = qtdProcessosTxt;
    
    while(qtdProcessosTxt > 0){
        
        fscanf(f, "%d %d", &proc, &mem);
        fscanf(f, "%d", &ent);
        
        novo = (Processo *) malloc (sizeof(Processo));
        novo->tempo_permanencia = 0;
        novo->num_processo = proc;
        novo->mem_requerida = mem;
        novo->entradas = ent;
        novo->prox = p;
        novo->alocado = 0;
        novo->t=NULL;
        while(ent > 0){
            fscanf(f, " %s %d", tag, &time);
            novo->t=Cria_Tarefa_Lista( novo->t, tag, time);
            ent--;
        }
        p=novo;
        
        qtdProcessosTxt--;
    }
    return p;
}

Tarefa * Cria_Tarefa_Lista(Tarefa * Task, char * tag, int time)
{
    Tarefa * temp= (Tarefa*)malloc(sizeof(Tarefa));
    strcpy(temp->tag, tag);
    temp->time = time;
    temp->prox = Task;
    return temp;
}

void Inicializa_Memoria (void)
{
    int i;
    for ( i = 0; i < 8; i++ ){
        if( i < 2 ) part3[i] = 0;
        if( i < 4 ) part2[i] = 0;
        part1[i] = 0;
    }
    part4[0] = 0;
    part5[0] = 0;
    return;
}

int Perpara_FirstFit(Processo * p)
{
    Processo * temp=p;
    int i;
    
    if(temp->alocado == 0){
        temp->alocado = 1;
        if (part1[0] == 0 && tamPart1 >= temp->mem_requerida){
            for( i = 0; i < temp->mem_requerida; i++){
                part1[i] = temp->num_processo;
            }
        }else if(part2[0] == 0 && tamPart2 >= temp->mem_requerida){
            for( i = 0; i < temp->mem_requerida; i++){
                part2[i] = temp->num_processo;
            }
        }else if(part3[0] == 0 && tamPart3 >= temp->mem_requerida){
            for( i = 0; i < temp->mem_requerida; i++){
                part3[i] = temp->num_processo;
            }
        }else if(part4[0] == 0 && tamPart4 >= temp->mem_requerida){
            for( i = 0; i < temp->mem_requerida; i++){
                part4[i] = temp->num_processo;
            }
        }else if(part5[0] == 0 && tamPart5 >= temp->mem_requerida){
            for( i = 0; i < temp->mem_requerida; i++){
                part5[i] = temp->num_processo;
            }
        }else{
            temp->alocado = 0;
            return -1;
        }
    }
    Prontos_Para_Execucao( Execucao, Prontos,temp->num_processo);
    return 0;
    
}

int Prepara_NextFit(Processo* p){
    
    Processo* temp = p;
    int i;
    
    while(next != 0){
        if(temp->alocado == 0){
            temp->alocado = 1;
            if (part1[0] == 0 && tamPart1 >= temp->mem_requerida && next == 1){
                next = 2;
                for( i = 0; i < temp->mem_requerida; i++){
                    part1[i] = temp->num_processo;
                }
                Prontos_Para_Execucao( Execucao, Prontos,temp->num_processo);
                return 0;
            }else if(part2[0] == 0 && tamPart2 >= temp->mem_requerida && next == 2){
                next = 3;
                for( i = 0; i < temp->mem_requerida; i++){
                    part2[i] = temp->num_processo;
                }
                Prontos_Para_Execucao( Execucao, Prontos,temp->num_processo);
                return 0;
            }else if(part3[0] == 0 && tamPart3 >= temp->mem_requerida && next == 3){
                next = 4;
                for( i = 0; i < temp->mem_requerida; i++){
                    part3[i] = temp->num_processo;
                }
                Prontos_Para_Execucao( Execucao, Prontos,temp->num_processo);
                return 0;
            }else if(part4[0] == 0 && tamPart4 >= temp->mem_requerida && next == 4){
                next = 5;
                for( i = 0; i < temp->mem_requerida; i++){
                    part4[i] = temp->num_processo;
                }
                Prontos_Para_Execucao( Execucao, Prontos,temp->num_processo);
                return 0;
                
            }else if(part5[0] == 0 && tamPart5 >= temp->mem_requerida && next == 5){
                next = 1;
                for( i = 0; i < temp->mem_requerida; i++){
                    part5[i] = temp->num_processo;
                }
                Prontos_Para_Execucao( Execucao, Prontos,temp->num_processo);
                return 0;
            }else{
                // se nao houver espaco continuo em memoria para alocar o programa, realocacao dos espacos
                next = 0;
            }
        }
    }
    next = 1;
    return -1;
    
    
    
    
}
void Tempo_Processos(Processo * p)
{
    Processo * temp = p;
    while(temp != NULL){
        printf("\tProcesso #%d  -  Tempo Restante: %d UT\n", temp->num_processo, temp->t->time);
        temp = temp->prox;
    }
    return;
}

void Reduz_Tempo_Execucao(Processo * p)
{
    int i;
    Processo * temp = p;
    while( temp!= NULL){
        temp->tempo_permanencia = temp->tempo_permanencia + 1;
        temp->t->time = temp->t->time - 1;
        if(temp->t->time == 0){
            temp->tempo_permanencia = 0;
            
            if (part1[0] == temp->num_processo ){
                for( i = 0; i < temp->mem_requerida; i++){
                    part1[i] = 0;
                }
            }else if(part2[0] == temp->num_processo){
                for( i = 0; i < temp->mem_requerida; i++){
                    part2[i] = 0;
                }
            }else if(part3[0] == temp->num_processo){
                for( i = 0; i < temp->mem_requerida; i++){
                    part3[i] = 0;
                }
            }else if(part4[0] == temp->num_processo){
                for( i = 0; i < temp->mem_requerida; i++){
                    part4[i] = 0;
                }
            }else if(part5[0] == temp->num_processo){
                for( i = 0; i < temp->mem_requerida; i++){
                    part5[i] = 0;
                }
            }
            
            if(temp->t->time == 0)
                Remove_Tarefa_Lista(temp);
            Execucao_Para_Prontos( Prontos, Execucao, temp->num_processo);
        }
        if(temp->tempo_permanencia == 10){
            temp->tempo_permanencia = 0;
            Execucao_Para_Prontos( Prontos, Execucao, temp->num_processo);
        }
        temp = temp->prox;
    }
    return;
}

void Reduz_Tempo_Bloqueio(Processo * p)
{
    Processo * temp = p;
    while( temp!= NULL){
        temp->t->time = temp->t->time - 1;
        if(temp->t->time == 0){
            Remove_Tarefa_Lista(temp);
            Bloq_Para_Prontos( Prontos, Bloqueados, temp->num_processo);
        }
        temp = temp->prox;
    }
    return;
}

void Desaloca_Processo(Processo * p)
{
    int proc_mais_antigo, tempo_mais_antigo, i;
    Processo * temp = p;
    
    proc_mais_antigo = p->num_processo;
    tempo_mais_antigo = p->tempo_permanencia;
    temp=temp->prox;
    
    while(temp != NULL)
    {
        if(tempo_mais_antigo < p->tempo_permanencia){
            tempo_mais_antigo = p->tempo_permanencia;
            proc_mais_antigo = p->num_processo;
        }
        temp=temp->prox;
    }
    
    if (part1[0] == proc_mais_antigo){
        for( i = 0; i < tamPart1; i++){
            part1[i] = 0;
        }
    }else if(part2[0] == proc_mais_antigo){
        for( i = 0; i < tamPart2; i++){
            part2[i] = 0;
        }
    }else if(part3[0] == proc_mais_antigo){
        for( i = 0; i < tamPart3; i++){
            part3[i] = 0;
        }
    }else if(part4[0] == proc_mais_antigo){
        for( i = 0; i < tamPart4; i++){
            part4[i] = 0;
        }
    }else if(part5[0] == proc_mais_antigo){
        for( i = 0; i < tamPart5; i++){
            part5[i] = 0;
        }
    }
    Execucao_Para_Prontos( Prontos, Execucao, proc_mais_antigo);
    
    return;
}

void Bloq_Para_Prontos(Processo * p1, Processo * p2, int num_proc)
{
    Processo * temp1 = p2;
    Processo * temp2 = p2->prox;
    
    if(temp1->num_processo == num_proc){
        if(p1 == NULL){
            temp1->prox = p1;
            p2 = temp2;
            Bloqueados = p2;
            Prontos = temp1;
            return;
        }
        while(p1->prox != NULL)
            p1=p1->prox;
        
        temp1->prox = p1->prox;
        p1->prox = temp1;
        
        p2 = temp2;
        Bloqueados = p2;
        return;
    }
    
    while(temp2 != NULL){
        if(temp2->num_processo == num_proc){
            temp1->prox = temp2->prox;
            temp2->prox = p1;
            p1 = temp2;
            Prontos = p1;
            return;
        }
        else{
            temp1->prox = temp2;
            temp2 = temp2->prox;
        }
    }
    return;
}

void Prontos_Para_Bloq(Processo * p1, Processo * p2, int num_proc)
{
    Processo * temp1 = p2;
    Processo * temp2 = p2->prox;
    Processo * p_start_lista = p1;
    
    if(temp1->num_processo == num_proc){
        if(p1 == NULL){
            temp1->prox = p1;
            p2 = temp2;
            Prontos = p2;
            Bloqueados = temp1;
            return;
        }
        while(p1->prox != NULL)
            p1=p1->prox;
        
        temp1->prox = p1->prox;
        p1->prox = temp1;
        p1 = p_start_lista;
        
        p2 = temp2;
        Execucao = p2;
        return;
    }
    
    while(temp2 != NULL){
        if(temp2->num_processo == num_proc){
            temp1->prox = temp2->prox;
            temp2->prox = p1;
            p1 = temp2;
            Bloqueados = p1;
            return;
        }
        else{
            temp1->prox = temp2;
            temp2 = temp2->prox;
        }
    }
    return;
}

void Execucao_Para_Prontos(Processo * p1, Processo * p2, int num_proc)
{
    Processo * temp1 = p2;
    Processo * temp2 = p2->prox;
    Processo * p_start_lista = p1;
    
    if(temp1->num_processo == num_proc){
        if(p1 == NULL){
            temp1->prox = p1;
            p2 = temp2;
            Execucao = p2;
            Prontos = temp1;
            return;
        }
        while(p1->prox != NULL)
            p1=p1->prox;
        
        temp1->prox = p1->prox;
        p1->prox = temp1;
        p1 = p_start_lista;
        
        p2 = temp2;
        Execucao = p2;
        return;
    }
    
    while(temp2 != NULL){
        if(temp2->num_processo == num_proc){
            temp1->prox = temp2->prox;
            temp2->prox = p1;
            p1 = temp2;
            Prontos = p1;
            return;
        }
        else{
            temp1->prox = temp2;
            temp2 = temp2->prox;
        }
    }
    return;
}

void Prontos_Para_Execucao(Processo * p1, Processo * p2, int num_proc)
{
    Processo * temp1 = p2;
    Processo * temp2 = p2->prox;
    if(Execucao == NULL){
        if(temp1->num_processo == num_proc){
            if(p1 == NULL){
                temp1->prox = p1;
                p2 = temp2;
                Prontos = p2;
                Execucao = temp1;
                return;
            }
            while(p1->prox != NULL)
                p1=p1->prox;
            
            temp1->prox = p1->prox;
            p1->prox = temp1;
            
            p2 = temp2;
            Prontos = p2;
            return;
        }
        
        while(temp2 != NULL){
            if(temp2->num_processo == num_proc){
                temp1->prox = temp2->prox;
                temp2->prox = p1;
                p1 = temp2;
                Execucao = p1;
                return;
            }
            else{
                temp1->prox = temp2;
                temp2 = temp2->prox;
            }
        }
    }
    
    return;
}

void Remove_Tarefa_Lista(Processo * p)
{
    Tarefa * temp;
    temp = p->t;
    p->t = p->t->prox;
    free(temp);
    
    return;
}

void Verifica_Tarefa_Lista( Processo * p)
{
    Processo * temp1 = NULL;
    Processo * temp2 = p;
    
    if(p == NULL)
        return;
    
    while(temp2 != NULL && temp2->t != NULL){
        temp1 = temp2;
        temp2 = temp2->prox;
    }
    
    if(temp2 == NULL)
        return;
    if(temp1 == NULL)
        Prontos = Prontos->prox;
    else
        temp1->prox = temp2->prox;
    printf("\n\n-----------Processo %d terminou-----------",temp2->num_processo);
    free(temp2);
    qtd_processos--;
    return;
}


int Prepara_WorstFit(Processo * p)
{
    
    int i;
    Processo * temp = p;
    int flag = 0;
    
    BubbleSort_Maior_Menor(vetorPart, 5);
    
    for( i = 0; i < sizeof(vetorPart)/sizeof(int); i++){
        if(flag == 0 && temp->alocado == 0){
            if (part1[0] == 0 && tamPart1 >= temp->mem_requerida && tamPart1 >= vetorPart[i]){
                for( i = 0; i < temp->mem_requerida; i++){
                    part1[i] = temp->num_processo;
                }
                flag = 1;
                temp->alocado = 1;
                
            }else if(part2[0] == 0 && tamPart2 >= temp->mem_requerida && tamPart2 >= vetorPart[i]){
                
                for( i = 0; i < temp->mem_requerida; i++){
                    part2[i] = temp->num_processo;
                }
                flag = 1;
                temp->alocado = 1;
                
            }else if(part3[0] == 0 && tamPart3 >= temp->mem_requerida && tamPart3 >= vetorPart[i]){
                
                for( i = 0; i < temp->mem_requerida; i++){
                    part3[i] = temp->num_processo;
                }
                flag = 1;
                temp->alocado = 1;
                
            }else if(part4[0] == 0 && tamPart4 >= temp->mem_requerida && tamPart4 >= vetorPart[i]){
                for( i = 0; i < temp->mem_requerida; i++){
                    part4[i] = temp->num_processo;
                }
                flag = 1;
                temp->alocado = 1;
                
            }else if(part5[0] == 0 && tamPart5 >= temp->mem_requerida && tamPart5 >= vetorPart[i]){
                for( i = 0; i < temp->mem_requerida; i++){
                    part5[i] = temp->num_processo;
                }
                flag = 1;
                temp->alocado = 1;
            }
        }
    }
    
    Prontos_Para_Execucao( Execucao, Prontos,temp->num_processo);
    return 0;
    
}

int Prepara_BestFit(Processo * p){
    
    int i;
    Processo * temp = p;
    int flag = 0;
    
    BubbleSort_Menor_Maior(5,vetorPart);
    printf("%d",vetorPart[0]);
    for( i = 0; i < sizeof(vetorPart)/sizeof(int); i++){
        if(flag == 0 && temp->alocado == 0){
            if (part1[0] == 0 && tamPart1 >= temp->mem_requerida && tamPart1 <= vetorPart[i]){
                temp->alocado = 1;
                for( i = 0; i < temp->mem_requerida; i++){
                    part1[i] = temp->num_processo;
                }
                flag = 1;
                
            }else if(part2[0] == 0 && tamPart2 >= temp->mem_requerida && tamPart2 <= vetorPart[i]){
                temp->alocado = 1;
                for( i = 0; i < temp->mem_requerida; i++){
                    part2[i] = temp->num_processo;
                }
                flag = 1;
                
            }else if(part3[0] == 0 && tamPart3 >= temp->mem_requerida && tamPart3 <= vetorPart[i]){
                temp->alocado = 1;
                for( i = 0; i < temp->mem_requerida; i++){
                    part3[i] = temp->num_processo;
                }
                flag = 1;
                
            }else if(part4[0] == 0 && tamPart4 >= temp->mem_requerida && tamPart4 <= vetorPart[i]){
                temp->alocado = 1;
                for( i = 0; i < temp->mem_requerida; i++){
                    part4[i] = temp->num_processo;
                }
                flag = 1;
                
            }else if(part5[0] == 0 && tamPart5 >= temp->mem_requerida && tamPart5 <= vetorPart[i]){
                temp->alocado = 1;
                for( i = 0; i < temp->mem_requerida; i++){
                    part5[i] = temp->num_processo;
                }
                flag = 1;
            }
        }
        
    }
    Prontos_Para_Execucao( Execucao, Prontos,temp->num_processo);
    return 0;
    
}


void BubbleSort_Menor_Maior (int n, int* v){
    int fim,i;
    for (fim=n-1; fim>0; fim--)
        for (i=0; i<fim; i++)
            if (v[i]>v[i+1]) {
                int temp = v[i]; /* troca */
                v[i] = v[i+1];
                v[i+1] = temp;
            }
}
void BubbleSort_Maior_Menor(int vetor[], int tamanho){
    int aux, i, j;
    
    for(j = tamanho - 1; j <= 1; j--)
    {
        for(i = 0; i > j; i++)
        {
            if(vetor[i] > vetor[i+1])
            {
                aux=vetor[i];
                vetor[i]=vetor[i+1];
                vetor[i+1]=aux;
            }
        }
    }
}