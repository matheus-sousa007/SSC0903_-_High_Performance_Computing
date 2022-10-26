# Descrição do Modelo PCAM para o trabalho 1

## [Link do gráfico do PCAM](https://docs.google.com/drawings/d/1CTyMyujnLBi05YhzYRaRM5ZwzRn30DW8Jpl-RURD1IM/edit?usp=sharing)

## Descrição do modelo PCAM:

O modelo PCAM apresenta a possível paralelização do código sequencial feito pelo grupo. 

### Particionamento

Primeiramente, no código paralelo, é feita a leitura, de forma sequencial, dos inputs referentes as quantidades de regiões, cidades e alunos, respectivamente. Juntamente, temos a leitura da semente do gerador de números pseudo-aleatório. Após a leitura, é feita, de forma sequencial, a alocação na memória e a geração dos números pseudo-aleatórios.

Com a criação dos dados podemos extrair as informações que necessitamos para resolução do problema. Para particionar o máximo possível, será atribuida uma tarefa para cada nota gerada, conténdo assim uma granulação fina. 

Para podermos adquirir os valores máximo, mínimo e a médiana, podemos ordenar o cada cidade utilizando um counting sort. Cada cidade poderá ser ordenada em paralelo já que não há dependência entre elas. 

Primeiramente teremos cada tarefa como sendo cada nota de uma respectiva cidade. Para cada tarefa, faremos comparações com cada valor: caso a nota seja igual à 0, igual à 1, e assim até igual à 100. Para cada comparação é feita uma operação de redução das tarefas, assim encontrando as frequencias de cada nota no final da operação.

Depois, ainda executando em paralelo cada cidade porém sequencialmente dentro de cada uma, podemos repreencher o vetor com os dados ordenados, assim, finalizando o counting sort.

Após isso, pegar os valores mínimo, máximo e a mediana são constantes em tempo. 

Para calcular a média podemos fazer uma operação de redução de soma para cada tarefa em cada cidade e obter a soma total das notas. No final, basta apenas dividir pelo valor de alunos fornecido que teremos cada média de cada cidade. 

Para calcular o desvio padrão em cada cidade, primeiro dependemos da média. Enquanto a média é calculada, podemos fazer em paralelo, outra operação de redução de soma dos quadrados de cada nota de cada cidade. Assim, calculada a média, podemos então dividir o resultado pelo número de alunos e subtrair pela média da cidade.

Para calcular os valores de máximo, mínimo, mediana, média e desvio padrão das regiões e posteriormente do país, basta-se fazer operações de redução entre os dados da cidade e consequentemente, os dados das regiões.

Para calcular a média de áreas superiores, podemos armazenar as médias de áreas menores e dividir pela quantidade de unidades daquela área. 

Para calcular o desvio padrão, podemos aproveitar e realizar operação de redução de soma em cada soma dos quadrados de cada área menor. Depois, divide-se pelo número de áreas menores e subtrai pela média dá área.

Quanto a cidade e a região com melhores médias, podem ser descorbetas por meio de duas reduções diferentes, uma no vetor de médias das regiões e outra no vetor de médias das cidades.

### Comunicação

Primeiramente, na contagem das frequências das notas para uma cidade, os valores de k (0 a 100) a serem comparados com cada nota devem ser enviados, via broadcast, à cada tarefa responsável por um determinado aluno. Ademais, para cada valor de k, será feita uma redução de soma para obter a frequência do valor k e atribui-lo ao vetor de frequências. O próximo ponto em que é necessário realizar uma comunicação entres as tarefas seria a soma das notas e das potências ao quadrado delas, a fim de obter a média e o desvio padrão.

Além disso, para obter a frequência de notas de uma região, os vetores de frequência de cada cidade devem ser unificados, logo, precisa haver uma sincronização. E para cada região também haverá uma redução de soma para descobrir a média e o desvio padrão das notas. Por fim, para o Brasil, ocorreria também uma unificação dos vetores de frequência das regiões e reduções de soma para a média  e o desvio padrão.

Vale ressaltar que, para que seja obtido a cidade e a região com as maiores médias, haverá uma redução para obter o máximo entre as médias das cidades e das regiões.

### Aglomeração

### Mapeamento
