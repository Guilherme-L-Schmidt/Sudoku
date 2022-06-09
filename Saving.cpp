#include <stdio.h>
#include <stdlib.h>

void SaveGame(int matriz[9][9][3], int resposta[9][9], int dificuldade) {
	// Abre o arquivo em que será salvo o jogo
	FILE* jogo;

	switch (dificuldade) {
	case 0:
		jogo = fopen("save_game_facil.txt", "W");
		break;
	case 1:
		jogo = fopen("save_game_medio.txt", "W");
		break;
	case 2:
		jogo = fopen("save_game_dificil.txt", "W");
		break;
	}

	// Salva a matriz incompleta modificada pelo jogador
	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				fprintf(jogo, "%d ", matriz[i][j][k]);
			}
		}
		fprintf(jogo, "\n");
	}

	// Salva a matriz de respostas
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			fprintf(jogo, "%d ", resposta[i][j]);
		}
	}

	// Fecha o arquivo
	fclose(jogo);
}

int LoadGame(int matriz[9][9][3], int resposta[9][9], int dificuldade) {
	// Abre o arquivo em que foi salvo o jogo
	FILE* jogo;

	switch (dificuldade) {
	case 0:
		jogo = fopen("save_game_facil.txt", "r");
		break;
	case 1:
		jogo = fopen("save_game_medio.txt", "r");
		break;
	case 2:
		jogo = fopen("save_game_dificil.txt", "r");
		break;
	}

	// Identifica falha em carregar
	if (jogo == NULL) {
		printf("Erro ao carregar");
		return 0;
	}
	else {
		// Carrega a matriz incompleta modificada pelo jogador
		for (int k = 0; k < 2; k++) {
			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {
					if (fscanf(jogo, "%d", &matriz[i][j][k]) != 1) {
						printf("Erro ao carregar matriz");
						return 0;
					}
				}
			}
		}

		// Carrega a matriz de respostas
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (fscanf(jogo, "%d", &resposta[i][j]) != 1) {
					printf("Erro ao carregar respostas");
					return 0;
				}
			}
		}

		// Fecha o arquivo
		fclose(jogo);

		return 1;
	}
}

void SaveConfig(int cor) {
	// Abre o arquivo de configurações
	FILE* config;
	config = fopen("configuration", "W");

	// Salva a configuração de cor
	fprintf(config, "%d\n", cor);

	fclose(config);
}

int LoadConfig() {
	int cor;

	// Abre o arquivo de configurações
	FILE* config;
	config = fopen("configuration", "r");

	// Identifica falha em carregar
	if (config == NULL) {
		printf("Erro ao carregar");
		return 0;
	}
	else {
		if (fscanf(config, "%d", &cor) != 1) {
			printf("Erro ao carregar respostas");
			return 0;
		}

		return cor;
	}
}