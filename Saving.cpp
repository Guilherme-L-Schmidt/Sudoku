#include <stdio.h>
#include <stdlib.h>

void SaveGame(int matriz[9][9][3], int resposta[9][9], int dificuldade) {
	// Abre o arquivo em que será salvo o jogo
	FILE *jogo = NULL;
	errno_t error;

	switch (dificuldade) {
	case 0:
		error = fopen_s(&jogo, "save_game_facil.txt", "w+");
		break;
	case 1:
		error = fopen_s(&jogo, "save_game_medio.txt", "w+");
		break;
	case 2:
	default:
		error = fopen_s(&jogo, "save_game_dificil.txt", "w+");
		break;
	}

	if (jogo == NULL) {
		printf("Erro em salvar jogo. Erro %d\n", error);
	}
	else {
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
		printf("Jogo salvo\n");
	}
}

int LoadGame(int matriz[9][9][3], int resposta[9][9], int dificuldade) {
	// Abre o arquivo em que foi salvo o jogo
	FILE* jogo = NULL;
	errno_t error;

	switch (dificuldade) {
	case 0:
		error = fopen_s(&jogo, "save_game_facil.txt", "r+");
		break;
	case 1:
		error = fopen_s(&jogo, "save_game_medio.txt", "r+");
		break;
	case 2:
	default:
		error = fopen_s(&jogo, "save_game_dificil.txt", "r+");
		break;
	}

	// Identifica falha em carregar
	if (jogo == NULL) {
		printf("Erro ao carregar. Erro %d\n", error);
		return 0;
	}
	else {
		// Carrega a matriz incompleta modificada pelo jogador
		for (int k = 0; k < 2; k++) {
			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {
					if (fscanf_s(jogo, "%d", &matriz[i][j][k]) != 1) {
						printf("Erro ao ler matriz\n");
						return 0;
					}
				}
			}
		}

		// Carrega a matriz de respostas
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (fscanf_s(jogo, "%d", &resposta[i][j]) != 1) {
					printf("Erro ao ler respostas\n");
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
	errno_t error;

	error = fopen_s(&config, "configuration", "W");

	if (error) {
		printf("Erro em salvar configuracoes\n");
	}
	else {
		// Salva a configuração de cor
		fprintf(config, "%d\n", cor);

		fclose(config);
	}
}

int LoadConfig() {
	int cor;

	// Abre o arquivo de configurações
	FILE* config;
	errno_t error;

	error = fopen_s(&config, "configuration", "r");

	// Identifica falha em carregar
	if (error) {
		printf("Erro ao carregar\n");
		return 0;
	}
	else {
		if (fscanf_s(config, "%d", &cor) != 1) {
			printf("Erro ao carregar respostas\n");
			return 0;
		}

		return cor;
	}
}