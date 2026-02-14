#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <math.h> 
#include <windows.h> 

// DEFINISI

#define MAP_SIZE 12
#define MAX_LEVEL 8
#define FILE_SCORE "highscore_pro.txt"

// DEFINISI WARNA
#define C_BLACK 0
#define C_BLUE 9
#define C_GREEN 10     
#define C_CYAN 11
#define C_RED 12
#define C_PINK 13
#define C_YELLOW 14
#define C_WHITE 15
#define C_NORMAL 7
#define C_GREY 8

// STRUKTUR DATA
typedef struct {
    char name[30];
    int currentHealth, maxHealth;
    int posX, posY;
    int score;
    int currentLevel;
} PlayerData;

// VARIABEL GLOBAL
char areaMap[MAP_SIZE][MAP_SIZE];
int mineGrid[MAP_SIZE][MAP_SIZE]; 
PlayerData mainPlayer;
int enemiesRemaining = 0; 

// INISIALISASI FUNGSI
// ============================================================

// Fungsi Utilitas & Desain
void setColor(int colorCode);
void gotoxy(int posX, int posY);
void hideCursor();
void delay(int milliseconds);
void drawBigTitle();
int drawMainMenu();
void drawHighScores(char *filename);
void drawHUD(char *playerName, int currentHP, int maxHP, int currentScore, int currentLevel, int remainingBugs);
void drawBattleInterface(char *name, int hp, int maxHp, int enemyHp, int enemyMax);
void drawMonster(int monsterType);
void drawGameOver(int finalScore);
void drawWinScreen(int finalScore, char *playerName);
void flashMessage(char *message, int colorCode);

// Fungsi Logika Game
void initGame();
void nextLevel();
void generateMap();
void renderGame(); 
void movePlayer(char inputDirection);
void battle(int enemyType);
void saveGame();
int solveQuestion(int difficultyLevel);
int handleGameOver(); 


// MAIN PROGRAM
int main() {
    srand(time(0));
    hideCursor(); 
    
    // LOOP MENU UTAMA
    while(1) {
        int menuChoice = drawMainMenu();
        
        if (menuChoice == 1) { // Leaderboard
            drawHighScores(FILE_SCORE);
            continue; 
        }
        else if (menuChoice == 2) { // Exit
            system("cls");
            printf("Shutting down...\n");
            exit(0);
        }
        else { // Start Game
            break; 
        }
    }

    system("cls");
    drawBigTitle();
    printf("\n\tMasukkan Nama User: ");
    setColor(C_YELLOW);
    
    fflush(stdin);
    fgets(mainPlayer.name, sizeof(mainPlayer.name), stdin);
    mainPlayer.name[strcspn(mainPlayer.name, "\n")] = 0;
    
    int isAppRunning = 1;
    while(isAppRunning) {
        initGame(); 
        nextLevel(); 

        system("cls"); 
        renderGame();

        int isGameRunning = 1;
        while (isGameRunning) {
            
            if (mainPlayer.currentHealth <= 0) {
                saveGame();
                if (handleGameOver()) isGameRunning = 0; 
                else { isAppRunning = 0; isGameRunning = 0; }
                break;
            }

            char playerInput = getch(); 
            if (playerInput == 'q' || playerInput == 'Q') {
                saveGame();
                isAppRunning = 0;
                break;
            } else {
                movePlayer(playerInput);
                renderGame(); 
            }
        }
    }
    return 0;
}


// IMPLEMENTASI FUNGSI DESAIN (UI/UX)

void setColor(int colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
}

void gotoxy(int posX, int posY) {
    COORD coordinates;
    coordinates.X = posX;
    coordinates.Y = posY;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinates);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE; 
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void delay(int milliseconds) {
    Sleep(milliseconds);
}

void drawBigTitle() {
    system("cls");

    setColor(C_CYAN);
    printf("\n\n");
    printf("\t   _______  _______  ______   _______\n");
    printf("\t  |       ||       ||      | |       |\n");
    printf("\t  |       ||   _   ||  _   ||    ___|\n");
    printf("\t  |       ||  | |  || | |  ||   |___ \n");
    printf("\t  |      _||  |_|  || |_|  ||    ___|\n");
    printf("\t  |     |_ |       ||      ||   |___ \n");
    printf("\t  |_______||_______||______| |_______|\n");

    printf("\n");

    setColor(C_YELLOW);
    printf("\t __   __  _______  ______    ______    ___   _______  ______   \n");
    printf("\t|  | |  ||       ||    _ |  |    _ |  |   | |       ||    _ |  \n");
    printf("\t|  |_|  ||   _   ||   | ||  |   | ||  |   | |   _   ||   | ||  \n");
    printf("\t|       ||  |_|  ||   |_||_ |   |_||_ |   | |  | |  ||   |_||_ \n");
    printf("\t|       ||       ||    __  ||    __  ||   | |  |_|  ||    __  |\n");
    printf("\t|   _   ||   _   ||   |  | ||   |  | ||   | |       ||   |  | |\n");
    printf("\t|__| |__||__| |__||___|  |_||___|  |_||___| |_______||___|  |_|\n");

    printf("\n");
    setColor(C_GREY);
    printf("\t      [ THE LOGIC LABYRINTH: MATH EDITION ]\n");
}

int drawMainMenu() {
    int currentSelection = 0;
    while(1) {
        drawBigTitle();
        
        printf("\n\n");
        // Opsi 1
        if(currentSelection == 0) { setColor(C_GREEN); printf("\t      >> INITIALIZE SYSTEM (Start Game) <<\n"); }
        else { setColor(C_GREY); printf("\t         INITIALIZE SYSTEM (Start Game)    \n"); }
        
        // Opsi 2
        if(currentSelection == 1) { setColor(C_PINK); printf("\t      >> VIEW DATABASE (Leaderboard) <<\n"); }
        else { setColor(C_GREY); printf("\t         VIEW DATABASE (Leaderboard)    \n"); }
        
        // Opsi 3
        if(currentSelection == 2) { setColor(C_RED); printf("\t      >> TERMINATE CONNECTION (End Game) <<\n"); }
        else { setColor(C_GREY); printf("\t         TERMINATE CONNECTION (End Game)    \n"); }
        
        // NAVIGASI 
        printf("\n\n");
        setColor(C_GREY);
        printf("\t   [ NAVIGATION: 'W' UP | 'S' DOWN | 'ENTER' SELECT ]\n");
        
        char inputKey = getch();
        if(inputKey == 'w' || inputKey == 'W') { currentSelection--; if(currentSelection < 0) currentSelection = 2; }
        if(inputKey == 's' || inputKey == 'S') { currentSelection++; if(currentSelection > 2) currentSelection = 0; }
        if(inputKey == 13) return currentSelection; 
    }
}

void drawHighScores(char *filename) {
    system("cls");
    setColor(C_PINK);
    printf("\n\n");
    printf("\t   [ TOP RANKED HACKERS ]\n");
    printf("\t   ======================\n\n");
    
    FILE *fileData = fopen(filename, "r");
    char lineBuffer[255];
    
    if(fileData == NULL) {
        setColor(C_RED);
        printf("\t   Database Empty or Not Found.\n");
    } else {
        setColor(C_WHITE);
        int rankCounter = 1;
        while(fgets(lineBuffer, 255, fileData)) {
            printf("\t   %d. %s", rankCounter++, lineBuffer);
            if(rankCounter > 10) break; 
        }
        fclose(fileData);
    }
    
    printf("\n\n");
    setColor(C_YELLOW);
    printf("\t   PRESS ANY KEY TO RETURN...");
    getch();
}

void drawHUD(char *playerName, int currentHP, int maxHP, int currentScore, 
    int currentLevel, int remainingBugs) {
    gotoxy(0, 0); 
    
    setColor(C_GREEN); printf(" USER: "); 
    setColor(C_YELLOW); printf("%-10s", playerName);
    
    printf(" | ");
    setColor(C_RED); printf("HP: ");
    int bars = currentHP / 10;
    for(int i = 0; i < 10; i++) {
        if(i < bars) { setColor(C_RED); printf("%c", 219); } 
        else { setColor(C_GREY); printf("."); } 
    }
    setColor(C_RED); printf(" %d%%", currentHP);

    printf(" | ");
    setColor(C_PINK); printf("DATA: %-4d MB", currentScore); 
    
    printf(" | ");
    setColor(C_CYAN); printf("LAYER: %d/8", currentLevel);

    printf(" | ");
    setColor(C_RED); printf("BUGS LEFT: %d ", remainingBugs);
    
    setColor(C_BLACK); printf("          "); 

    gotoxy(0, 1);
    setColor(C_WHITE);
    for(int i = 0; i < 78; i++) printf("="); 
    printf("\n");
}

// UI KHUSUS BATTLE (DI ATAS LAYAR)
void drawBattleInterface(char *name, int hp, int maxHp, int enemyHp, int enemyMax) {
    gotoxy(0, 0);
    setColor(C_CYAN);
    printf("==============================================================================\n");
    printf("|| "); 
    
    // Tampilkan Nama & HP Pemain
    setColor(C_GREEN); printf("USER: %-10s", name);
    setColor(C_WHITE); printf(" | ");
    
    setColor(C_RED); printf("HP: ");
    int bars = (hp * 10) / maxHp; 
    if (bars < 0) bars = 0;
    if (bars > 10) bars = 10;
    
    for(int i=0; i<10; i++) {
        if(i < bars) { setColor(C_RED); printf("%c", 219); } 
        else { setColor(C_GREY); printf("%c", 176); }        
    }
    setColor(C_WHITE); printf(" %d/%d", hp, maxHp);

    // Tampilkan Status Musuh
    printf("     "); 
    setColor(C_WHITE); printf("| ");
    setColor(C_PINK);  printf("BUGS: ");
    
    for(int i=0; i<enemyMax; i++) {
        if(i < enemyHp) { setColor(C_PINK); printf("O "); } 
        else { setColor(C_GREY); printf("X "); }            
    }
    
    setColor(C_CYAN);
    printf("\n==============================================================================\n");
}

void drawMonster(int monsterType) {
    setColor(C_RED); printf("\n");
    if (monsterType == 1) { 
        printf("\t    /\\__/\\ \n");
        printf("\t   (  !!  )  [BUG DETECTED]\n");
    } 
    else if (monsterType == 3) { 
        printf("\t    ( .  . ) \n");
        printf("\t   (  [__]  )  [BOSS: SEGMENTATION FAULT]\n");
    }
    setColor(C_NORMAL);
}

void drawGameOver(int score) {
    system("cls");
    setColor(C_RED);
    printf("\n\n");
    printf("\t  ___   _   __  __ ___   \n");
    printf("\t / __| /_\\ |  \\/  | __|  \n");
    printf("\t| (_ |/ _ \\| |\\/| | _|   \n");
    printf("\t \\___/_/ \\_\\_|  |_|___|  \n");
    printf("\t                         \n");
    printf("\t  _____   _____ ___      \n");
    printf("\t / _ \\ \\ / / __| _ \\     \n");
    printf("\t| (_) \\ V /| _||   /     \n");
    printf("\t \\___/ \\_/ |___|_|_\\     \n");
    
    printf("\n\t [ SYSTEM CRASHED ]\n");
    printf("\t ------------------\n");
    
    setColor(C_WHITE);
    printf("\t MEMORY DUMP COMPLETE.\n");
    printf("\t FINAL SCORE: %d\n\n", score);
    setColor(C_GREY);
    printf("\t [Y] RESTART SYSTEM\n");
    printf("\t [N] POWER OFF\n");
}

void drawWinScreen(int finalScore, char *playerName) {
    system("cls");
    setColor(C_GREEN);
    printf("\n\n");
    printf("\t         ___________\n");
    printf("\t        '._==_==_=_.'\n");
    printf("\t        .-\\:      /-.\n");
    printf("\t       | (|:.     |) |\n");
    printf("\t        '-|:.     |-'\n");
    printf("\t          \\::.    /\n");
    printf("\t           '::. .'\n");
    printf("\t             ) (\n");
    printf("\t           _.' '._\n");
    printf("\t          `\"\"\"\"\"\"\"`\n");
    
    printf("\n\t [ SYSTEM RESTORED SUCCESSFULLY ]\n");
    printf("\t --------------------------------\n");
    
    setColor(C_CYAN);
    printf("\t CONGRATULATIONS, %s!\n", playerName);
    setColor(C_WHITE);
    printf("\t MALWARE REMOVED. DATA SAVED.\n");
    printf("\t FINAL SCORE: %d\n\n", finalScore);
    
    setColor(C_YELLOW);
    printf("\t PRESS ENTER TO EXIT...");
    getch();
}

void flashMessage(char *message, int colorCode) {
    int yPosition = MAP_SIZE + 6; 
    gotoxy(5, yPosition); 
    setColor(colorCode);
    printf(">> %-40s", message); 
    delay(800); 
    gotoxy(5, yPosition);
    setColor(C_BLACK);
    printf("                                        "); 
}


// IMPLEMENTASI FUNGSI LOGIKA

void initGame() {
    mainPlayer.currentHealth = 100;
    mainPlayer.maxHealth = 100;
    mainPlayer.score = 0;
    mainPlayer.currentLevel = 0; 
}

void generateMap() {
    enemiesRemaining = 0;
    
    // 1. Reset Map
    for(int row = 0; row < MAP_SIZE; row++) {
        for(int col = 0; col < MAP_SIZE; col++) {
            mineGrid[row][col] = 0;
            if (row == 0 || row == MAP_SIZE-1 || col == 0 || col == MAP_SIZE-1) areaMap[row][col] = '#'; 
            else areaMap[row][col] = '.'; 
        }
    }

    // 2. Tembok dengan SAFE ZONE
    int wallDensity = 15 + (mainPlayer.currentLevel * 2); 
    if (wallDensity > 25) wallDensity = 25; 

    for(int row = 1; row < MAP_SIZE-1; row++) {
        for(int col = 1; col < MAP_SIZE-1; col++) {
            if (row <= 2 && col <= 2) continue; // Safe zone

            if ((rand() % 100) < wallDensity) {
                areaMap[row][col] = '#';
            }
        }
    }

    // 3. Spawn Musuh
    int totalEnemies = 3 + (mainPlayer.currentLevel / 2);
    for(int k = 0; k < totalEnemies; k++) {
        int enemyX, enemyY;
        do { 
            enemyX = rand() % (MAP_SIZE-2) + 1; 
            enemyY = rand() % (MAP_SIZE-2) + 1; 
        } while(areaMap[enemyY][enemyX] != '.' || (enemyX <= 2 && enemyY <= 2)); 
        
        areaMap[enemyY][enemyX] = 'E';
        enemiesRemaining++;
    }

    // 4. Spawn Boss
    int bossX, bossY;
    do { 
        bossX = rand() % (MAP_SIZE-2) + 1; 
        bossY = rand() % (MAP_SIZE-2) + 1; 
    } while(areaMap[bossY][bossX] != '.' || (bossX <= 3 && bossY <= 3));
    areaMap[bossY][bossX] = 'B';

    // 5. Spawn Ranjau
    int totalMines = 2 + (mainPlayer.currentLevel / 4); 
    for(int k = 0; k < totalMines; k++) {
        int trapX, trapY;
        do {
            trapX = rand() % (MAP_SIZE-2) + 1; 
            trapY = rand() % (MAP_SIZE-2) + 1;
        } while(areaMap[trapY][trapX] != '.' && areaMap[trapY][trapX] != 'E' && areaMap[trapY][trapX] != 'B'); 
        
        if(areaMap[trapY][trapX] == '.') mineGrid[trapY][trapX] = 1; 
    }

    // 6. Spawn Heal
    if (mainPlayer.currentLevel >= 2) {
        int healX, healY;
        do { healX = rand()%(MAP_SIZE-2)+1; healY = rand()%(MAP_SIZE-2)+1; } while(areaMap[healY][healX] != '.');
        areaMap[healY][healX] = '+';
    }
}

void renderGame() {
    gotoxy(0, 0); 
    drawHUD(mainPlayer.name, mainPlayer.currentHealth, mainPlayer.maxHealth, mainPlayer.score, mainPlayer.currentLevel, enemiesRemaining);
    
    for(int row = 0; row < MAP_SIZE; row++) {
        gotoxy(5, row + 3); 
        for(int col = 0; col < MAP_SIZE; col++) {
            if (row == mainPlayer.posY && col == mainPlayer.posX) {
                setColor(C_CYAN); 
                printf("%c ", mainPlayer.name[0]); 
            } else if (areaMap[row][col] == '#') {
                setColor(C_GREY); printf("%c%c", 219, 219); 
            } else if (areaMap[row][col] == 'E') {
                setColor(C_RED); printf("! "); 
            } else if (areaMap[row][col] == 'B') {
                setColor(C_PINK); printf("B "); 
            } else if (areaMap[row][col] == '+') {
                setColor(C_GREEN); printf("P "); 
            } else {
                setColor(C_WHITE); printf(". "); 
            }
        }
        setColor(C_BLACK); printf("                    "); 
    }
    
    // LEGENDA
    int yPosition = MAP_SIZE + 4;
    gotoxy(5, yPosition);
    setColor(C_WHITE); printf("LEGEND: ");
    
    setColor(C_CYAN); printf("%c", mainPlayer.name[0]); setColor(C_GREY); printf(":You  ");
    setColor(C_RED);  printf("!"); setColor(C_GREY); printf(":Bug  ");
    setColor(C_PINK); printf("B"); setColor(C_GREY); printf(":Boss  ");
    setColor(C_GREEN);printf("P"); setColor(C_GREY); printf(":Heal");

    gotoxy(5, yPosition + 1);
    setColor(C_WHITE);
    printf("CONTROLS: [WASD] MOVE  [Q] ABORT          "); 
}

void nextLevel() {
    // Jika level sudah maksimal (Tamat)
    if (mainPlayer.currentLevel >= MAX_LEVEL) {
        saveGame();
        drawWinScreen(mainPlayer.score, mainPlayer.name);
        exit(0);
    }

    mainPlayer.currentLevel++;
    system("cls"); 
    // Animasi Loading
    printf("\n\n\tACCESSING LAYER %d...\n", mainPlayer.currentLevel);
    printf("\t[");
    for(int i = 0; i < 20; i++) { printf("#"); delay(30); }
    printf("]");
    delay(500);

    mainPlayer.posX = 1; mainPlayer.posY = 1;
    generateMap();
    system("cls");
    renderGame();
}

void movePlayer(char inputDirection) {
    int moveX = 0, moveY = 0;
    switch(inputDirection) {
        case 'w': case 'W': moveY = -1; break;
        case 's': case 'S': moveY = 1; break;
        case 'a': case 'A': moveX = -1; break;
        case 'd': case 'D': moveX = 1; break;
        default: return; 
    }
    
    int targetX = mainPlayer.posX + moveX;
    int targetY = mainPlayer.posY + moveY;
    char targetTile = areaMap[targetY][targetX];

    if (targetTile == '#') return; 

    mainPlayer.posX = targetX; mainPlayer.posY = targetY;

    if (mineGrid[targetY][targetX] == 1) {
        renderGame(); 
        flashMessage("FIREWALL TRAP! HP -15", C_RED);
        mainPlayer.currentHealth -= 15;
        mineGrid[targetY][targetX] = 0; 
        return; 
    }

    if (targetTile == 'E') {
        battle(1); 
        areaMap[targetY][targetX] = '.'; 
        enemiesRemaining--;
        system("cls"); renderGame();  
    } 
    else if (targetTile == 'B') {
        if (enemiesRemaining > 0) {
            flashMessage("LOCKED! DEFEAT BUGS FIRST!", C_YELLOW);
            mainPlayer.posX -= moveX; mainPlayer.posY -= moveY; 
        } else {
            battle(3); 
            if (mainPlayer.currentHealth > 0) {
                areaMap[targetY][targetX] = '.';
                system("cls");
                printf("\n\n\tLAYER %d SECURED.\n", mainPlayer.currentLevel);
                delay(1000);
                nextLevel();
            }
            system("cls"); renderGame();
        }
    } 
    else if (targetTile == '+') {
        flashMessage("RECOVERY! HP +30", C_GREEN);
        mainPlayer.currentHealth += 30;
        if(mainPlayer.currentHealth > mainPlayer.maxHealth) mainPlayer.currentHealth = mainPlayer.maxHealth;
        areaMap[targetY][targetX] = '.'; 
    }
}

// LOGIKA SOAL 
int solveQuestion(int difficultyLevel) {
    int correctAnswer, playerInputAnswer = -999;
    int operandOne, operandTwo;
    int gameMode; 
    

    printf("\n\n");
    
    setColor(C_CYAN);
    printf("\t [ LOGIC GATE: LEVEL %d ]\n", difficultyLevel);
    printf("\t ----------------------\n");
    setColor(C_WHITE);

    // LEVEL 1-2
    if (difficultyLevel <= 2) { 
        operandOne = (rand() % 51) - 20; 
        operandTwo = (rand() % 51) - 20;
        if(rand() % 2 == 0) { 
            printf("\t SOLVE: %d + %d       \n\n", operandOne, operandTwo); 
            correctAnswer = operandOne + operandTwo; 
        } else { 
            printf("\t SOLVE: %d - %d       \n\n", operandOne, operandTwo); 
            correctAnswer = operandOne - operandTwo; 
        }
    } 
    // LEVEL 3-4
    else if (difficultyLevel <= 4) {
        if(rand() % 2 == 0) { 
            operandOne = (rand() % 23) - 10; 
            operandTwo = (rand() % 10) + 1; 
            printf("\t SOLVE: %d * %d       \n\n", operandOne, operandTwo); 
            correctAnswer = operandOne * operandTwo; 
        } 
        else { 
            operandTwo = (rand() % 9) + 2;     
            correctAnswer = (rand() % 21) - 10; 
            operandOne = operandTwo * correctAnswer; 
            printf("\t SOLVE: %d / %d       \n\n", operandOne, operandTwo); 
        }
    }
    // LEVEL 5-6
    else if (difficultyLevel <= 6) {
        int startValue = (rand() % 21) - 10; 
        int difference = (rand() % 6) + 2;    
        if (rand() % 2 == 0) {
            printf("\t [ARITHMETIC SEQUENCE]\n");
            printf("\t %d, %d, %d, ... (Suku ke-4?)\n\n", startValue, startValue+difference, startValue+(difference*2));
            correctAnswer = startValue + (difference*3);
        } else {
            startValue = (rand() % 5) + 1;
            difference = (rand() % 3) + 2;
            printf("\t [GEOMETRIC SEQUENCE]\n");
            printf("\t %d, %d, %d, ... (Suku ke-4?)\n\n", startValue, startValue*difference, startValue*difference*difference);
            correctAnswer = startValue * difference * difference * difference;
        }
    }
    // LEVEL 7-8
    else {
        int decimalVal = (rand() % 31) + 1;
        gameMode = rand() % 2; 
        int binaryVisual = 0, remainder, place = 1, tempVal = decimalVal;
        while(tempVal > 0) { 
            remainder = tempVal % 2; binaryVisual = binaryVisual + (remainder * place); 
            tempVal /= 2; place *= 10; 
        }
        if (gameMode == 0) {
            printf("\t [BINARY DECRYPTION]\n");
            printf("\t Convert Binary: %d -> Decimal ?\n\n", binaryVisual);
            correctAnswer = decimalVal;
        } else {
            printf("\t [BINARY ENCRYPTION]\n");
            printf("\t Convert Decimal: %d -> Binary ?\n", decimalVal);
            printf("\t (Input example: 101)\n\n");
            correctAnswer = binaryVisual; 
        }
    }
    
    printf("\t ANSWER: ");
    printf("             \b\b\b\b\b\b\b\b\b\b\b\b\b"); 
    
    char buffer[100]; 
    fflush(stdin);    
    
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%d", &playerInputAnswer) != 1) {
            playerInputAnswer = -999999; 
        }
    } else {
        playerInputAnswer = -999999; 
    } 
    return (playerInputAnswer == correctAnswer);
}

// SIMULASI BATTLE DENGAN UI
void battle(int enemyType) {
    int enemyHealth, maxEnemyHealth;
    
    // Setup Nyawa Musuh
    if (enemyType == 3) {
        enemyHealth = 5 + (mainPlayer.currentLevel / 2); // Boss
        maxEnemyHealth = enemyHealth;
    } else {
        enemyHealth = 3; // Musuh Biasa
        maxEnemyHealth = 3;
    }
    
    // Intro Musuh 
    system("cls");
    drawMonster(enemyType); 
    setColor(C_RED);
    printf("\n\t [ BUG INTEGRITY: %d ]\n", enemyHealth);
    setColor(C_WHITE);
    printf("\t Tekan sembarang tombol untuk hack...\n");
    getch(); 
    
    // LOOP BATTLE
    while(enemyHealth > 0 && mainPlayer.currentHealth > 0) {
        system("cls"); 
        
        // 1. GAMBAR UI ATAS 
        drawBattleInterface(mainPlayer.name, mainPlayer.currentHealth, mainPlayer.maxHealth, enemyHealth, maxEnemyHealth);
        
        // 2. GAMBAR MONSTER 
        drawMonster(enemyType);
        
        // 3. TAMPILKAN SOAL 
        if (solveQuestion(mainPlayer.currentLevel)) {
            //  JIKA BENAR 
            flashMessage("CODE FIXED! (+100 PTS)", C_GREEN);
            mainPlayer.score += 100;
            if(enemyHealth == 1) mainPlayer.score += 500; 
            enemyHealth--;
        } else {
            // JIKA SALAH 
            int damage = 10 + mainPlayer.currentLevel;
            
            // Kurangi HP
            mainPlayer.currentHealth -= damage;
            if (mainPlayer.currentHealth < 0) mainPlayer.currentHealth = 0;
            
            drawBattleInterface(mainPlayer.name, mainPlayer.currentHealth, mainPlayer.maxHealth, enemyHealth, maxEnemyHealth);
            
            // Tampilkan Pesan Error
            char msgBuffer[100]; 
            sprintf(msgBuffer, "ERROR! HP -%d | SYSTEM UNSTABLE!", damage);
            flashMessage(msgBuffer, C_RED);
        }
    }
}

int handleGameOver() {
    drawGameOver(mainPlayer.score);
    while(1) {
        char playerChoice = getch();
        if (playerChoice == 'y' || playerChoice == 'Y') return 1; 
        else if (playerChoice == 'n' || playerChoice == 'N') return 0; 
    }
}

void saveGame() {
    FILE *fileData = fopen(FILE_SCORE, "a");
    if(fileData) {
        fprintf(fileData, "%s | Lvl: %d | Score: %d\n", mainPlayer.name, mainPlayer
            .currentLevel, mainPlayer.score);
        fclose(fileData);
    }
}