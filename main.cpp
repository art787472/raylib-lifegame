#include "raylib.h"
#include <vector>
#include <iostream>
#include <emscripten/emscripten.h>
// �C�����A�T�|
enum class GameState {
    Drawing,
    Computing
};

// �����ܼ�
const int GRID_WIDTH = 30;
const int GRID_HEIGHT = 20;
const int CELL_SIZE = 20;
const int GRID_OFFSET_X = 50;
const int GRID_OFFSET_Y = 50;

bool cells[GRID_WIDTH][GRID_HEIGHT] = { false };
GameState gameState = GameState::Drawing;
Rectangle button1 = { 700, 400, 70, 40 };
float timer = 0.0f;

// LifeGameService �� GetNextState �禡
bool** GetNextState(bool currentCells[GRID_WIDTH][GRID_HEIGHT]) {
    // �إ߷s���G���}�C
    static bool newCells[GRID_WIDTH][GRID_HEIGHT];

    // ��l�Ʒs�}�C
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            newCells[i][j] = false;
        }
    }

    // Conway's Game of Life �W�h
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            int neighbors = 0;

            // �p��P��8�ӾF�~
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue; // ���L�ۤv

                    int ni = i + di;
                    int nj = j + dj;

                    // �ˬd���
                    if (ni >= 0 && ni < GRID_WIDTH && nj >= 0 && nj < GRID_HEIGHT) {
                        if (currentCells[ni][nj]) {
                            neighbors++;
                        }
                    }
                }
            }

            // ���� Conway's Game of Life �W�h
            if (currentCells[i][j]) {
                // ���ӭM
                if (neighbors == 2 || neighbors == 3) {
                    newCells[i][j] = true; // �s��
                }
                else {
                    newCells[i][j] = false; // ���`
                }
            }
            else {
                // ���ӭM
                if (neighbors == 3) {
                    newCells[i][j] = true; // �ϥ�
                }
                else {
                    newCells[i][j] = false; // �O�����`
                }
            }
        }
    }

    // �ƻs���G�^��}�C
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            currentCells[i][j] = newCells[i][j];
        }
    }

    return nullptr; // ���ݭn�^�ǡA�����ק��}�C
}

// ø�s����u
void DrawCells(int posX, int posY, int width, int height, int colNum, int rowNum, Color color) {
    int colStep = width / colNum;
    int rowStep = height / rowNum;

    // ø�s�����u
    for (int i = 0; i <= colNum; i++) {
        DrawLine(
            posX + i * colStep,
            posY,
            posX + i * colStep,
            posY + height,
            color
        );
    }

    // ø�s�����u
    for (int i = 0; i <= rowNum; i++) {
        DrawLine(
            posX,
            posY + i * rowStep,
            posX + width,
            posY + i * rowStep,
            color
        );
    }
}

// �N�ƹ��y���ഫ������y��
Vector2 ToCoordinate(Vector2 v) {
    return {
        (float)((int)(v.x - GRID_OFFSET_X) / CELL_SIZE),
        (float)((int)(v.y - GRID_OFFSET_Y) / CELL_SIZE)
    };
}



void UpdateDrawFrame(void)
{
	if (gameState == GameState::Computing) {
            // �ˬd�Ȱ����s
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(GetMousePosition(), button1)) {
                    gameState = GameState::Drawing;
                }
            }

            // ��s�p�ɾ�
            timer += GetFrameTime();
            if (timer > 1.0f) {
                GetNextState(cells);
                timer = 0.0f;
            }

            // ø�s
            BeginDrawing();
            ClearBackground(GRAY);

            // ø�s�Ȱ����s
            DrawRectangleRec(button1, WHITE);
            DrawText("pause", (int)button1.x + 10, (int)button1.y + 15, 20, BLACK);

            // ø�s����
            DrawCells(GRID_OFFSET_X, GRID_OFFSET_Y, 600, 400, GRID_WIDTH, GRID_HEIGHT, WHITE);

            // ø�s���ӭM
            for (int i = 0; i < GRID_WIDTH; i++) {
                for (int j = 0; j < GRID_HEIGHT; j++) {
                    if (cells[i][j]) {
                        DrawRectangleV(
                            { (float)(i * CELL_SIZE + GRID_OFFSET_X), (float)(j * CELL_SIZE + GRID_OFFSET_Y) },
                            { 18, 18 },
                            LIME
                        );
                    }
                }
            }

            EndDrawing();
        }

        if (gameState == GameState::Drawing) {
            Vector2 mousePos = GetMousePosition();

            // �ˬd�}�l���s
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, button1)) {
                    gameState = GameState::Computing;
                }
            }

            // ø�s�ӭM
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (mousePos.x >= GRID_OFFSET_X && mousePos.x <= GRID_OFFSET_X + 600 &&
                    mousePos.y >= GRID_OFFSET_Y && mousePos.y <= GRID_OFFSET_Y + 400) {
                    Vector2 pos = ToCoordinate(mousePos);
                    int gridX = (int)pos.x;
                    int gridY = (int)pos.y;

                    // �ˬd���
                    if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                        cells[gridX][gridY] = true;
                    }
                }
            }

            // ø�s
            BeginDrawing();
            ClearBackground(GRAY);

            // ø�s�}�l���s
            DrawRectangleRec(button1, WHITE);
            DrawText("start", (int)button1.x + 10, (int)button1.y + 15, 20, BLACK);

            // ø�s����
            DrawCells(GRID_OFFSET_X, GRID_OFFSET_Y, 600, 400, GRID_WIDTH, GRID_HEIGHT, WHITE);

            // ø�s���ӭM
            for (int i = 0; i < GRID_WIDTH; i++) {
                for (int j = 0; j < GRID_HEIGHT; j++) {
                    if (cells[i][j]) {
                        DrawRectangleV(
                            { (float)(i * CELL_SIZE + GRID_OFFSET_X), (float)(j * CELL_SIZE + GRID_OFFSET_Y) },
                            { 18, 18 },
                            LIME
                        );
                    }
                }
            }

            EndDrawing();
        }
}

int main() {
    // ��l�Ƶ���
    InitWindow(800, 500, "Conway's Game of Life");
	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);

    CloseWindow();
    return 0;
}