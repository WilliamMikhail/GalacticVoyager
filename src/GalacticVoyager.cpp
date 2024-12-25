#include <GLUT/glut.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <iostream>

// Character properties
float characterX = 0.0f, characterY = -0.8f;
float characterSize = 0.05f;
float glideSpeed = 0.02f;

// Movement states
bool movingLeft = false;
bool movingRight = false;

// Obstacles (blocks)
struct Block {
    float x, y, speed;
};
std::vector<Block> blocks;
float blockSpawnInterval = 2.0f; // Seconds between block spawns
float timeSinceLastSpawn = 0.0f;

// Background stars (parallax effect)
const int STAR_LAYERS = 3;
const int STARS_PER_LAYER = 100;
float starX[STAR_LAYERS][STARS_PER_LAYER];
float starY[STAR_LAYERS][STARS_PER_LAYER];
float starSpeeds[STAR_LAYERS] = { 0.005f, 0.003f, 0.001f };

// Game state
bool gameOver = false;
float score = 0.0f;

// Function to draw a rectangle
void drawRectangle(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x - width / 2, y - height / 2);
    glVertex2f(x + width / 2, y - height / 2);
    glVertex2f(x + width / 2, y + height / 2);
    glVertex2f(x - width / 2, y + height / 2);
    glEnd();
}

// Function to draw stars
void drawStar(float x, float y) {
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

// Function to display text on screen
void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw stars for parallax background
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int layer = 0; layer < STAR_LAYERS; layer++) {
        for (int i = 0; i < STARS_PER_LAYER; i++) {
            drawStar(starX[layer][i], starY[layer][i]);
        }
    }

    if (!gameOver) {
        // Draw the player character
        glColor3f(0.0f, 1.0f, 0.0f);
        drawRectangle(characterX, characterY, characterSize, characterSize);

        // Draw blocks (obstacles)
        glColor3f(1.0f, 0.0f, 0.0f);
        for (const auto& block : blocks) {
            drawRectangle(block.x, block.y, 0.1f, 0.05f);
        }

        // Display score
        glColor3f(1.0f, 1.0f, 1.0f);
        char scoreText[20];
        sprintf(scoreText, "Score: %.1f", score);
        drawText(scoreText, -0.9f, 0.9f);
    } else {
        // Game over screen
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText("Game Over!", -0.2f, 0.2f);
        char finalScore[30];
        sprintf(finalScore, "Final Score: %.1f", score);
        drawText(finalScore, -0.3f, 0.1f);
        drawText("Press R to Restart", -0.3f, 0.0f);
        drawText("Press E to Exit", -0.3f, -0.2f);
    }

    glFlush();
}

// Update function
void update(int value) {
    if (!gameOver) {
        // Update stars for parallax effect
        for (int layer = 0; layer < STAR_LAYERS; layer++) {
            for (int i = 0; i < STARS_PER_LAYER; i++) {
                starY[layer][i] -= starSpeeds[layer];
                if (starY[layer][i] < -1.0f) {
                    starY[layer][i] = 1.0f;
                    starX[layer][i] = ((rand() % 200) - 100) / 100.0f;
                }
            }
        }

        // Update blocks (falling obstacles)
        for (auto& block : blocks) {
            block.y -= block.speed;
        }
        blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
            [](const Block& b) { return b.y < -1.0f; }),
            blocks.end());

        // Spawn new blocks
        timeSinceLastSpawn += 0.016f;
        if (timeSinceLastSpawn >= blockSpawnInterval) {
            timeSinceLastSpawn = 0.0f;
            float randomX = ((rand() % 200) - 100) / 100.0f;
            blocks.push_back({ randomX, 1.0f, 0.03f });
        }

        // Check for collisions
        for (const auto& block : blocks) {
            if (abs(characterX - block.x) < 0.075f && abs(characterY - block.y) < 0.05f) {
                gameOver = true;
            }
        }

        // Increment score
        score += 0.016f;

        // Update character position
        if (movingLeft) characterX -= glideSpeed;
        if (movingRight) characterX += glideSpeed;
        characterX = std::clamp(characterX, -0.9f, 0.9f);

        glutPostRedisplay();
    }
    glutTimerFunc(16, update, 0);
}

// Handle keyboard input
void handleKeys(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') { // Restart
        if (gameOver) {
            characterX = 0.0f;
            blocks.clear();
            score = 0.0f;
            gameOver = false;
        }
    } else if (key == 'e' || key == 'E') { // Exit
        exit(0);
    }
}

void handleSpecialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) movingLeft = true;
    if (key == GLUT_KEY_RIGHT) movingRight = true;
}

void handleSpecialKeyUp(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) movingLeft = false;
    if (key == GLUT_KEY_RIGHT) movingRight = false;
}

// Initialize the game
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    srand(static_cast<unsigned>(time(0)));

    // Initialize stars
    for (int layer = 0; layer < STAR_LAYERS; layer++) {
        for (int i = 0; i < STARS_PER_LAYER; i++) {
            starX[layer][i] = ((rand() % 200) - 100) / 100.0f;
            starY[layer][i] = ((rand() % 200) - 100) / 100.0f;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Falling Blocks Game");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutKeyboardFunc(handleKeys);
    glutSpecialFunc(handleSpecialKeys);
    glutSpecialUpFunc(handleSpecialKeyUp);
    glutMainLoop();
    return 0;
}