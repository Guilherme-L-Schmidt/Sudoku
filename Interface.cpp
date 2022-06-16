#include <stdio.h>
#include <time.h>
#include <string.h>
#include <raylib.h>
#include "Sudoku.h"
#include "Saving.h"

// Variáveis globais do Sudoku
float grid[8][2], pontos[9][24][2], num[9][9][2];
int quadrado[3][3][2];
Rectangle bot[9][9];
bool vitoria = false;
int timer = 0;

// Variáveis globais do Menu
float grid_menu[8][2];
float bot_menu_pos[5][2];
Rectangle bot_menu[4];
int dificuldade = 1;

void SudokuPoints(float scale, int pos[2]);
void GenerateSudoku(float scale);
void MenuPoints(float scale, int pos[2]);
int Menu(float scale, int pos[2], bool saved_games[3]);
int Jogo(int selected[2], float scale, int stdPos[2], time_t tempo);
bool Inserir(int i, int j);
bool Check();

int main(void) {

    // Definição do tamanho da janela
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Definição da posição do Sudoku e de sua escala
    int stdPos[2] = {screenWidth / 2, screenHeight / 2};
    float scale = screenHeight / 4;

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
    bool x = true, y = true;
    int selected[2] = { 10, 10 };
    int estado = 0;
    time_t tempo = time(NULL);

    // 0 = menu; 1 = jogo;
    // Loop enquanto não houver um comando de encerramento
    while (!WindowShouldClose()) {
        switch (estado) {
        case 0:
            {bool saved_games[3];
            if (x) {
                x = false;
                y = true;

                for (int n = 0; n < 3; n++) {
                    saved_games[n] = LoadGame(matriz_incompleta, matriz_resposta, n);
                }
            }
            estado = Menu(scale, stdPos, saved_games);
            }
            break;
        case 1:
            if (y) {
                y = false;
                x = true;
                tempo = time(NULL);
            }
            estado = Jogo(selected, scale, stdPos, tempo);            
            break;
        }  
    }
    if (estado == 1) {
        printf("Salvo\n");
        SaveGame(matriz_incompleta, matriz_resposta, dificuldade);
    }

    // Fechamento da janela e do contexto OpenGL
    CloseWindow();

    return 0;
}

void SudokuPoints(float scale, int pos[2]) {
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
        }
    }

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
        DrawCircle(grid[i][0], grid[i][1], scale / 50, LIME);
        if (i % 2 == 0)
            DrawLineEx({ grid[i][0], grid[i][1] }, {grid[i + 1][0], grid[i + 1][1]}, scale / 25, LIME);
    }

    // Desenho das grades internas
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 9; j++) {
            DrawCircle(pontos[j][i][0], pontos[j][i][1], scale / 70, LIGHTGRAY);
            if (i % 2 == 0)
                DrawLineEx({ pontos[j][i][0], pontos[j][i][1] }, { pontos[j][i + 1][0], pontos[j][i + 1][1] }, scale / 35, LIGHTGRAY);
        }
    }

    // Inserção dos números na tela
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (matriz_incompleta[i][j][0] != 0) {
                char resp[2];
                sprintf_s(resp, "%d", matriz_incompleta[i][j][0]);
                Vector2 offset = MeasureTextEx(GetFontDefault(), resp, 40, 1);
                if (matriz_incompleta[i][j][1] == 1) {
                    DrawCircle(num[i][j][0], num[i][j][1], (scale / 3 - scale / 25) / 2, { 200, 200, 200, 100 });
                }
                DrawText(resp, num[i][j][0] - offset.x / 2, num[i][j][1] - offset.y / 2, 40, BLACK);
            }
        }
    }
}

void MenuPoints(float scale, int pos[2]) {
    // Definição dos principais pontos da grade
    for (int i = 0; i < 2; i++) {
        grid_menu[2 * i][0] = grid_menu[1 + 2 * i][0] = pos[0] + (1 - 2 * i) * scale / 5;
        grid_menu[4 + i][0] = grid_menu[6 + i][0] = pos[0] + (1 - 2 * i) * scale * 3 / 5;
        grid_menu[i][1] = grid_menu[2 + i][1] = pos[1] * 2 / 5 + (1 - 2 * i) * scale * 3 / 5;
        grid_menu[4 + 2 * i][1] = grid_menu[5 + 2 * i][1] = pos[1] * 2 / 5 + (1 - 2 * i) * scale / 5;
    }

    // Definição da posição de botões do menu
    // bot_menu_pos[0][0] = caixa de texto de dificuldade
    bot_menu_pos[0][0] = bot_menu_pos[3][0] = bot_menu_pos[4][0] = pos[0];
    bot_menu_pos[1][0] = pos[0] - scale;
    bot_menu_pos[2][0] = pos[0] + scale;
    bot_menu_pos[0][1] = bot_menu_pos[1][1] = bot_menu_pos[2][1] = pos[1];
    bot_menu_pos[3][1] = pos[1] + scale / 2;
    bot_menu_pos[4][1] = pos[1] + scale;

    // Definição da área dos botões do menu
    // bot_menu_pos - 1
    bot_menu[0] = { bot_menu_pos[1][0] - scale / 6, bot_menu_pos[1][1] - scale / 6, scale / 3, scale / 3 };
    bot_menu[1] = { bot_menu_pos[2][0] - scale / 6, bot_menu_pos[2][1] - scale / 6, scale / 3, scale / 3 };
    bot_menu[2] = { bot_menu_pos[3][0] - scale * 5 / 6, bot_menu_pos[3][1] - scale / 6, scale * 10 / 6, scale / 3 };
    bot_menu[3] = { bot_menu_pos[4][0] - scale * 5 / 6, bot_menu_pos[4][1] - scale / 6, scale * 10 / 6, scale / 3 };
}

int Menu(float scale, int pos[2], bool saved_games[3]) {

    // Definição de um vetor representativo de posições do mouse
    Vector2 mousePoint = GetMousePosition();

    int selected = -1;
    
    // Detecção de cliques em casas botões do menu
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < 4; i++) {
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
                        LoadGame(matriz_incompleta, matriz_resposta, dificuldade);
                        return 1;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }

    BeginDrawing();

    // Desenho da grade
    DrawCircle(pos[0], pos[1] * 2 / 5, scale * 7 / 9, LIGHTGRAY);
    for (int i = 0; i < 8; i++) {
        DrawCircle(grid_menu[i][0], grid_menu[i][1], scale / 80, LIME);
        if (i % 2 == 0)
            DrawLineEx({ grid_menu[i][0], grid_menu[i][1] }, { grid_menu[i + 1][0], grid_menu[i + 1][1] }, scale / 40, LIME);
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
    Vector2 offset = MeasureTextEx(GetFontDefault(), dif, 40, 5);
    DrawTextEx(GetFontDefault(), dif, {bot_menu_pos[0][0] - offset.x / 2, bot_menu_pos[0][1] - offset.y / 2}, 40, 5, GRAY);

    // Desenha as setas de seleção de dificuldade
    if (dificuldade > 0) {
        DrawCircle(bot_menu_pos[1][0], bot_menu_pos[1][1], scale / 10, LIGHTGRAY);
        DrawLineEx({ bot_menu_pos[1][0] + scale / 30, bot_menu_pos[1][1] + scale / 15 }, {bot_menu_pos[1][0] - scale / 30, bot_menu_pos[1][1]}, scale / 100, GRAY);
        DrawLineEx({ bot_menu_pos[1][0] - scale / 30, bot_menu_pos[1][1] }, { bot_menu_pos[1][0] + scale / 30, bot_menu_pos[1][1] - scale / 15 }, scale / 100, GRAY);
    }
    if (dificuldade < 2) {
        DrawCircle(bot_menu_pos[2][0], bot_menu_pos[2][1], scale / 10, LIGHTGRAY);
        DrawLineEx({ bot_menu_pos[2][0] - scale / 30, bot_menu_pos[2][1] + scale / 15 }, { bot_menu_pos[2][0] + scale / 30, bot_menu_pos[2][1] }, scale / 100, GRAY);
        DrawLineEx({ bot_menu_pos[2][0] + scale / 30, bot_menu_pos[2][1] }, { bot_menu_pos[2][0] - scale / 30, bot_menu_pos[2][1] - scale / 15 }, scale / 100, GRAY);
    }

    // Desenha o batão "Novo Jogo"
    char ini[10] = "Novo Jogo";
    Vector2 offset2 = MeasureTextEx(GetFontDefault(), ini, scale / 4.5, scale / 36);
    DrawTextEx(GetFontDefault(), ini, { bot_menu_pos[3][0] - offset2.x / 2, bot_menu_pos[3][1] - offset2.y / 2 }, scale / 4.5, scale / 36, BLACK);

    // Desenha o botão "Continuar" se houver jogo salvo
    if (saved_games[dificuldade]) {
        char cont[10] = "Continuar";
        Vector2 offset3 = MeasureTextEx(GetFontDefault(), cont, scale / 4.5, scale / 36);
        DrawTextEx(GetFontDefault(), cont, { bot_menu_pos[4][0] - offset3.x / 2, bot_menu_pos[4][1] - offset3.y / 2 }, scale / 4.5, scale / 36, BLACK);
    }

    ClearBackground(RAYWHITE);

    EndDrawing();

    return 0;
}

int Jogo(int selected[2], float scale, int stdPos[2], time_t tempo) {

    // Definição de um vetor representativo de posições do mouse
    Vector2 mousePoint = GetMousePosition();

    BeginDrawing();

    // Checa a vitória
    if (vitoria == false) {
        // Detecção de cliques em casas do Sudoku
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            //printf("solto \n");
            selected[0] = 10;
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (CheckCollisionPointRec(mousePoint, bot[i][j])) {
                        selected[0] = i;
                        selected[1] = j;
                        //printf("colisao \n \n");
                    }
                }
            }
        }

        // Destaque da casa selecionada e de demais relevantes
        if (selected[0] != 10) {
            DrawCircle(num[selected[0]][selected[1]][0], num[selected[0]][selected[1]][1], (scale / 3 - scale / 25) / 2, LIME);
            if (matriz_incompleta[selected[0]][selected[1]][0] != 0) {
                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) {
                        if (matriz_incompleta[i][j][0] == matriz_incompleta[selected[0]][selected[1]][0] && i != selected[0] && j != selected[1])
                            DrawCircle(num[i][j][0], num[i][j][1], (scale / 3 - scale / 25) / 2, LIME);
                    }
                }
            }
            if (matriz_incompleta[selected[0]][selected[1]][1] == 0) {
                vitoria = Inserir(selected[0], selected[1]);
            }
        }
        timer = time(NULL) - tempo;
    }

    ClearBackground(RAYWHITE);

    // Desenho da matriz do Sudoku
    GenerateSudoku(scale);

    // Definição de temporizador
    char temp[10];
    sprintf_s(temp, "%d s", timer);
    Vector2 offset = MeasureTextEx(GetFontDefault(), temp, 30, 1);
    DrawText(temp, stdPos[0] - offset.x / 2, stdPos[1] / 12 - offset.y / 2, 30, DARKGREEN);

    if (vitoria == true) {
        DrawRectangle(stdPos[0] - scale * 2, stdPos[1] - scale / 2, scale * 4, scale, GRAY);
        Vector2 offset_2 = MeasureTextEx(GetFontDefault(), "Parabéns, você venceu!", 50, 5);
        DrawTextEx(GetFontDefault(), "Parabens, voce venceu!", { stdPos[0] - offset_2.x / 2, stdPos[1] - offset_2.y / 2 }, 50, 5, BLACK);
    }

    EndDrawing();

    if (vitoria == true) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            vitoria = false;
            return 0;
        }
    }

    return 1;
}

// Insere um valor na matriz quando pressionada uma tecla
bool Inserir(int i, int j) {
    int tecla = GetCharPressed();
    int vitoria = false;
    if (tecla - 48 > -1 && tecla - 48 < 10) {
        matriz_incompleta[i][j][0] = tecla - 48;
        vitoria = Check();
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