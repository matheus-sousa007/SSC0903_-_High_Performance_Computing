#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include<time.h>

//funcao merge para juntar particoes do vetor e ordena-los
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];
 
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
 

    i = 0; 
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
 
//funcao mergesort para ordenacao
void mergeSort(int arr[], int l, int r)
{
    if (l < r) {
   
        int m = l + (r - l) / 2;
 
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
 
        merge(arr, l, m, r);
    }
}


int main(){
    
    int R, C, A, seed, i, j, k;
    int maiorB, menorB, premR=0, premC[2];
    double mediaB, dpB, wtime;
    premC[0] = 0;
    premC[1] = 0;

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

    //imprimir matrizes
    for(i=0; i<R; i++){

        for(j=0; j<C; j++){

            for(k=0; k<A; k++){

                printf("%d ", notas[i][j][k]);

            }

             printf("\n");
        }

        printf("\n");

    }

    wtime = omp_get_wtime();

    mediaB=0;
    for(i=0; i<R; i++){

        mediaR[i]=0;
        for(j=0; j<C; j++){
            
            //Ordenar as notas da cidade
            mergeSort(notas[i][j], 0, A-1);
            
            //pegar a maior e menor nota da cidade
            maiorC[i][j] = notas[i][j][A-1];
            menorC[i][j] = notas[i][j][0];

            //pegar a mediana da cidade
            if(A % 2 == 0){
                medianaC[i][j] = (notas[i][j][A/2] + notas[i][j][(A/2) - 1])/2;
            } else {
                medianaC[i][j] = notas[i][j][A/2];
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
            
        }

        //somar notas a variavel mediaB tambem e pegar a media da regiao i
        mediaB += mediaR[i];
        mediaR[i] /= (C*A);
        if(mediaR[i] > mediaR[premR]){
            premR = i;
        }

        //descobrir a maior e a menor nota da regiao i // calcular dp da regiao i
        dpR[i] = 0;
        maiorR[i] = maiorC[i][0];
        menorR[i] = menorC[i][0];
        for(j=0; j<C; j++){
            if(maiorR[i] < maiorC[i][j]){
                maiorR[i] = maiorC[i][j];
            }

            if(menorR[i] > menorC[i][j]){
                menorR[i] = menorC[i][j];
            }

            for(k=0; k<A; k++){

                dpR[i] += pow(notas[i][j][k] - mediaR[i],2);

            }
        }

        dpR[i] = sqrt(dpR[i]/(A*C));

    }

    //pegar media do brasil
    mediaB /= (R*C*A);

    //pegar a maior e a menor nota do brasil // calcular dp do brasil
    dpB=0;
    maiorB = maiorR[0];
    menorB = menorR[0];
    for(i=0; i<R; i++){
        if(maiorB < maiorR[i]){
            maiorB = maiorR[i];
        }

        if(menorB> menorR[i]){
            menorB = menorR[i];
        }

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
        printf("Reg %d: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", i, menorR[i], maiorR[i], 0.0, mediaR[i], dpR[i]);
    }
    printf("\n");

    printf("Brasil: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", menorB, maiorB, 0.0, mediaB, dpB);

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
    free(medianaC);
    free(dpC);
    free(dpR);

    return 0;
}
