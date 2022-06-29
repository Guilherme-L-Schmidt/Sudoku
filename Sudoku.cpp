#include "Sudoku.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Defini��o de vari�veis principais
int matriz_numeros_disponiveis[9][9][9], matriz_resposta[9][9], matriz_incompleta[9][9][3];

void SudokuCreation(int dificuldade);
void GerarResposta();
void NumAleatorio(int i, int j);
void NumCerto();
int* Quadrado(int i, int j);
void NumerosDisponiveis(int i, int j, int resposta, int quadrado[2]);
void Recalculate();
int CheckPlace(int i, int j);
void Backtrack(int i, int j);
void Remover(int dificuldade);
int Resolver();

void SudokuCreation(int dificuldade) {
	srand(time(NULL));

	// Certo 1 = Sudoku valido
	int certo = 0;
	int n = 0;

	// Gera��o de matrizes at� o retorno de uma v�lida
	while (certo == 0) {
		certo = 1;
		GerarResposta();
		// Checagem da validade da resposta
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (matriz_resposta[i][j] == 0) {
					certo = 0;
				}
			}
		}
		n++;
	}


	Remover(dificuldade);

	// Atribuir a condi��o "alter�vel" (0) � matriz incompleta
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (matriz_incompleta[i][j][0] == 0)
				matriz_incompleta[i][j][1] = 0;
			else
				matriz_incompleta[i][j][1] = 1;
		}
	}
}

void GerarResposta() {
	// Defini��o inicial de todos os n�meros dispon�veis
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {
				matriz_numeros_disponiveis[i][j][k] = k + 1;
			}
			matriz_resposta[i][j] = 0;
		}
	}

	// Cria��o da matriz resposta original do sudoku
	// Cria��o de quadrados da diagonal principal, limitando as respostas
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			NumAleatorio(i, j);
		}
	}
	for (int i = 3; i < 6; i++) {
		for (int j = 3; j < 6; j++) {
			NumAleatorio(i, j);
		}
	}
	for (int i = 6; i < 9; i++) {
		for (int j = 6; j < 9; j++) {
			NumAleatorio(i, j);
		}
	}
	// Determina��o das casas restantes
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			// Tomada de numeros aleatorios para as respostas iniciais
			if (matriz_resposta[i][j] == 0) {
				NumAleatorio(i, j);
			}
			// Determinacao de valores ja certos
			NumCerto();
		}
	}
}

void NumAleatorio(int i, int j) {
	// Defini��o inicial de vari�veis
	int* quadrado;
	
	int o = 0;
	int index[9], h = 0;
	int resposta = 0;
	
	// Determina��o do n�mero a ser atribu�do
	while (h == 0 && o < 16) {
		for (int n = 0; n < 9; n++) {
			if (matriz_numeros_disponiveis[i][j][n] != 0) {
				index[h] = n;
				h++;
			}
		}
		// Atribuir n�mero caso dispon�vel
		if (h > 0) {
			resposta = index[rand() % h];
		}
		// Alterar n�meros anteriores, caso n�o
		else {
			Backtrack(i, j);
			o++;
		}
	}
	
	// Determina��o do quadrado 3x3 a que o ponto pertence
	quadrado = Quadrado(i, j);
	
	// Atribui��o da resposta obtida
	matriz_resposta[i][j] = matriz_numeros_disponiveis[i][j][resposta % 9];

	// Retirada do n�mero escolhido das matrizes de n�meros dispon�veis
	NumerosDisponiveis(i, j, resposta, quadrado);
}

// Atribui��o de resposta a casas com apenas um n�mero dispon�vel
void NumCerto() {
	int necessidade = 1;
	while (necessidade > 0) {
		necessidade = 81;
		for (int x = 0; x < 9; x++) {
			for (int y = 0; y < 9; y++) {
				int num = 0;
				for (int resp = 0; resp < 9; resp++) {
					if (matriz_numeros_disponiveis[x][y][resp] != 0 && matriz_resposta[x][y] == 0) {
						num++;
					}
				}
				if (num == 1) {
					for (int resp = 0; resp < 9; resp++) {
						if (matriz_numeros_disponiveis[x][y][resp] != 0) {
							matriz_resposta[x][y] = matriz_numeros_disponiveis[x][y][resp];
							NumerosDisponiveis(x, y, resp, Quadrado(x, y));
						}
					}
				}
				else {
					necessidade--;
				}
			}
		}
	}
}

// Determina��o da coordenada a que uma casa pertence
int* Quadrado(int i, int j) {
	static int quadrado[2];
	switch (i) {
	case 3:
	case 4:
	case 5:
		quadrado[0] = 1;
		break;
	case 6:
	case 7:
	case 8:
		quadrado[0] = 2;
		break;
	default:
		quadrado[0] = 0;
		break;
	}
	switch (j) {
	case 3:
	case 4:
	case 5:
		quadrado[1] = 1;
		break;
	case 6:
	case 7:
	case 8:
		quadrado[1] = 2;
		break;
	default:
		quadrado[1] = 0;
		break;
	}
	return quadrado;
}

// Retirada do n�mero selecionado da matriz de n�meros dispon�veis
void NumerosDisponiveis(int i, int j, int resposta, int quadrado[2]) {
	// Retirada da linha
	for (int x = 0; x < 9; x++) {
		matriz_numeros_disponiveis[i][x][resposta % 9] = 0;
	}
	// Retirada da coluna
	for (int y = 0; y < 9; y++) {
		matriz_numeros_disponiveis[y][j][resposta % 9] = 0;
	}
	// Retirada do quadrado
	for (int x = quadrado[0] * 3; x < (quadrado[0] + 1) * 3; x++) {
		for (int y = quadrado[1] * 3; y < (quadrado[1] + 1) * 3; y++) {
			matriz_numeros_disponiveis[x][y][resposta % 9] = 0;
		}
	}
}

// Recalcula a matriz de n�meros dispon�veis
void Recalculate() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {
				matriz_numeros_disponiveis[i][j][k] = k + 1;
			}
		}
	}
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (matriz_resposta[i][j] != 0) {
				int* quadrado = Quadrado(i, j);
				NumerosDisponiveis(i, j, matriz_resposta[i][j] - 1, quadrado);
			}
		}
	}
}

// Checa a exist�ncia de n�meros dispon�veis em uma posi��o
int CheckPlace(int i, int j) {
	for (int x = 0; x < 9; x++) {
		if (matriz_numeros_disponiveis[i][j][x] != 0) {
			return 1;
		}
	}
	return 0;
}

// Busca casas anteriores da matriz que podem ter seu valor alterado
void Backtrack(int i, int j) {
	int y = i;
	int x = 8;
	int resp = 0;
	while (y >= 0 && CheckPlace(i, j) == 0) {
		while (x >= 0 && CheckPlace(i, j) == 0) {
			while (resp < 9 && CheckPlace(i, j) == 0) {
				if (matriz_numeros_disponiveis[y][x][resp] != 0 && resp + 1 != matriz_resposta[y][x] && matriz_resposta[x][y] != 0) {
					matriz_resposta[y][x] = matriz_numeros_disponiveis[y][x][resp];
					Recalculate();
				}
				resp++;
			}
			x--;
		}
		y--;
	}
}

// Cria��o da matriz final do sudoku
void Remover(int dificuldade) {
	int index[81];

	// Atribui��o inicial da matriz
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			matriz_incompleta[i][j][0] = matriz_resposta[i][j];
			matriz_incompleta[i][j][2] = matriz_resposta[i][j];
		}
	}

	// Remo��o de n�meros da matriz
	int n = 0;
	while (n < dificuldade) {
		int h = 0;
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (matriz_incompleta[i][j][2] != 0) {
					index[h] = i * 9 + j;
					h++;
				}
			}
		}
		if (h == 0) {
			break;
		}
		int pos = index[rand() % h];

		// Defini��o da matriz a ser testada
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				matriz_incompleta[i][j][1] = matriz_incompleta[i][j][0];
			}
		}
		matriz_incompleta[(pos - pos % 9) / 9][pos % 9][1] = 0;

		// Remo��o do n�mero escolhido caso poss�vel
		if (Resolver() == 1) {
			matriz_incompleta[(pos - pos % 9) / 9][pos % 9][0] = 0;
			matriz_incompleta[(pos - pos % 9) / 9][pos % 9][2] = 0;
			n++;
		}
		else {
			matriz_incompleta[(pos - pos % 9) / 9][pos % 9][2] = 0;
		}
	}
}

int Resolver() {
	// Cria��o da matriz de n�meros dispon�veis
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {
				matriz_numeros_disponiveis[i][j][k] = k + 1;
			}
		}
	}
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (matriz_incompleta[i][j][1] != 0) {
				int* quadrado = Quadrado(i, j);
				NumerosDisponiveis(i, j, matriz_incompleta[i][j][1] - 1, quadrado);
			}
		}
	}
	
	// Resolu��o da matriz incompleta
	int necessidade = 1;
	while (necessidade > 0) {
		necessidade = 81;
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				int num = 0;
				for (int resp = 0; resp < 9; resp++) {
					if (matriz_numeros_disponiveis[i][j][resp] != 0 && matriz_incompleta[i][j][1] == 0) {
						num++;
					}
				}
				if (num == 1) {
					for (int resp = 0; resp < 9; resp++) {
						if (matriz_numeros_disponiveis[i][j][resp] != 0) {
							matriz_incompleta[i][j][1] = matriz_numeros_disponiveis[i][j][resp];
							NumerosDisponiveis(i, j, resp, Quadrado(i, j));
						}
					}
				}
				else {
					necessidade--;
				}
			}
		}
	}
	
	// Checa se a matriz resolvida est� completa
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (matriz_incompleta[i][j][1] == 0)
				return 0;
		}
	}

	return 1;
}