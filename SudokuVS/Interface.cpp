#include <stdio.h>
#include <time.h>
#include <string.h>
#include <raylib.h>

void GenerateSudoku(int w, int h);

int main(void) {
    time_t timer = time(NULL);

    // Definição do tamanho da janela
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Ativa antialiasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // Criação da janela
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    // Definição do FPS
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    // Loop enquanto não houver um comando de encerramento
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        GenerateSudoku(screenWidth, screenHeight);

        // Definição de temporizador
        char temp[10];
        sprintf_s(temp, "%d", (time(NULL) - timer));
        DrawText(temp, screenWidth * 99 / 200, screenHeight / 100, 30, DARKGREEN);

        EndDrawing();
    }

    // Fechamento da janela e do contexto OpenGL
    CloseWindow();

    return 0;
}

void GenerateSudoku(int w, int h) {
    // Definição dos principais pontos da grade
    float vectors[8][2];
    for (int i = 0; i < 2; i++) {
        vectors[2*i][0] = vectors[1+2*i][0] = w / 2 + (1 - 2 * i) * h * 1 / 8;
        vectors[4 + i][0] = vectors[6 + i][0] = w / 2 + (1 - 2 * i) * h * 3 / 8;
        vectors[i][1] = vectors[2 + i][1] = h * (1 + 6 * i) / 8;
        vectors[4 + 2 * i][1] = vectors[5 + 2 * i][1] = h / 2 + (1 - 2 * i) * h * 1 / 8;
    }
    // Desenho da grade
    for (int i = 0; i < 8; i++) {
        // printf("vetores[%d]: %.2f, %.2f \n", i, vectors[i][0], vectors[i][1]);
        DrawCircle(vectors[i][0], vectors[i][1], h / 200, DARKGREEN);
        if (i % 2 == 0)
            DrawLineEx({vectors[i][0], vectors[i][1]}, {vectors[i+1][0], vectors[i+1][1]}, h / 105, DARKGREEN);
    }
}