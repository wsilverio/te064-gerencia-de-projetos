### Trabalho Final da disciplina de GP

##### Objetivo
Criar um programa em linguagem de programação C ou C++ ANSI, capaz de ler um arquivo no formato de texto (.txt), o qual contenha as atividades, durações, sequenciamento de um projeto e o registro da realização das atividades (dia-a-dia). A partir dos dados de entrada, o programa deverá exibir estatísticas que serão definidas no documento em tela.

##### Arquivo de Entrada
O arquivo de entrada (.txt) será composto por três seções, cada qual iniciada pelo caractere '#'. A primeira seção deverá conter o nome e a duração de cada atividade, conforme o modelo apresentado abaixo:

```
#
{{inicio,-1},{A1,3},{A2,10},{fim,-1}}
```

Convém destacar algumas observações importantes a respeito da primeira seção:
* início de cada seção com o caractere # deverá estar na linha logo acima à descrição das atividades;
* não deverão existir caracteres em branco;
* não deverão existir linhas em branco;
* obrigatoriamente a primeira atividade listada deverá ser o inicio e a última o fim, ambos com duração igual a -1, notação que corresponde a atividade não ter duração;
* obrigatoriamente a primeira informação contida entre as chaves deverá ser o nome da atividade e a segunda a sua duração, sempre separados por vírgula;
* as atividades poderão apresentar qualquer nomeação, desde que a mesma não se repita;
* o nome das atividades não poderá apresentar acentuação e deverá conter pelo menos um caractere;
* cada atividade deverá ter uma duração expressa por meio de um número racional e maior do que zero, com exceção do início e do fim em que a duração será igual a -1.

A segunda seção do arquivo .txt, também será iniciada pelo caractere '#' e conterá as informações das ligações entre cada atividade, delimitadas por '{' e '}', sendo que cada atividade será separada por vírgula. As ligações deverão ser descritas em pares, conforme exemplo que segue abaixo:

```
#
{inicio,A1}
{A1,A2}
{A2,fim}
```

Deverão ser respeitadas as seguintes observações na segunda seção:
* os nomes das atividades deverão ser exatamente iguais aos listados na primeira seção;
* cada par de atividades deverá ser colocado entre chaves;
* não há ordem na especificação das ligações.

A terceira seção do arquivo .txt, também será iniciada pelo caractere '#' e conterá as atividades finalizadas e iniciadas a cada dia de execução do projeto, sendo definida no formato:

```
dia: {i:<atividades iniciadas no dia>;f:<atividades concluídas separadas por vírgula>}
```

As atividades, iniciadas ou finalizadas, serão separadas por vírgulas. Caso não existam atividades iniciadas ou finalizadas a sessão respectiva será suprimida da linha.

Exemplos:
* atividade1 iniciada no dia 1 -> `1: {i:atividade1}`
* atividade2 iniciada no dia 2 -> `2: {i:atividade2}`
* atividade3 iniciada no dia 3, atividades 1 e 2 finalizadas na mesma data -> `3: {i:atividade3;f:atividade1, atividade2}`

##### Mecanismos de funcionamento
O programa deve ler o arquivo texto e, a cada ENTER, ler as ocorrências de evento de um dia e calcular:

* Early Start (ES):
Data mais cedo que uma atividade pode iniciar quando suas antecessoras são executadas no tempo estimado.

* Early Finish (EF):
Data mais cedo que uma atividade pode terminar começando na sua ES e sendo executada no tempo estimado.

* Late Start (LS):
Data mais tarde que uma atividade pode iniciar sem prejudicar o cronograma.

* Late Finish (LF):
Data mais tarde que uma atividade pode terminar sem prejudicar o cronograma.

* Slack:
Representa a quantidade de tempo que uma atividade pode atrasar sem prejudicar o projeto
Slack = LS – ES= LF - EF

Obs: Considerar, para fins de desenvolvimento, as atividades não conclusas como se fossem um novo projeto.

##### Documentos a serem entregues

* Termo de abertura de projeto
* [Documento de requisitos](http://www.cin.ufpe.br/~mexplorer/metodologia/requisitos/documentoRequisitos.doc)
* [Análise de riscos e oportunidades com o SWOT](http://excelenciaemprojetos.com/?p=214)
* Cronograma
* Termo de encerramento de projeto
* Código fonte

##### Pré-requisito
O programa deve ser desenvolvido em C++ ANSI como condição para sua execução em dispositivos embarcados que rodem Linux.

##### Bonificação
A equipe que entregar o sistema em execução em ambiente embarcado a escolha, como o RASPBERRY PI 3, receberá bonificação de 20% na nota obtida.

##### Data de entrega
25 de junho

##### Data de defesa
26 de junho (horário a combinar) –15h

##### Equipe
Máximo 2 pessoas
