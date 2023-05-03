
// Inclui a biblioteca "pitches.h" para utilizar as notas musicais
#include "pitches.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

/* Constantes - define os números dos pinos dos LEDs,
botões, do speaker e também as notas do jogo: */

const byte ledPins[] = {2,4,6,8};
const byte buttonPins[] = {3, 5,7,9};
#define SPEAKER_PIN 10

#define MAX_GAME_LENGTH 100

const int gameTones[] = { NOTE_G3, NOTE_C4, NOTE_E4, NOTE_G5};

/* Variáveis globais - armazenam o estado do jogo */
byte gameSequence[MAX_GAME_LENGTH] = {0};
byte gameIndex = 0;
byte recorde = 0;

/**
   Configura a placa Arduino e inicializa a comunicação serial
*/
void setup() {
  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  //lcd.noBacklight(); // DESLIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY
  
  Serial.begin(9600);
  
  for (byte i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  pinMode(SPEAKER_PIN, OUTPUT);
  // A linha seguinte prepara o gerador de números aleatórios.
 // Ela assume que o pino A0 está flutuante (desconectado):
  randomSeed(analogRead(A0));
}

/**
 Acende o LED indicado e toca uma nota correspondente
*/
void lightLedAndPlayTone(byte ledIndex) {
  digitalWrite(ledPins[ledIndex], HIGH);
  tone(SPEAKER_PIN, gameTones[ledIndex]);
  delay(300);
  digitalWrite(ledPins[ledIndex], LOW);
  noTone(SPEAKER_PIN);
}

/**
   Toca a sequência atual de notas que o usuário deve repetir
*/
void playSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte currentLed = gameSequence[i];
    lightLedAndPlayTone(currentLed);
    delay(50);
  }
}

/**
   Espera até que o usuário pressione um dos botões,
e retorna o índice desse botãon
*/
byte readButtons() {
  while (true) {
    for (byte i = 0; i < 4; i++) {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW) {
        return i;
      }
    }
    delay(1);
  }
}

/**
  Toca a sequência de game over e exibe a pontuação do jogador
*/
void gameOver() {

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Fim de jogo");
  lcd.setCursor(0, 1);
  lcd.print("Placar:");
  lcd.setCursor(8, 1);
  lcd.print(gameIndex - 1);
  lcd.setCursor(10, 1);
  lcd.print("pontos");
  
  Serial.print("Fim de jogo! Sua Pontuação: ");
  Serial.println(gameIndex - 1);
  gameIndex = 0;
  delay(300);
/**
  Toca uma sequência de notas para indicar o fim do jogo
*/
  tone(SPEAKER_PIN, NOTE_DS5);
  delay(300);
  tone(SPEAKER_PIN, NOTE_D5);
  delay(300);
  tone(SPEAKER_PIN, NOTE_CS5);
  delay(300);
  for (byte i = 0; i < 10; i++) {
    for (int pitch = -10; pitch <= 10; pitch++) {
      tone(SPEAKER_PIN, NOTE_C5 + pitch);
      delay(5);
    }
  }
  noTone(SPEAKER_PIN);
  delay(500);
}

/**
   Obtém a entrada do usuário e compara com a sequência esperada
*/
bool checkUserSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte expectedButton = gameSequence[i];
    byte actualButton = readButtons();
    lightLedAndPlayTone(actualButton);
    if (expectedButton != actualButton) {
      return false;
    }
  }

  return true;
}

/**
 Reproduz um som de vitória sempre que o usuário termina um nível  
*/
void playLevelUpSound() {
  tone(SPEAKER_PIN, NOTE_E4);
  delay(150);
  tone(SPEAKER_PIN, NOTE_G4);
  delay(150);
  tone(SPEAKER_PIN, NOTE_E5);
  delay(150);
  tone(SPEAKER_PIN, NOTE_C5);
  delay(150);
  tone(SPEAKER_PIN, NOTE_D5);
  delay(150);
  tone(SPEAKER_PIN, NOTE_G5);
  delay(150);
  noTone(SPEAKER_PIN);
}

/**
   Loop principal do jogo
*/
void loop() {
  if (gameIndex > recorde) {
    recorde = gameIndex;
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pontos");
  lcd.setCursor(0, 1);
  lcd.print(gameIndex);
  
  lcd.setCursor(7, 0);
  lcd.print("|");
  lcd.setCursor(7, 1);
  lcd.print("|");
  
  lcd.setCursor(9, 0);
  lcd.print("Recorde");
  lcd.setCursor(9, 1);
  lcd.print(recorde);
  
  // Add a random color to the end of the sequence
  gameSequence[gameIndex] = random(0, 4);
  gameIndex++;
  if (gameIndex >= MAX_GAME_LENGTH) {
    gameIndex = MAX_GAME_LENGTH - 1;
  }

  playSequence();
  if (!checkUserSequence()) {
    gameOver();
  }

  delay(300);

  if (gameIndex > 0) {
    // Pontuação atual, depois transferir para lcd
    Serial.println(gameIndex);
    playLevelUpSound();
    delay(300);
  }
}
