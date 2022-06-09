#ifndef SAVING_H
#define SAVING_H

void SaveGame(int matriz[9][9][3], int resposta[9][9], int dificuldade);
int LoadGame(int matriz[9][9][3], int resposta[9][9], int dificuldade);
void SaveConfig(int cor);
int LoadConfig();


#endif
