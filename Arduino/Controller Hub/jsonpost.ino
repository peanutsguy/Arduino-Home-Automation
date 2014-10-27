#include <SPI.h>
#include <Ethernet.h>
#include <JsonParser.h>
#include "structs.h"

using namespace ArduinoJson::Parser;

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177);

EthernetServer server(80);

void setup(){
  Serial.begin(9600);
  //  Ethernet.begin(mac, ip);
  if(Ethernet.begin(mac) == 0) { // start ethernet using mac & DHCP
    Serial.println("Failed to configure Ethernet using DHCP");  
    while(true){   // no point in carrying on, so stay in endless loop:
      pinMode(13,OUTPUT);
      int state = digitalRead(13);
      digitalWrite(13,~state);
      delay(500);
    }
  } 
  delay(1000); // give the Ethernet shield a second to initialize

  Serial.print("This IP address: ");
  IPAddress myIPAddress = Ethernet.localIP(); 
  Serial.println(myIPAddress);  
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}

void loop(){
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {

          String json_s = String("");
          //json = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.75608,2.302038]}";

          Serial.print("RCVD: ");
          while(client.available())
          {
            char c = client.read();
            Serial.write(c);
            //client.print(c);
            json_s += c;
          }
          Serial.println("");
          int length = json_s.length()+2;
          char json[length];
          json_s.toCharArray(json,length);

          Serial.print("JSON: ");
          Serial.println(json);

          Sensor payload;
          json2array(&payload,json);

          http_header(client);
          html_header(client);

          //BODY

          if(payload.p_status == 1){

            client.print("SENSOR: ");
            client.print(payload.sensor);
            client.println("</br>");
            client.print("TIME: ");
            client.print(payload.time);
            client.println("</br>");
            client.print("LATITUDE: ");
            client.print(payload.latitude);
            client.println("</br>");
            client.print("LONGITUDE: ");
            client.print(payload.longitude);
            client.println("</br>");

            client.print("<img src=\"https://maps.googleapis.com/maps/api/staticmap?center=");            
            client.print(payload.latitude);
            client.print("+");
            client.print(payload.longitude);
            client.print("&zoom=16&size=600x300&maptype=roadmap&format=png&markers=color:blue%7Clabel:PIN%7C");
            client.print(payload.latitude);
            client.print(",");
            client.print(payload.longitude);
            client.println("\">");
          }

          else{
            client.println("<h1>ERROR PARSING JSON</h1>");
          }

          //END BODY

          html_footer(client);
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}

void json2array(Sensor* p,char* json){
  JsonParser<16> parser;
  JsonObject root = parser.parse(json);

  if (!root.success()){
    p->p_status = 0;
    return;
  }
  else{
    p->p_status = 1;
    p->sensor = root["sensor"];
    p->time = root["time"];
    p->latitude = root["data"][0];
    p->longitude = root["data"][1];
  }

}

void http_header(EthernetClient client){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
}

void html_header(EthernetClient client){
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>JSON POST Parser</title>");
  client.println("</head>");
  client.println("<body>");
}

void html_footer(EthernetClient client){
  client.println("</body>");
  client.println("</html>");
}


