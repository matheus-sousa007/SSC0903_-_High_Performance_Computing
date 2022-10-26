#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define MIN_GRADE 0
#define MAX_GRADE 100

#define NUM_THREADS 8

typedef short int sh;

sh ***allocate_data(int R, int C, int A, int seed);
void deallocate_data(sh ***data, int R, int C, int A);
void print_freq(int *freq);
int get_minimum(int * freq);
int get_maximum(int * freq);
int get_median(int * freq, size_t n);
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

    int * freq_cidade = (int *)malloc((MAX_GRADE + 1) * sizeof(int));
    int * freq_regiao = (int *)malloc((MAX_GRADE + 1) * sizeof(int));
    int * freq_br = (int *)malloc((MAX_GRADE + 1) * sizeof(int));

    double start = omp_get_wtime();

    double media = 0.0F;

    // omp_set_nested(1); 

    double media_cidades[R * C];
    double media_regioes[R];
    double media_br = 0.0F;
    
    double soma_quad_br = 0.0F;


    sh min = -1, max = -1, median = -1;


    #pragma omp parallel for num_threads(NUM_THREADS) reduction(+: freq_br[:(MAX_GRADE + 1)], media_br)
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
            media_cidades[(R * i) + j] = media_cidade;
            media_regiao += media_cidade;
        
            soma_quad_regiao += soma_quad_cidade;

            min = get_minimum(freq_cidade);
            max = get_maximum(freq_cidade);
            median = get_median(freq_cidade, A);

            #pragma omp critical (data)
            {
                printf("Reg %hd - Cid %hd: ", i, j);
                print_data(min, max, median, media_cidade, sqrt(soma_quad_cidade/A - (media_cidade * media_cidade)));
            }
            // print_freq(freq);
            merge_freq(freq_regiao, freq_cidade);
        }

        media_regiao /= C;
        media_regioes[i] = media_regiao;
        media_br += media_regiao;
        
        soma_quad_br += soma_quad_regiao;
        
        min = get_minimum(freq_regiao);
        max = get_maximum(freq_regiao);
        median = get_median(freq_regiao, C);

        #pragma omp critical (data)
        {
            printf("Reg %hd: ", i);
            print_data(min, max, median, media_regiao, sqrt(soma_quad_regiao/(C * A) - (media_regiao * media_regiao)));
        }
        merge_freq(freq_br, freq_regiao);

        // print_freq(freq);
    }

    media_br /= R;


    min = get_minimum(freq_br);
    max = get_maximum(freq_br);
    median = get_median(freq_br, R);

    #pragma omp critical (data)
    {
        printf("Brasil: ");
        print_data(min, max, median, media_br, sqrt(soma_quad_br/(R * C * A) - (media_br * media_br)));
    }

    double end = omp_get_wtime();

    double time = end - start;

    // print_freq(freq);

    printf("\n");

    printf("Tempo executado em paralelo: %lf", time);

    free(freq_cidade);
    free(freq_regiao);
    free(freq_br);

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

int get_median(int * freq, size_t n){
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

    return (left_val + right_val) / 2; 
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
