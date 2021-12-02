/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include "Definitions.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <elapsedMillis.h>

elapsedMillis TimerCompteurPrincipal;  

// Replace with your network credentials
const char* ssid = "PALM_Wifi";
const char* password = "palmpalm";

const int btnGauche = 13;
const int btnDroite = 14;
const int btnAv = 26;
const int btnAr = 25;

uint8_t ui_EtatBtn = 0;
uint8_t ui_EtatBtn_Z1 = 0;

StateBtnLaby stateBtnLaby;
StateBtnLaby stateBtnLaby_Z1;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

//todo pas beau
uint8_t tache_ConvertBtnState(boolean b_Btn1, boolean b_Btn2, boolean b_Btn3, boolean b_Btn4)
{
  uint8_t ui_result = 0;
  if((b_Btn1 == true) && (b_Btn2 == false))
  {
    ui_result = ui_result + 1;
  }
  if((b_Btn2 == true) && (b_Btn1 == false))
  {
    ui_result = ui_result + 2;
  }
  if((b_Btn3 == true) && (b_Btn4 == false))
  {
    ui_result = ui_result + 4;
  }
  if((b_Btn4 == true) && (b_Btn3 == false))
  {
    ui_result = ui_result + 8;
  }
  return ui_result;
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(btnGauche, INPUT_PULLUP);
  pinMode(btnDroite, INPUT_PULLUP);
  pinMode(btnAv, INPUT_PULLUP);
  pinMode(btnAr, INPUT_PULLUP);



  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Start server
  server.begin();
}

void loop() {
  if (TimerCompteurPrincipal >= 100)      // toutes les 10ms
  {
    TimerCompteurPrincipal = TimerCompteurPrincipal - 100;
    stateBtnLaby_Z1.b_StateBtnLaby[0] = stateBtnLaby.b_StateBtnLaby[0];
    stateBtnLaby_Z1.b_StateBtnLaby[1] = stateBtnLaby.b_StateBtnLaby[1];
    stateBtnLaby_Z1.b_StateBtnLaby[2] = stateBtnLaby.b_StateBtnLaby[2];
    stateBtnLaby_Z1.b_StateBtnLaby[3] = stateBtnLaby.b_StateBtnLaby[3];
    ui_EtatBtn_Z1 = ui_EtatBtn;
    //stateBtnLaby_Z1.ui_StateBtnLaby = stateBtnLaby.ui_StateBtnLaby;
    stateBtnLaby.b_StateBtnLaby[0] = !digitalRead(btnGauche);
    stateBtnLaby.b_StateBtnLaby[1] = !digitalRead(btnDroite);
    stateBtnLaby.b_StateBtnLaby[2] = !digitalRead(btnAv);
    stateBtnLaby.b_StateBtnLaby[3] = !digitalRead(btnAr);
    ui_EtatBtn = tache_ConvertBtnState(stateBtnLaby.b_StateBtnLaby[0], stateBtnLaby.b_StateBtnLaby[1], stateBtnLaby.b_StateBtnLaby[2], stateBtnLaby.b_StateBtnLaby[3]);
    //Serial.println(stateBtnLaby.ui_StateBtnLaby);
  
    ws.cleanupClients();
    /*String res = String(stateBtnLaby.b_StateBtnLaby[0]) + String(stateBtnLaby.b_StateBtnLaby[1]) + String(stateBtnLaby.b_StateBtnLaby[2]) + String(stateBtnLaby.b_StateBtnLaby[3]);
    String res_Z1 = String(stateBtnLaby_Z1.b_StateBtnLaby[0]) + String(stateBtnLaby_Z1.b_StateBtnLaby[1]) + String(stateBtnLaby_Z1.b_StateBtnLaby[2]) + String(stateBtnLaby_Z1.b_StateBtnLaby[3]);
    Serial.println(res);*/
    /*if(stateBtnLaby_Z1.ui_StateBtnLaby != stateBtnLaby.ui_StateBtnLaby)
    {
      String res = String(stateBtnLaby.b_StateBtnLaby[0]) + String(stateBtnLaby.b_StateBtnLaby[1]) + String(stateBtnLaby.b_StateBtnLaby[2]) + String(stateBtnLaby.b_StateBtnLaby[3]);
      ws.textAll(res);
    }*/
    /*if(ui_EtatBtn_Z1 != ui_EtatBtn)
    {
      ws.textAll("LBR@" + String(ui_EtatBtn));
    }*/
  
    if(ui_EtatBtn != 0){
      ws.textAll("LBR@" + String(ui_EtatBtn));
    }
    else if(ui_EtatBtn_Z1 != ui_EtatBtn)
    {
      ws.textAll("LBR@" + String(ui_EtatBtn));
    }
  }

}
