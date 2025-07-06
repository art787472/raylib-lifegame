#include "raylib.h"
#include <vector>
#include <iostream>
#include <emscripten/emscripten.h>
// 遊戲狀態枚舉
enum class GameState {
    Drawing,
    Computing
};

// 全域變數
const int GRID_WIDTH = 30;
const int GRID_HEIGHT = 20;
const int CELL_SIZE = 20;
const int GRID_OFFSET_X = 50;
const int GRID_OFFSET_Y = 50;

bool cells[GRID_WIDTH][GRID_HEIGHT] = { false };
GameState gameState = GameState::Drawing;
Rectangle button1 = { 700, 400, 70, 40 };
float timer = 0.0f;

// LifeGameService 的 GetNextState 函式
bool** GetNextState(bool currentCells[GRID_WIDTH][GRID_HEIGHT]) {
    // 建立新的二維陣列
    static bool newCells[GRID_WIDTH][GRID_HEIGHT];

    // 初始化新陣列
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            newCells[i][j] = false;
        }
    }

    // Conway's Game of Life 規則
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            int neighbors = 0;

            // 計算周圍8個鄰居
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue; // 跳過自己

                    int ni = i + di;
                    int nj = j + dj;

                    // 檢查邊界
                    if (ni >= 0 && ni < GRID_WIDTH && nj >= 0 && nj < GRID_HEIGHT) {
                        if (currentCells[ni][nj]) {
                            neighbors++;
                        }
                    }
                }
            }

            // 應用 Conway's Game of Life 規則
            if (currentCells[i][j]) {
                // 活細胞
                if (neighbors == 2 || neighbors == 3) {
                    newCells[i][j] = true; // 存活
                }
                else {
                    newCells[i][j] = false; // 死亡
                }
            }
            else {
                // 死細胞
                if (neighbors == 3) {
                    newCells[i][j] = true; // 誕生
                }
                else {
                    newCells[i][j] = false; // 保持死亡
                }
            }
        }
    }

    // 複製結果回原陣列
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            currentCells[i][j] = newCells[i][j];
        }
    }

    return nullptr; // 不需要回傳，直接修改原陣列
}

// 繪製網格線
void DrawCells(int posX, int posY, int width, int height, int colNum, int rowNum, Color color) {
    int colStep = width / colNum;
    int rowStep = height / rowNum;

    // 繪製垂直線
    for (int i = 0; i <= colNum; i++) {
        DrawLine(
            posX + i * colStep,
            posY,
            posX + i * colStep,
            posY + height,
            color
        );
    }

    // 繪製水平線
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

// 將滑鼠座標轉換為網格座標
Vector2 ToCoordinate(Vector2 v) {
    return {
        (float)((int)(v.x - GRID_OFFSET_X) / CELL_SIZE),
        (float)((int)(v.y - GRID_OFFSET_Y) / CELL_SIZE)
    };
}



void UpdateDrawFrame(void)
{
	if (gameState == GameState::Computing) {
            // 檢查暫停按鈕
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(GetMousePosition(), button1)) {
                    gameState = GameState::Drawing;
                }
            }

            // 更新計時器
            timer += GetFrameTime();
            if (timer > 1.0f) {
                GetNextState(cells);
                timer = 0.0f;
            }

            // 繪製
            BeginDrawing();
            ClearBackground(GRAY);

            // 繪製暫停按鈕
            DrawRectangleRec(button1, WHITE);
            DrawText("pause", (int)button1.x + 10, (int)button1.y + 15, 20, BLACK);

            // 繪製網格
            DrawCells(GRID_OFFSET_X, GRID_OFFSET_Y, 600, 400, GRID_WIDTH, GRID_HEIGHT, WHITE);

            // 繪製活細胞
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

            // 檢查開始按鈕
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, button1)) {
                    gameState = GameState::Computing;
                }
            }

            // 繪製細胞
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (mousePos.x >= GRID_OFFSET_X && mousePos.x <= GRID_OFFSET_X + 600 &&
                    mousePos.y >= GRID_OFFSET_Y && mousePos.y <= GRID_OFFSET_Y + 400) {
                    Vector2 pos = ToCoordinate(mousePos);
                    int gridX = (int)pos.x;
                    int gridY = (int)pos.y;

                    // 檢查邊界
                    if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                        cells[gridX][gridY] = true;
                    }
                }
            }

            // 繪製
            BeginDrawing();
            ClearBackground(GRAY);

            // 繪製開始按鈕
            DrawRectangleRec(button1, WHITE);
            DrawText("start", (int)button1.x + 10, (int)button1.y + 15, 20, BLACK);

            // 繪製網格
            DrawCells(GRID_OFFSET_X, GRID_OFFSET_Y, 600, 400, GRID_WIDTH, GRID_HEIGHT, WHITE);

            // 繪製活細胞
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
    // 初始化視窗
    InitWindow(800, 500, "Conway's Game of Life");
	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);

    CloseWindow();
    return 0;
}