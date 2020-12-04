#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
#define hostname "MMDVM-board1# "
uint16_t n;
void(* resetFunc) (void) = 0;
//uint8_t rand = random(0, 10);
//byte mac[] = {
//  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x00
//};
//Default IP setting for first time launch
#define MACADDRESS 0x00,0xAA,0x02,0xCC,0x04,0x05
#define MYIPADDR 192,168,1,6
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,1,1
#define MYGW 192,168,1,1
#define mmdvmport 7777
#define telnet 23
EthernetServer server(mmdvmport);
EthernetServer server2(telnet);
uint8_t mac[6] = {MACADDRESS};
uint8_t myIP[4];
uint8_t myMASK[4];
uint8_t myDNS[4];
uint8_t myGW[4];


bool welcome = 1;
uint8_t bufsize = 0;
String uartData = "";
void setup() {
  Serial.begin(115200);
  EEPROM.get(10, myIP);
  EEPROM.get(6, myMASK);
//  if (strcmp(myIP, "") != 1) {
//     Ethernet.begin(mac, myIP);
//   } else {
//  uint8_t myIP[4] = {MYIPADDR};  
//  uint8_t myMASK[4] = {MYIPMASK};
//  uint8_t myDNS[4] = {MYDNS};
//  uint8_t myGW[4] = {MYGW};
  Ethernet.begin(mac, myIP);
//  }
  Ethernet.init(10);
  // Serial.println("UART started");
  server.begin();
  server2.begin();
  //IPAddress lip = Ethernet.localIP();
  sensors.begin();

}

void loop() {
  EthernetClient client = server.available();
  EthernetClient client2 = server2.available();
  if (client2) {
    if (welcome) {
      client2.println("Welcome to MMDVM-duino CLI");
      client2.print(hostname);
      welcome = 0;
    }
    String tcpData;
    while (client2.available()) {
      char c2 = client2.read();
      if (c2 != '\n') {
        tcpData.concat(c2);
      }
      else {
        tcpData.trim();
        //Serial.println(tcpData);
        if (tcpData == "uptime") {
          client2.print("System uptime: ");
          client2.println(millis());
          client2.print(hostname);
        } else if (tcpData == "version") {
          client2.print("Firmware version: 0.2 compiled ");
          client2.println( __DATE__ " " __TIME__);
          client2.print(hostname);
        } else if (tcpData == "quit") {
          client2.println("Goodbye");
          client2.stop();
          welcome = 1;
        } else if (tcpData == "") {
          client2.print(hostname);
        } else if (tcpData == "reboot") {
          client2.println("Board is rebooting. Goodbye");
          resetFunc();
        } else if (tcpData == "enviroment") {
          sensors.requestTemperatures();
          client2.print("Battery temperature: ");
          client2.print(sensors.getTempCByIndex(0));
          client2.println(" °C");
          client2.print(hostname);
        } else if (tcpData == "ifconfig") {
          //client2.println(Ethernet.localIP());
          for (int i=0; i < 4; i++){
            client2.print(myIP[i]);
            if ( i < 3 ){
              client2.print(".");
            } else {
              client2.println("");
            }
          }               
          client2.print(hostname);
        } else if (tcpData.startsWith("ifconfig set")) {
          unsigned int start = tcpData.indexOf(" ", 11);
          unsigned int stop = tcpData.indexOf(" ", start + 1);
          String data = tcpData.substring(start, stop);
          data.trim();
          unsigned int pos = 0;
          for (int i=0; i < 4; i++) {
            String octet;            
            unsigned int ipcomma = data.indexOf(".",pos);                 
            octet = data.substring( pos  , ipcomma);  
            pos = ipcomma + 1;
            myIP[i] = octet.toInt();
           // client2.print(myIP[i]);         
          }
          client2.println("");                  
          EEPROM.put(10, myIP);          
          start = tcpData.indexOf(" ", stop);
          stop = tcpData.indexOf(" ", start + 1);
          data = tcpData.substring(start, stop);
          data.trim();
          pos = 0;
          for (int i=0; i < 4; i++) {
            String octet;            
            unsigned int ipcomma = data.indexOf(".",pos);                 
            octet = data.substring( pos  , ipcomma);  
            pos = ipcomma + 1;
            myMASK[i] = octet.toInt();
            //client2.print(myMASK[i]);         
          }
          EEPROM.put(6, myMASK);
          client2.print(hostname);
        } else if (tcpData == "help") {
          client2.println(" uptime - print system uptime\n version - print firmware version\n reboot - reboot board\n quit - exit console");
          client2.print(hostname);
        } else {
          client2.println("Unknown command\n Type 'help' for additional info");
          client2.print(hostname);
        }
        tcpData = "";
      }
    }

  }
  while (client) {
    n = Serial.available();
    while (n >= bufsize) {
      char serial = Serial.read();
      uartData.concat(serial);
      bufsize++;
      if (bufsize == n) {
        client.print(uartData);
        uartData = "";
        bufsize = 0;
        break;
      }
    }

    if (client.available()) {
      char uart = client.read();
      Serial.print(uart);
    }
    // Workaround for Ethernet library bug
    // after invoking client.available() client2 always return 0
    EthernetClient client2 = server2.available();
    if (client2) {
      break;
    }

  }
}
