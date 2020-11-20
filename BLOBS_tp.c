#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>

int main();

//typedef's
typedef struct MATCHED {  //pixeis pertencentes ao intervalo de cor num determinado frame
  int id;
  struct MATCHED *nseg;
}Matched;

typedef struct BLOB { //blobs de cor encontrados na pesquisa
  int count, c_l, c_c;
  struct BLOB *nseg;
  char nome[30];
}Blob;

typedef struct COLUNA { //colunas dos frames
  int n_coluna;
  int id;
  struct COLUNA *nseg;
  int canal_1, canal_2, canal_3;
}Coluna;

typedef struct IMAGEM { //pixeis pertencentes ao intervalo de cor organizados por frame
  struct IMAGEM *nseg;
  Matched *match;
  char nome[30];
}Imagem;

typedef struct LINHA {  //linhas dos frames
  int n_linha;
  struct LINHA *nseg;
  Coluna *colunas;;
}Linha;

typedef struct FRAME  {
  int line, col, canal;
  char nome[30];
  struct FRAME *nseg;
  Linha *linhas;
}Frame;

//funcoes auxiliares dos nodos
Frame* insertFrame(Frame *L, Frame *nv) {  //estilo insertFirst para não ter de correr as listas para descobrir o ultimo elemento
                                          //mais rápido do que a altenativa insertLast
	if(L == NULL)
		return nv;

  nv->nseg = L;
  return nv;
}

Imagem* insertImagem(Imagem *L, Imagem *nv) {
	if(L == NULL)
		return nv;

  nv->nseg = L;
  return nv;
}

Matched* insertMatched(Matched *L, Matched *nv) {
	if(L == NULL)
		return nv;

  nv->nseg = L;
  return nv;
}

Matched* insertMatchedLast(Matched *L, Matched *nv) { //tipo insert last para ir adicionando os novos pixeis com vizinhança
  Matched *head = L;

	if(L == NULL)
		return nv;

  while (L->nseg != NULL)
    L = L->nseg;

  L->nseg = nv;

  return head;
}

Coluna* insertColuna(Coluna *L, Coluna *nv) {
	if(L == NULL)
		return nv;

  nv->nseg = L;
  return nv;
}

Blob* insertBlobByCount(Blob *L, Blob *nv) {  //ordena os blobs por quantidade de pixeis
  Blob *prev = L, *head = L;

	if(L == NULL)
		return nv;

  while(L != NULL) {
    if(L->count < nv->count) {
      if(prev == L) {
        nv->nseg = L;
        return nv;
      }
      else {
        nv->nseg = L;
        prev->nseg = nv;
        return head;
      }
    }
    prev = L;
    L = L->nseg;
  }
  prev->nseg = nv;
  nv->nseg = NULL;

  return head;
}

Linha* insertLinha(Linha *L, Linha *nv) {
	if(L == NULL)
		return nv;

  nv->nseg = L;
  return nv;
}

Matched* removeFirstMatch(Matched **M) {  //remove primeiro nodo e atualiza lista atravez dos parametros e retorna
                                          //o primeiro para ser adicionado nouta lista
  Matched *aux = *M;

  if(*M != NULL) {
    *M = (*M)->nseg;
    aux->nseg = NULL;
  }
  return aux;
}

Matched* removeMatch(Matched *M, int id) {  //remove por id e devolve cabeça da lista atualizada
  Matched *head = M, *prev = M;

  while(M != NULL) {
    if(M->id == id) {
      if(M == prev) {
        M = M->nseg;
        prev->nseg = NULL;
        return M;
      }
      prev->nseg = M->nseg;
      M->nseg = NULL;
      return head;
    }
    prev = M;
    M = M->nseg;
  }
  return head;
}


//funções auxiliares de organização de dados
int frameOrganizer(Frame **auxFrame, char fileLine[30], int frameNameLine, int count) { //calcula as linhas e colunas e indica a linha
                                                                                        //onde começa a informação do frame seguinte
  int k = 0;
  Frame *aux = *auxFrame;

  if(count == frameNameLine + 1) {
    fileLine = strtok(fileLine, " "); //divide os números da linha, coluna, canal ("parse")

    while(fileLine != NULL) {
      if(k == 0)
        aux->line = atoi(fileLine); //guarda o número de linhas
      if(k == 1)
        aux->col = atoi(fileLine);  //guarda o número de colunas
      if(k == 2)
        aux->canal = atoi(fileLine);  //guarda o número de canais

      fileLine = strtok(NULL, " "); //avança no "parse"
      k++;
    }

    k = 0;
    frameNameLine = (aux->line * aux->col * aux->canal) + count + 1;  //guarda a linha do próximo frame
  }
  *auxFrame = aux;
  return frameNameLine;
}


int fileReader(Frame **frame) { //lê o ficheiro e ordena a informação RGB numa "matriz de listas"
  char c[30];
  FILE *fp;
  int count = 1, frameNameLine = 1;
  Frame *auxFrame = NULL;
  int k;

  if ((fp = fopen("IMG_4095_frame_100-102.txt", "r")) == NULL) {  //abre ficheiro (adicionar .txt a seguir ao nome)
      printf("\n !!! Erro ( fopen() == NULL ) !!!\n");
      exit(1);
  }
  //buffer da ultima linha

  while(!feof(fp)) {
    if(count == frameNameLine) {  //aloca um novo frame e guarda o nome
      fgets(c, 30, fp);
      *frame = insertFrame(*frame, auxFrame);
      auxFrame = (Frame*)malloc(sizeof(Frame));
      strcpy(auxFrame->nome, c);
      k = frameNameLine + 1;
      count++;
      continue;
    }
    if(count == k) {
      fgets(c, 30, fp);
      frameNameLine = frameOrganizer(&auxFrame, c, frameNameLine, count);
      count++;
      continue;
    }
    if(count > k) { //organiza os valores RGB na "matriz de listas"
      Linha *linhaMatriz = NULL, *auxLinha = NULL;

      for(int j = 0; j < auxFrame->line; j++) {
        auxLinha = (Linha*)malloc(sizeof(Linha));
        auxLinha->n_linha = j + 1;
        Coluna *colunaMatriz = NULL, *auxColuna = NULL;

        for(int l = 0; l < auxFrame->col; l++) {
          auxColuna = (Coluna*)malloc(sizeof(Coluna));
          auxColuna->n_coluna = l + 1;
          auxColuna->id = ((j + 1) * 10000) + (l + 1);  //ex 15600156 significa 1560 linhas por 0156 colunas
          for(int i = 0; i < auxFrame->canal; i++) {
            fgets(c, 30, fp);
            if(i == 0)
              auxColuna->canal_1 = atoi(c); //converte char em int

            if(i == 1)
              auxColuna->canal_2 = atoi(c);

            if(i == 2)
              auxColuna->canal_3 = atoi(c);
          }
          colunaMatriz = insertColuna(colunaMatriz, auxColuna);
        }
        auxLinha->colunas = colunaMatriz;
        linhaMatriz = insertLinha(linhaMatriz, auxLinha);
      }
      auxFrame->linhas = linhaMatriz;
      count = frameNameLine;
    }
  }
  *frame = insertFrame(*frame, auxFrame); //para adicionar o último frame
  fclose(fp);
  return 0;
}

//funções auxiliares de PESQUISA
int colorBlob(Imagem *I, int size) {  //recebe a lista que contem os pixeis pertencentes ao intervalo de cor
                            //e verifica se cumprem a 4-vizinhança e guarda os blobs de cor

  Matched *head_I = NULL, *adjacente = NULL, *aux = NULL;
  int h_min, h_max, w_min, w_max, count = 0, add, count_b = 0;
  Blob *blobs = NULL;
  char nome[30];

  while(I != NULL) {
    strcpy(nome, I->nome);

    if(I->match ==  NULL) { //caso já não haja pixeis para verificar num frame passa-se ao seguinte
      I = I->nseg;
      continue;
    }

    //se I->match != NULL significa que ainda existem pixeis que não foram comparados

    if(adjacente == NULL) { //se adjacente == NULL significa que tem de se começar uma nova pesquisa de blob
                            //retira-se um match de I e insere-se em adjacente para iniciar a pesquisa
      adjacente = insertMatched(adjacente, removeFirstMatch(&I->match));
      h_min = h_max = adjacente->id / 10000; //linha
      w_min = w_max = adjacente->id % 10000; //coluna
      add = 1;
      count++;
    }

    while(adjacente != NULL) {  //como já não é NULL tem de se comparar com os restantes pixeis d«pertencentes
                                //ao intervalo de cor para ver se existem adjacentes
      head_I = I->match;
      while(head_I != NULL) {
        if(head_I->id == adjacente->id + 10000 || head_I->id == adjacente->id - 10000 || head_I->id == adjacente->id + 1 || head_I->id == adjacente->id - 1) {
          //caso existam adjacentes são adicionados no fim da lista (para poderem ser tambem eles copmarados com os restantes I->match)
          //e são atualizados os valores das "alturas e larguras" para se calcular o centro do blob
          if(head_I->id / 10000 < h_min)
            h_min = head_I->id / 10000;

          if(head_I->id / 10000 > h_max)
            h_max = head_I->id / 10000;

          if(head_I->id % 10000 < w_min)
            w_min = head_I->id % 10000;

          if(head_I->id % 10000 > w_max)
            w_max = head_I->id % 10000;

          count++;
          add++;
          aux = head_I;
          head_I = head_I->nseg;

          I->match = removeMatch(I->match, aux->id);
          adjacente = insertMatchedLast(adjacente, aux);
          continue;
        }
        head_I = head_I->nseg;
      }
      adjacente = adjacente->nseg;
    }

    //quando os whiles chegam ao fim é porque já todos os elementos da lista "adjacente"
    //foram comparados com os restantes pixeis e já se verificaram a 4-vizinhança

    Blob *toInsert = (Blob*)malloc(sizeof(Blob));
    toInsert->count = count;
    toInsert->c_l = (h_min + h_max) / 2;
    toInsert->c_c = (w_min + w_max) / 2;
    strcpy(toInsert->nome, nome);
    count = 0;
    blobs = insertBlobByCount(blobs, toInsert);
    free(adjacente);
    adjacente = NULL;
  }
  free(I);  //liberta a memoria dos pixeis pertencentes ao intervalo de cor

  //imprime os blobs por ordem decrecente de pixeis
  while(blobs != NULL && blobs->count >= size) {
    //if(blobs->count >= 2) { //se tiver apenas 1 pixel é porque não tem vizinhança
      printf("\n\t%s\t(L %d, C %d): %d pixeis\n", blobs->nome, blobs->c_l, blobs->c_c, blobs->count);
      count_b++;
    //}
    blobs = blobs->nseg;
  }

  printf("\n\n\tNúmero de BLOBS com pelo menos '%d' pixeis: %d\n", size, count_b);
  free(blobs);  //liberta a memoria do resultado da pesquisa

  return 0;
}

int colorSearch(Frame *F) { //pede os valores RGB e o intervalo de tolerância e verifica quais os pixeis
                            //pertencentes ao intervalo e guarda-os por frame para serem passados para a
                            //próxima função que calcula os blobs
  Frame *auxFrame = F;
  Linha *auxLinha = NULL;
  Coluna *auxColuna = NULL;
  Imagem *imgs = NULL;
  int r, g, b, d, size;

  while(1) {
    printf("\n Insira os valores RGB:\n R -> ");
    scanf("%d", &r);
    printf(" G -> ");
    scanf("%d", &g);
    printf(" B -> ");
    scanf("%d", &b);
    printf("\n Insira o intervalo de tolerância:\n -> ");
    scanf("%d", &d);
    printf("\n Insira o tamanho minimo (em pixeis) do BLOB: \n -> ");
    scanf("%d", &size);

    if(size < 2) {  //o valor "default" é 2, porque uma blob com apenas um pixel era impossível cumprir
                    //o requesito da 4-vizinhança não sendo por isso válida
        printf("\n Min. size value set to default: '2'\n\n");
        size = 2;
        break;
    }

    else{break;}
  }

  while(auxFrame != NULL) {  //já verifiquei na main que (F != NULL)
                              //quando um pixel pertence ao intervalo é guardado em "imgs"
    auxLinha = auxFrame->linhas;
    Imagem *auxImagem = NULL;
    auxImagem = (Imagem*)malloc(sizeof(Imagem));
    strcpy(auxImagem->nome, auxFrame->nome);
    Matched *colorMatch = NULL;

    while(auxLinha != NULL) {
      auxColuna = auxLinha->colunas;
      while (auxColuna != NULL) {
        if((auxColuna->canal_1 <= r + d && auxColuna->canal_1 >= r - d) && (auxColuna->canal_2 <= g + d && auxColuna->canal_2 >= g - d) && (auxColuna->canal_3 <= b + d && auxColuna->canal_3 >= b - d)) {
          Matched *toInsert = (Matched*)malloc(sizeof(Matched));
          toInsert->id = auxColuna->id;
          colorMatch = insertMatched(colorMatch, toInsert);
        }
        auxColuna = auxColuna->nseg;
      }
      auxLinha = auxLinha->nseg;
    }
    if(colorMatch != NULL) {
      auxImagem->match = colorMatch;
      imgs = insertImagem(imgs, auxImagem);
    }
    auxFrame = auxFrame->nseg;
  }

  if(imgs == NULL) {  //se não houver pixeis pertencentes ao intervalo não é nescessário calcular os blobs
    printf("\n !!! Não existem pixeis pertencentes ao intervalo RGB pretendido !!!\n");
    return 0;
  }

  printf("\n ------------------  Inicio de pesquisa  ------------------\n");
  //chama função de pesquisa de blobs
  colorBlob(imgs, size);
  return 0;
}

void printMenu() {
  printf("\n\n\n ---------  Menu  ---------\n");
  printf("\n   1 -> Pesquisa\n   0 -> Sair\n\n -------------------------- \n\n Selecione Opção -> ");
}

//função main
int main() {
  Frame *frame = NULL;
  int select;
  clock_t start_clock, end_clock; //variáveis tempo

  //carregar ficheiro
  printf("\n -----------------  A carregar ficheiro  ------------------\n");
  start_clock = clock();	//inicio relógio
  fileReader(&frame);
  end_clock = clock();	//fim relógio
  printf("\n ---------  Ficheiro carregado (em aprox. %ld seg.) ---------\n", (end_clock - start_clock) / CLOCKS_PER_SEC);  	//calcula e imprime o tempo gasto pela função
  printMenu();

  while(1) {
    scanf("%d", &select);
    switch (select) {
      case 1:
        //pesquisa
        printf("\n ---------  Pesquisa  ---------\n");
        start_clock = clock();	//inicio relógio
        colorSearch(frame);
        end_clock = clock();	//fim relógio
        printf("\n ---------  Pesquisa concluida (em aprox. %ld seg.) ---------\n", (end_clock - start_clock) / CLOCKS_PER_SEC);
        printMenu();
      break;

      case 0:
        //SAIR
        free(frame);  //liberta a memória alocada das frames, e consequentemente a estutura toda
        exit(1);
      break;

      default:
        printf("\n !!! Seleção inválida !!!\n\n Selecione Opção -> ");
        continue;
      break;
    }
  }
}

// António Abreu  a37150
