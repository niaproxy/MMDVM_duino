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

//uint8_t rand = random(0, 10);
//byte mac[] = {
//  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x00
//};
#define MACADDRESS 0x00,0xAA,0x02,0xCC,0x04,0x05
#define MYIPADDR 192,168,1,6
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,1,1
#define MYGW 192,168,1,1
#define port 7777
#define port2 23
EthernetServer server(port);
EthernetServer server2(port2);
uint8_t mac[6];
uint8_t myIP[4];
uint8_t myMASK[4];
uint8_t myDNS[4];
uint8_t myGW[4];


bool welcome = 1;
uint8_t bufsize = 0;
String uartData = "";
void setup() {
  EEPROM.get(10, myIP);
  if (myIP != "") {
    Ethernet.begin(mac, myIP);
  } else {
    uint8_t mac[6] = {MACADDRESS};
    uint8_t myIP[4] = {MYIPADDR};
    uint8_t myMASK[4] = {MYIPMASK};
    uint8_t myDNS[4] = {MYDNS};
    uint8_t myGW[4] = {MYGW};
    Ethernet.begin(mac, myIP);
  }
  Serial.begin(115200);
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
        } else if (tcpData == "enviroment") {
          sensors.requestTemperatures();
          client2.print("Battery temperature: ");
          client2.print(sensors.getTempCByIndex(0));
          client2.println(" °C");
          client2.print(hostname);
        } else if (tcpData == "ifconfig") {
          //EEPROM.get(10, test);
          client2.println(EEPROM.read(10));
          client2.print(hostname);
        } else if (tcpData == "help") {
          client2.println(" uptime - print system uptime\n version - print firmware version\n quit - exit console");
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
