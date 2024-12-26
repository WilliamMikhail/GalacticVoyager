#include <GLUT/glut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

// Character properties
float characterX = 0.0f, characterY = -0.8f;
float characterSize = 0.05f;
float targetX = 0.0f; // Target position for smooth movement
float glideSpeed = 0.02f; // Speed of the glide

// Key states
bool movingLeft = false;
bool movingRight = false;

// Obstacle properties
std::vector<float> carX; // Lane positions
std::vector<float> carY; // Starting positions
std::vector<float> carSpeed; // Speeds

// Star layers for parallax effect
float starX[3][100]; // 3 layers of stars
float starY[3][100];
float starSpeed[3] = { 0.005f, 0.001f, 0.002f }; // Different speeds for each layer

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

// Function to draw a star (small square for simplicity)
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

// Function to display the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the stars with parallax effect
    glColor3f(1.0f, 1.0f, 1.0f); // White
    for (int layer = 0; layer < 3; layer++) {
        for (int i = 0; i < 100; i++) {
            drawStar(starX[layer][i], starY[layer][i]);
        }
    }

    if (!gameOver) {
        // Draw the player character
        glColor3f(0.0f, 1.0f, 0.0f); // Green
        drawRectangle(characterX, characterY, characterSize, characterSize);

        // Draw the cars
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        for (size_t i = 0; i < carX.size(); i++) {
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
        // Move stars downward with different speeds (parallax effect)
        for (int layer = 0; layer < 3; layer++) {
            for (int i = 0; i < 100; i++) {
                starY[layer][i] -= starSpeed[layer];
                if (starY[layer][i] < -1.0f) {
                    starY[layer][i] = 1.0f; // Reset the star to top of screen
                    starX[layer][i] = ((rand() % 200) - 100) / 100.0f; // Randomize x position
                }
            }
        }

        // Move cars downward
       for (size_t i = 0; i < carX.size(); i++) {
    carY[i] -= carSpeed[i];
    if (carY[i] < -1.0f) {
        carY[i] = 1.0f;
        carX[i] = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f)); // Randomize position across the screen
    }
}

        // Check for collisions
        for (size_t i = 0; i < carX.size(); i++) {
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

        //Move the character smoothly towards the target position
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

// Function to spawn new cars every 4 seconds
void spawnCar(int value) {
    if (!gameOver) {
        // Randomize X position across more lanes: 9 positions (-1.0 to 1.0 range)
        float newCarX = static_cast<float>(rand() % 20 - 10) * 0.1f;  // Randomize across more lanes

        // Randomize Y position: Start from the top and move downward
        float newCarY = 1.0f;  // Initial spawn at the top

        // Randomize speed: Vary the speed more for unpredictability
        float newCarSpeed = 0.01f + static_cast<float>(rand() % 16) / 1000.0f; // Speed from 0.01 to 0.025
        // Add car properties to the vectors
        carX.push_back(newCarX);
        carY.push_back(newCarY);
        carSpeed.push_back(newCarSpeed);

        // Randomize the next spawn time between 2 to 5 seconds
        int nextSpawnTime = 2000 + rand() % 3000;  // Random between 2 and 5 seconds

        // Recursively call spawnCar with a randomized time delay
        glutTimerFunc(nextSpawnTime, spawnCar, 0);
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
            carX.clear();
            carY.clear();
            carSpeed.clear();
            for (int i = 0; i < 3; i++) {
                carX.push_back(((rand() % 3) - 1) * 0.6f);
                carY.push_back(1.0f + i * 0.5f);
                carSpeed.push_back(0.01f);
            }
            score = 0.0f;
            gameOver = false;
            glutTimerFunc(16, update, 0); // Restart the update loop
            glutTimerFunc(4000, spawnCar, 0); // Restart car spawning
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

    // Initialize the stars in the three layers
    for (int layer = 0; layer < 3; layer++) {
        for (int i = 0; i < 100; i++) {
            starX[layer][i] = ((rand() % 200) - 100) / 100.0f; // Randomize x position
            starY[layer][i] = ((rand() % 200) - 100) / 100.0f; // Randomize y position
        }
    }

    // Initialize the cars
    for (int i = 0; i < 3; i++) {
        carX.push_back(((rand() % 3) - 1) * 0.6f);
        carY.push_back(1.0f + i * 0.5f);
        carSpeed.push_back(0.01f);
    }
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
    glutTimerFunc(4000, spawnCar, 0); // Start spawning cars every 4 seconds
    glutMainLoop();

    return 0;
}