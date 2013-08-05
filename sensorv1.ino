#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x86, 0x8D, 0xBE, 0x8F, 0xFE, 0xED };
//char server[] = "ec2-54-213-70-96.us-west-2.compute.amazonaws.com";
char server[] = "10.32.24.114";
int myId = -1;
char myName[] = "PetersDesk";
int sensorPin = A0;
int sensorValue = 0;

EthernetClient client;

char contype[] = "Content-Type: application/x-www-form-urlencoded";
char conlen[] = "Content-Length: 8";
char conclose[] = "Connection: close";

int debugInfoTCP = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(".");
  if(Ethernet.begin(mac)==0)
    Serial.println("Failed to configure Ethernet with DHCP");
  else 
    Serial.println("Phew");
  delay(1000);
  
  TCPConnect();
  
  client.println("POST /sensors/add HTTP/1.0");
  client.println(contype);
  client.print("Content-Length: ");
  client.println((sizeof(myName)/sizeof(myName[0]))+4);
  client.println(conclose);
  client.println();
  client.print("city=");
  client.println(myName);
  client.println();
  Serial.println("All good");
  
  while (myId == -1) {
    if(client.available()){
      char c = client.read();
      Serial.print(c);
      
      if(c=='i'){
        getId();
      }
    }
    else delay(100);
  }
}



void loop()
{
  delay(50);
  while (!client.connected()) {
    stop();
    if(debugInfoTCP)
      Serial.println("Connecting...");
    if(client.connect(server, 4000))
      break;
  }
  client.println(String("POST /sensors/")+ myId + String(" HTTP/1.0"));
  client.println(contype);
  client.println(conlen);
  client.println(conclose);
  client.println();
  sensorValue = analogRead(sensorPin);
  client.println(String("val=")+ sensorValue);
  client.println();
  Serial.println(String("val=")+ sensorValue);
}

//Connects to server via TCP
void TCPConnect(){
  if(debugInfoTCP)
    Serial.println("connecting TCP...");
  while(!client.connect(server,4000)){
    if(debugInfoTCP)
      Serial.println("Connection failed, trying again in 1s");
    delay(1000);
  }
  if(debugInfoTCP)
    Serial.println("connected TCP");
}

void stop(){
  while(client.available())
    client.read();
  client.flush();
  if(!client.connected()){
    client.stop();
    if(debugInfoTCP)
      Serial.println("Stopping TCP...");
  }
}

void getId(){
  char c = client.read();
  Serial.print(c);
  if(c=='d'){
    c = client.read();
    Serial.print(c);
    if(c==':'){
      c = client.read();
      Serial.print(c);
      if(c >= '0' && c<= '9'){
        myId=c-48;
        c = client.read();
        while(c>='0' && c<='9'){
          myId*=10;
          myId+=(c-48);
          c = client.read();
        }
        Serial.print(String("\nMyId found: ") + myId + '\n');
        stop();
      }
    }
  }
}
