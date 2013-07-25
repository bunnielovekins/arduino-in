#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x8E, 0x8D, 0xBE, 0x8F, 0xFE, 0xED };
char server[] = "ec2-54-213-87-44.us-west-2.compute.amazonaws.com";
int myId = -1;
char myName[] = "PetersDesk";
int sensorPin = A0;
int sensorValue = 0;

EthernetClient client;

char contype[] = "Content-Type: application/x-www-form-urlencoded";
char conlen[] = "Content-Length: 8";
char conclose[] = "Connection: close";

void setup() {
  Serial.begin(9600);
  Serial.println("Setup Beginning");
  if(Ethernet.begin(mac)==0)
    Serial.println("Failed to configure Ethernet with DHCP");
  else 
    Serial.println("Phew");
  delay(1000);
  Serial.println("connecting...");
  
  while(!client.connect(server,4000)){
    Serial.println("Connection failed, trying again in 1s");
    delay(1000);
  }
  Serial.println("connected");
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
  }
}



void loop()
{
  delay(50);
  while (!client.connected()) {
    stop();
    Serial.println("Connecting...");
    if(client.connect(server, 4000))
      break;
  }
  client.println(String("POST /sensors/")+ myId + String(" HTTP/1.0"));
  client.println(contype);
  client.println(conlen);
  client.println(conclose);
  client.println();
  client.println(String("val=")+ (sensorValue = analogRead(sensorPin)));
  client.println();
  Serial.println(String("val=")+ (sensorValue = analogRead(sensorPin)));
//  stop();
}

void stop(){

  client.flush();
  while(client.available()){
    client.read();
  }
  if(!client.connected()){
    client.stop();
    Serial.println("Stopping...");
  }
}

void readAll(){
  while(client.available()){
    client.read();
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
