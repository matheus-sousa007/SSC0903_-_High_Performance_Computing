#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define MIN_GRADE 0
#define MAX_GRADE 100

//#define NUM_THREADS 8

typedef short int sh;

sh ***allocate_data(int R, int C, int A, int seed);
void deallocate_data(sh ***data, int R, int C, int A);
void allocate_city_data(int ***maxC, int ***minC, double ***medianC, double ***mediaC, double ***dpC, int R, int C);
void deallocate_city_data(int **maxC, int **minC, double **medianC, double **mediaC, double **dpC, int R);
void allocate_region_data(int **maxR, int **minR, double **medianR, double **mediaR, double **dpR, int R);
void deallocate_region_data(int *maxR, int *minR, double *medianR, double *mediaR, double *dpR);
void print_freq(int *freq);
int get_minimum(int * freq);
int get_maximum(int * freq);
double get_median(int * freq, size_t n);
void print_data(sh min, sh max, sh median, double mean, double std_deviation);
void reset_freq(int *freq);
void merge_freq(int *freq1, int *freq2);

int main(){

    int R = 0, C = 0, A = 0, seed = 0;

    scanf("%d", &R);
    scanf("%d", &C);
    scanf("%d", &A);
    scanf("%d", &seed);

    sh *** data = allocate_data(R, C, A, seed);

    /* int * freq_cidade = (int *)malloc((MAX_GRADE + 1) * sizeof(int));
    int * freq_regiao = (int *)malloc((MAX_GRADE + 1) * sizeof(int));
    int * freq_br = (int *)malloc((MAX_GRADE + 1) * sizeof(int));
    reset_freq(freq_br); */
    int freq_cidade[MAX_GRADE+1];
    int freq_regiao[MAX_GRADE+1];
    int freq_br[MAX_GRADE+1];
    reset_freq(freq_br);

    // dados das cidades a serem salvos
    int** maxC;
    int** minC;
    double** medianC;
    double** mediaC;
    double** dpC;
    allocate_city_data(&maxC, &minC, &medianC, &mediaC, &dpC, R, C);

    // dados das regioes a serem salvos
    int* maxR;
    int* minR;
    double* medianR;
    double* mediaR;
    double* dpR;
    allocate_region_data(&maxR, &minR, &medianR, &mediaR, &dpR, R);

    // cidade e regiao premiada
    int premR=0, premC[2] = {0};
    mediaC[premC[0]][premC[1]] = 0;
    mediaR[premR] = 0;

    double start = omp_get_wtime();

    // omp_set_nested(1); 

    double media_br = 0.0F;
    
    double soma_quad_br = 0.0F;

//    #pragma omp parallel for num_threads(NUM_THREADS) private(freq_cidade, freq_regiao) reduction(+: freq_br[:(MAX_GRADE + 1)], media_br, soma_quad_br)
    #pragma omp parallel for private(freq_cidade, freq_regiao) shared(premR, premC) reduction(+: freq_br[:(MAX_GRADE + 1)], media_br, soma_quad_br)
    for(int i = 0; i < R; ++i){
        double media_regiao = 0;
        double soma_quad_regiao = 0.0f;
        reset_freq(freq_regiao);
        for(int j = 0; j < C; ++j){
            double media_cidade = 0;
            double soma_quad_cidade = 0.0f;
            reset_freq(freq_cidade);
            for(int k = 0; k < A; ++k){
                ++freq_cidade[data[i][j][k]];
                media_cidade += data[i][j][k];
                soma_quad_cidade += data[i][j][k] * data[i][j][k];
            } 
            
            media_cidade /= A;
            mediaC[i][j] = media_cidade;
            media_regiao += media_cidade;
            #pragma omp critical(prem_c)
            {
                if(mediaC[i][j] > mediaC[premC[0]][premC[1]]){
                    premC[0] = i;
                    premC[1] = j;
                }
            }

            dpC[i][j] = sqrt(soma_quad_cidade/A - (media_cidade * media_cidade));
            soma_quad_regiao += soma_quad_cidade;

            minC[i][j] = get_minimum(freq_cidade);
            maxC[i][j] = get_maximum(freq_cidade);
            medianC[i][j] = get_median(freq_cidade, A);

            /* #pragma omp critical (data)
            {
                printf("Reg %hd - Cid %hd: ", i, j);
                print_data(min, max, median, media_cidade, sqrt(soma_quad_cidade/A - (media_cidade * media_cidade)));
            } */
            // print_freq(freq);

            merge_freq(freq_regiao, freq_cidade);
        }

        media_regiao /= C;
        mediaR[i] = media_regiao;
        media_br += media_regiao;
        #pragma omp critical(prem_r)
        {
            if(mediaR[i] > mediaR[premR]){
                premR = i;
            }
        }

        dpR[i] = sqrt(soma_quad_regiao/(C * A) - (media_regiao * media_regiao));
        soma_quad_br += soma_quad_regiao;
        
        minR[i] = get_minimum(freq_regiao);
        maxR[i] = get_maximum(freq_regiao);
        medianR[i] = get_median(freq_regiao, C * A);

        /* #pragma omp critical (data)
        {
            printf("Reg %hd: ", i);
            print_data(min, max, median, media_regiao, sqrt(soma_quad_regiao/(C * A) - (media_regiao * media_regiao)));
        } */
        // print_freq(freq);

        merge_freq(freq_br, freq_regiao);
    }

    media_br /= R;
    double dp_br = sqrt(soma_quad_br/(R * C * A) - (media_br * media_br));

    sh min_br = get_minimum(freq_br);
    sh max_br = get_maximum(freq_br);
    double median_br = get_median(freq_br, R * C * A);

    double end = omp_get_wtime();

    double time = end - start;

    //imprimir resultados
    for(int i=0; i<R; i++){
        for(int j=0; j<C; j++){
            printf("Reg %d - Cid %d: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", i, j, minC[i][j], maxC[i][j], medianC[i][j], mediaC[i][j], dpC[i][j]);
        }
        printf("\n");
    }

    for(int i=0; i<R; i++){
        printf("Reg %d: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", i, minR[i], maxR[i], medianR[i], mediaR[i], dpR[i]);
    }
    printf("\n");

    printf("Brasil: menor: %d, maior: %d, mediana: %0.2lf, média: %0.2lf e DP: %0.2lf\n", min_br, max_br, median_br, media_br, dp_br);

    printf("Melhor região: Região %d\n", premR);
    printf("Melhor cidade: Região %d, Cidade %d\n", premC[0], premC[1]);

    printf("\n");

    printf("Tempo executado em paralelo: %lfs\n", time);

    //desalocar memoria
    /* free(freq_cidade);
    free(freq_regiao);
    free(freq_br); */
    deallocate_data(data, R, C, A);
    deallocate_city_data(maxC, minC, medianC, mediaC, dpC, R);
    deallocate_region_data(maxR, minR, medianR, mediaR, dpR);

    return 0;
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

void print_freq(int *freq){

    for(size_t i = 0; i < (MAX_GRADE + 1); ++i){
        printf("%d ", freq[i]);
    }
    printf("\n");
}


int get_minimum(int * freq){
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

void print_data(sh min, sh max, sh median, double mean, double std_deviation){
    printf("menor: %hd, maior: %hd, mediana: %hd, média: %.2lf e DP: %.2lf\n", min, max, median, mean, std_deviation);
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
