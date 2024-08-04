#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "VeejayLaptop"
#define WIFI_PASSWORD "dandasan7"
#define API_KEY "AIzaSyCdr44vTmbcZlPhfbjaCarIWHWqwJuUS7w"
#define DATABASE_URL "https://micropit-b5b3e-default-rtdb.asia-southeast1.firebasedatabase.app/"

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int burgerPrice = 25; // Price of french fries
int friesPrice = 20; // Price of burger
int sodaPrice = 15; // Price of burger

int currentOrder = 0; // Stores the current order
int quantity = 0; // Stores the quantity of the current order

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 12, 13};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
String totalItems = "";
float totalPrice = 0.0;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int tableNumber = 0; // Initialize table number
bool orderConfirmed = false;
bool tableNumberSet = false;
int enteredTableNumber = 0; // Declare globally

// Function to display menu on LCD
void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select from Menu");
  lcd.setCursor(0, 1);
  lcd.print(" 'A' to confirm ");
}

// Function to add item to order and send data to Firebase
void addItem(int itemNumber, int quantity) {
  switch (itemNumber) {
    case 1:
      Serial.println("No. of Burger: ");
      Serial.print(quantity);
      if (!totalItems.isEmpty()) totalItems += "  ,  ";
      totalItems += "Burger (" + String(quantity) + "x)";
      totalPrice += burgerPrice * quantity; // Price for Burger
      break;
    case 2:
      Serial.println("No. of Fries: ");
      Serial.print(quantity);
      if (!totalItems.isEmpty()) totalItems += "  ,  ";
      totalItems += "Fries (" + String(quantity) + "x)";
      totalPrice += friesPrice * quantity; // Price for Fries
      break;
    case 3:
      Serial.println("No. of Soda: ");
      Serial.print(quantity);
      if (!totalItems.isEmpty()) totalItems += " , ";
      totalItems += "Soda (" + String(quantity) + "x)";
      totalPrice += sodaPrice * quantity; // Price for Soda
      break;
    default:
      break;
  }
  
  // Prepare data for Firebase
  String path = "Table" + String(tableNumber);
  Firebase.RTDB.setString(&fbdo, path + "/TotalItems", totalItems);
  Firebase.RTDB.setFloat(&fbdo, path + "/TotalPrice", totalPrice);
}

// Function to display order summary
void displayOrderSummary() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total: P");
  lcd.print(totalPrice);
  lcd.setCursor(0, 1);
  lcd.print("   Thank you!   ");
  delay(3000);
  lcd.setCursor(0, 0);
  lcd.print(" Please Press 5");
  lcd.setCursor(0, 1);
  lcd.print(" on the REMOTE! ");
  
  orderConfirmed = true; // Set order confirmation flag

  totalItems = "";
  totalPrice = 0.0;
  tableNumberSet = false; // Reset table number flag
  enteredTableNumber = 0; // Reset entered table number
}

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.backlight();

  // Display initial prompt for table number
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Table No:");
  
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signUp OK");
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  
}

void loop() {
  char key = keypad.getKey();
  
  if (key && !tableNumberSet) {
    if (key >= '0' && key <= '9') {
      enteredTableNumber = enteredTableNumber * 10 + (key - '0');
      lcd.setCursor(0, 1);
      lcd.print(enteredTableNumber);
    } else if (key == '#') {
      tableNumber = enteredTableNumber;
      tableNumberSet = true;
      lcd.clear();
      displayMenu();
    }
  } else if (key && tableNumberSet) {
    int quantityB = 0, quantityFries = 0, quantityS = 0; // Initialize quantity outside the switch-case block
    
    switch (key) {
      case '1':
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("No. of Burger:");
        
        while (true) {
          char quantityKeyB = keypad.getKey();
          Serial.print(quantityKeyB);
          if (quantityKeyB >= '0' && quantityKeyB <= '9') {
            quantityB = quantityB * 10 + (quantityKeyB - '0');
            lcd.setCursor(0, 1);
            lcd.print(quantityB);
          } else if (quantityKeyB == '#') {
            addItem(1, quantityB); // Pass item number directly
            displayMenu(); // Redisplay the menu after selecting quantity
            break;
          }
        }
        break; // Add break statement here
      case '2':
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("No. of Fries:");
        while (true) {
          char quantityKeyF = keypad.getKey();
          Serial.print(quantityKeyF);
          if (quantityKeyF >= '0' && quantityKeyF <= '9') {
            quantityFries = quantityFries * 10 + (quantityKeyF - '0');
            lcd.setCursor(0, 1);
            lcd.print(quantityFries);
          } else if (quantityKeyF == '#') {
            addItem(2, quantityFries); // Pass item number directly
            displayMenu(); // Redisplay the menu after selecting quantity
            break;
          }
        }
        break; // Add break statement here
      case '3':
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("No. of Soda:");
        while (true) {
          char quantityKeyS = keypad.getKey();
          Serial.print(quantityKeyS);
          if (quantityKeyS >= '0' && quantityKeyS <= '9') {
            quantityS = quantityS * 10 + (quantityKeyS - '0');
            lcd.setCursor(0, 1);
            lcd.print(quantityS);
          } else if (quantityKeyS == '#') {
            addItem(3, quantityS); // Pass item number directly
            displayMenu(); // Redisplay the menu after selecting quantity
            break;
          }
        }
        break; // Add break statement here
      case 'A': // Assuming 'A' is a button for confirming order
        displayOrderSummary();
        break;
      default:
        break;
    }
    
    if (orderConfirmed) {
      delay(6000); // Delay for 5 seconds
      // Reset and prompt for table number again
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Table No:");
      orderConfirmed = false; // Reset the order confirmation flag
    }
  }
}
