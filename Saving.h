#ifndef SAVING_H
#define SAVING_H

//Parâmetros importantes para os tempos obtidos
typedef struct {
	int ponts;
	int horas;
	int minutos;
	int dia;
	int mes;
	int ano;
} tempos;

void SaveGame(int matriz[9][9][3], int resposta[9][9], int dificuldade, int tempo);
int LoadGame(int matriz[9][9][3], int resposta[9][9], int dificuldade, int* tempo);
void SaveConfig(int cor);
int LoadConfig();
void SaveAllScores(int score, int dificuldade);
void SaveHighScores(int score, int dificuldade);
int LoadAllScores(int dificuldade, tempos saves[10]);
int LoadHighScores(int dificuldade, tempos saves[3]);

#endif
