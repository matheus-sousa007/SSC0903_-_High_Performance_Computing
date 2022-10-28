#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

typedef short int sh;

#define MAX_GRADE 100
#define MIN_GRADE 0

void countFrequencies(sh arr[], int A, int freq[])
{
    for (int i = 0; i < A; i++){
        freq[arr[i]]++;
    }
}

sh ***allocate_data(int R, int C, int A, int seed){
    srand(seed);

    sh *** data = (sh ***)malloc(R * sizeof(sh **));

    for(int i = 0; i < R; ++i){
        data[i] = (sh **)malloc(C * sizeof(sh *));
        for(int j = 0; j < C; ++j){
            data[i][j] = (sh *)malloc(A * sizeof(sh));
            for(int k = 0; k < A; ++k){
                data[i][j][k] = rand() % (MAX_GRADE + 1);
            }
        }
    }

    return data;
}

void deallocate_data(sh ***data, int R, int C, int A){
    for(int i = 0; i < R; ++i){
        for(int j = 0; j < C; ++j){
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);
}

void allocate_city_data(int ***maxC, int ***minC, double ***medianC, double ***mediaC, double ***dpC, int R, int C){
    *maxC = (int **)malloc(R*sizeof(int*));
    *minC = (int **)malloc(R*sizeof(int*));
    *medianC = (double **)malloc(R*sizeof(double*));
    *mediaC = (double **)malloc(R*sizeof(double*));
    *dpC = (double **)malloc(R*sizeof(double*));

    for(int i=0; i<R; i++){
        (*maxC)[i] = (int*)malloc(C*sizeof(int));
        (*minC)[i] = (int*)malloc(C*sizeof(int));
        (*medianC)[i] = (double*)malloc(C*sizeof(double));
        (*mediaC)[i] = (double*)malloc(C*sizeof(double));
        (*dpC)[i] = (double*)malloc(C*sizeof(double));
    }
}

void deallocate_city_data(int **maxC, int **minC, double **medianC, double **mediaC, double **dpC, int R){
    for(int i=0; i<R; i++){
        free(maxC[i]);
        free(minC[i]);
        free(medianC[i]);
        free(mediaC[i]);
        free(dpC[i]);
    }
    
    free(maxC);
    free(minC);
    free(medianC);
    free(mediaC);
    free(dpC);
}

void allocate_region_data(int **maxR, int **minR, double **medianR, double **mediaR, double **dpR, int R){
    *maxR = (int *)malloc(R*sizeof(int));
    *minR = (int *)malloc(R*sizeof(int));
    *medianR = (double *)malloc(R*sizeof(double));
    *mediaR = (double *)malloc(R*sizeof(double));
    *dpR = (double *)malloc(R*sizeof(double));
}

void deallocate_region_data(int *maxR, int *minR, double *medianR, double *mediaR, double *dpR){
    free(maxR);
    free(minR);
    free(medianR);
    free(mediaR);
    free(dpR);
}


int get_minimum(int *freq){
    size_t i = 0;

    while(i < (MAX_GRADE + 1) && freq[i] == 0) ++i;

    return i;
}

int get_maximum(int * freq){
    size_t i = MAX_GRADE;

    while(i >= 0 && freq[i] == 0) --i;

    return i;
}

double get_median(int * freq, size_t n){
    size_t left_pos = n/2, right_pos = n/2 + 1;
    sh left_val = -1, right_val = -1;


    if(n % 2 != 0) left_pos = right_pos;

    size_t count = 0;

    for(size_t i = 0; i < (MAX_GRADE + 1) && (left_val == -1 || right_val == -1); ++i){
        count += freq[i];

        if(count >= left_pos){
            left_val = i; 
            left_pos = n + 1;
        }
        if(count >= right_pos){
            right_val = i;
            right_pos = n + 1;
        }
    }

    return (left_val + right_val) / 2.0F; 
}

void reset_freq(int *freq){
    for(size_t i = 0; i < (MAX_GRADE + 1); ++i){
        freq[i] = 0;
    }
}

void merge_freq(int *freq1, int *freq2){
    for(size_t i = 0; i < (MAX_GRADE + 1); ++i){
        freq1[i] += freq2[i];
    }
}

int main(){
    
    int R, C, A, seed, i, j, k;
    int maxB, minB, premR=0, premC[2] = {0};
    double mediaB, medianB, dpB, wtime;

    scanf("%d %d %d %d", &R, &C, &A, &seed);
    
    //alocar memoria
    //data
    sh*** data = allocate_data(R, C, A, seed);

    //dados da cidade
    int** maxC;
    int** minC;
    double** medianC;
    double** mediaC;
    double** dpC;
    allocate_city_data(&maxC, &minC, &medianC, &mediaC, &dpC, R, C);

    //dados da regiao
    int* maxR;
    int* minR;
    double* medianR;
    double* mediaR;
    double* dpR;
    allocate_region_data(&maxR, &minR, &medianR, &mediaR, &dpR, R);

    //gerar valores
    srand(seed);
    for(i=0; i<R; i++){

        for(j=0; j<C; j++){

            for(k=0; k<A; k++){

                data[i][j][k] = rand() % 101;

            }
        }

    }

    /* //imprimir matrizes
    for(i=0; i<R; i++){
        for(j=0; j<C; j++){
            for(k=0; k<A; k++){
                printf("%d ", data[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    } */

    wtime = omp_get_wtime();

    int* frequenciesB = (int *)calloc(101, sizeof(int));
    int* frequenciesR = (int *)malloc(101*sizeof(int));
    int* frequenciesC = (int *)malloc(101*sizeof(int));

    mediaB=0;
    double soma_quad_br = 0.0F;
    for(i=0; i<R; i++){
        double soma_quad_regiao = 0.0F;
        
        reset_freq(frequenciesR);
        mediaR[i]=0;
        for(j=0; j<C; j++){
            double soma_quad_cidade = 0.0F;
            
            //contar frequencia de cada nota
            reset_freq(frequenciesC);
            countFrequencies(data[i][j], A, frequenciesC);

            //pegar a menor nota da cidade
            minC[i][j] = get_minimum(frequenciesC);

            //pegar a maior nota da cidade
            maxC[i][j] = get_maximum(frequenciesC);

            //pegar a mediana da cidade
            medianC[i][j] = get_median(frequenciesC, A);

            //somar data a variavel mediaR tambem e pegar a media da cidade j
            mediaC[i][j]=0;
            for(k=0; k<A; k++){
                mediaC[i][j] += data[i][j][k];
                soma_quad_cidade += data[i][j][k] * data[i][j][k];
            }

            mediaC[i][j] /= A;
            mediaR[i] += mediaC[i][j];
            if(mediaC[i][j] > mediaC[premC[0]][premC[1]]){
                premC[0] = i;
                premC[1] = j;
            }

            //calcular desvio padrao da cidade j 
            dpC[i][j] = sqrt(soma_quad_cidade/A - (mediaC[i][j]*mediaC[i][j]));
            soma_quad_regiao += soma_quad_cidade;
            
            //adicionar frequencias da cidade a regiao
            merge_freq(frequenciesR, frequenciesC);
        }

        //somar data a variavel mediaB tambem e pegar a media da regiao i
        mediaR[i] /= C;
        mediaB += mediaR[i];
        if(mediaR[i] > mediaR[premR]){
            premR = i;
        }

        //pegar a menor nota da regiao
        minR[i] = get_minimum(frequenciesR);

        //pegar a maior nota da regiao
        maxR[i] = get_maximum(frequenciesR);

        //pegar a mediana da regiao
        medianR[i] = get_median(frequenciesR, C*A);

        //calcular dp da regiao i
        dpR[i] = sqrt(soma_quad_regiao/(C * A) - (mediaR[i] * mediaR[i]));
        soma_quad_br += soma_quad_regiao;

        //adicionar frequencias da regiao ao brasil
        merge_freq(frequenciesB, frequenciesR);
    }

    //pegar media do brasil
    mediaB /= R;


    //pegar a menor nota do brasil
    minB = get_minimum(frequenciesB);

    //pegar a maior nota do brasil
    maxB = get_maximum(frequenciesB);

    //pegar a mediana do brasil
    medianB = get_median(frequenciesB, R*C*A);

    //calcular dp do brasil
    dpB = sqrt(soma_quad_br/(R * C * A) - (mediaB * mediaB)); 

    wtime = omp_get_wtime() - wtime;

    //imprimir resultados
    for(i=0; i<R; i++){

        for(j=0; j<C; j++){

            printf("Reg %d - Cid %d: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", i, j, minC[i][j], maxC[i][j], medianC[i][j], mediaC[i][j], dpC[i][j]);

        }

        printf("\n");

    }

    for(i=0; i<R; i++){
        printf("Reg %d: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", i, minR[i], maxR[i], medianR[i], mediaR[i], dpR[i]);
    }
    printf("\n");

    printf("Brasil: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", minB, maxB, medianB, mediaB, dpB);

    printf("Melhor região: Região %d\n", premR);
    printf("Melhor cidade: Região %d, Cidade %d\n", premC[0], premC[1]);

    printf("Tempo de resposta sem considerar E/S, em segundos: %lfs\n", wtime);
 
    //desalocar memoria

    deallocate_data(data, R, C, A);
    deallocate_city_data(maxC, minC, medianC, mediaC, dpC, R);
    deallocate_region_data(maxR, minR, medianR, mediaR, dpR);
    free(frequenciesB);
    free(frequenciesR);
    free(frequenciesC);

    return 0;
}
