#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>


void countFrequencies(int arr[], int A, int freq[])
{
    for (int i = 0; i < A; i++){
        freq[arr[i]]++;
    }
}


int main(){
    
    int R, C, A, seed, i, j, k;
    int maiorB, menorB, premR=0, premC[2] = {0};
    double mediaB, medianaB, dpB, wtime;

    scanf("%d %d %d %d", &R, &C, &A, &seed);
    
    //alocar memoria
    int*** notas = (int***)malloc(R*sizeof(int**));
    int** maiorC = (int **)malloc(R*sizeof(int*));
    int** menorC = (int **)malloc(R*sizeof(int*));
    double** medianaC = (double **)malloc(R*sizeof(double*));
    double** mediaC = (double **)malloc(R*sizeof(double*));
    double** dpC = (double **)malloc(R*sizeof(double*));

    for(i=0; i<R; i++){
        notas[i] = (int **) malloc(C * sizeof(int *));
        maiorC[i] = (int*)malloc(C*sizeof(int));
        menorC[i] = (int*)malloc(C*sizeof(int));
        mediaC[i] = (double*)malloc(C*sizeof(double));
        medianaC[i] = (double*)malloc(C*sizeof(double));
        dpC[i] = (double*)malloc(C*sizeof(double));

        for(j=0; j<C; j++){
            notas[i][j] = (int*) malloc(A*sizeof(int));
        }
    }

    int* maiorR = (int *)malloc(R*sizeof(int));
    int* menorR = (int *)malloc(R*sizeof(int));
    double* medianaR = (double *)malloc(R*sizeof(double));
    double* mediaR = (double *)malloc(R*sizeof(double));
    double* dpR = (double *)malloc(R*sizeof(double));

    //gerar valores
    srand(seed);
    for(i=0; i<R; i++){

        for(j=0; j<C; j++){

            for(k=0; k<A; k++){

                notas[i][j][k] = rand() % 101;

            }
        }

    }

    /* //imprimir matrizes
    for(i=0; i<R; i++){
        for(j=0; j<C; j++){
            for(k=0; k<A; k++){
                printf("%d ", notas[i][j][k]);
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
            countFrequencies(notas[i][j], A, frequencies);

            //pegar a menor nota da cidade
            for (int k = 0; k < 101; k++){
                if (frequencies[k] > 0){
                    menorC[i][j] = k;
                    break;
                }
            }

            //pegar a maior nota da cidade
            for (int k = 100; k >= 0; k--){
                if (frequencies[k] > 0){
                    maiorC[i][j] = k;
                    break;
                }
            }

            //pegar a mediana da cidade
            int count = 0, leftPos = A/2, rightPos = A/2 + 1;
            if (A%2 != 0)
                leftPos = rightPos;
            for (int k = 0; k < 101; k++){
                count += frequencies[k];
                if (count >= leftPos){
                    medianaC[i][j] = k;
                    leftPos = 101;
                }
                if (count >= rightPos){
                    medianaC[i][j] += k;
                    medianaC[i][j] /= 2;
                    break;
                }
            }

            //somar notas a variavel mediaR tambem e pegar a media da cidade j
            mediaC[i][j]=0;
            for(k=0; k<A; k++){
                mediaC[i][j] += notas[i][j][k];
            }
            mediaR[i] += mediaC[i][j];
            mediaC[i][j] /= A;
            if(mediaC[i][j] > mediaC[premC[0]][premC[1]]){
                premC[0] = i;
                premC[1] = j;
            }

            //calcular desvio padrao da cidade j 
            for(k=0; k<A; k++){
                dpC[i][j] += pow(notas[i][j][k] - mediaC[i][j],2);
            }
            dpC[i][j] = sqrt(dpC[i][j]/A); 
            
            for (int i = 0; i < 101; i++){
                frequenciesR[i] += frequencies[i];
            }
            free(frequencies);
        }

        //somar notas a variavel mediaB tambem e pegar a media da regiao i
        mediaB += mediaR[i];
        mediaR[i] /= (C*A);
        if(mediaR[i] > mediaR[premR]){
            premR = i;
        }

        //pegar a menor nota da regiao
        for (int k = 0; k < 101; k++){
            if (frequenciesR[k] > 0){
                menorR[i] = k;
                break;
            }
        }

        //pegar a maior nota da regiao
        for (int k = 100; k >= 0; k--){
            if (frequenciesR[k] > 0){
                maiorR[i] = k;
                break;
            }
        }

        //pegar a mediana da regiao
        int count = 0, leftPos = (C*A)/2, rightPos = (C*A)/2 + 1;
        if ((C*A)%2 != 0)
            leftPos = rightPos;
        for (int k = 0; k < 101; k++){
            count += frequenciesR[k];
            if (count >= leftPos){
                medianaR[i] = k;
                leftPos = 101;
            }
            if (count >= rightPos){
                medianaR[i] += k;
                medianaR[i] /= 2;
                break;
            }
        }

        //descobrir a maior e a menor nota da regiao i // calcular dp da regiao i
        dpR[i] = 0;
        for(j=0; j<C; j++){
            for(k=0; k<A; k++){

                dpR[i] += pow(notas[i][j][k] - mediaR[i],2);

            }
        }

        dpR[i] = sqrt(dpR[i]/(A*C));

        for (int i = 0; i < 101; i++){
            frequenciesB[i] += frequenciesR[i];
        }
        free(frequenciesR);
    }

    //pegar media do brasil
    mediaB /= (R*C*A);

    //pegar a menor nota do brasil
    for (int k = 0; k < 101; k++){
        if (frequenciesB[k] > 0){
            menorB = k;
            break;
        }
    }

    //pegar a maior nota do brasil
    for (int k = 100; k >= 0; k--){
        if (frequenciesB[k] > 0){
            maiorB = k;
            break;
        }
    }

    //pegar a mediana do brasil
    int count = 0, leftPos = (R*C*A)/2, rightPos = (R*C*A)/2 + 1;
    if ((R*C*A)%2 != 0)
        leftPos = rightPos;
    for (int k = 0; k < 101; k++){
        count += frequenciesB[k];
        if (count >= leftPos){
            medianaB = k;
            leftPos = 101;
        }
        if (count >= rightPos){
            medianaB += k;
            medianaB /= 2;
            break;
        }
    }
    free(frequenciesB);

    //pegar a maior e a menor nota do brasil // calcular dp do brasil
    dpB=0;
    for(i=0; i<R; i++){

        for(j=0; j<C; j++){

            for(k=0; k<A; k++){

                dpB += pow(notas[i][j][k] - mediaB,2);

            }
        }
    }

    dpB = sqrt(dpB/(A*R*C)); 

    wtime = omp_get_wtime() - wtime;

    //imprimir resultados
    for(i=0; i<R; i++){

        for(j=0; j<C; j++){

            printf("Reg %d - Cid %d: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", i, j, menorC[i][j], maiorC[i][j], medianaC[i][j], mediaC[i][j], dpC[i][j]);

        }

        printf("\n");

    }

    for(i=0; i<R; i++){
        printf("Reg %d: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", i, menorR[i], maiorR[i], medianaR[i], mediaR[i], dpR[i]);
    }
    printf("\n");

    printf("Brasil: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", menorB, maiorB, medianaB, mediaB, dpB);

    printf("Melhor região: Região %d\n", premR);
    printf("Melhor cidade: Região %d, Cidade %d\n", premC[0], premC[1]);

    printf("Tempo de resposta sem considerar E/S, em segundos: %lfs\n", wtime);
 
    //desalocar memoria
    for(i=0; i<R; i++){
        for(j=0; j<C; j++){
            free(notas[i][j]);
        }
        free(notas[i]);
    }
    free(notas);
    for(i=0; i<R; i++){
        free(maiorC[i]);
        free(menorC[i]);
        free(mediaC[i]);
        free(medianaC[i]);
        free(dpC[i]);
    }

    free(maiorC);
    free(menorC);
    free(maiorR);
    free(menorR);
    free(mediaC);
    free(mediaR);
    free(medianaR);
    free(medianaC);
    free(dpC);
    free(dpR);

    return 0;
}
