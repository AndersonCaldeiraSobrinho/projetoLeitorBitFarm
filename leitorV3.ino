//Leitor RFID
#include "RTClib.h"
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Ethernet.h>
// Define MacAddress, IP, Mascara de sub-rede e Gateway da placa de rede
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Define o Mac Address
  byte ip[] = { 10, 1, 140, 57 };                       // Define o IP
  byte subnet[] = {255, 255, 252, 0};                   // Define a máscara de rede
  byte gateway[] = {10, 1,143, 254};                    // Define o gateway

//#include <MySQL_Connection.h> //Necessario depois inserir conexão
//#include <MySQL_Cursor.h> // Necesario depois inseir conexão
#include <SPI.h>
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
 
LiquidCrystal lcd(6, 7, 5, 4, 3, 2); 

RTC_DS1307 rtc;

char id_leitor[11] = "1";
char st[20];
char daysOfTheWeek[7][12] = {"Domingo", "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sabado"};

IPAddress server_addr(10,1,140,54);  // Define o Servidor de Banco de dados
char user[] = "sistema";             // Define o usuario do Banco de Dados
char password[] = "123456";          // Define a senha do usuario

char BANCO_DE_DADOS[] = "";
 
EthernetClient client;
//MySQL_Connection conn((Client *)&client); //Inserir dados da conexão
 
void setup() 
{
  pinMode(8, OUTPUT); // led e tone de sucesso
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  //Define o número de colunas e linhas do LCD:  
  lcd.begin(16, 2);  
  mensageminicial();
  
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
     rtc.adjust(DateTime(2020, 07, 05, 22, 11, 10));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
   //rtc.adjust(DateTime(2020, 07, 04, 17, 59, 10));

  Ethernet.begin(mac, ip, subnet, gateway);
  
   /*if (conn.connect(server_addr, 3306, user, password))
      {
      Serial.println("Conectando ao Servidor de Banco de Dados");

      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      cur_mem->execute(BANCO_DE_DADOS);
      delete cur_mem;
      }
   else
      {
      Serial.println("A conexão com o Banco de Dados falhou");
      conn.close();
      }*/ 
      
      //comentado para evitar erro por não estar com conexão
   
}
 
void loop() 
{

  //MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

  //calcula valor da carga da bateria
  Serial.println(float(analogRead(A0))/float(1023)*5);

  //produz a impressão do calendario no terminal serial
  DateTime now = rtc.now();

    //Serial.print(now.year(), DEC);
    //Serial.print('/');
    //Serial.print(now.month(), DEC);
    //Serial.print('/');
    //Serial.print(now.day(), DEC);
    //Serial.print(" (");
    //Serial.print(now.hour(), DEC);
    //Serial.print(':');
    //Serial.print(now.minute(), DEC);
    //Serial.print(':');
    //Serial.print(now.second(), DEC);
    //Serial.println();

 

    String horario = (String(now.hour(), DEC))+(':')+(String(now.minute(), DEC)+(':')+(now.second(), DEC));
    String calendario = (String (now.day(), DEC))+('/')+(String(now.month(), DEC))+('/')+(String(now.year(), DEC));

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem : ");


  char INSERIR_DADOS[256]; // 256 = valor arbitrario, use uma quantidade que garanta que o texto (conteudo) nunca ira passar deste comprimento
  snprintf_P(INSERIR_DADOS,sizeof(INSERIR_DADOS),PSTR("INSERT INTO registro_leitor_RFID (cod_leitor, cod_tag,horario, calendario) VALUES (%d, %d, %d, %d)"),id_leitor,conteudo,horario,calendario);
     
   //cur_mem->execute(INSERIR_DADOS); //para executar conexão
   Serial.println("Dados gravados.");
   
   //delete cur_mem; //para eliminar conexão
  
  conteudo.toUpperCase();
    //Serial.println("Ola Usuario A !");
    //Serial.println();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(conteudo);
    lcd.setCursor(0,1);
    lcd.print("Registrado");
    digitalWrite(8, HIGH);
    tone(8, 2000, 1000);
    delay(1500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(horario);
    lcd.setCursor(0,1);
    lcd.print(calendario);
    delay(1500);
    digitalWrite(8, LOW);
    mensageminicial();
    horario = "";
    calendario = "";
} 
 
void mensageminicial()
{
  lcd.clear();
  lcd.print("Aguardando");  
  lcd.setCursor(0,1);
  lcd.print("Proximo leitura");  
}
