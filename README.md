# Sudoku
Este repositório traz um jogo de Sudoku com interface gráfica, feito completamente em C/C++ utilizando a biblioteca Raylib.

![Tela Inicial](https://i.imgur.com/6zTlsFm.png)

![Tela do Jogo](https://i.imgur.com/Z33DgfH.png)

## Funcionalidades

### Menu Inicial com "Novo Jogo" e "Continuar"

![Menu Inicial](https://i.imgur.com/o5MYPBd.png)

### Diferentes Temas

![Temas](https://i.imgur.com/BpwpEM6.png)

### Placar com tempos

![Placar](https://i.imgur.com/BwwcCpx.png)

### Dicas e Anotações

![Dicas](https://i.imgur.com/XAKZ4M1.png)

### Você Venceu!

![Vitoria](https://i.imgur.com/KbQs9AE.png)

## Códigos
O código do programa é dividido em três arquivos principais, além de dois headers:

### Sudoku.cpp
É onde fica o algoritmo de geração da matriz do Sudoku, gerando a resposta e a matriz parcial a ser resolvida. Acompanha o header Sudoku.h, que leva as funções e variáveis necessárias ao Interface.cpp.

### Interface.cpp
Contém todos os códigos referentes à interface gráfica, sendo responsável também por lidar com o estado geral do jogo, recebendo inputs, criando botões, definindo as telas a serem mostradas e chamando funções de outros arquivos.

### Saving.cpp
Este arquivo lida com todo o sistema de save do programa, escrevendo e lendo arquivos tratando de jogos não terminados, tempos obtidos previamente, cores-tema escolhidas, e resolução adotada. É complementado pelo header Saving.h, que leva suas funções e variáveis ao Interface.cpp.

## Compilação
Para que o programa seja adequadamente compilado, é necessária a inclusão da bilblioteca gráfica [Raylib](https://www.raylib.com/). Realizando o download da biblioteca em sua pégina, basta compilá-la através do compilador já fornecido e referenciar os caminhos necessários em sua IDE. A seguir temos um exemplo das referências utilizando o Visual Studio: as regiões marcadas em negrito são os endereços dos arquivos a serem linkados.

![VC++ Directories](https://i.imgur.com/ao7T1xR.png)

![Linker](https://i.imgur.com/rYnUF1i.png)

Além disso, é importante se atentar se todos os arquivos de código necessários foram incluídos ao projeto na IDE. Já são fornecidos arquivos de projeto para o Visual Studio, se preferível, encontrados na pasta SudokuVS.

Sendo assim, basta compilar normalmente agora, tomando-se o cuidado de incluir a dll criada pelo Raylib na pasta do projeto, de forma que o executável gerado seja capaz de atuar em seu computador.

## Execução
Para a execução, são necessários apenas o executável gerado e a dll Raylib da biblioteca, podendo este último ser definido como um PATH no sistema operacional, de forma a não ser mais necessário incluí-lo na pasta do jogo.

