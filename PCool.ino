#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <CertStoreBearSSL.h>
BearSSL::CertStore certStore;
#include <time.h>
#include <StackThunk.h>
#include <TimeLib.h>
#ifdef ESP8266
#include <BlynkSimpleEsp8266.h>
#elif defined(ESP32)
#include <BlynkSimpleEsp32.h>
#else
#error "Board not found"
#endif

//#define _stackSize (6000)

#define BLYNK_PRINT Serial
#include <ESPAsyncTCP.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <Wire.h>
#include "RTClib.h"
#include <Blynk.h>
#include <PolledTimeout.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include<Separador.h>
#include<string.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include <Espalexa.h>

#define NUM_LEDS 10
#define LED_PIN 14
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
#define UPDATES_PER_SECOND 10
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

//#define CLOCK_PIN 12
// Define the array of leds
//#define PIXEL_PIN    4  // Digital IO pin connected to the NeoPixels.
//#define PIXEL_COUNT 16  // Number of NeoPixels
// Declare our NeoPixel strip object:
//Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


char auth_token[33];
bool connected_to_internet = 0;

Separador s;
//Provide credentials for your ESP server
char* esp_ssid = "WiFiconf";
char* esp_pass = "";


Espalexa espAlexa;

Ticker TimeReset;
Ticker TimeReloj;
Ticker Temper;

// Declaracion e inicializacion de variables utilizadas para
// realizar la conexion a la red Wifi local
char ssid[50];
char pass[50];
char WiFiSSID[50];
char WiFiPSK[50];
char ifttt[20];
char numif[1];


// Informacion de nuestro servidorconst 
char host[] = "192.168.4.1"; //Direccion IP de nuestro servidor
const int httpPort = 2525; //Puerto de enlace
String macID;//String donde se almacenara nuestra macID

String AP_NameString = "ESPdispHub";
//const char WiFiAPPSK[] = "admin";

char auth[33] = "";
String act,conex; 

int data[30],i,asciiresp,val,cont,error,proto,ini;
int addr=0; //apuntadores romm
int conect=0;

int romnumdisp=1;
int romenableif=2;
int romnumifttt=3;
int romlenssid=4;
int romlenpass=5;
int romwificonf=6;
int romini=7;
int romlenifttt=8;
int romestado=9;
int romnumif=11;
int romerrorip=12;
int romenu=13;
int romtokok=14;
int romanterior=15;
int romestadoblu=16;
int romestadoW=17;
int romestadoR=18;
int romestadoG=19;
int romestadoB=20;
int romifttt=21;
int romodorgb=22;
int rominimodo=23;
int romautofan=24;
int romconectoint=25;
int romasigpo=26;
int romWiFiSSID=50;
int romWiFiPSK=100; 

int lenssid;
int lenpass;

int estado;
int estadoW;
int estadoR;
int estadoG;
int estadoB;
int estadoblu;
int anterior;
int numdisp;
int enableif;
int numifttt;
int lenifttt;
int lenumif;
int pos;
int asigpo;
int errorip;

//String ifttt; 
String ipasig;
unsigned long caso=0;
int serie;
int timesleep=1000;
int crono;
int menu;
int tokok;
int guarda;
int tokenconfi;
int conectoint;
int lentoken;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long tiempo = 0;

String nombre,comando,nombrechar;
int tipdisp=6;int funcion;int wifirf;
String elemento0,elemento1,elemento2,elemento3,elemento4,elemento5,elemento6,elemento7,elemento8,elemento9,elemento10,elemento11;
String req="";
String trama;char fin='\r';String inicio="135";
String remoteip="";
char remote_ip[3]="";
int valadc;
int miliVolts;
int temperatura;
int valpwm;
int fanstop=0;

int inimodo;
int modorgb;

int autofan;

/////////////////////// Inicializacion de pines  //////////////////////////////

const int ir = 5;
const int out = 4;
const int pwmout = 0;
const int onoff = 2;

const int led = 14;
//const int dato = 14;
const int pwmout2 = 12;
const int modo = 13;

const int analogInPin = A0;

IRrecv irrecv(ir);      // pin objeto irrecv
decode_results codigo;      // crea objeto codigo de la clase decode_results

//WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wm;
WiFiManagerParameter custom_field; // global param ( for non blocking w params )
bool res;
bool blockWM = true;
int customFieldLength = 40;
String aut_token = "";

void ICACHE_RAM_ATTR handleInterrupt();
void ICACHE_RAM_ATTR handleInterrupt2();
void ICACHE_RAM_ATTR modrgbInterrupt();
void ICACHE_RAM_ATTR calctemp();

WiFiClient client;
WiFiServer localserver(2525);

String str;
String urlpage="http://blynk-cloud.com/";
BlynkTimer timer;

const String FirmwareVer={"5.0"}; //**********************version***********************¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡

#define URL_fw_Bin "https://raw.githubusercontent.com/Qubit1001/PCool/master/PCool.ino.generic.bin"
#define URL_fw_Version "/Qubit1001/Pcool/master/version.txt"
const char* hostgit = "raw.githubusercontent.com";
const int httpsPortgit = 443;
const char *   path = "/";




// DigiCert High Assurance EV Root CA
const char trustRoot[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j
ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL
MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3
LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug
RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm
+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW
PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM
xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB
Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3
hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg
EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF
MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA
FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec
nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z
eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF
hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2
Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe
vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep
+OkuE6N36B9K
-----END CERTIFICATE-----
)EOF";
X509List cert(trustRoot);

extern const unsigned char caCert[] PROGMEM;
extern const unsigned int caCertLen;

void setClock() {
   // Set time via NTP, as required for x.509 validation
  configTime(-7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
  
}
  
void FirmwareUpdate()
{  
  //BearSSL::WiFiClientSecure client;
  //BearSSL::PublicKey key(pubkey);
  //client.setKnownKey(&key);
  
  WiFiClientSecure client;
  client.setTrustAnchors(&cert);
  if (!client.connect(hostgit, httpsPortgit)) {
    Serial.println("Connection failed");
    return;
  }
  client.print(String("GET ") + URL_fw_Version + " HTTP/1.1\r\n" +
               "Host: " + hostgit + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }
  String payload = client.readStringUntil('\n');

  payload.trim();
  if(payload.equals(FirmwareVer) )
  {   
     Serial.println("Device already on latest firmware version");
     client.stop();
     delay(3000);
     ESP.restart();  
  }
  else
  {
    Serial.println("New firmware detected");
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW); 
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, URL_fw_Bin);
        
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    } 
  }
}

/*void connect_wifi()//****************............conectar wifi para actualizacion por internet.............**************************************
{
  WiFi.begin(WiFiSSID,WiFiPSK);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(3000);
    Serial.print("O");
  }                                   
  Serial.println("Connected to WiFi");
}*/

unsigned long previousMillis_2 = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 60000;
const long mini_interval = 1000;

void repeatedCall(){
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis) >= interval) 
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      setClock();
      FirmwareUpdate();
    }

    if ((currentMillis - previousMillis_2) >= mini_interval) {
      static int idle_counter=0;
      previousMillis_2 = currentMillis;    
      Serial.print(" Active fw version:");
      Serial.println(FirmwareVer);
      Serial.print("Idle Loop....");
      wdt_reset();
      Serial.println(idle_counter++);
     if(idle_counter%2==0)
      digitalWrite(LED_BUILTIN, HIGH);
     else 
      digitalWrite(LED_BUILTIN, LOW);
     if(WiFi.status() == !WL_CONNECTED) 
          //connect_wifi();
            connectWiFi();
   }
}

void putestado(){
  WiFiClient client;
    HTTPClient http;  
    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, urlpage); //HTTP
    http.addHeader("Content-Type", "application/json");
    Serial.print("[HTTP] PUT...\n");
  
    // start connection and send HTTP header and body    
    int httpCode = http.PUT(str);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] PUT... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end(); 
}

void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
 
}

void ConnectWiFi_STA(bool useStaticIP = false)
{

   IPAddress ip(192, 168, 1, 200);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
  Serial.println("");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFiSSID, WiFiPSK);
  if(useStaticIP) WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) 
  { 
    delay(100);  
    Serial.print('.'); 
  }

  Serial.println("");
  Serial.print("Iniciado STA:\t");
  Serial.println(WiFiSSID);
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void ConnectWiFi_AP(bool useStaticIP = false)
{ 

  IPAddress ip(192, 168, 1, 200);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
  Serial.println("");
  WiFi.mode(WIFI_AP);
  while(!WiFi.softAP(WiFiSSID, WiFiPSK))
  {
    Serial.println(".");
    delay(100);
  }
  if(useStaticIP) WiFi.softAPConfig(ip, gateway, subnet);

  Serial.println("");
  Serial.print("Iniciado AP:\t");
  Serial.println(WiFiSSID);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
}

void InitOTA()
{
  

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("%u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();
  Serial.println("");
  Serial.println("OTA iniciado");
}


void ISReset(){ 
               TimeReset.detach();
               if(error==1){EEPROM.write(romwificonf,1);EEPROM.commit();ESP.restart();}//se logró conexion
               if(error==2){EEPROM.write(romwificonf,0);EEPROM.commit();ESP.restart();}//error no se logró conexion
               if(error==3){EEPROM.write(romerrorip,1);EEPROM.commit();ESP.restart();}//rinicia y actuliza ip rapido
               if(error==4){ESP.restart();}
               if(error==6){estado='b';EEPROM.write(romestado, 'b');EEPROM.commit();Serial.print('b');digitalWrite(out,LOW);
                            estadoblu='b';EEPROM.write(romestadoblu, 'b');EEPROM.commit();Serial.print('b');digitalWrite(led,HIGH);}//cronometro off
               error=0;
}

void connectWiFi()
{
  // Obtenemos la macID de nuestro ESP8266
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  
  //  Configuramos nuestro dispositivo IoT como estacion
  WiFi.mode(WIFI_STA); 
  // Se inicia la conexion a SSID
  WiFi.begin(WiFiSSID, WiFiPSK);
   
   while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  error=1;//se logra conexion
}

void enviedo(){

  if (!client.connect(host, httpPort)) {Serial.println("connection failed");delay(1000);
                            return;
               }                 
              elemento2="2";elemento3=nombre;comando="actedo";elemento6=String(tipdisp,DEC);
              elemento5=String(estado);//manda 97
              //elemento5=String(char(estado));//manda a
              trama=inicio+"/"+elemento1+"/"+elemento2+"/"+nombre+"/"+comando+"/"+elemento5+"/"+elemento6+"/"+elemento7+"/"+
              elemento8+"/"+elemento9+"/"+elemento10+"/"+elemento11+fin;
              client.println(trama); 
              if (client.connected()) {conex = client.readStringUntil('\r');Serial.print(conex);}                                    
             client.stop();
             decotramaconex(); 
              if(elemento7=="o"){Serial.println("Edo actualizado");}
              if(elemento7=="x"){Serial.println("No actualizado");}
}

void actip() {    
                //client.setTimeout(1000);
               if (!client.connect(host, httpPort)) {
                          Serial.println("connection failed");delay(1000);
                            return;
               }   
               //client.connect(host,2525);
              elemento2="2";elemento3=nombre;comando="actip";
              //nombre="Seleccione";
              elemento6=String(tipdisp,DEC);
              trama=inicio+"/"+elemento1+"/"+elemento2+"/"+nombre+"/"+comando+"/"+elemento5+"/"+elemento6+"/"+elemento7+"/"+
              elemento8+"/"+elemento9+"/"+elemento10+"/"+elemento11+fin;
              client.println(trama);
              
              if (client.connected()) {conex = client.readStringUntil('\r');Serial.print(conex);}                                    
             client.stop();
             decotramaconex();
             ipasig=elemento7;
             if(elemento5=="a"){EEPROM.write(romestado,'a');EEPROM.commit();digitalWrite(out,HIGH);Serial.print('a'); }
             else if (elemento5=="b"){EEPROM.write(romestado,'b');EEPROM.commit();digitalWrite(out,LOW);Serial.print('b');}   
}

void asigpos() {  
                if (!client.connect(host, httpPort)) {
                                                    //Serial.println("connection failed");
                                                    return;
                                                      }
              elemento2="2";elemento3=nombre;comando="regis";
              //nombre="Seleccione";
              elemento6=String(tipdisp,DEC);
              trama=inicio+"/"+elemento1+"/"+elemento2+"/"+nombre+"/"+comando+"/"+elemento5+"/"+elemento6+"/"+elemento7+"/"+
              elemento8+"/"+elemento9+"/"+elemento10+"/"+elemento11+fin;
              client.println(trama);  

              if (client.connected()) {
                                            conex = client.readStringUntil('\r');
                                            Serial.println(conex);                                          
                                           }
            client.stop(); 
            decotramaconex();
            if(elemento7=="o"){EEPROM.write(romasigpo,1);EEPROM.commit();asigpo=1;errorip=1;//asignar ip rapidamente
            Serial.print('a'); digitalWrite(out, HIGH);
                 delay(2000);
                Serial.print('b'); digitalWrite(out, LOW);    
            }
            if(elemento7=="x"){Serial.print("Disp no registrado");} 
                                                  
}


void handleInterrupt() { //.....................INTERRUPCION BUTTON.................................                    
                     
                     // check to see if you just pressed the button
                    // (i.e. the input went from LOW to HIGH), and you've waited long enough
                    // since the last press to ignore any noise:
                    lastDebounceTime = millis();
                    // If the switch changed, due to noise or pressing:
                    
                    while(digitalRead(onoff)==LOW){wdt_reset();}
                    
                    if ((millis() - lastDebounceTime) < debounceDelay) {
                    // whatever the reading is at, it's been there for longer than the debounce
                    // delay, so take it as the actual current state:
                    lastDebounceTime = millis();    
                    caso=0;
                    }

                  if ((millis() - lastDebounceTime) > debounceDelay) {
                  // whatever the reading is at, it's been there for longer than the debounce
                  // delay, so take it as the actual current state:
                 tiempo= (millis() - lastDebounceTime)/1000;
                  caso=tiempo;
                  if(caso<=1){caso=1;}
                 
                 Serial.print(caso);
                  lastDebounceTime = millis();
                                     
                    if(caso>=1 and caso<=3){if(estado=='b' and caso!=0){estado='a';EEPROM.write(romestado, 'a');EEPROM.commit();Serial.print('a');digitalWrite(out,HIGH);caso=0;enviedo();}
                                    else if(estado=='a' and caso!=0){estado='b';EEPROM.write(romestado, 'b');EEPROM.commit();Serial.print('b');digitalWrite(out,LOW);caso=0;}enviedo();}
                  if(caso>=4 and caso<=7){EEPROM.write(romerrorip,1);EEPROM.commit();ESP.restart();}//rinicia y actuliza ip rapido                    
                  if(caso>=8 and caso<=12){EEPROM.write(romenu,5);EEPROM.commit();ESP.restart();caso=0;}//modo internet alexa                                                    
                  if(caso>=13 and caso<=17){EEPROM.write(romanterior,menu);EEPROM.write(romenu,1);EEPROM.commit();ESP.restart();caso=0;}//modo ota           
                  if(caso>=18 and caso<=22){if(WiFi.softAPdisconnect()==false and caso!=0){ WiFi.mode(WIFI_AP);WiFi.softAP(AP_NameString);caso=0;} //actia desactiva AP STA
                                            else if(WiFi.status() != WL_CONNECTED and caso!=0 and numdisp!=0){WiFi.mode(WIFI_STA);ESP.restart();caso=0;}}
                  if(caso>=23 and caso<=27){EEPROM.write(romenu,4);EEPROM.write(romanterior,menu);EEPROM.write(romconectoint,0);EEPROM.write(romtokok,0);for (int i = 0; i < lentoken; ++i){EEPROM.write(214 + i,0);}EEPROM.commit();ESP.restart();}//modo borrar  wifi
                  if(caso>=45 and caso<=55){EEPROM.write(romini,0);EEPROM.commit();ESP.restart();}
                caso=0; 
                  }                           
}

void handleInterrupt2() {
                   
                    lastDebounceTime = millis();
                    // If the switch changed, due to noise or pressing:
                    
                    while(digitalRead(onoff)==LOW){wdt_reset();}
                    
                    if ((millis() - lastDebounceTime) < debounceDelay) {
                    // whatever the reading is at, it's been there for longer than the debounce
                    // delay, so take it as the actual current state:
                    lastDebounceTime = millis();    
                    caso=0;
                    }

                  if ((millis() - lastDebounceTime) > debounceDelay) {
                  // whatever the reading is at, it's been there for longer than the debounce
                  // delay, so take it as the actual current state:
                 tiempo= (millis() - lastDebounceTime)/1000;
                  caso=tiempo;
                  if(caso<=1){caso=1;}
                 Serial.print(caso);
                  lastDebounceTime = millis();
                                     
                   if(caso>=1 and caso<=3){if(estado=='b' and caso!=0){str="[\"1\"]";estado='a';guarda=1;estadoblu='b';digitalWrite(led, HIGH);EEPROM.write(romestadoblu,'b');EEPROM.write(romestado, 'a');EEPROM.commit();Serial.print('a');digitalWrite(out,HIGH);caso=0;}
                                    else if(estado=='a' and caso!=0){str="[\"0\"]";estado='b';guarda=1;EEPROM.write(romestado, 'b');EEPROM.commit();Serial.print('b');digitalWrite(out,LOW);caso=0;}}                
                  if(caso>=4 and caso<=7){EEPROM.write(romenu,2);EEPROM.commit();ESP.restart();caso=0;}//modo blynk
                  if(caso>=8 and caso<=12){EEPROM.write(romenu,0);EEPROM.commit();ESP.restart();caso=0;}  //modo control con hub 
                  if(caso>=13 and caso<=17){EEPROM.write(romanterior,menu);EEPROM.write(romenu,1);EEPROM.commit();ESP.restart();caso=0;} //modo ota          
                  if(caso>=18 and caso<=22){EEPROM.write(romanterior,menu);EEPROM.write(romenu,3);EEPROM.commit();ESP.restart();caso=0;}//modo ota internet
                  if(caso>=23 and caso<=27){EEPROM.write(romanterior,menu);EEPROM.write(romenu,4);EEPROM.write(romconectoint,0);EEPROM.write(romtokok,0);for (int i = 0; i < lentoken; ++i){EEPROM.write(214 + i,0);}EEPROM.commit();ESP.restart();}//modo borrar  wifi
                  if(caso>=45 and caso<=55){EEPROM.write(romini,0);EEPROM.commit();ESP.restart();}
                caso=0; 
                  } 
}

/*void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}*/

void modrgbInterrupt(){modorgb++;}

void modrgb(){
    //ChangePalettePeriodically();
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    //FillLEDsFromPaletteColors( startIndex);
    //FastLED.show();
    //FastLED.delay(1000 / UPDATES_PER_SECOND);
    
  
  if (modorgb==0){
     if(inimodo==0){inimodo=1;EEPROM.write(rominimodo,1);EEPROM.commit();}
     //FastLED.show();
  }
  if(modorgb==1){
    if(inimodo==0){inimodo=1;EEPROM.write(rominimodo,0);EEPROM.commit();}
     if(estado=='a'){
                     
                  }

               }
  if(modorgb==2){
    if(estado=='a'){
    if(inimodo==0){inimodo=1;EEPROM.write(rominimodo,1);EEPROM.commit();}
    
    caso=0;
    }
  }
   if(modorgb==3){
    if(estado=='a'){
      if(inimodo==0){;inimodo=1;EEPROM.write(rominimodo,1);EEPROM.commit();}      
    }
    }
 
}

void subvol (){
  
  
}

void bajvol () {
   
  
}

BLYNK_WRITE(V4){
int pinValue=param.asInt();

     if (pinValue==1){
         digitalWrite(out,HIGH); // on
         estado='a';EEPROM.write(romestado, 'a');EEPROM.commit();        
       } 
      else if(pinValue==0) {
      digitalWrite(out, LOW); // off
      estadoblu='b';EEPROM.write(romestadoblu,'b');estado='b';EEPROM.write(romestado, 'b');EEPROM.commit();
     
     }
}

BLYNK_WRITE(V12){
  int volmas=param.asInt();
  if (volmas==1){
    subvol();
  }
  
}

BLYNK_WRITE(V22){
  int volmenos=param.asInt();
  if (volmenos==1){
  bajvol();
  }
}

BLYNK_WRITE(V30){
   int actualiza=param.asInt();
   if(actualiza==1){
                    EEPROM.write(romanterior,menu);EEPROM.write(romenu,3);EEPROM.commit();
                    TimeReset.detach();error=4;TimeReset.attach(3,ISReset);}
   
}

BLYNK_WRITE(V31){
   int reiniciar=param.asInt();
   if(reiniciar==1){TimeReset.detach();error=4;TimeReset.attach(5,ISReset);}
}

BLYNK_WRITE(V16){
   int bl=param.asInt();

    if (bl==1){
         digitalWrite(led,LOW); // on
         estadoblu='a';EEPROM.write(romestadoblu, 'a');EEPROM.commit();        
       } 
      else if(bl==0) {
      digitalWrite(led, HIGH); // off
      estadoblu='b';EEPROM.write(romestadoblu, 'b');EEPROM.commit();    
     }
 
}

BLYNK_WRITE(V26){Temper.detach();
                int manualfan=param.asInt();
                analogWrite(pwmout,700);analogWrite(pwmout2,700);delay(4000);
  
                if(autofan==0){ if(manualfan==0){digitalWrite(pwmout,LOW);digitalWrite(pwmout2,LOW);fanstop=0;}
                if(manualfan>0 and manualfan<750){analogWrite(pwmout,500);analogWrite(pwmout2,500);delay(1000);
                                  analogWrite(pwmout,550);analogWrite(pwmout2,550);delay(1000);
                                  analogWrite(pwmout,600);analogWrite(pwmout2,600);delay(1000);
                                  analogWrite(pwmout,650);analogWrite(pwmout2,650);delay(1000);
                                  analogWrite(pwmout,670);analogWrite(pwmout2,670);delay(1000);
                                  analogWrite(pwmout,manualfan);analogWrite(pwmout2,manualfan);
                                  } 
    
              if(manualfan>750 and manualfan<800){
                                  analogWrite(pwmout,720);analogWrite(pwmout2,720);delay(1000);
                                  analogWrite(pwmout,750);analogWrite(pwmout2,750);delay(1000);
                                  analogWrite(pwmout,770);analogWrite(pwmout2,770);delay(1000);
                                  analogWrite(pwmout,manualfan);analogWrite(pwmout2,manualfan);
                                  } 
                if(manualfan>800 and manualfan<850){
                                  analogWrite(pwmout,720);analogWrite(pwmout2,720);delay(1000);
                                  analogWrite(pwmout,750);analogWrite(pwmout2,750);delay(1000);
                                  analogWrite(pwmout,770);analogWrite(pwmout2,770);delay(1000);
                                  analogWrite(pwmout,790);analogWrite(pwmout2,790);delay(1000);
                                  analogWrite(pwmout,800);analogWrite(pwmout2,800);delay(1000);
                                  analogWrite(pwmout,820);analogWrite(pwmout2,820);delay(1000);
                                  analogWrite(pwmout,manualfan);analogWrite(pwmout2,manualfan);
                                  }
                if(manualfan>850 and manualfan<900){
                                  analogWrite(pwmout,720);analogWrite(pwmout2,720);delay(1000);
                                  analogWrite(pwmout,750);analogWrite(pwmout2,750);delay(1000);
                                  analogWrite(pwmout,770);analogWrite(pwmout2,770);delay(1000);
                                  analogWrite(pwmout,800);analogWrite(pwmout2,800);delay(1000);
                                  analogWrite(pwmout,820);analogWrite(pwmout2,820);delay(1000);
                                  analogWrite(pwmout,850);analogWrite(pwmout2,850);delay(1000);
                                  analogWrite(pwmout,860);analogWrite(pwmout2,860);delay(1000);
                                  analogWrite(pwmout,870);analogWrite(pwmout2,870);delay(1000);
                                  analogWrite(pwmout,880);analogWrite(pwmout2,880);delay(1000);
                                  analogWrite(pwmout,manualfan);analogWrite(pwmout2,manualfan);
                                  }  
               if(manualfan>900){
                                  analogWrite(pwmout,720);analogWrite(pwmout2,720);delay(1000);
                                  analogWrite(pwmout,750);analogWrite(pwmout2,750);delay(1000);
                                  analogWrite(pwmout,770);analogWrite(pwmout2,770);delay(1000);
                                  analogWrite(pwmout,800);analogWrite(pwmout2,800);delay(1000);
                                  analogWrite(pwmout,820);analogWrite(pwmout2,820);delay(1000);
                                  analogWrite(pwmout,830);analogWrite(pwmout2,830);delay(1000);
                                  analogWrite(pwmout,840);analogWrite(pwmout2,840);delay(1000);
                                  analogWrite(pwmout,850);analogWrite(pwmout2,850);delay(1000);
                                  analogWrite(pwmout,860);analogWrite(pwmout2,860);delay(1000);
                                  analogWrite(pwmout,870);analogWrite(pwmout2,870);delay(1000);
                                  analogWrite(pwmout,880);analogWrite(pwmout2,875);delay(1000);
                                  analogWrite(pwmout,885);analogWrite(pwmout2,885);delay(1000);        
                                  analogWrite(pwmout,890);analogWrite(pwmout2,890);delay(1000);
                                  analogWrite(pwmout,893);analogWrite(pwmout2,893);delay(1000);
                                  analogWrite(pwmout,895);analogWrite(pwmout2,895);delay(1000);
                                  analogWrite(pwmout,897);analogWrite(pwmout2,897);delay(1000);                                      
                                  analogWrite(pwmout,900);analogWrite(pwmout2,900);delay(1000);
                                  }
            Temper.attach(60,calctemp);                                          
           }
}

void putestadovir(){
  if(estado=='a'){
  Blynk.virtualWrite(V4,1);}
  else if (estado=='b'){
  Blynk.virtualWrite(V4,0);}
   delay(100);
   if(estadoblu=='a'){
  Blynk.virtualWrite(V16,1);}
  else if (estadoblu=='b'){
  Blynk.virtualWrite(V16,0);}
}

BLYNK_CONNECTED() {Blynk.syncVirtual(V4);}

void calctemp(){
  if(autofan==0){
      valadc = analogRead(analogInPin);
      //miliVolts = (valadc * 5000L) / 1023; 
      //temperatura = miliVolts / 10;
      temperatura = valadc * 0.09765625; // costance (1/1024*100)
      //Serial.println(temperatura);
      if(temperatura<=25){digitalWrite(pwmout,LOW);digitalWrite(pwmout2,LOW);fanstop=0;}
      else if(temperatura>25){if(fanstop==0){analogWrite(pwmout,HIGH);analogWrite(pwmout2,HIGH);delay(3000);
                                        analogWrite(pwmout,800);analogWrite(pwmout2,800);fanstop=1;}
                                        valpwm = map(temperatura, 25, 40,700, 900);
                                        analogWrite(pwmout,valpwm);analogWrite(pwmout2,valpwm);}                                           
      if (connected_to_internet){Blynk.virtualWrite(V0,temperatura);}
      }
  }

void codir(){
  caso=1;
   if (codigo.value ==0xE0E036C9 or codigo.value ==0x2119D9A ){      // si codigo recibido es igual a rojo
                                  if(estado=='b' and caso!=0){guarda=1;estado='a';EEPROM.write(romestado,'a');EEPROM.commit();Serial.print('a');digitalWrite(out,HIGH);caso=0;}
                                    else if(estado=='a' and caso!=0){guarda=1;estado='b';estadoblu='b';digitalWrite(led, HIGH);EEPROM.write(romestadoblu,'b');EEPROM.write(romestado,'b');EEPROM.commit();Serial.print('b');digitalWrite(out,LOW);caso=0;}
                                  }
   if (codigo.value ==0xE0E028D7 or codigo.value ==0x6E8C20D7){
                                                               subvol();                
                                                } //
   if (codigo.value ==0xE0E0A857 or codigo.value ==0x894AF9E4){
                                                                bajvol();                                       
                                                } //
    if (codigo.value ==0xE0E06897 or codigo.value ==0xEC6EF35E){      // si codigo recibido es igual a azul bluetooth
                                    if(estadoblu=='b' and caso!=0){guarda=1;estadoblu='a';EEPROM.write(romestadoblu,'a');EEPROM.commit();Serial.print('a');digitalWrite(led,LOW);caso=0;}
                                    else if(estadoblu=='a' and caso!=0){guarda=1;estadoblu='b';EEPROM.write(romestadoblu,'b');EEPROM.commit();Serial.print('b');digitalWrite(led,HIGH);caso=0;}    
                                             } //
    delay(80);
}

void decotrama (){elemento0=s.separa(req,'/',0);
      elemento1=s.separa(req,'/',1);elemento2=s.separa(req,'/',2);elemento3=s.separa(req,'/',3);
      elemento4=s.separa(req,'/',4);elemento5=s.separa(req,'/',5);elemento6=s.separa(req,'/',6);elemento7=s.separa(req,'/',7);
      elemento8=s.separa(req,'/',8);elemento9=s.separa(req,'/',9);elemento10=s.separa(req,'/',10);elemento11=s.separa(req,'/',11);
}
void decotramaconex (){elemento0=s.separa(conex,'/',0);
      elemento1=s.separa(conex,'/',1);elemento2=s.separa(conex,'/',2);elemento3=s.separa(conex,'/',3);
      elemento4=s.separa(conex,'/',4);elemento5=s.separa(conex,'/',5);elemento6=s.separa(conex,'/',6);elemento7=s.separa(conex,'/',7);
      elemento8=s.separa(conex,'/',8);elemento9=s.separa(conex,'/',9);elemento10=s.separa(conex,'/',10);elemento11=s.separa(conex,'/',11);
}

void firstLightChanged(uint8_t brightness) {
    Serial.print("Device 1 changed to ");
    
    //do what you need to do here

    //EXAMPLE
    if (brightness) {
      Serial.print("ON, brightness ");
      Serial.println(brightness);
    }
    else  {
      Serial.println("OFF");
    }
}

String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM customfieldid = " + getParam("customfieldid"));

  String auth = getParam("customfieldid");
   Serial.println("writing eeprom auth token:");
          for (int i = 0; i < auth.length(); ++i)
          {
            EEPROM.write(214 + i, auth[i]); //64+150=214
            Serial.print("Wrote: ");
            Serial.println(auth[i]);
          }
          EEPROM.commit();
  
}

void setup(){
  EEPROM.begin(512);
  Serial.begin(115200);
  pinMode(onoff, INPUT);
  pinMode(out,OUTPUT);
  pinMode(pwmout, OUTPUT);
  //pinMode(dato,OUTPUT);
  pinMode(modo,INPUT);
  pinMode(pwmout2, OUTPUT);
  pinMode(ir,INPUT);
  //pinMode(blue, OUTPUT);
  
 if(EEPROM.read(romini)!=1){for (int i = 0; i < 512; i++){EEPROM.write(i, 0);}EEPROM.write(romestadoblu,'b');EEPROM.write(romestado,'b');
 EEPROM.write(romini,1);EEPROM.write(romenu,4);EEPROM.commit();}//inicia rom a cero
  ini=0;
  irrecv.enableIRIn();     // inicializa recepcion de datos
 while (!Serial)  // Wait for the serial connection to be establised.
  delay(50);
  Serial.print("IR is now running");
//Serial.printf("BSSL stack: %d\n", stack_thunk_get_max_usage());
    // Uncomment/edit one of the following lines for your leds arrangement.
    // ## Clockless types ##
    //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    // FastLED.addLeds<SM16703, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1829, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1812, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1904, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS2903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2852, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<GS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA106, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<PL9823, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2813, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811_400, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GE8822, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD1886, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD1886_8BIT, DATA_PIN, RGB>(leds, NUM_LEDS);
    // ## Clocked (SPI) types ##
    // FastLED.addLeds<LPD6803, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2803, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
  //LEDS.setBrightness(84);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
  
  
  attachInterrupt(digitalPinToInterrupt(modo), modrgbInterrupt, FALLING);
  
  lenifttt=EEPROM.read(romlenifttt);
  for (int j =0; j < lenifttt; j++){ifttt[j]=EEPROM.read(romifttt);romifttt++;}
  numifttt=EEPROM.read(romnumif);                     
  estado=EEPROM.read(romestado);
  numdisp=EEPROM.read(romnumdisp);
  enableif = EEPROM.read(romenableif);
  val = EEPROM.read(romwificonf);
  errorip=EEPROM.read(romerrorip);
  menu=EEPROM.read(romenu);
  tokok=EEPROM.read(romtokok);
  estadoblu=EEPROM.read(romestadoblu);
  autofan=EEPROM.read(romautofan);
  conectoint=EEPROM.read(romconectoint);
  analogWriteFreq(20000);
  if(autofan==1){Temper.detach();digitalWrite(pwmout,HIGH);
                digitalWrite(pwmout2,HIGH);
  }
  if(autofan==0){
      analogWrite(pwmout,700);
      analogWrite(pwmout2,700);     
      //calctemp(); 
      Temper.attach(60,calctemp);  
    }
    
  if(estado=='a'){digitalWrite(out,HIGH);}
  else if(estado=='b'){digitalWrite(out,LOW);}
  
  if(estadoblu=='a'){digitalWrite(led,LOW);}
  else if(estadoblu=='b'){digitalWrite(led,HIGH);}
  
  for (int i = 64; i < 100; ++i){aut_token += char(EEPROM.read(150+i));}
  Serial.print("Auth Token: ");
  Serial.println(aut_token);
  
  new (&custom_field) WiFiManagerParameter("customfieldid", "Token o Nombre", "", customFieldLength,"placeholder=\"Custom Field Placeholder\"");
  new (&custom_field) WiFiManagerParameter(custom_field); // custom html input
  wm.addParameter(&custom_field);
  wm.setSaveParamsCallback(saveParamCallback);
  std::vector<const char *> mennu = {"wifi","info","param","sep","restart","exit"};
  wm.setMenu(mennu);

   
 while(menu==1){/////////////////////////////OTA//////////////////////////////////////////////////
              if(autofan==0){analogWrite(pwmout,700);analogWrite(pwmout2,700);}
              Temper.detach();
              if(val==1){/////////////////********************************************conectar por medio de hub
               
              if(conectoint==1){//el dispositivo ya se ha coenctado alguna vez
                         wm.setConfigPortalBlocking(false);//continuar con codigo
                          while(connected_to_internet==0){res = wm.autoConnect();
                                                      if(!res) {Serial.println("Failed to connect");WiFi.mode(WIFI_STA);
                                                        for (uint8_t t = 30; t > 0; t--) {Serial.println(t);delay(1000);}} 
                                                      else {Serial.println("Connected...)");connected_to_internet=1;}    
                                                    }      
                        }
              }

              if(val==0){
                        WiFi.mode(WIFI_AP);WiFi.softAP("ESPotA"); 
                       ArduinoOTA.setPort(8266);
                      ArduinoOTA.setHostname("ESPota");

              }
              anterior=EEPROM.read(romanterior);
              EEPROM.write(romenu,anterior);
              EEPROM.commit();
              InitOTA(); //conecta por medio de red dispositivo o de hub
              //EEPROM.write(romenu,0);EEPROM.commit();
              while(menu==1){ArduinoOTA.handle();
                            //digitalWrite(blue, LOW);   
                            delay(500);      
                            Serial.print(".");
                            //digitalWrite(blue, HIGH);
                            delay(1000);  
                            }
}

//////////////////////////////////////////////////////modo blynk //////////////////////////////////////////////////////////////
 while(menu==2){   attachInterrupt(digitalPinToInterrupt(onoff), handleInterrupt2, FALLING);           
                 for (uint8_t t = 4; t > 0; t--) {Serial.println(t);delay(1000);}
                 String auth_string=aut_token;// auth_token ahora se lee desde la eeprom como aut_token
                 auth_string.toCharArray(auth_token, 33); 
                 lentoken = strlen(auth_token);
                 
                 if(lentoken==0){tokok=0;}   //no se ha configurado token entra en modo configura  
                  if(lentoken!=0){tokok=1;}          
                 if(tokok==0){WiFi.disconnect();//no se a configurado token o entra modo configuracion por medio del boton 
                                //EEPROM.write(romtokok,0);EEPROM.commit();
                                //EEPROM.write(romconectoint,0);EEPROM.commit(); conectoint=0;                      
                                //Credentials.setupAP(esp_ssid, esp_pass);
                                EEPROM.write(romanterior,menu);
                                EEPROM.write(romenu,4);EEPROM.write(romconectoint,0);EEPROM.write(romtokok,0);EEPROM.write(romwificonf,0);
                                EEPROM.commit();
                                ESP.restart();
                                connected_to_internet = 0;                          
                              }
                              
                if(tokok==1){//token configurado
                            wm.setConfigPortalBlocking(false);//continuar con codigo
                            res = wm.autoConnect();
                            if (res)
                            {
                            connected_to_internet = 1;  
                            EEPROM.write(romconectoint,1);EEPROM.commit();
                            }
                            else
                            {
                           // Credentials.setupAP(esp_ssid, esp_pass);
                           
                            connected_to_internet = 0;
                            if(conectoint==0){
                            wm.resetSettings();
                                lentoken = strlen(auth_token);
                                for (int i = 0; i < lentoken; ++i){EEPROM.write(214 + i,0);} //64+150=214   
                                EEPROM.commit();
                            }
                            

                              if(conectoint==1){Temper.detach();
                              while (connected_to_internet == 0){
                                                      res = wm.autoConnect();
                                                      if(!res) {Serial.println("Failed to connect");WiFi.mode(WIFI_STA);
                                                      for (uint8_t t = 30; t > 0; t--){Serial.println(t);delay(1000);
                                                          valadc = analogRead(analogInPin);temperatura = valadc * 0.09765625;valpwm = map(temperatura, 25, 40,700, 900);
                                                          analogWrite(pwmout,valpwm);analogWrite(pwmout2,valpwm);
                                                          }} 
                                                        else {Serial.println("Connected...)");connected_to_internet=1;} 
                                }
                              Temper.attach(60,calctemp);
                              }
                            }
                          }
                          
                
                        if (connected_to_internet)
                           {
                                  // .  Write the setup part of your code
                                  //Serial.printf("BSSL stack: %d\n", stack_thunk_get_max_usage());
                                   int lenauth = strlen(auth_token);
                                    
                                    for (int i = 64; i <lenauth+64; ++i){urlpage += char(EEPROM.read(150+i));}
                                    urlpage += "/update/V4";
                                    //if(estado=='a'){str="[\"1\"]";estado='a';putestado();}
                                    //if(estado=='b'){str="[\"0\"]";estado='b';putestado();} 
                                     Blynk.config(auth_token);
                                                                                                             
                            }
                        
                                                       
                         while(menu==2){
                                    //Credentials.server_loops();
                                    if (connected_to_internet)
                                          {                               
                                            Blynk.run();
                                            //modrgb();
                                            if(guarda==1){putestadovir();guarda=0;}
                                          
                                            if (irrecv.decode(&codigo)) {      // si existen datos ya decodificados
                                                serialPrintUint64(codigo.value, HEX);    // imprime valor en hexadecimal en monitor
                                                codir();
                                                irrecv.resume();        // resume la adquisicion de datos
                                                                      }
                                      }
                         }                                                
}

////////////////////////////////////////update webb/////////////////////////////////////////////////////////////////////////
while(menu==3){Temper.detach();
                analogWrite(pwmout,800);analogWrite(pwmout2,800);
                anterior=EEPROM.read(romanterior);EEPROM.write(romenu,anterior);EEPROM.commit();
                for (uint8_t t = 4; t > 0; t--) {Serial.println(t);delay(1000);}
                 String auth_string = aut_token;
                 auth_string.toCharArray(auth_token, 33); 
                 lentoken = strlen(auth_token);
                 wm.setConfigPortalBlocking(false);
                 res = wm.autoConnect();        
                 
                 if(!res){tokok=1;}                 
                 if(lentoken==0){tokok=1;}

                 if(tokok==1){anterior=EEPROM.read(romanterior);EEPROM.write(romenu,anterior);EEPROM.commit();delay(2000);ESP.restart();}//no se a configurado token o entra modo configuracion por medio del boton 
                 if(tokok==0){
                            if(res){
                                    if(WiFi.status() == WL_CONNECTED){setClock();}
                                    while(menu==3){repeatedCall();}}
                }
}
///////////////////////////////////////configuracion////////////////////////////////////////
while(menu==4){Temper.detach();
                analogWrite(pwmout,800);analogWrite(pwmout2,800);              
                String auth_string = aut_token;
                 auth_string.toCharArray(auth_token, 33); 
                 lentoken = strlen(auth_token);
                 wm.setConfigPortalBlocking(false);
                 res = wm.autoConnect();
                 if(lentoken==0){tokok=0;}   //no se ha configurado token entra en modo configura   
                 if(lentoken!=0){tokok=1;}          
                 if(tokok==0){WiFi.disconnect();//no se a configurado token o entra modo configuracion por medio del boton 
                                EEPROM.write(romtokok,0);EEPROM.commit(); 
                                EEPROM.write(romconectoint,0);EEPROM.write(romwificonf,0);EEPROM.commit(); conectoint=0;               
                       
                                connected_to_internet = 0;                          
                              }
                                       
               if(tokok==1){//token configurado
                            if (res)
                            {
                              WiFi.mode(WIFI_AP_STA);                       
                              connected_to_internet = 1; 
                              EEPROM.write(romwificonf,1); 
                              anterior=EEPROM.read(romanterior);EEPROM.write(romenu,anterior);EEPROM.commit();
                              EEPROM.write(romconectoint,1);EEPROM.commit();
                              ESP.restart(); 
                            }
                            else
                            {
                            connected_to_internet = 0;
                            if(conectoint==0){//al dispositivo nunca se a conectaado
                                wm.resetSettings();
                                lentoken = strlen(auth_token);
                                for (int i = 0; i < lentoken; ++i){EEPROM.write(214 + i,0); //64+150=214   
                                EEPROM.commit();}
                            }
                            if(conectoint==1){//el dispositivo ya se ha coenctado alguna vez
                                while(connected_to_internet==0){res = wm.autoConnect();
                                                         if(!res) {Serial.println("Failed to connect");WiFi.mode(WIFI_STA);
                                                          for (uint8_t t = 30; t > 0; t--){Serial.println(t);delay(1000);}} 
                                                        else {Serial.println("Connected...");connected_to_internet=1;}    
                                }
     
                              }
                            }    
                        }
                        
                        if (connected_to_internet)
                           {
                                  // .  Write the setup part of your code   
                                     anterior=EEPROM.read(romanterior);EEPROM.write(romenu,anterior);EEPROM.commit();ESP.restart();                                                                                                
                            }

             while(menu==4){wm.process();if(WiFi.status() == WL_CONNECTED){break;}}                       
}

//////////////////////////////////////////////////////modo Alexa //////////////////////////////////////////////////////////////
 while(menu==5){attachInterrupt(digitalPinToInterrupt(onoff), handleInterrupt2, FALLING);           
                 for (uint8_t t = 4; t > 0; t--) {Serial.println(t);delay(1000);}
                 String auth_string = aut_token;
                 auth_string.toCharArray(auth_token, 33); 
                 lentoken = strlen(auth_token);
                 nombre=auth_string.c_str();                 
                 Serial.println(nombre);     
                 if(lentoken==0){tokok=0;}//token no configurado                 
                 if(lentoken!=0){tokok=1;}//token configurado     
                 if(tokok==0){WiFi.disconnect();//no se a configurado token o entra modo configuracion por medio del boton 
                                EEPROM.write(romanterior,menu);
                                EEPROM.write(romenu,4);EEPROM.write(romconectoint,0);EEPROM.write(romtokok,1);EEPROM.commit();
                                EEPROM.write(romwificonf,0);
                                ESP.restart();                          
                              }
                                       
               if(tokok==1){//wifi configurado
                            wm.setConfigPortalBlocking(false);//continuar con codigo
                            res = wm.autoConnect();
                            if (res){
                              connected_to_internet = 1;EEPROM.write(romconectoint,1);EEPROM.commit();
                            }
                            else
                            {  
                              connected_to_internet = 0;
                              if(conectoint==0){//al dispositivo nunca se a conectaado
                                                      wm.resetSettings();
                                                      lentoken = strlen(auth_token);
                                                      for (int i = 0; i < lentoken; ++i){EEPROM.write(214 + i,0);} //64+150=214   
                                                      EEPROM.commit();
                                                } 
                               if(conectoint==1){//el dispositivo ya se ha coenctado alguna vez
                                              while(connected_to_internet==0){res = wm.autoConnect();
                                                         if(!res) {Serial.println("Failed to connect");WiFi.mode(WIFI_STA);
                                                                for (uint8_t t = 30; t > 0; t--){Serial.println(t);delay(1000);}} 
                                                          else {Serial.println("Connected...)");connected_to_internet=1;}    
                                                          }
                                             }
                            }
                         }
                            
                        if (connected_to_internet)
                           {
                            espAlexa.addDevice(nombre, firstLightChanged); //simplest definition, default state off
                            espAlexa.begin();                                                                          
                            }
                                     
                while(menu==5){if(connected_to_internet){espAlexa.loop();delay(1);}}                        
}

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 attachInterrupt(digitalPinToInterrupt(onoff), handleInterrupt, FALLING);
 
  if(val==0){WiFi.mode(WIFI_AP);WiFi.softAP(AP_NameString);}//no conex con hub activa AP
  if(val==1){  String auth_string = aut_token;
          auth_string.toCharArray(auth_token, 33); 
          lentoken = strlen(auth_token);
          nombre=auth_string.c_str();
            wm.setConfigPortalBlocking(false);//continuar con codigo
            res = wm.autoConnect();
         if(conectoint==1){//el dispositivo ya se ha coenctado alguna vez
                          while(connected_to_internet==0){res = wm.autoConnect();
                                                         if(!res) {Serial.println("Failed to connect");WiFi.mode(WIFI_STA);
                                                          for (uint8_t t = 30; t > 0; t--){Serial.println(t);delay(1000);}} 
                                                        else {Serial.println("Connected...");connected_to_internet=1;}    
                                }        
                        }
             }


if (WiFi.isConnected()==true and asigpo==0 and WiFi.softAPdisconnect()==false){
    asigpos(); //++++++++++++la conexion la cierra el hub  
    }
    
if(lentoken!=0 and ipasig=="" and WiFi.isConnected()==true and WiFi.softAPdisconnect()==false and asigpo==1){ 
    cont = random(70,100);Serial.println(cont);
    if(errorip==1){cont=0;EEPROM.write(romerrorip,0);EEPROM.commit();delay(1000);}//desactiva act ip inmediato
    while (cont!=0){cont--;delay(1000);}
    actip(); //recibe x  o ++++++++++++
} 
  localserver.begin(); //Se inicia el dispositivo como un servidor
}
//////////////////////////////////////////fin de SETUP///////////////////////////////////////////////////////////////////////////////////////


void loop(){
  
 proto=0;
    WiFiClient client = localserver.available();
    client.setTimeout(500);   //tiempo de espera para '\r'
    if (!client) {//modrgb();
                  if (irrecv.decode(&codigo)) {// si existen datos ya decodificados
                                              serialPrintUint64(codigo.value, HEX);    // imprime valor en hexadecimal en monitor
                                              codir();
                                              irrecv.resume();        // resume la adquisicion de datos
                                              }
                  return;}//esp cliente
    
    conect=1;i=0;
    
    while(conect==1){
        if (!client) {conect=0;}//el cliente se desconecto
        String req=client.readStringUntil('\r');
        //Serial.print(req);
        decotrama();
        int protocolo=elemento0.toInt();comando=elemento4;funcion=elemento1.toInt();wifirf=elemento2.toInt();
        
  if(protocolo==135){
    //proto=1;
    if(comando=="a" and nombre==elemento3){Serial.print('a');EEPROM.write(romestado,'a');EEPROM.commit();digitalWrite(out,HIGH);estado='a';}
    if(comando=="b" and nombre==elemento3){Serial.print('b');estadoblu='b';EEPROM.write(romestadoblu,'b');digitalWrite(led, HIGH);EEPROM.write(romestado,'b');EEPROM.commit();digitalWrite(out,LOW);estado='b';}
    if(comando=="c" and nombre==elemento3){
    if(estadoblu=='b' and caso!=0){estadoblu='a';EEPROM.write(romestadoblu,'a');EEPROM.commit();Serial.print('a');digitalWrite(led,LOW);caso=0;}
                                    else if(estadoblu=='a' and caso!=0){estadoblu='b';EEPROM.write(romestadoblu,'b');EEPROM.commit();Serial.print('b');digitalWrite(led,HIGH);caso=0;}    
      }

    if(comando=="h" and  nombre==elemento3){subvol();}//+
    if(comando=="i" and  nombre==elemento3){bajvol();}//-
    if(comando=="t" and  nombre==elemento3){//MOD1                                      
                                                                     
                        }  
    if(comando=="u" and nombre==elemento3){ // MOD2
                        }
    
    if(nombre!=elemento3 and wifirf==1){ //#error de ip
                        client.stop(); //#cierra conexion para que "numconex" se actualizé en hub 
                        delay(3000);
                        actip();  //#++++++++++++la conexion la cierra el hub
                        caso=1;
                        conect=0;}
    //if(req[15]==0){client.stop();conect=0;}               
    if(req[5]==0 and enableif==1 and WiFi.isConnected()==true and numdisp==req[3]){ //#envia ifttt req[15]==1
                       
                    }
   if(req[4]==102 and numdisp==req[3]){}//#F1                      
   if(req[4]==103 and numdisp==req[3]){}//#F2
                        
   caso=0;conect=0;  
  }
  
  if(req[0]==131 and proto==0){client.stop();conect=0;}//enviar pos asignada solo desde pc
  if(req[0]==134){client.stop();WiFi.mode(WIFI_AP);conect=0;}
  
   if(req[0]==140 and proto==0){ //#eliminar dispositivo de hub  se pude recibir desde pc o hub
                    client.stop();                     //RECIBE 140 para que solo se puede eliminar por medio del HUB
                    WiFi.mode(WIFI_AP);
                    EEPROM.write(romnumdisp, 0);EEPROM.commit();
                    numdisp=0;
                    EEPROM.write(romwificonf,0);EEPROM.commit();
                    val=0;
                    EEPROM.write(romestado, 'b');EEPROM.commit();
                    conect=0;
                    error=4; //reinicia 
                    
                    TimeReset.attach(1,ISReset);}
                    
  if(req[0]==141 and proto==0){ //mostrar num de disp
                    delay(500);
                    client.print(numdisp);
                    client.stop();
                    conect=0;}
  }

}
