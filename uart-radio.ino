#include <SPI.h> // это нужно для работы Ethernet-шилда
#include <Ethernet.h>

uint8_t n;
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x00
};
IPAddress ip(192, 168, 0, 155); // укажите желаемый IP
#define port 7777 // укажите желаемый порт
#define port2 23
EthernetServer server(port);
EthernetServer server2(port2);
bool welcome = 1;
//String tcpData = ""; //строки для временного хранения сообщений
uint8_t bufsize = 0;
String uartData = "";
void setup() {
  Serial.begin(115200);
  Ethernet.init(10);
  // Serial.println("UART started");
  Ethernet.begin(mac, ip);
  server.begin();
  server2.begin();
  //IPAddress lip = Ethernet.localIP();

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
