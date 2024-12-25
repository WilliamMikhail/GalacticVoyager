#include <GLUT/glut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Character properties
float characterX = 0.0f, characterY = -0.8f;
float characterSize = 0.05f;
float targetX = 0.0f; // Target position for smooth movement
float glideSpeed = 0.05f; // Speed of the glide

// Key states
bool movingLeft = false;
bool movingRight = false;

// Obstacle properties
float carX[3] = { -0.6f, 0.0f, 0.6f }; // Lane positions
float carY[3] = { 1.0f, 1.5f, 2.0f };  // Starting positions
float carSpeed[3] = { 0.01f, 0.015f, 0.02f }; // Speeds

// Game state
bool gameOver = false;
float score = 0.0f; // Time-based score

// Function to draw a rectangle
void drawRectangle(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x - width / 2, y - height / 2);
    glVertex2f(x + width / 2, y - height / 2);
    glVertex2f(x + width / 2, y + height / 2);
    glVertex2f(x - width / 2, y + height / 2);
    glEnd();
}

// Function to display text on screen
void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

// Function to display the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (!gameOver) {
        // Draw the player character
        glColor3f(0.0f, 1.0f, 0.0f); // Green
        drawRectangle(characterX, characterY, characterSize, characterSize);

        // Draw the cars
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        for (int i = 0; i < 3; i++) {
            drawRectangle(carX[i], carY[i], 0.1f, 0.05f);
        }

        // Display the score
        glColor3f(1.0f, 1.0f, 1.0f); // White
        char scoreText[20];
        sprintf(scoreText, "Score: %.1f", score);
        drawText(scoreText, -0.9f, 0.9f);

    } else {
        // Game Over message
        glColor3f(1.0f, 1.0f, 1.0f); // White
        drawText("Game Over!", -0.2f, 0.2f);

        char finalScoreText[30];
        sprintf(finalScoreText, "Final Score: %.1f", score);
        drawText(finalScoreText, -0.3f, 0.1f);
        drawText("Press R to Restart", -0.3f, 0.0f);
        drawText("Press E to Exit Game", -0.3f, -0.2f);
    }

    glFlush();
}

// Function to update the scene
void update(int value) {
    if (!gameOver) {
        // Move cars downward
        for (int i = 0; i < 3; i++) {
            carY[i] -= carSpeed[i];
            if (carY[i] < -1.0f) {
                carY[i] = 1.0f;
                carX[i] = ((rand() % 3) - 1) * 0.6f; // Randomize lane
            }
        }

        // Check for collisions
        for (int i = 0; i < 3; i++) {
            if (characterY < carY[i] + 0.05f && characterY > carY[i] - 0.05f &&
                characterX < carX[i] + 0.05f && characterX > carX[i] - 0.05f) {
                gameOver = true;
            }
        }

        // Increment score
        score += 0.016f; // Increment based on time (~60 FPS)

        // Glide the character smoothly towards the target position
        if (movingLeft) {
            targetX -= glideSpeed;
            if (targetX < -0.9f) targetX = -0.9f; // Boundary check
        }
        if (movingRight) {
            targetX += glideSpeed;
            if (targetX > 0.9f) targetX = 0.9f; // Boundary check
        }

        // Move the character smoothly towards the target position
        if (characterX < targetX) {
            characterX += glideSpeed;
            if (characterX > targetX) characterX = targetX; // Stop when the target is reached
        } else if (characterX > targetX) {
            characterX -= glideSpeed;
            if (characterX < targetX) characterX = targetX; // Stop when the target is reached
        }

        glutPostRedisplay();
        glutTimerFunc(16, update, 0); // Call update every 16ms (~60 FPS)
    }
}

// Function to handle keyboard input (special keys)
void handleSpecialKeys(int key, int x, int y) {
    if (!gameOver) {
        switch (key) {
            case GLUT_KEY_LEFT: // Left arrow key
                movingLeft = true;
                break;

            case GLUT_KEY_RIGHT: // Right arrow key
                movingRight = true;
                break;
        }
    }
}

// Function to handle keyboard input (regular keys)
void handleRegularKeys(unsigned char key, int x, int y) {
    if (key == 27) { // Escape key
        gameOver = true; // Trigger the pause menu
    } else if (key == 'r' || key == 'R') { // Restart the game
        if (gameOver) {
            characterX = 0.0f;
            characterY = -0.8f;
            for (int i = 0; i < 3; i++) carY[i] = 1.0f + i * 0.5f; // Reset cars
            score = 0.0f;
            gameOver = false;
            glutTimerFunc(16, update, 0); // Restart the update loop
        }
    } else if (key == 'e' || key == 'E') { // Exit the game
        exit(0); // Terminate the program
    }
}

// Function to handle key release
void handleKeyUp(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        movingLeft = false;
    }
    if (key == GLUT_KEY_RIGHT) {
        movingRight = false;
    }
}

// Initialize OpenGL settings
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Set the coordinate system
}

int main(int argc, char **argv) {
    srand(static_cast<unsigned>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Smooth Glide Movement");

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(handleSpecialKeys); // Handle special keys (arrow keys)
    glutKeyboardFunc(handleRegularKeys); // Handle regular keys (esc, r, e, etc.)
    glutSpecialUpFunc(handleKeyUp); // Handle key release
    glutTimerFunc(16, update, 0); // Start the update loop
    glutMainLoop();

    return 0;
}