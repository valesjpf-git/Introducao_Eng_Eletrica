#include <LiquidCrystal.h>

// ===== PINOS (ajuste se o seu circuito for diferente) =====
const int PIN_TEMP_AMB  = A0;  // sensor LM35 do ambiente
const int PIN_TEMP_GELO = A1;  // sensor LM35 da camara com gelo
const int PIN_VENTOINHA = 9;   // PWM -> transistor -> ventoinha
const int PIN_BTN_MAIS  = 7;   // acelera a ventoinha (resfria mais)
const int PIN_BTN_MENOS = 8;   // desacelera a ventoinha
const int PIN_ALERTA    = 13;  // LED/buzzer de aviso

// LCD 16x2 (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ===== PARAMETROS =====
const float LIMITE_GELO = 10.0;  // acima disso, o resfriante esta acabando (C)
const int   PASSO       = 25;    // variacao de velocidade por clique (0-255)
const unsigned long INTERVALO_LCD = 500;  // ms entre atualizacoes da tela
const unsigned long DEBOUNCE      = 200;  // ms entre cliques validos

int velocidade = 0;
unsigned long ultimoLCD = 0;
unsigned long ultimoClique = 0;

float lerTemperatura(int pino) {
  // LM35: 10 mV por grau C, referencia de 5 V, ADC de 10 bits
  int leitura = analogRead(pino);
  return leitura * (5.0 / 1023.0) * 100.0;
}

void setup() {
  pinMode(PIN_VENTOINHA, OUTPUT);
  pinMode(PIN_ALERTA, OUTPUT);
  pinMode(PIN_BTN_MAIS, INPUT_PULLUP);   // botao ligado ao GND
  pinMode(PIN_BTN_MENOS, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.print("Iniciando...");
  delay(1000);
  lcd.clear();
}

void loop() {
  // --- Botoes ---
  if (millis() - ultimoClique > DEBOUNCE) {
    if (digitalRead(PIN_BTN_MAIS) == LOW) {
      velocidade = constrain(velocidade + PASSO, 0, 255);
      ultimoClique = millis();
    } else if (digitalRead(PIN_BTN_MENOS) == LOW) {
      velocidade = constrain(velocidade - PASSO, 0, 255);
      ultimoClique = millis();
    }
  }
  analogWrite(PIN_VENTOINHA, velocidade);

  // --- Leituras e tela ---
  if (millis() - ultimoLCD >= INTERVALO_LCD) {
    ultimoLCD = millis();

    float tAmb  = lerTemperatura(PIN_TEMP_AMB);
    float tGelo = lerTemperatura(PIN_TEMP_GELO);
    int percentual = map(velocidade, 0, 255, 0, 100);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Amb:");
    lcd.print(tAmb, 1);
    lcd.print("C ");
    lcd.print(percentual);
    lcd.print("%");

    lcd.setCursor(0, 1);
    if (tGelo > LIMITE_GELO) {
      lcd.print("TROCAR GELO!");
      digitalWrite(PIN_ALERTA, HIGH);
    } else {
      lcd.print("Gelo:");
      lcd.print(tGelo, 1);
      lcd.print("C OK");
      digitalWrite(PIN_ALERTA, LOW);
    }
  }
}
