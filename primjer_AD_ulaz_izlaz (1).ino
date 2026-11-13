/* =========================================================
   1. PRETPROCESORSKI BLOK (#define)
   =========================================================
   Definiranje fizičkih priključaka mikroupravljača.
   Ne zauzima radnu memoriju (SRAM) kontrolera.
*/
#define PIN_TIPKALO 2
#define PIN_POT A0
#define PIN_LED_PWM 9
#define PIN_LED_IND 8

/* =========================================================
   2. BLOK VARIJABLI
   =========================================================
   Deklaracija memorijskih varijabli u radnoj memoriji.
*/
int stanjeTipkala;
int zadnjeStanjeTipkala = 1;
int vrijednostPot;
int pwmIzlaz;
bool sustavUkljucen = false;

/* =========================================================
   3. FUNKCIJA SETUP (Početno konfiguriranje sustava)
   =========================================================
   Izvodi se točno jednom pri uključenju napajanja.
*/
void setup() {
  // Pokretanje serijske komunikacije brzinom 9600 bauda
  Serial.begin(9600);

  // Konfiguracija ulaza uz interni pull-up otpornik
  pinMode(PIN_TIPKALO, INPUT_PULLUP);
  pinMode(PIN_POT, INPUT);

  // Konfiguracija digitalnih izlaza
  pinMode(PIN_LED_PWM, OUTPUT);
  pinMode(PIN_LED_IND, OUTPUT);

  // Postavljanje početnog sigurnosnog nultog stanja
  analogWrite(PIN_LED_PWM, 0);
  digitalWrite(PIN_LED_IND, LOW);

  Serial.println("Sustav ugradbene regulacije spreman.");
}

/* =========================================================
   4. FUNKCIJA LOOP (Glavni radni ciklus)
   =========================================================
   Izvodi se neprekidno u beskonačnoj petlji u stvarnom vremenu.
*/
void loop() {
  // Očitavanje trenutnih stanja senzora i ulaza
  stanjeTipkala = digitalRead(PIN_TIPKALO);
  vrijednostPot = analogRead(PIN_POT);

  // Detekcija silaznog brida (prijelaz s HIGH na LOW)
  if (stanjeTipkala == LOW && zadnjeStanjeTipkala == HIGH) {
    sustavUkljucen = !sustavUkljucen; // Preokretanje stanja
    delay(50);                        // Debouncing zadrška za kontaktne smetnje
  }
  zadnjeStanjeTipkala = stanjeTipkala;

  // Upravljanje aktuatorima ovisno o stanju sustava
  if (sustavUkljucen) {
    digitalWrite(PIN_LED_IND, HIGH); // Indikatorska LED svijetli

    // Skaliranje 10-bitnog očitanja (0-1023) u 8-bitni PWM (0-255)
    pwmIzlaz = vrijednostPot / 4;
    pwmIzlaz = constrain(pwmIzlaz, 0, 255); // Clamping zaštita

    analogWrite(PIN_LED_PWM, pwmIzlaz);

    // Telemetrijski ispis
    Serial.print("STATUS: UKLJUCEN | Pot: ");
    Serial.print(vrijednostPot);
    Serial.print(" | PWM (pin 9): ");
    Serial.println(pwmIzlaz);
  } else {
    // Sigurnosno stanje kada je sustav isključen
    digitalWrite(PIN_LED_IND, LOW);
    analogWrite(PIN_LED_PWM, 0); // Eksplicitno gašenje trošila

    Serial.print("STATUS: ISKLJUCEN | Pot: ");
    Serial.print(vrijednostPot);
    Serial.println(" | PWM (pin 9): 0 (iskljuceno)");
  }

  delay(50); // Stabilizacijska zadrška ciklusa
}
