#define MACADDRESS 0x00,0x01,0x02,0x03,0x04,0x05
#define MYIPADDR 192,168,1,6
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,1,1
#define MYGW 192,168,1,1
#define LISTENPORT 1000
#define UARTBAUD 115200
#include <UIPEthernet.h>

#define port 7777 // укажите желаемый порт
#define port2 23
EthernetServer server(port);
EthernetServer server2(port2);
bool welcome = 1;
//String tcpData = ""; //строки для временного хранения сообщений
uint8_t n;

uint8_t bufsize = 0;
String uartData = "";
void setup() {
  Serial.begin(115200);
  uint8_t mac[6] = {MACADDRESS};
  uint8_t myIP[4] = {MYIPADDR};
  uint8_t myMASK[4] = {MYIPMASK};
  uint8_t myDNS[4] = {MYDNS};
  uint8_t myGW[4] = {MYGW};
  // Serial.println("UART started");
  Ethernet.begin(mac, myIP, myDNS, myGW, myMASK);
  server.begin();
  server2.begin();
  //Serial.write(0xE0);
  //Serial.write(0x03);
  //Serial.write(0x00);
}

void loop() {
  EthernetClient client = server.available(); // ожидаем подключения

  EthernetClient client2 = server2.available(); // ожидаем подключения
  if (client2) { // принимаем запрос на подключение TCP клиента
    if (welcome) {
      client2.println("Welcome to CLI");
      client2.print("Test-board1# ");
      welcome = 0;
    }

    String tcpData; //строки для временного хранения сообщений
    while (client2.available()) { // пока клиент подключён

      char c2 = client2.read();
      if (c2 != '\n') {
        tcpData.concat(c2);
      }
      else {
        tcpData.trim();
        //Serial.println(tcpData);
        if (tcpData == "uptime") {
          client2.print("System uptime: ");
          client2.println(millis()); // отправляем принятые данные в UART
        }
        if (tcpData == "quit") {
          client2.println("Goodbye");
          client2.stop();
          welcome = 1;
        }

        client2.print("Test-board1# ");
        tcpData = "";
      }
    }

  }
  while (client) { // принимаем запрос на подключение TCP клиента
    //   if (client.available()) { // пока клиент подключён

    // ЕСЛИ ПРИШЛИ ДАННЫЕ ПО UART:
    n = Serial.available();
    while (n >= bufsize) {
      char serial = Serial.read();
      uartData.concat(serial); // добавляем прочитанный символ к временной строке
      bufsize++;
      if (bufsize == n) {
        // Serial.println("UART data received: " + uartData);
        client.print(uartData); // отправляем принятую строку TCP клиенту
        uartData = ""; // обнуляем временную строку
        bufsize = 0;
        break;
      }
    }

    // ЕСЛИ ПРИШЛИ ДАННЫЕ ПО TCP:
    if (client.available()) {

      char uart = client.read();

      Serial.print(uart);

    }

    EthernetClient client2 = server2.available(); // ожидаем подключения
    //  }
    //client.stop(); // закрываем соединение
    if (client2) {
      break;
    }
  }
}
