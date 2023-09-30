#include <Arduboy2.h>
#include <ArduboyTones.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

enum GameState { LOADING, PLAYING, GAME_WON };
GameState gameState;

int targetNumber;
int playerGuess = 501;
int triesLeft = 10;
bool showFeedback = false;
String feedbackMessage = "";

int guessHistory[10];
int historyIndex = 0;

unsigned long lastUpdateLeftRight = 0;
unsigned long lastUpdateUpDown = 0;
int delayLeftRight = 300;
int delayUpDown = 300;

void displayMessage(int x, int y, const char* message) {
  arduboy.setCursor(x, y);
  arduboy.print(message);
}

void displayHistory() {
  int startIdx = max(0, historyIndex - 3);
  for (int i = startIdx; i < historyIndex; i++) {
    arduboy.setCursor(110, (i - startIdx) * 10);
    arduboy.print(guessHistory[i]);
  }
}

void resetGame() {
  triesLeft = 10;
  targetNumber = random(0, 1001);
  gameState = PLAYING;
  showFeedback = false;
  historyIndex = 0;
}

void setup() {
  arduboy.begin();
  unsigned long seed = analogRead(0) * micros();
  randomSeed(seed);
  gameState = LOADING;
}

void loop() {
  arduboy.clear();
  arduboy.pollButtons();
  unsigned long currentTime = millis();

  switch (gameState) {
    case LOADING:
      displayMessage(0, 0, "Guess the Number");
      displayMessage(0, 10, "Press A to start");
      displayMessage(0, 20, "Number is 0-1000");
      displayMessage(0, 30, "L/R: +/- 1");
      displayMessage(0, 40, "U/D: +/- 10");
      if (arduboy.justPressed(A_BUTTON)) {
        resetGame();
      }
      break;

    case PLAYING:
      displayMessage(0, 0, "Tries left: ");
      arduboy.print(triesLeft);
      displayMessage(0, 10, "Your guess: ");
      arduboy.print(playerGuess);
      displayHistory();
      
      if (!showFeedback) {
        if (arduboy.pressed(LEFT_BUTTON) || arduboy.pressed(RIGHT_BUTTON)) {
          if (currentTime - lastUpdateLeftRight > delayLeftRight) {
            playerGuess += arduboy.pressed(RIGHT_BUTTON) ? 1 : -1;
            playerGuess = constrain(playerGuess, 0, 1000);
            lastUpdateLeftRight = currentTime;
            delayLeftRight = max(50, delayLeftRight - 10);
            sound.tone(1000, 50);
          }
        } else {
          delayLeftRight = 300;
        }

        if (arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON)) {
          if (currentTime - lastUpdateUpDown > delayUpDown) {
            playerGuess += arduboy.pressed(UP_BUTTON) ? 10 : -10;
            playerGuess = constrain(playerGuess, 0, 1000);
            lastUpdateUpDown = currentTime;
            delayUpDown = max(50, delayUpDown - 10);
            sound.tone(1000, 50);
          }
        } else {
          delayUpDown = 300;
        }

        if (arduboy.justPressed(A_BUTTON)) {
          if (historyIndex < 10) {
            guessHistory[historyIndex++] = playerGuess;
          }
          triesLeft--;
          if (playerGuess == targetNumber) {
            feedbackMessage = "You guessed it!";
            gameState = GAME_WON;
          } else {
            feedbackMessage = (playerGuess < targetNumber) ? "Too small!" : "Too big!";
          }
          showFeedback = true;
        }
      } else {
        displayMessage(0, 40, feedbackMessage.c_str());
        if (arduboy.justPressed(A_BUTTON)) {
          showFeedback = false;
        }
      }

      if (triesLeft == 0 && !showFeedback) {
        displayMessage(0, 50, "Game Over! Press B");
        if (arduboy.justPressed(B_BUTTON)) {
          resetGame();
        }
      }
      break;

    case GAME_WON:
      displayMessage(0, 25, "Winner!");
      displayMessage(0, 50, "Press B to continue");
      if (arduboy.justPressed(B_BUTTON)) {
        resetGame();
      }
      break;
  }

  arduboy.display();
}
