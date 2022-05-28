#include <stdio.h>
#include <time.h>
#include <string.h>
#include <raylib.h>
#include "Sudoku.h"

void GenerateSudoku(int scale, float grid[8][2], float pontos[9][24][2], float num[9][9][2]);
void Inserir(int i, int j);
bool Check();

int main(void) {
    SudokuCreation(40);

    time_t timer = time(NULL);

    // Definição do tamanho da janela
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Definição da posição do Sudoku e de sua escala
    int stdPos[2] = {screenWidth / 2, screenHeight / 2};
    float stdScale = screenHeight / 4;

    // Ativa antialiasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // Definição de um vetor representativo de posições do mouse
    Vector2 mousePoint = { 0.0f, 0.0f };

    // Definição dos principais pontos da grade
    float grid[8][2];
    for (int i = 0; i < 2; i++) {
        grid[2 * i][0] = grid[1 + 2 * i][0] = stdPos[0] + (1 - 2 * i) * stdScale / 2;
        grid[4 + i][0] = grid[6 + i][0] = stdPos[0] + (1 - 2 * i) * stdScale * 3 / 2;
        grid[i][1] = grid[2 + i][1] = stdPos[1] + (1 - 2 * i) * stdScale * 3 / 2;
        grid[4 + 2 * i][1] = grid[5 + 2 * i][1] = stdPos[1] + (1 - 2 * i) * stdScale / 2;
    }

    // Criação de pequenas divisões internas
    int quadrado[3][3][2];
    float pontos[9][24][2];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            quadrado[i][j][0] = stdPos[0] + (1 - j) * stdScale;
            quadrado[i][j][1] = stdPos[1] + (1 - i) * stdScale;
            for (int n = 0; n < 2; n++) {
                pontos[i * 3 + j][2 * n][0] = pontos[i * 3 + j][1 + 2 * n][0] = quadrado[i][j][0] + (1 - 2 * n) * stdScale / 6;
                pontos[i * 3 + j][4 + n][0] = pontos[i * 3 + j][6 + n][0] = quadrado[i][j][0] + stdScale / 3 + (1 - 2 * n) * stdScale / 8;
                pontos[i * 3 + j][n][1] = pontos[i * 3 + j][2 + n][1] = quadrado[i][j][1] + stdScale / 3 + (1 - 2 * n) * stdScale / 8;
                pontos[i * 3 + j][4 + 2 * n][1] = pontos[i * 3 + j][5 + 2 * n][1] = quadrado[i][j][1] + (1 - 2 * n) * stdScale / 6;
                
                pontos[i * 3 + j][8 + 2 * n][0] = pontos[i * 3 + j][9 + 2 * n][0] = quadrado[i][j][0] + (1 - 2 * n) * stdScale / 6;
                pontos[i * 3 + j][12 + n][0] = pontos[i * 3 + j][14 + n][0] = quadrado[i][j][0] - stdScale / 3 + (1 - 2 * n) * stdScale / 8;
                pontos[i * 3 + j][8 + n][1] = pontos[i * 3 + j][10 + n][1] = quadrado[i][j][1] - stdScale / 3 + (1 - 2 * n) * stdScale / 8;
                pontos[i * 3 + j][12 + 2 * n][1] = pontos[i * 3 + j][13 + 2 * n][1] = quadrado[i][j][1] + (1 - 2 * n) * stdScale / 6;
                
                pontos[i * 3 + j][16 + 2 * n][0] = pontos[i * 3 + j][17 + 2 * n][0] = quadrado[i][j][0] + (1 - 2 * n) * stdScale / 6;
                pontos[i * 3 + j][20 + n][0] = pontos[i * 3 + j][22 + n][0] = quadrado[i][j][0] + (1 - 2 * n) * stdScale / 8;
                pontos[i * 3 + j][16 + n][1] = pontos[i * 3 + j][18 + n][1] = quadrado[i][j][1] + (1 - 2 * n) * stdScale / 8;
                pontos[i * 3 + j][20 + 2 * n][1] = pontos[i * 3 + j][21 + 2 * n][1] = quadrado[i][j][1] + (1 - 2 * n) * stdScale / 6;

            }
        }
    }

    // Definição dos pontos de posicionamento dos números
    float num[9][9][2];
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            num[i][j][0] = stdPos[0] + (j - 4) * stdScale / 3;
            num[i][j][1] = stdPos[1] + (i - 4) * stdScale / 3;
        }
    }

    // Definição de botões para cada ponto
    Rectangle bot[9][9];
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            bot[i][j] = { num[i][j][0] - stdScale / 6, num[i][j][1] - stdScale / 6, stdScale / 3, stdScale / 3 };
        }
    }
    int selected[2] = {10, 10};

    // Criação da janela
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    // Definição do FPS
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    // Loop enquanto não houver um comando de encerramento
    while (!WindowShouldClose())
    {
        bool vitoria = false;

        // Checa a vitória
        if (Check() == false) {

            mousePoint = GetMousePosition();

            // Detecção de cliques em casas do Sudoku
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
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
                DrawCircle(num[selected[0]][selected[1]][0], num[selected[0]][selected[1]][1], (stdScale / 3 - stdScale / 25) / 2, LIME);
                if (matriz_incompleta[selected[0]][selected[1]][0] != 0) {
                    for (int i = 0; i < 9; i++) {
                        for (int j = 0; j < 9; j++) {
                            if (matriz_incompleta[i][j][0] == matriz_incompleta[selected[0]][selected[1]][0] && i != selected[0] && j != selected[1])
                                DrawCircle(num[i][j][0], num[i][j][1], (stdScale / 3 - stdScale / 25) / 2, LIME);
                        }
                    }
                }
                if (matriz_incompleta[selected[0]][selected[1]][1] == 0) {
                    Inserir(selected[0], selected[1]);
                }
            }
        }
        else {
            vitoria = true;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Desenho da matriz do Sudoku
        GenerateSudoku(stdScale, grid, pontos, num);

        // Definição de temporizador
        char temp[10];
        sprintf_s(temp, "%d s", (time(NULL) - timer));
        Vector2 offset = MeasureTextEx(GetFontDefault(), temp, 30, 1);
        DrawText(temp, screenWidth / 2 - offset.x / 2, screenHeight / 25 - offset.y / 2, 30, DARKGREEN);

        if (vitoria == true) {
            DrawRectangle(stdPos[0] - stdScale * 2, stdPos[1] - stdScale / 2, stdScale * 4, stdScale, GRAY);
            Vector2 offset_2 = MeasureTextEx(GetFontDefault(), "Parabéns, você venceu!", 50, 5);
            DrawTextEx(GetFontDefault(), "Parabens, voce venceu!", { stdPos[0] - offset_2.x / 2, stdPos[1] - offset_2.y / 2 }, 50, 5, BLACK);
        }

        EndDrawing();
    }

    // Fechamento da janela e do contexto OpenGL
    CloseWindow();

    return 0;
}

void GenerateSudoku(int scale, float grid[8][2], float pontos[9][24][2], float num[9][9][2]) {
    // Desenho da grade
    for (int i = 0; i < 8; i++) {
        // printf("vetores[%d]: %.2f, %.2f \n", i, vectors[i][0], vectors[i][1]);
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

// Insere um valor na matriz quando pressionada uma tecla
void Inserir(int i, int j) {
    int tecla = GetCharPressed();
    if (tecla - 48 > -1 && tecla - 48 < 10) {
        matriz_incompleta[i][j][0] = tecla - 48;
    }
}

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