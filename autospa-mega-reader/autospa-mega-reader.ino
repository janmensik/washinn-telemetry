// Internal WashInn APi Telemetry code
// For Autospa CarWash - using Arduino MEGA

#include <SPI.h>
#include <Ethernet.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// for manual configuration:
IPAddress ip(192, 168, 0, 109);
IPAddress myDns(8, 8, 8, 8);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// initialize the library instance:
EthernetClient client;

char server[] = "api.washinn.eu";

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

#define LED 13

int numberOfBoxes = 2;
int numberOfPrograms = 5;
int boxNumbers[2] = {2,3};
int programs[2] = {0,0};
int pinNumbers[2][5] = {
  /*{22,23,24,25,26},*/ /* BOX 1, program 1-5 */
  {28,29,30,31,32}, /* BOX 2, program 1-5 */
  {34,35,36,37,38} /*,*/ /* BOX 3, program 1-5 */
  /*{40,41,42,43,44}*/ /* BOX 4, program 1-5 */
};

int ib;
int ipr;
int i;
int pinStatus;

String output;
String outputData;

void setup() {
  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(LED, OUTPUT);

  // initialize pins for programs and boxes 
  for (ib=0; ib<numberOfBoxes; ib++) {
    for (ipr=0; ipr<numberOfPrograms; ipr++) {
      pinMode(pinNumbers[ib][ipr], INPUT_PULLUP);
    }
  }

  // give the ethernet module time to boot up:
  delay(1000);

  // start Eth in DHCP mode
  Ethernet.begin(mac);
  // start the Ethernet connection using a fixed IP address and DNS server:
  //Ethernet.begin(mac, ip, myDns, gateway, subnet);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());  
}

void loop() {
  // for DHCP
  Ethernet.maintain();
  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  /*
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  */

  getPrograms();  

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {

    Serial.println(output);
    //Serial.println(outputData);

    httpRequest();
  }
}

void getPrograms() {
  output = '\n';
  outputData = "";
  // initialize pins for programs and boxes 
  for (ib=0; ib<numberOfBoxes; ib++) {
    programs[ib] = 0;
    for (ipr=0; ipr<numberOfPrograms; ipr++) {
      pinStatus = digitalRead(pinNumbers[ib][ipr]);      
      if (pinStatus==LOW) {
        programs[ib] = ipr+1;
        //break;
      }
    }
    output += "BOX "+String(boxNumbers[ib])+"="+String(programs[ib])+"  ";
    outputData += "&"+String(boxNumbers[ib])+"="+String(programs[ib]);
  }
}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /call/autospa/kladno/?v=1"+outputData+" HTTP/1.1");
    client.println("Host: api.washinn.eu");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
    Serial.println("Done");
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
