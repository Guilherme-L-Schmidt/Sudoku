#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Parâmetros importantes para os tempos obtidos
typedef struct {
	int ponts;
	int horas;
	int minutos;
	int dia;
	int mes;
	int ano;
} tempos;

void SaveGame(int matriz[9][9][3], int resposta[9][9], int dificuldade, int tempo) {
	// Abre o arquivo em que será salvo o jogo
	FILE *jogo = NULL;
	errno_t error;

	// Checa qual dos arquivos abrir, baseado na dificuldade
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

	// Detecta se houveram erros ao abrir o arquivo
	if (jogo == NULL) {
		printf("Erro ao abrir save_game (escrita). Erro %d\n", error);
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

		// Salva o tempo de jogo
		fprintf(jogo, "%d", tempo);

		// Fecha o arquivo
		fclose(jogo);
		printf("Jogo salvo\n");
	}
}

int LoadGame(int matriz[9][9][3], int resposta[9][9], int dificuldade, int* tempo) {
	// Abre o arquivo em que foi salvo o jogo
	FILE* jogo = NULL;
	errno_t error;
	
	// Checa qual dos arquivos abrir, baseado na dificuldade
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
		printf("Erro ao abir save_game (leitura). Erro %d\n", error);
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

		// Carrega o tempo de jogo
		if (fscanf_s(jogo, "%d", tempo) != 1) {
			printf("Erro ao ler tempo\n");
			return 0;
		}

		// Fecha o arquivo
		fclose(jogo);

		return 1;
	}
}

void SaveConfig(int cor) {
	// Abre o arquivo de configurações
	FILE* config = NULL;
	errno_t error;

	error = fopen_s(&config, "colors.txt", "w+");

	if (config == NULL) {
		printf("Erro em abrir config para escrita. Erro %d\n", error);
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
	FILE* config = NULL;
	errno_t error;

	error = fopen_s(&config, "colors.txt", "r+");

	// Identifica falha em carregar
	if (config == NULL) {
		printf("Erro ao carregar config para leitura. Erro %d\n", error);
		return 0;
	}
	else {
		if (fscanf_s(config, "%d", &cor) != 1) {
			fclose(config);
			printf("Erro ao ler config\n");
			return 0;
		}
		// Atribui 0 quando assume valor inválido
		if (cor > 4) {
			cor = 0;
		}

		fclose(config);

		// Retorna o valor lido, quando possível
		return cor;
	}
}

void SaveAllScores (int score, int dificuldade) {
	// Abre o arquivo de pontuações
	FILE* scores;
	errno_t error;

	tempos saves[10];
	int count = 0;

	switch (dificuldade) {
	case 0:
		error = fopen_s(&scores, "scores_facil.txt", "r+");
		break;
	case 1:
		error = fopen_s(&scores, "scores_medio.txt", "r+");
		break;
	case 2:
	default:
		error = fopen_s(&scores, "scores_dificil.txt", "r+");
		break;
	}

	if (error) {
		printf("Erro em abrir all scores. Erro %d\n", error);
	}
	else {
		// Lê o número de scores exisentes
		while (fscanf_s(scores, "%d %d %d %d %d %d", &saves[count].ponts, &saves[count].horas, &saves[count].minutos, &saves[count].dia, &saves[count].mes, &saves[count].ano) != EOF && count < 9) {
			count++;
		}
		printf("%d linhas lidas em all_scores\n", count);
		fclose(scores);
	}

	for (int n = count-1; n >= 0; n--) {
		saves[n + 1] = saves[n];
	}

	// Determinação do tempo local
	time_t temp;
	time(&temp);
	struct tm local;
	localtime_s(&local, &temp);

	saves[0] = { score, local.tm_hour, local.tm_min, local.tm_mday, local.tm_mon, local.tm_year };

	printf("Save all scores iniciado\n");

	switch (dificuldade) {
	case 0:
		error = fopen_s(&scores, "scores_facil.txt", "w+");
		break;
	case 1:
		error = fopen_s(&scores, "scores_medio.txt", "w+");
		break;
	case 2:
	default:
		error = fopen_s(&scores, "scores_dificil.txt", "w+");
		break;
	}

	if (error) {
		printf("Erro em salvar all scores. Erro %d\n", error);
	}
	else {
		// Salva as pontuações
		for (int n = 0; n <= count; n++) {
			fprintf(scores, "%d %d %d %d %d %d\n", saves[n].ponts, saves[n].horas, saves[n].minutos, saves[n].dia, saves[n].mes, saves[n].ano);
		}
		fclose(scores);
	}
}

void SaveHighScores(int score, int dificuldade) {
	// Abre o arquivo de pontuações
	FILE* scores;
	errno_t error;

	tempos saves[3];
	int count = 0;

	switch (dificuldade) {
	case 0:
		error = fopen_s(&scores, "high_scores_facil.txt", "r+");
		break;
	case 1:
		error = fopen_s(&scores, "high_scores_medio.txt", "r+");
		break;
	case 2:
	default:
		error = fopen_s(&scores, "high_scores_dificil.txt", "r+");
		break;
	}

	if (error) {
		printf("Erro em abrir high scores. Erro %d\n", error);
	}
	else {
		// Lê o número de scores exisentes
		while (fscanf_s(scores, "%d %d %d %d %d %d", &saves[count].ponts, &saves[count].horas, &saves[count].minutos, &saves[count].dia, &saves[count].mes, &saves[count].ano) != EOF && count < 3) {
			count++;
		}
		printf("%d linhas lidas em high_scores\n", count);

		fclose(scores);
	}

	// Determinação do tempo local
	time_t temp;
	time(&temp);
	struct tm local;
	localtime_s(&local, &temp);

	if (count == 0) {
		saves[0] = { score, local.tm_hour, local.tm_min, local.tm_mday, local.tm_mon, local.tm_year };
	}
	else {
		int ordem = 0;
		for (int n = 0; n < count; n++) {
			if (score > saves[n].ponts) {
				ordem++;
			}
		}

		switch (ordem) {
		case 0:
			if (count > 0) {
				saves[2] = saves[1];
			}
			saves[1] = saves[0];
			saves[0] = { score, local.tm_hour, local.tm_min, local.tm_mday, local.tm_mon, local.tm_year };
			break;
		case 1:
			if (count > 0) {
				saves[2] = saves[1];
			}
			saves[1] = { score, local.tm_hour, local.tm_min, local.tm_mday, local.tm_mon, local.tm_year };
			break;
		case 2:
			saves[2] = { score, local.tm_hour, local.tm_min, local.tm_mday, local.tm_mon, local.tm_year };
			break;
		default:
			break;
		}
	}

	switch (dificuldade) {
	case 0:
		error = fopen_s(&scores, "high_scores_facil.txt", "w+");
		break;
	case 1:
		error = fopen_s(&scores, "high_scores_medio.txt", "w+");
		break;
	case 2:
	default:
		error = fopen_s(&scores, "high_scores_dificil.txt", "w+");
		break;
	}

	if (error) {
		printf("Erro em salvar high scores. Erro %d\n", error);
	}
	else {
		// Salva as pontuações
		if (count == 3) {
			count--;
		}
		for (int n = 0; n <= count; n++) {
			fprintf(scores, "%d %d %d %d %d %d\n", saves[n].ponts, saves[n].horas, saves[n].minutos, saves[n].dia, saves[n].mes, saves[n].ano);
		}

		fclose(scores);
	}
}

int LoadAllScores(int dificuldade, tempos saves[10]) {
	// Abre o arquivo de pontuações
	FILE* scores;
	errno_t error;

	int count = 0;

	switch (dificuldade) {
	case 0:
		error = fopen_s(&scores, "scores_facil.txt", "r+");
		break;
	case 1:
		error = fopen_s(&scores, "scores_medio.txt", "r+");
		break;
	case 2:
	default:
		error = fopen_s(&scores, "scores_dificil.txt", "r+");
		break;
	}

	if (error) {
		printf("Erro em abrir all scores. Erro %d\n", error);
	}
	else {
		// Lê o número de scores exisentes
		while (fscanf_s(scores, "%d %d %d %d %d %d", &saves[count].ponts, &saves[count].horas, &saves[count].minutos, &saves[count].dia, &saves[count].mes, &saves[count].ano) != EOF && count < 10) {
			count++;
		}
		printf("%d linhas lidas em all_scores\n", count);
		fclose(scores);
	}

	return count;
}

int LoadHighScores(int dificuldade, tempos saves[3]) {
	// Abre o arquivo de pontuações
	FILE* scores;
	errno_t error;

	int count = 0;

	switch (dificuldade) {
	case 0:
		error = fopen_s(&scores, "high_scores_facil.txt", "r+");
		break;
	case 1:
		error = fopen_s(&scores, "high_scores_medio.txt", "r+");
		break;
	case 2:
	default:
		error = fopen_s(&scores, "high_scores_dificil.txt", "r+");
		break;
	}

	if (error) {
		printf("Erro em abrir high scores. Erro %d\n", error);
	}
	else {
		// Lê o número de scores exisentes
		while (fscanf_s(scores, "%d %d %d %d %d %d", &saves[count].ponts, &saves[count].horas, &saves[count].minutos, &saves[count].dia, &saves[count].mes, &saves[count].ano) != EOF && count < 3) {
			count++;
		}
		printf("%d linhas lidas em high_scores\n", count);
		fclose(scores);
	}

	return count;
}