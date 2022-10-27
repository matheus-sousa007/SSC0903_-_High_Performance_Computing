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
    int* maxR = (int *)malloc(R*sizeof(int));
    int* minR = (int *)malloc(R*sizeof(int));
    double* medianR = (double *)malloc(R*sizeof(double));
    double* mediaR = (double *)malloc(R*sizeof(double));
    double* dpR = (double *)malloc(R*sizeof(double));
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
    mediaB=0;
    for(i=0; i<R; i++){
        
        int* frequenciesR = (int *)calloc(101, sizeof(int));
        mediaR[i]=0;
        for(j=0; j<C; j++){
            
            //contar frequencia de cada nota
            int* frequencies = (int *)calloc(101, sizeof(int));
            countFrequencies(data[i][j], A, frequencies);

            //pegar a menor nota da cidade
            minC[i][j] = get_minimum(frequencies);

            //pegar a maior nota da cidade
            maxC[i][j] = get_maximum(frequencies);

            //pegar a mediana da cidade
            medianC[i][j] = get_median(frequencies, A);

            //somar data a variavel mediaR tambem e pegar a media da cidade j
            mediaC[i][j]=0;
            for(k=0; k<A; k++){
                mediaC[i][j] += data[i][j][k];
            }

            mediaC[i][j] /= A;
            mediaR[i] += mediaC[i][j];
            if(mediaC[i][j] > mediaC[premC[0]][premC[1]]){
                premC[0] = i;
                premC[1] = j;
            }

            //calcular desvio padrao da cidade j 
            dpC[i][j] = 0;
            for(k=0; k<A; k++){
                dpC[i][j] += pow(data[i][j][k] - mediaC[i][j],2);
            }
            dpC[i][j] = sqrt(dpC[i][j]/A); 
            
            //adicionar frequencias da cidade a regiao
            for (int i = 0; i < 101; i++){
                frequenciesR[i] += frequencies[i];
            }
            free(frequencies);
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
        maxR[i] = get_minimum(frequenciesR);

        //pegar a mediana da regiao
        medianR[i] = get_median(frequenciesR, C*A);

        //calcular dp da regiao i
        dpR[i] = 0;
        for(j=0; j<C; j++){
            for(k=0; k<A; k++){

                dpR[i] += pow(data[i][j][k] - mediaR[i],2);

            }
        }

        dpR[i] = sqrt(dpR[i]/(A*C));

        //adicionar frequencias da regiao ao brasil
        for (int i = 0; i < 101; i++){
            frequenciesB[i] += frequenciesR[i];
        }
        free(frequenciesR);
    }

    //pegar media do brasil
    mediaB /= R;

    //pegar a menor nota do brasil
    minB = get_minimum(frequenciesB);

    //pegar a maior nota do brasil
    maxB = get_maximum(frequenciesB);

    //pegar a mediana do brasil
    medianB = get_median(frequenciesB, R*C*A);

    free(frequenciesB);

    //calcular dp do brasil
    dpB=0;
    for(i=0; i<R; i++){

        for(j=0; j<C; j++){

            for(k=0; k<A; k++){

                dpB += pow(data[i][j][k] - mediaB,2);

            }
        }
    }
    dpB = sqrt(dpB/(A*R*C)); 

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
    deallocate_data(data, R, C, A);
    deallocate_city_data(maxC, minC, medianC, mediaC, dpC, R);
    deallocate_region_data(maxR, minR, medianR, mediaR, dpR);

    return 0;
}
