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
    Tarefa * t;
    
    struct processo * prox;
} Processo;


//variaveis globais
Processo * Execucao;
Processo * Bloqueados;
Processo * Prontos;
int memoria_global[16] ; //vetor que indica como está memória
int qtd_memoria_livre = 16; //quantidade de memoria livre

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

int qtd_processos = 0;
int TEMPO_TOTAL = 0;
int next= 0; // variavel para a politica next fit;

Processo * Cria_Processo_Lista(Processo * p);
Tarefa * Cria_Tarefa_Lista(Tarefa * Task, char * tag, int time);
void Inicializa_Memoria (void);
int Aloca_Processo_FirstFit(Processo * p);
int Aloca_Processo_NextFit(Processo* p);
int Aloca_Processo_BestFit(Processo * p);
int Aloca_Processo_WorstFit(Processo * p);
void Tempo_Processos(Processo * p);
void Reduz_Tempo_Execucao(Processo * p);
void Reduz_Tempo_Bloqueio(Processo * p);
void Transfere_Bloq2Prontos(Processo * p1, Processo * p2, int num_proc);
void Transfere_Prontos2Bloq(Processo * p1, Processo * p2, int num_proc);
void Transfere_Execucao2Prontos(Processo * p1, Processo * p2, int num_proc);
void Transfere_Prontos2Execucao(Processo * p1, Processo * p2, int num_proc);
void Desaloca_Processo(Processo * p);
void Remove_Tarefa_Lista(Processo * p);
void Verifica_Tarefa_Lista( Processo * p);

int main(void)
{
    int i=0, politic;
    Processo * pAux;
    Prontos = NULL; Execucao = NULL; Bloqueados = NULL;
    Prontos = Cria_Processo_Lista(Prontos);
    pAux = Prontos;
    Inicializa_Memoria();
    printf("\n\n===========================================================================================\n\n");
    printf("Escolha uma das Politicas de Alocacao de Memoria:\n\n\t1-FIRST FIT\n\t2-NEXT FIT\n\t3-BEST FIT\n\t4-WORST FIT\n\n");
   
    printf("\tDigite a Politica Escolhida: ");
    scanf("%d", &politic);
    printf("\n\n================================COMECANDO OS PROCESSOS=====================================");
    
    //escolher politica de alocacao de memoria
    while(qtd_processos != 0){
        if(TEMPO_TOTAL% 10 == 0){
            printf("\n\n");
            if(politic == 1){
                if(strcmp(pAux->t->tag, "exec")==0)
                    while(Aloca_Processo_FirstFit(Prontos)== -1)
                        Desaloca_Processo( Execucao);
                else if(strcmp(pAux->t->tag, "io")==0)
                    Transfere_Prontos2Bloq( Bloqueados, Prontos, pAux->num_processo);
                pAux = Prontos;
            }
            else if(politic == 2){
                if(strcmp(pAux->t->tag, "exec")==0){
                    while(Aloca_Processo_NextFit(Prontos)== -1){
                        if(Aloca_Processo_NextFit(Prontos)!=-1)
                            break;
                        Desaloca_Processo( Execucao);
                    }
                }
                else if(strcmp(pAux->t->tag, "io")==0){
                    Transfere_Prontos2Bloq( Bloqueados, Prontos, pAux->num_processo);
                }
                pAux = Prontos;
            }
            else if(politic == 3){
                if(strcmp(pAux->t->tag, "exec")==0)
                    while(Aloca_Processo_BestFit(Prontos)== -1)
                        Desaloca_Processo( Execucao);
                
                else if(strcmp(pAux->t->tag, "io")==0)
                    Transfere_Prontos2Bloq( Bloqueados, Prontos, pAux->num_processo);
                
                pAux = Prontos;
            }
            
            else if(politic == 4){
                if(strcmp(pAux->t->tag, "exec")==0)
                    while(Aloca_Processo_WorstFit(Prontos)== -1)
                        Desaloca_Processo( Execucao);
                
                else if(strcmp(pAux->t->tag, "io")==0)
                    Transfere_Prontos2Bloq( Bloqueados, Prontos, pAux->num_processo);
                
                pAux = Prontos;
            }
            

            printf("=============================Uso de Processos na Memoria:=================================\n\n\tMemoria: ");
            for( i = 0; i < 8; i++)  printf("[%d]", part1[i]);
            for( i = 0; i < 4; i++)  printf("[%d]", part2[i]);
            for( i = 0; i < 2; i++)  printf("[%d]", part3[i]);
            printf("[%d]", part4[0]);
            printf("[%d]", part5[0]);

            printf("\n\n");
            
            
            printf("\n--Processos em Execucao\n\n");
            Tempo_Processos( Execucao);
            printf("\n--Processos Prontos\n\n");
            Tempo_Processos(Prontos);
            printf("\n--Processos Bloqueados\n\n");
            Tempo_Processos( Bloqueados);
        }
        
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
        
        printf("%d", proc);
        
        novo = (Processo *) malloc (sizeof(Processo));
        novo->tempo_permanencia = 0;
        novo->num_processo = proc;
        novo->mem_requerida = mem;
        novo->entradas = ent;
        novo->prox = p;
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
    for(i=0;i<16;i++){
        memoria_global[i]=0;
    }
    for ( i = 0; i < 8; i++ ){
        if( i < 2 ) part3[i] = 0;
        if( i < 4 ) part2[i] = 0;
        part1[i] = 0;
    }
    part4[0] = 0;
    part5[0] = 0;
    return;
}


//first fit
int Aloca_Processo_FirstFit(Processo * p)
{
    Processo * temp=p;
    int i, posicoes_livres=0;
    
    // posicoes livres consecutivas na memoria
    
//    for(i=0;i<16;i++){
//        if(memoria_global[i] == 0)//conta posições livres consecutivas na memoria
//            posicoes_livres++;
//        else // caso a posição não esteja livre zera o contador e continua buscando por uma região consecutiva de memoria
//            posicoes_livres = 0;
//        
//        if(posicoes_livres == temp->mem_requerida){ // se ha n posicoes livres consecutivas na memoria, suficientes para alocar o programa, as coloca nas posicoes respectivas na memoria
//            while(posicoes_livres){
//                memoria_global[i+1-posicoes_livres] = temp->num_processo;
//                posicoes_livres--;
//            }
//            Transfere_Prontos2Execucao( Execucao, Prontos,temp->num_processo);
//            return 0;
//        }
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
            // se nao houver espaco continuo em memoria para alocar o programa, realocacao dos espacos
            return -1;
        }
        Transfere_Prontos2Execucao( Execucao, Prontos,temp->num_processo);
        return 0;
//    }
    
}
int Aloca_Processo_NextFit(Processo* p){
    
    Processo* temp = p;
    int i, posicoes_livres = 0;
    
    
    for(i=next;i<16;i++){
        if(memoria_global[i] == 0)//quando posicoes livres = mem_requerida
            posicoes_livres++;
        else // se programanao possuir posicoes livre consecutivas = mem_requerida, continua buscando na memoria
            posicoes_livres = 0;
        
        if(posicoes_livres == temp->mem_requerida){// se ha n posicoes livres consecutivas na memoria, suficientes para alocar o programa, as coloca nas posicoes respectivas na memoria
            next = i+1;
            if(next > 15){ // se chegou no fim da memoria precisa voltar a buscar desde e o inicio
                i = 0;
                
                next = 0;
            }
            
            
            
            while(posicoes_livres){
                memoria_global[i+1-posicoes_livres] = temp->num_processo;
                posicoes_livres--;
            }
            Transfere_Prontos2Execucao( Execucao, Prontos,temp->num_processo);
            return 0;
        }
        
        
    }
    
    
    
    // se nao houver espaco continuo em memoria para alocar o programa, realocacao dos espacos
    next = i+1;
    if(next > 15){ // se chegou no fim da memoria precisa voltar a buscar desde e o inicio
        i = 0;
        
        next = 0;
    }
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
        if(temp->tempo_permanencia == 10 || temp->t->time == 0){
            temp->tempo_permanencia = 0;
//            for(i=0; i<16; i++){
//                if(memoria_global[i] == temp->num_processo)
//                    memoria_global[i]=0;
//            }
            if (part1[0] == temp->num_processo){
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
            Transfere_Execucao2Prontos( Prontos, Execucao, temp->num_processo);
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
            Transfere_Bloq2Prontos( Prontos, Bloqueados, temp->num_processo);
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
    
//    for(i=0; i<16; i++){
//        if(memoria_global[i] == proc_mais_antigo)
//            memoria_global[i] = 0;
//    }
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
    Transfere_Execucao2Prontos( Prontos, Execucao, proc_mais_antigo);
    
    return;
}

void Transfere_Bloq2Prontos(Processo * p1, Processo * p2, int num_proc)
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

void Transfere_Prontos2Bloq(Processo * p1, Processo * p2, int num_proc)
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

void Transfere_Execucao2Prontos(Processo * p1, Processo * p2, int num_proc)
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

void Transfere_Prontos2Execucao(Processo * p1, Processo * p2, int num_proc)
{
    Processo * temp1 = p2;
    Processo * temp2 = p2->prox;
    
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


int Aloca_Processo_WorstFit(Processo * p)
{
    
    int i, j;
    int endereco[16];// guarda o endereco de memoria inicial de um bloco de memoria
    int espaco[16];// guarda o espaco total do bloco de memoria associado ao endereco inicial
    int idx_wf; // indice do vetor de espaco aonde esta o best fit
    int worstfit; // variavel auxiliar
    
    for(i=0, j=0; i<16; i++){
        if(memoria_global[i]==0){
            endereco[j]=i;
            espaco[j]=0;
            while(memoria_global[i]==0 && i<16){
                i++;
                espaco[j] = espaco[j] + 1;
            }
            j++;
        }
    }
    
    if(j==0) return -1;
    
    worstfit = espaco[0];
    idx_wf = 0;
    for(i=1; i<j; i++)
    {
        if(espaco[i]>worstfit && espaco[i] >= p->mem_requerida){
            worstfit = espaco[i];
            idx_wf = i;
        }
    }
    if(espaco[idx_wf]<p->mem_requerida)
        return -1;
    
    for(i=0; i<p->mem_requerida; i++){
        memoria_global[endereco[idx_wf]] = p->num_processo;
        endereco[idx_wf] = endereco [idx_wf]+1;
    }
    
    Transfere_Prontos2Execucao(Execucao, Prontos, p->num_processo);
    
    return 0;
}

int Aloca_Processo_BestFit(Processo * p)
{
    
    int i, j;
    int endereco[16];// guarda o endereco de memoria inicial de um bloco de memoria
    int espaco[16];// guarda o espaco total do bloco de memoria associado ao endereco inicial
    int idx_bf; // indice do vetor de espaco aonde esta o best fit
    int bestfit; // variavel auxiliar
    
    for(i=0, j=0; i<16; i++){
        if(memoria_global[i]==0){
            endereco[j]=i;
            espaco[j]=0;
            while(memoria_global[i]==0 && i<16){
                i++;
                espaco[j] = espaco[j] + 1;
            }
            j++;
        }
    }
    
    if(j==0) return -1;
    
    bestfit = espaco[0];
    idx_bf = 0;
    for(i=1; i<j; i++)
    {
        if(espaco[i]<bestfit && espaco[i] >= p->mem_requerida){
            bestfit = espaco[i];
            idx_bf = i;
        }
    }
    if(espaco[idx_bf]<p->mem_requerida)
        return -1;
    
    for(i=0; i<p->mem_requerida; i++){
        memoria_global[endereco[idx_bf]] = p->num_processo;
        endereco[idx_bf] = endereco [idx_bf]+1;
    }
    
    Transfere_Prontos2Execucao(Execucao, Prontos, p->num_processo);
    
    return 0;
}
