#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <raylib.h>
#include "Sudoku.h"
#include "Saving.h"

// Definição do struct de cores
typedef struct {
    Color color1;  // Cor tema
    Color color2;  // Background
    Color color3;  // Cor destaque
    Color color4;  // Cor texto 
} cores_tema;

// Definição de cores possíveis no jogo (rgba)
cores_tema themes[5] = {
 { {47,160,46,255}, {17,17,17,255}, {38,35,31,255}, {200,213,200,255} }, 
 { {0,96,138,255}, {13,22,29,255}, {18,53,72,255}, {157,175,192,255} },
 { {186,85,93,255}, {255,255,255,255}, {239,233,234,255}, {115,115,115,255} },
 { {105,160,191,255}, {249,249,249,255}, {220,220,220,255}, {127,127,127,255} },
 { {226,123,30,255}, {52,51,51,255}, {89,89,89,255}, {213,213,213,255} } };

// Determinação da cor a ser utilizada no jogo
int color_used = LoadColors();

// Define globalmente o estado do botão de anotação
bool anotar = false;

// Variáveis globais do Sudoku
float grid[8][2], pontos[9][24][2], num[9][9][2], place_annotation[9][9][9][2];
float quadrado[3][3][2];
Rectangle bot[9][9];
bool vitoria = false;
int timer = 0;
int tempo_prev = 0;
bool dicar = true;
int annotation[9][9][9];

// Variáveis globais do Menu
float grid_menu[8][2];
float bot_menu_pos[6][2];
Rectangle bot_menu[5];
int dificuldade = 1;

// Declaração de funções empregadas ao longo do código
void SudokuPoints(float scale, float pos[2]);
void GenerateSudoku(float scale);
void MenuPoints(float scale, float pos[2]);
int Menu(float scale, float pos[2], bool saved_games[3]);
int Jogo(int selected[2], float scale, float stdPos[2], time_t tempo);
bool Inserir(int i, int j);
bool Check();
int ScoreBoard(float stdPos[2], float scale, tempos total[10], tempos melhor[3], int num_melhor, int num_total);
bool Dica();
void Cores(float stdPos[2], float scale, Vector2 mousePoint);
void Anotar(float stdPos[2], float scale, Vector2 mousePoint);

// Função main
int main(void) {
    // Definição do tamanho da janela a partir de arquivo
    int resolution[2];
    LoadRes(resolution);
    int screenWidth = resolution[0];
    int screenHeight = resolution[1];

    // Definição da posição do Sudoku e de sua escala
    float stdPos[2] = {(float)screenWidth / 2, (float)screenHeight / 2};
    float scale = (float)screenHeight / 4;

    // Geração de pontos do Sudoku
    SudokuPoints(scale, stdPos);

    // Geração de pontos do Menu
    MenuPoints(scale, stdPos);

    // Ativa antialiasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // Criação da janela
    InitWindow(screenWidth, screenHeight, "Sudoku Incrivel");

    // Definição do FPS
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    // Definição de variáveis iniciais
    bool x = true, y = true, z = true;
    int selected[2] = { 10, 10 };
    int estado = 0;
    time_t tempo = time(NULL);
    tempos melhor[3], total[10];
    int num_melhor, num_total;

    // 0 = menu; 1 = jogo; 2 = placar
    // Loop enquanto não houver um comando de encerramento
    while (!WindowShouldClose()) {
        switch (estado) {
        case 0:
            {bool saved_games[3];
            if (x) {
                x = false;
                y = true;
                z = true;
                
                // Checa a existência de jogos salvos
                for (int n = 0; n < 3; n++) {
                    saved_games[n] = LoadGame(matriz_incompleta, matriz_resposta, annotation, n, &tempo_prev);
                    tempo_prev = 0;
                }
            }
            // Muda o estado baseado em escolhas do menu
            estado = Menu(scale, stdPos, saved_games);
            }
            break;
        case 1:
            if (y) {
                y = false;
                x = true;
                z = true;
                // Inicia o timer do jogo
                tempo = time(NULL);
                dicar = true;
            }
            // Muda o estado quando finalizado o jogo
            estado = Jogo(selected, scale, stdPos, tempo);            
            break;
        case 2:
            if (z) {
                x = true;
                y = true;
                z = false;
                num_total = LoadAllScores(dificuldade, total);
                num_melhor = LoadHighScores(dificuldade, melhor);
            }
            estado = ScoreBoard(stdPos, scale, total, melhor, num_melhor, num_total);
            break;
        }

    }
    if (estado == 1) {
        printf("Salvo\n");
        SaveGame(matriz_incompleta, matriz_resposta, annotation, dificuldade, timer);
    }

    // Fechamento da janela e do contexto OpenGL
    CloseWindow();

    return 0;
}

void SudokuPoints(float scale, float pos[2]) {
    // Definição dos principais pontos da grade
    for (int i = 0; i < 2; i++) {
        grid[2 * i][0] = grid[1 + 2 * i][0] = pos[0] + (1 - 2 * i) * scale / 2;
        grid[4 + i][0] = grid[6 + i][0] = pos[0] + (1 - 2 * i) * scale * 3 / 2;
        grid[i][1] = grid[2 + i][1] = pos[1] + (1 - 2 * i) * scale * 3 / 2;
        grid[4 + 2 * i][1] = grid[5 + 2 * i][1] = pos[1] + (1 - 2 * i) * scale / 2;
    }

    // Criação de pequenas divisões internas
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            quadrado[i][j][0] = pos[0] + (1 - j) * scale;
            quadrado[i][j][1] = pos[1] + (1 - i) * scale;
            for (int n = 0; n < 2; n++) {
                pontos[i * 3 + j][2 * n][0] = pontos[i * 3 + j][1 + 2 * n][0] = quadrado[i][j][0] + (1 - 2 * n) * scale / 6;
                pontos[i * 3 + j][4 + n][0] = pontos[i * 3 + j][6 + n][0] = quadrado[i][j][0] + scale / 3 + (1 - 2 * n) * scale / 8;
                pontos[i * 3 + j][n][1] = pontos[i * 3 + j][2 + n][1] = quadrado[i][j][1] + scale / 3 + (1 - 2 * n) * scale / 8;
                pontos[i * 3 + j][4 + 2 * n][1] = pontos[i * 3 + j][5 + 2 * n][1] = quadrado[i][j][1] + (1 - 2 * n) * scale / 6;

                pontos[i * 3 + j][8 + 2 * n][0] = pontos[i * 3 + j][9 + 2 * n][0] = quadrado[i][j][0] + (1 - 2 * n) * scale / 6;
                pontos[i * 3 + j][12 + n][0] = pontos[i * 3 + j][14 + n][0] = quadrado[i][j][0] - scale / 3 + (1 - 2 * n) * scale / 8;
                pontos[i * 3 + j][8 + n][1] = pontos[i * 3 + j][10 + n][1] = quadrado[i][j][1] - scale / 3 + (1 - 2 * n) * scale / 8;
                pontos[i * 3 + j][12 + 2 * n][1] = pontos[i * 3 + j][13 + 2 * n][1] = quadrado[i][j][1] + (1 - 2 * n) * scale / 6;

                pontos[i * 3 + j][16 + 2 * n][0] = pontos[i * 3 + j][17 + 2 * n][0] = quadrado[i][j][0] + (1 - 2 * n) * scale / 6;
                pontos[i * 3 + j][20 + n][0] = pontos[i * 3 + j][22 + n][0] = quadrado[i][j][0] + (1 - 2 * n) * scale / 8;
                pontos[i * 3 + j][16 + n][1] = pontos[i * 3 + j][18 + n][1] = quadrado[i][j][1] + (1 - 2 * n) * scale / 8;
                pontos[i * 3 + j][20 + 2 * n][1] = pontos[i * 3 + j][21 + 2 * n][1] = quadrado[i][j][1] + (1 - 2 * n) * scale / 6;

            }
        }
    }

    // Definição dos pontos de posicionamento dos números
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            num[i][j][0] = pos[0] + (j - 4) * scale / 3;
            num[i][j][1] = pos[1] + (i - 4) * scale / 3;
            // Definição da posição de anotações
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    place_annotation[i][j][3 * k + l][0] = num[i][j][0] + (l - 1) * scale / 10;
                    place_annotation[i][j][3 * k + l][1] = num[i][j][1] + (k - 1) * scale / 10;
                }
            }
        }
    }

    // 

    // Definição de botões para cada ponto
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            bot[i][j] = { num[i][j][0] - scale / 6, num[i][j][1] - scale / 6, scale / 3, scale / 3 };
        }
    }
}

void GenerateSudoku(float scale) {
    // Desenho da grade
    for (int i = 0; i < 8; i++) {
        DrawCircleV({ grid[i][0], grid[i][1] }, scale / 70, themes[color_used].color1);
        if (i % 2 == 0)
            DrawLineEx({ grid[i][0], grid[i][1] }, {grid[i + 1][0], grid[i + 1][1]}, scale / 35, themes[color_used].color1);
    }

    // Desenho das grades internas
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 9; j++) {
            DrawCircleV({ pontos[j][i][0], pontos[j][i][1] }, scale / 200, themes[color_used].color1);
            if (i % 2 == 0)
                DrawLineEx({ pontos[j][i][0], pontos[j][i][1] }, { pontos[j][i + 1][0], pontos[j][i + 1][1] }, scale / 100, themes[color_used].color1);
        }
    }

    // Inserção dos números na tela
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (matriz_incompleta[i][j][0] != 0) {
                char resp[2];
                sprintf_s(resp, "%d", matriz_incompleta[i][j][0]);
                Vector2 offset = MeasureTextEx(GetFontDefault(), resp, scale * 9 / 40, 1);
                if (matriz_incompleta[i][j][1] == 1) {
                    DrawCircleV({ num[i][j][0], num[i][j][1] }, (scale / 3 - scale / 25) / 2, themes[color_used].color3);
                }
                DrawTextEx(GetFontDefault(), resp, { num[i][j][0] - offset.x / 2, num[i][j][1] - offset.y / 2 }, scale * 9 / 40, 1, themes[color_used].color4);
            }
            for (int k = 0; k < 9; k++) {
                if (annotation[i][j][k] != 0) {
                    char resp[2];
                    sprintf_s(resp, "%d", annotation[i][j][k]);
                    Vector2 offset = MeasureTextEx(GetFontDefault(), resp, scale / 10, 1);
                    DrawTextEx(GetFontDefault(), resp, { place_annotation[i][j][k][0] - offset.x / 2, place_annotation[i][j][k][1] - offset.y / 2}, scale / 10, 1, themes[color_used].color4);
                }
            }
        }
    }
}

void MenuPoints(float scale, float pos[2]) {
    // Definição dos principais pontos da grade
    for (int i = 0; i < 2; i++) {
        grid_menu[2 * i][0] = grid_menu[1 + 2 * i][0] = pos[0] + (1 - 2 * i) * scale / 5;
        grid_menu[4 + i][0] = grid_menu[6 + i][0] = pos[0] + (1 - 2 * i) * scale * 3 / 5;
        grid_menu[i][1] = grid_menu[2 + i][1] = pos[1] * 4 / 9 + (1 - 2 * i) * scale * 3 / 5;
        grid_menu[4 + 2 * i][1] = grid_menu[5 + 2 * i][1] = pos[1] * 4 / 9 + (1 - 2 * i) * scale / 5;
    }

    // Definição da posição de botões do menu
    // bot_menu_pos[0][0] = caixa de texto de dificuldade
    bot_menu_pos[0][0] = bot_menu_pos[3][0] = bot_menu_pos[4][0] = bot_menu_pos[5][0] = pos[0];
    bot_menu_pos[1][0] = pos[0] - scale;
    bot_menu_pos[2][0] = pos[0] + scale;
    bot_menu_pos[0][1] = bot_menu_pos[1][1] = bot_menu_pos[2][1] = pos[1];
    bot_menu_pos[3][1] = pos[1] + scale / 2;
    bot_menu_pos[4][1] = pos[1] + scale;
    bot_menu_pos[5][1] = pos[1] + scale * 3 / 2;

    // Definição da área dos botões do menu
    // bot_menu_pos - 1
    bot_menu[0] = { bot_menu_pos[1][0] - scale / 6, bot_menu_pos[1][1] - scale / 6, scale / 3, scale / 3 };
    bot_menu[1] = { bot_menu_pos[2][0] - scale / 6, bot_menu_pos[2][1] - scale / 6, scale / 3, scale / 3 };
    bot_menu[2] = { bot_menu_pos[3][0] - scale * 5 / 6, bot_menu_pos[3][1] - scale / 6, scale * 10 / 6, scale / 3 };
    bot_menu[3] = { bot_menu_pos[4][0] - scale * 5 / 6, bot_menu_pos[4][1] - scale / 6, scale * 10 / 6, scale / 3 };
    bot_menu[4] = { bot_menu_pos[5][0] - scale * 5 / 6, bot_menu_pos[5][1] - scale / 6, scale * 10 / 6, scale / 3 };
}

int Menu(float scale, float pos[2], bool saved_games[3]) {

    // Definição de um vetor representativo de posições do mouse
    Vector2 mousePoint = GetMousePosition();

    int selected = -1;
    
    // Detecção de cliques em casas botões do menu
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < 5; i++) {
            if (CheckCollisionPointRec(mousePoint, bot_menu[i])) {
                switch (i) {
                case 0:
                    if (dificuldade > 0)
                        dificuldade--;
                    break;
                case 1:
                    if (dificuldade < 2)
                        dificuldade++;
                    break;
                case 2:
                    {int dif = 50;
                    switch (dificuldade) {
                    case 0:
                        dif = 25;
                        break;
                    case 1:
                        dif = 50;
                        break;
                    case 2:
                        dif = 81;
                        break;
                    }
                    SudokuCreation(dif);
                    }
                    return 1;
                    break;
                case 3:
                    if (saved_games[dificuldade]) {
                        LoadGame(matriz_incompleta, matriz_resposta, annotation, dificuldade, &tempo_prev);
                        return 1;
                    }
                    break;
                case 4:
                    return 2;
                    break;
                default:
                    break;
                }
            }
        }
    }

    BeginDrawing();

    // Implementa o botão de mudança de cor
    Cores(pos, scale, mousePoint);

    // Desenho da grade
    DrawCircleV({ pos[0], pos[1] * 4 / 9 }, scale * 7 / 9, themes[color_used].color1);
    for (int i = 0; i < 8; i++) {
        DrawCircleV({ grid_menu[i][0], grid_menu[i][1] }, scale / 30, themes[color_used].color2);
        if (i % 2 == 0)
            DrawLineEx({ grid_menu[i][0], grid_menu[i][1] }, { grid_menu[i + 1][0], grid_menu[i + 1][1] }, scale / 15, themes[color_used].color2);
    }

    // Escreve a dificuldade na tela
    char dif[10];
    switch (dificuldade) {
    case 0:
        sprintf_s(dif, "Facil");
        break;
    case 1:
        sprintf_s(dif, "Medio");
        break;
    case 2:
        sprintf_s(dif, "Dificil");
        break;
    default:
        break;
    }
    Vector2 offset = MeasureTextEx(GetFontDefault(), dif, scale * 9 / 40, 5);
    DrawTextEx(GetFontDefault(), dif, {bot_menu_pos[0][0] - offset.x / 2, bot_menu_pos[0][1] - offset.y / 2}, scale * 9 / 40, 5, themes[color_used].color4);

    // Desenha as setas de seleção de dificuldade
    if (dificuldade > 0) {
        DrawCircleV({ bot_menu_pos[1][0], bot_menu_pos[1][1] }, scale / 10, themes[color_used].color3);
        DrawLineEx({ bot_menu_pos[1][0] + scale / 30, bot_menu_pos[1][1] + scale / 15 }, {bot_menu_pos[1][0] - scale / 30, bot_menu_pos[1][1]}, scale / 100, themes[color_used].color4);
        DrawLineEx({ bot_menu_pos[1][0] - scale / 30, bot_menu_pos[1][1] }, { bot_menu_pos[1][0] + scale / 30, bot_menu_pos[1][1] - scale / 15 }, scale / 100, themes[color_used].color4);
    }
    if (dificuldade < 2) {
        DrawCircleV({ bot_menu_pos[2][0], bot_menu_pos[2][1] }, scale / 10, themes[color_used].color3);
        DrawLineEx({ bot_menu_pos[2][0] - scale / 30, bot_menu_pos[2][1] + scale / 15 }, { bot_menu_pos[2][0] + scale / 30, bot_menu_pos[2][1] }, scale / 100, themes[color_used].color4);
        DrawLineEx({ bot_menu_pos[2][0] + scale / 30, bot_menu_pos[2][1] }, { bot_menu_pos[2][0] - scale / 30, bot_menu_pos[2][1] - scale / 15 }, scale / 100, themes[color_used].color4);
    }

    // Desenha o botão "Novo Jogo"
    char ini[10] = "Novo Jogo";
    Vector2 offset2 = MeasureTextEx(GetFontDefault(), ini, scale / 4.5f, scale / 36);
    DrawTextEx(GetFontDefault(), ini, { bot_menu_pos[3][0] - offset2.x / 2, bot_menu_pos[3][1] - offset2.y / 2 }, scale / 4.5f, scale / 36.0f, themes[color_used].color4);

    // Desenha o botão "Continuar" se houver jogo salvo
    if (saved_games[dificuldade]) {
        char cont[10] = "Continuar";
        Vector2 offset3 = MeasureTextEx(GetFontDefault(), cont, scale / 4.5f, scale / 36);
        DrawTextEx(GetFontDefault(), cont, { bot_menu_pos[4][0] - offset3.x / 2, bot_menu_pos[4][1] - offset3.y / 2 }, scale / 4.5f, scale / 36.0f, themes[color_used].color4);
    }

    // Desenha o botão "Pontuação"
    char pontu[15] = "Pontuacao";
    Vector2 offset4 = MeasureTextEx(GetFontDefault(), pontu, scale / 4.5f, scale / 36);
    DrawTextEx(GetFontDefault(), pontu, { bot_menu_pos[5][0] - offset4.x / 2, bot_menu_pos[5][1] - offset4.y / 2 }, scale / 4.5f, scale / 36.0f, themes[color_used].color4);

    // Define acor de background
    ClearBackground(themes[color_used].color2);

    EndDrawing();

    return 0;
}

int Jogo(int selected[2], float scale, float stdPos[2], time_t tempo) {
    // Definição de um vetor representativo de posições do mouse
    Vector2 mousePoint = GetMousePosition();

    BeginDrawing();

    // Desenho da matriz do Sudoku
    GenerateSudoku(scale);

    // Definição do botão voltar
    DrawCircleV({ scale / 2, scale / 2 }, scale / 9, themes[color_used].color3);
    DrawLineEx({ scale / 2 + scale / 28, scale / 2 + scale / 14 }, { scale / 2 - scale / 28, scale / 2 }, scale / 90, themes[color_used].color4);
    DrawLineEx({ scale / 2 - scale / 28, scale / 2 }, { scale / 2 + scale / 28, scale / 2 - scale / 14 }, scale / 90, themes[color_used].color4);
    Rectangle voltar = { scale / 2 - scale / 9, scale / 2 - scale / 9,  scale * 2 / 9, scale * 2 / 9 };

    // Implementação do botão anotar
    Anotar(stdPos, scale, mousePoint);

    // Definição do botão dica
    // Muda a cor caso não seja mais possível dar dicas
    Color cor1_dica = themes[color_used].color3, cor2_dica = themes[color_used].color3;
    if (dicar) {
        cor1_dica = themes[color_used].color1;
        cor2_dica = themes[color_used].color4;
    }
    // Desenha o botão de dicas e define uma caixa de colisão
    DrawRectangleV({ stdPos[0] + scale * 2.8f - scale / 4 - scale * 0.02f, stdPos[1] - scale * 1.5f - scale / 12 - scale * 0.02f }, { scale / 2 + scale * 0.04f, scale / 6 + scale * 0.04f }, cor1_dica);
    DrawRectangleV({ stdPos[0] + scale * 2.8f - scale / 4, stdPos[1] - scale * 1.5f - scale / 12 }, { scale / 2, scale / 6 }, themes[color_used].color2);
    Vector2 offset_dica = MeasureTextEx(GetFontDefault(), "Dica", scale / 8, scale / 50);
    DrawTextEx(GetFontDefault(), "Dica", {stdPos[0] + scale * 2.8f - offset_dica.x / 2, stdPos[1] - scale * 1.5f - offset_dica.y / 2}, scale / 8, scale / 50, cor2_dica);
    Rectangle dica = { stdPos[0] + scale * 2.8f - scale / 3, stdPos[1] - scale * 1.5f - scale / 12, scale / 2, scale / 6 };

    // Checa a vitória
    if (vitoria == false) {
        // Detecção de cliques em casas do Sudoku
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            selected[0] = 10;
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (CheckCollisionPointRec(mousePoint, bot[i][j])) {
                        selected[0] = i;
                        selected[1] = j;
                    }
                }
            }
            // Seleção do botão voltar
            if (CheckCollisionPointRec(mousePoint, voltar)) {
                SaveGame(matriz_incompleta, matriz_resposta, annotation, dificuldade, timer);
                return 0;
            }
            // Seleção do botão dica
            else if (CheckCollisionPointRec(mousePoint, dica)) {
                dicar = Dica();
            }
        }

        // Destaque da casa selecionada e de demais relevantes
        if (selected[0] != 10) {
            // Destaque somente da casa selecionada, quando 0
            if (matriz_incompleta[selected[0]][selected[1]][0] == 0) {
                DrawCircleV({ num[selected[0]][selected[1]][0], num[selected[0]][selected[1]][1] }, (scale / 3 - scale / 25) / 2, themes[color_used].color1);
                for (int i = 0; i < 9; i++) {
                    if (annotation[selected[0]][selected[1]][i] != 0) {
                        char resp[2];
                        sprintf_s(resp, "%d", annotation[selected[0]][selected[1]][i]);
                        Vector2 offset = MeasureTextEx(GetFontDefault(), resp, scale / 10, 1);
                        DrawTextEx(GetFontDefault(), resp, { place_annotation[selected[0]][selected[1]][i][0] - offset.x / 2, place_annotation[selected[0]][selected[1]][i][1] - offset.y / 2 }, scale / 10, 1, themes[color_used].color4);
                    }
                }
            }
            // Destaque de todas as casas de mesmo número, quando não
            else {
                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) {
                        if (matriz_incompleta[i][j][0] == matriz_incompleta[selected[0]][selected[1]][0]) {
                            DrawCircleV({ num[i][j][0], num[i][j][1] }, (scale / 3 - scale / 25) / 2, themes[color_used].color1);
                            char resp[2];
                            sprintf_s(resp, "%d", matriz_incompleta[i][j][0]);
                            Vector2 offset = MeasureTextEx(GetFontDefault(), resp, scale * 9 / 40, 1);
                            DrawTextEx(GetFontDefault(), resp, { num[i][j][0] - offset.x / 2, num[i][j][1] - offset.y / 2 }, scale * 9 / 40, 1, themes[color_used].color4);
                        }
                    }
                }
            }
            if (matriz_incompleta[selected[0]][selected[1]][1] == 0) {
                vitoria = Inserir(selected[0], selected[1]);
            }
        }
        timer = time(NULL) - tempo + tempo_prev;
    }

    // Define a cor de background
    ClearBackground(themes[color_used].color2);

    // Definição de temporizador
    char temp[10];
    sprintf_s(temp, "%d s", timer);
    Vector2 offset = MeasureTextEx(GetFontDefault(), temp, scale * 7 / 40, 2);
    DrawTextEx(GetFontDefault(), temp, { stdPos[0] - offset.x / 2, stdPos[1] / 12 - offset.y / 2 }, scale * 7 / 40, 2, themes[color_used].color1);

    // Tela de fim de jogo
    if (vitoria == true) {
        DrawRectangleV({ stdPos[0] - scale * 2 - scale * 0.02f, stdPos[1] - scale - scale * 0.02f }, { scale * 4 + scale * 0.04f, scale * 2 + scale * 0.04f }, themes[color_used].color1);
        DrawRectangleV({ stdPos[0] - scale * 2, stdPos[1] - scale }, { scale * 4, scale * 2 }, themes[color_used].color2);
        Vector2 offset_2 = MeasureTextEx(GetFontDefault(), "Parabens, você venceu!", scale * 12 / 40, 5);
        DrawTextEx(GetFontDefault(), "Parabens, voce venceu!", { stdPos[0] - offset_2.x / 2, stdPos[1] - scale / 2 - offset_2.y / 2 }, scale * 12 / 40, 5, themes[color_used].color1);
        Vector2 offset_3 = MeasureTextEx(GetFontDefault(), "Clique para ver o placar", scale * 12 / 40, 5);
        DrawTextEx(GetFontDefault(), "Clique para ver o placar", { stdPos[0] - offset_3.x / 2, stdPos[1] + scale / 2 - offset_3.y / 2 }, scale * 12 / 40, 5, themes[color_used].color1);
    }

    EndDrawing();

    // Em caso de vitória, transfere para o scoreboard ao pressionar o botão esquerdo do mouse
    if (vitoria == true) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            SaveAllScores(timer, dificuldade);
            SaveHighScores(timer, dificuldade);
            vitoria = false;
            return 2;
        }
    }

    return 1;
}

// Insere um valor na matriz quando pressionada uma tecla
bool Inserir(int i, int j) {
    int tecla = GetCharPressed();
    int vitoria = false;
    if (tecla - 48 > -1 && tecla - 48 < 10) {
        // Insere anotações
        if (anotar) {
            if (tecla - 48 > 0) {
                // Retira o número inserido, caso já presente na anotação
                if (annotation[i][j][tecla - 49] == tecla - 48)
                    annotation[i][j][tecla - 49] = 0;
                else
                    annotation[i][j][tecla - 49] = tecla - 48;
                matriz_incompleta[i][j][0] = 0;
            }
            else {
                for (int x = 0; x < 9; x++) {
                    annotation[i][j][x] = 0;
                }
            }
        }
        // Insere os números esperados
        else {
            matriz_incompleta[i][j][0] = tecla - 48;
            vitoria = Check();
            for (int x = 0; x < 9; x++) {
                annotation[i][j][x] = 0;
            }
        }
    }
    // Remove o número quando pressionado backspace ou delete
    else if (GetKeyPressed() == KEY_BACKSPACE || GetKeyPressed() == KEY_DELETE) {
        matriz_incompleta[i][j][0] = 0;
        for (int x = 0; x < 9; x++) {
            annotation[i][j][x] = 0;
        }
    }
    return vitoria;
}

// Checa se a resposta está correta
bool Check() {
    bool n = true;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (matriz_incompleta[i][j][0] != matriz_resposta[i][j])
                n = false;
        }
    }
    return n;
}

// Desenha o Scoreboard
int ScoreBoard(float stdPos[2], float scale, tempos total[10], tempos melhor[3], int num_melhor, int num_total) {
    // Definição de um vetor representativo de posições do mouse
    Vector2 mousePoint = GetMousePosition();

    //Inicia o desenho
    BeginDrawing();

    // Cria uma área inicial
    DrawRectangleV({ stdPos[0] - scale * 2 - scale * 0.02f, stdPos[1] - scale * 3 / 2 - scale * 0.02f }, { scale * 4 + scale * 0.04f, scale * 3 + scale * 0.04f }, themes[color_used].color1);
    DrawRectangleV({ stdPos[0] - scale * 2, stdPos[1] - scale * 3 / 2 }, { scale * 4, scale * 3 }, themes[color_used].color3);

    // Definição da dificuldade a ser exposta no topo da tela
    char dif[15];
    switch (dificuldade) {
    case 0:
        sprintf_s(dif, "Facil");
        break;
    case 1:
        sprintf_s(dif, "Medio");
        break;
    case 2:
        sprintf_s(dif, "Dificil");
        break;
    default:
        break;
    }

    // Expõe a dificuldade da seção
    Vector2 offset0 = MeasureTextEx(GetFontDefault(), dif, scale / 4, 4);
    DrawTextEx(GetFontDefault(), dif, { stdPos[0] - offset0.x / 2, stdPos[1] - scale * 1.8f - offset0.y / 2 }, scale / 4, 4, themes[color_used].color1);

    // Expõe o título da seção
    Vector2 offset = MeasureTextEx(GetFontDefault(), "Tempos Obtidos", scale * 3 / 20, 3);
    DrawTextEx(GetFontDefault(), "Tempos Obtidos", { stdPos[0] - offset.x / 2, stdPos[1] - scale * 1.35f - offset.y / 2 }, scale * 3 / 20, 3, themes[color_used].color1);

    // Expõe tempos salvos
    // Melhores tempos
    Vector2 offset_1 = MeasureTextEx(GetFontDefault(), "Melhores Tempos:", scale / 9, 2);
    DrawTextEx(GetFontDefault(), "Melhores Tempos:", { stdPos[0] - scale * 1.3f - offset_1.x / 2, stdPos[1] - scale * 1.15f - offset_1.y / 2 }, scale / 9, 2, themes[color_used].color1);
    for (int n = 0; n < num_melhor; n++) {
        // Datas
        char data[25];
        sprintf_s(data, "%02d:%02d   %02d/%02d/%d", melhor[n].horas, melhor[n].minutos, melhor[n].dia, melhor[n].mes + 1, melhor[n].ano + 1900);
        Vector2 offset1 = MeasureTextEx(GetFontDefault(), data, scale / 10, 2);
        DrawTextEx(GetFontDefault(), data, { stdPos[0] - scale * 1.3f - offset1.x / 2, stdPos[1] - scale - offset1.y / 2 + offset1.y * 1.5f * n }, scale / 10, 2, themes[color_used].color1);
        // Tempos
        char tempo[10];
        sprintf_s(tempo, "%d s", melhor[n].ponts);
        Vector2 offset2 = MeasureTextEx(GetFontDefault(), tempo, scale / 10, 2);
        DrawTextEx(GetFontDefault(), tempo, { stdPos[0] + scale * 1.3f - offset2.x / 2, stdPos[1] - scale - offset2.y / 2 + offset1.y * 1.5f * n }, scale / 10, 2, themes[color_used].color1);
        // Linha de separação
        DrawLineEx({ stdPos[0] - scale * 1.85f, stdPos[1] - scale + offset1.y * 0.7f + offset1.y * 1.5f * n }, { stdPos[0] + scale * 1.85f, stdPos[1] - scale + offset1.y * 0.7f + offset1.y * 1.5f * n }, scale / 200, themes[color_used].color1);
    }
    // 10 últimos tempos
    Vector2 offset_2 = MeasureTextEx(GetFontDefault(), "Ultimos Tempos:", scale / 9, 2);
    DrawTextEx(GetFontDefault(), "Ultimos Tempos:", { stdPos[0] - scale * 1.3f - offset_1.x / 2, stdPos[1] - scale / 2.5f - offset_1.y / 2 }, scale / 9, 2, themes[color_used].color1);
    for (int n = 0; n < num_total; n++) {
        // Datas
        char data[25];
        sprintf_s(data, "%02d:%02d   %02d/%02d/%d", total[n].horas, total[n].minutos, total[n].dia, total[n].mes + 1, total[n].ano + 1900);
        Vector2 offset1 = MeasureTextEx(GetFontDefault(), data, scale / 10, 2);
        DrawTextEx(GetFontDefault(), data, { stdPos[0] - scale * 1.3f - offset1.x / 2, stdPos[1] - scale / 4 - offset1.y / 2 + offset1.y * 1.5f * n }, scale / 10, 2, themes[color_used].color1);
        // Tempos
        char tempo[10];
        sprintf_s(tempo, "%d s", total[n].ponts);
        Vector2 offset2 = MeasureTextEx(GetFontDefault(), tempo, scale / 10, 2);
        DrawTextEx(GetFontDefault(), tempo, { stdPos[0] + scale * 1.3f - offset2.x / 2, stdPos[1] - scale / 4 - offset2.y / 2 + offset1.y * 1.5f * n }, scale / 10, 2, themes[color_used].color1);
        // Linha de separação
        DrawLineEx({ stdPos[0] - scale * 1.85f, stdPos[1] - scale / 4 + offset1.y * 0.7f + offset1.y * 1.5f * n }, { stdPos[0] + scale * 1.85f, stdPos[1] - scale / 4 + offset1.y * 0.7f + offset1.y * 1.5f * n }, scale / 200, themes[color_used].color1);
    }

    // Botão de Voltar
    Vector2 offset_3 = MeasureTextEx(GetFontDefault(), "Voltar ao Menu", scale / 7, 2);
    DrawTextEx(GetFontDefault(), "Voltar ao Menu", { stdPos[0] - offset_3.x / 2, stdPos[1] + scale * 1.8f - offset_3.y / 2 }, scale / 7, 2, themes[color_used].color1);

    // Detecção de clique do botão voltar
    Rectangle botao = { stdPos[0] - offset_3.x / 2, stdPos[1] + scale * 1.8f - offset_3.y / 2, offset_3.x,  offset_3.y };
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePoint, botao)) {
            return 0;
        }
    }

    //Define a cor do background
    ClearBackground(themes[color_used].color2);

    EndDrawing();

    return 2;
}

// Escolha de uma casa aleatória para ser revelada
bool Dica() {
    srand(time(NULL));
    int* index = (int*) calloc(81, sizeof(int));
    int h = 0;

    // Salva todas as casa vazias do jogo em um vetor
    if (index) {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (matriz_incompleta[i][j][0] == 0) {
                    index[h] = i * 9 + j;
                    h++;
                }
            }
        }
        // Fornece uma dica quando houver mais de 1 casa vazia no Sudoku
        if (h > 1) {
            int revelada = index[rand() % h];
            matriz_incompleta[(revelada - revelada % 9) / 9][revelada % 9][0] = matriz_resposta[(revelada - revelada % 9) / 9][revelada % 9];
            matriz_incompleta[(revelada - revelada % 9) / 9][revelada % 9][1] = 1;
            for (int i = 0; i < 9; i++) {
                annotation[(revelada - revelada % 9) / 9][revelada % 9][i] = 0;
            }
            // Penalidade de tempo pela dica
            tempo_prev += 30;
        }
        // Retorna falso quando não for possível prover dicas
        else {
            return false;
        }

        // Libera o vetor alocado
        free(index);
    }
    return true;
}

// Define globalmente o estado do botão de troca de cor
bool show_colors = false;
// Troca as cores do jogo e salva a escolhida
void Cores(float stdPos[2], float scale, Vector2 mousePoint) {
    Rectangle bot_cores[5];

    // Desenha conjunto de cores disponíveis, quando ativado
    if (show_colors == true) {
        // Desenha faixa de fundo
        DrawCircleV({ stdPos[0] + scale * 1.2f, stdPos[1] - scale * 1.7f }, scale / 5, themes[color_used].color3);
        DrawLineEx({ stdPos[0] + scale * 1.2f, stdPos[1] - scale * 1.7f }, { stdPos[0] + scale * 3.2f, stdPos[1] - scale * 1.7f }, scale / 2.5f, themes[color_used].color3);
        
        // Desenha conjuntos de cores e define botões
        for (int i = 0; i < 5; i++) {
            DrawCircleV({ stdPos[0] + scale * (2.8f - 0.4f * i), stdPos[1] - scale * 1.7f }, scale / 5.5f, themes[i].color2);
            DrawCircleV({ stdPos[0] + scale * (2.8f - 0.4f * i), stdPos[1] - scale * 1.7f }, scale / 13, themes[i].color1);
            bot_cores[i] = {stdPos[0] + scale * (2.8f - 0.4f * i) - scale / 5, stdPos[1] - scale * 1.7f - scale / 5, scale / 2.5f, scale / 2.5f };
        }
    }

    // Desenha o botão de troca de cor e define uma zona de colisão
    DrawCircleV({ stdPos[0] + scale * 3.2f, stdPos[1] - scale * 1.7f}, scale / 5, themes[color_used].color3);
    DrawCircleV({ stdPos[0] + scale * 3.2f, stdPos[1] - scale * 1.7f }, scale / 5.5, themes[color_used].color2);
    DrawCircleV({ stdPos[0] + scale * 3.2f, stdPos[1] - scale * 1.7f }, scale / 13, themes[color_used].color1);
    Rectangle bot_cor = { stdPos[0] + scale * 3.2f - scale / 5, stdPos[1] - scale * 1.7f - scale / 5, scale / 2.5f, scale / 2.5f };

    // Mostra as opções de troca de cor, caso pressionado o botão
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePoint, bot_cor)) {
            if (show_colors == false)
                show_colors = true;
            else if (show_colors == true)
                show_colors = false;
        }
        for (int i = 0; i < 5; i++) {
            if (CheckCollisionPointRec(mousePoint, bot_cores[i]) && show_colors == true) {
                color_used = i;
                show_colors = false;
                // Salva o cor escolhida
                SaveColors(i);
            }
        }
    }
}

// Definição do botão de anotações
void Anotar(float stdPos[2], float scale, Vector2 mousePoint) {
    Color cor_bot;
    // Troca a cor utilizada a Depender do estado do botão
    if (anotar)
        cor_bot = themes[color_used].color1;
    else
        cor_bot = themes[color_used].color2;

    // Desenha o botão de anotação
    DrawCircleV({ stdPos[0] + scale * 2.8f, stdPos[1] }, scale / 5, themes[color_used].color3);
    DrawCircleV({ stdPos[0] + scale * 2.8f, stdPos[1] }, scale / 5.5, cor_bot);
    Vector2 offset = MeasureTextEx(GetFontDefault(), "A", scale / 4, 1);
    DrawTextEx(GetFontDefault(), "A", { stdPos[0] + scale * 2.8f - offset.x / 2, stdPos[1] - offset.y / 2}, scale / 4, 1, themes[color_used].color4);
    Rectangle bot_ano = { stdPos[0] + scale * 2.8f - scale / 5, stdPos[1] - scale / 5, scale / 2.5f, scale / 2.5f };

    // Troca o estado do botão quando pressionado
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePoint, bot_ano)) {
            if (anotar == false)
                anotar = true;
            else
                anotar = false;
        }
    }
}