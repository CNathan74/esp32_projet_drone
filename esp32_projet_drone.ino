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

uint8_t ui_Sequence = 0;

uint8_t ui_EtatBtn_LBR_CTRL = 0;
uint8_t ui_EtatBtn_LBR_CTRL_Z1 = 0;

uint8_t ui_NbBtnDejaAppui_LBR = 0;
uint8_t ui_NbBtnDejaAppui_LBR_Z1 = 0;
boolean b_BtnDejaAppui_LBR[3] = {false, false, false};
//boolean b_BtnDejaAppui_LBR_Z1[3] = {false; false; false};
const uint8_t ui_PinBtn_LBR[3] = {LBR_BTN_1, LBR_BTN_2, LBR_BTN_3};
const uint8_t ui_PinBtn_LED_LBR[3] = {LBR_BTN_1_LED, LBR_BTN_2_LED, LBR_BTN_3_LED};

uint8_t ui_NbBtnDejaAppui_DRT = 0;
uint8_t ui_NbBtnDejaAppui_DRT_Z1 = 0;
boolean b_BtnDejaAppui_DRT[3] = {false, false, false};

uint16_t ui_NbTour_PMP = 0;
uint16_t ui_NbTour_PMP_Z1 = 0;
boolean b_EtatAnemo_PMP = false;
boolean b_EtatAnemo_PMP_Z1 = false;
boolean b_Changement_PMP = true;

uint8_t ui_EtatBtn_DRT_CTRL = 0;
uint8_t ui_EtatBtn_DRT_CTRL_Z1 = 0;

const uint8_t ui_PinBtn_SLD[3] = {SLD_RETRO_1, SLD_RETRO_2, SLD_RETRO_3};
boolean b_BtnEtat_SLD[3] = {false, false, false};
boolean b_BtnEtat_SLD_Z1[3] = {false, false, false};
boolean b_Changement_SLD = false;


StateBtnLaby stateBtnLaby;
StateBtnLaby stateBtnLaby_Z1;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, uint32_t client_id) {
  int i;
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String d = "";
    for (int i = 0; i < len; i++)
    {
      d +=(char)data[i];
    }
    Serial.println(d);
    if(d.indexOf("@") != -1){
  		String srv = d.substring(0, d.indexOf("@"));
  		if(srv == "CMD")
  		{
  			String cmd = d.substring(d.indexOf("@") + 1, len);
  			Serial.println(cmd);
  			if(cmd == "RAZ")
  			{
  				tache_RAZ();
  				//ws.text(client_id, "RAZ");
  				ws.textAll("RAZ");
  			}
  			else
  			{
  				ws.text(client_id, "Cmd inconnue");
  			}
  
  		}
     else if(srv = "IO")
     {
        String io = d.substring(d.indexOf("@") + 1, d.indexOf(":"));
        String value = d.substring(d.indexOf(":") + 1, len);
        boolean temp;
        Serial.print(io);
        Serial.println(value);
        if(io == "DRT_INFO")
        {
           ui_NbBtnDejaAppui_DRT_Z1 = ui_NbBtnDejaAppui_DRT;
            for(i = 0; i < 3; i++)
            {
              temp = value.substring(i, i + 1).toInt();
              Serial.print(i);
              Serial.print(" : ");
              Serial.print(temp);
              Serial.print(" - ");
              Serial.println(temp == 1);
              if((b_BtnDejaAppui_DRT[i] == false) && (temp == 1))
              {
                if(i > 0)
                {
                  if(b_BtnDejaAppui_DRT[i - 1] == true)
                  {
                    b_BtnDejaAppui_DRT[i] = true;
                    ui_NbBtnDejaAppui_DRT++;
                    ws.textAll("DRT@INFO:" + String(ui_NbBtnDejaAppui_DRT));
                  }
                }
                else
                {
                    b_BtnDejaAppui_DRT[i] = true;
                    ui_NbBtnDejaAppui_DRT++;
                    ws.textAll("DRT@INFO:" + String(ui_NbBtnDejaAppui_DRT));
                }  
              }
            }
        }
        if(io == "DRT_CTRL")
        {
          ui_EtatBtn_DRT_CTRL_Z1 = ui_EtatBtn_DRT_CTRL;
          ui_EtatBtn_DRT_CTRL = value.toInt();
          if(ui_EtatBtn_DRT_CTRL_Z1 != ui_EtatBtn_DRT_CTRL)
          {
            ws.textAll("DRT@CTRL:" + String(ui_EtatBtn_DRT_CTRL));
          }
          else if(ui_EtatBtn_DRT_CTRL != 0)
          {
            ws.textAll("DRT@CTRL:" + String(ui_EtatBtn_DRT_CTRL));
          }
        }
     }
  		else
  		{
  			ws.text(client_id, "Code service inconnu");
  		}
    }


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
      handleWebSocketMessage(arg, data, len, client->id());
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

void tache_RAZ() {
  uint8_t i;
  ui_NbBtnDejaAppui_LBR = 0;
  ui_NbBtnDejaAppui_DRT = 0;
  for(i = 0; i < 3; i++)
  {
    b_BtnDejaAppui_LBR[i] = false;
    b_BtnDejaAppui_DRT[i] = false;
  }
  ui_NbTour_PMP = 0;
}

//todo pas beau
uint8_t tache_ConvertBtnState_4(boolean b_Btn1, boolean b_Btn2, boolean b_Btn3, boolean b_Btn4)
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

//todo pas beau
uint8_t tache_ConvertBtnState_3(boolean b_Btn1, boolean b_Btn2, boolean b_Btn3)
{
  uint8_t ui_result = 0;
  if(b_Btn1 == true)
  {
    ui_result = ui_result + 1;
  }
  if(b_Btn2 == true)
  {
    ui_result = ui_result + 2;
  }
  if(b_Btn3 == true)
  {
    ui_result = ui_result + 4;
  }
  return ui_result;
}

void setup(){
  uint8_t i;
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(LBR_BTN_GAUCHE, INPUT_PULLUP);
  pinMode(LBR_BTN_DROITE, INPUT_PULLUP);
  pinMode(LBR_BTN_AV, INPUT_PULLUP);
  pinMode(LBR_BTN_AR, INPUT_PULLUP);
    
  for(i = 0; i < 3; i++)
  { pinMode(ui_PinBtn_LBR[i], INPUT_PULLUP);
    pinMode(ui_PinBtn_LED_LBR[i], OUTPUT);  }
  
  pinMode(PMP_ANEMO, INPUT_PULLUP);

  pinMode(SLD_RETRO_1, INPUT_PULLUP);
  pinMode(SLD_RETRO_2, INPUT_PULLUP);
  pinMode(SLD_RETRO_3, INPUT_PULLUP);



  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Start server
  server.begin();
}

void loop() {
  uint8_t i;
  b_EtatAnemo_PMP_Z1 = b_EtatAnemo_PMP;
  b_EtatAnemo_PMP = !digitalRead(PMP_ANEMO);
  if((b_EtatAnemo_PMP == false) && (b_EtatAnemo_PMP_Z1 == true))
  {
    if(ui_NbTour_PMP < 65535)
    {
      ui_NbTour_PMP_Z1 = ui_NbTour_PMP;
      ui_NbTour_PMP++;
      b_Changement_PMP = true;
    }
  }
  if (TimerCompteurPrincipal >= FREQUENCE_TIMER_PRINCIPAL)      // toutes les 10ms
  {
    TimerCompteurPrincipal = TimerCompteurPrincipal - FREQUENCE_TIMER_PRINCIPAL;
    ui_EtatBtn_LBR_CTRL_Z1 = ui_EtatBtn_LBR_CTRL;
    ui_EtatBtn_LBR_CTRL = tache_ConvertBtnState_4(!digitalRead(LBR_BTN_GAUCHE), !digitalRead(LBR_BTN_DROITE), !digitalRead(LBR_BTN_AV), !digitalRead(LBR_BTN_AR));

    for(i = 0; i < 3; i++)
    {
      //b_BtnDejaAppui_LBR_Z1[i] = b_BtnDejaAppui_LBR[i];
      if((b_BtnDejaAppui_LBR[i] == false) && ((!digitalRead(ui_PinBtn_LBR[i])) == true))
      {
        Serial.print(i);
        b_BtnDejaAppui_LBR[i] = true;
        ui_NbBtnDejaAppui_LBR++;        
      }
    }

    
    //Serial.println(stateBtnLaby.ui_StateBtnLaby);
  
    ws.cleanupClients();    
    if(ui_EtatBtn_LBR_CTRL_Z1 != ui_EtatBtn_LBR_CTRL)
    {
      ws.textAll("LBR@CTRL:" + String(ui_EtatBtn_LBR_CTRL));
    }
    else if(ui_EtatBtn_LBR_CTRL != 0)
    {
      if(ui_Sequence == 0)
      {
        ws.textAll("LBR@CTRL:" + String(ui_EtatBtn_LBR_CTRL));
      }
    }

    if(ui_NbBtnDejaAppui_LBR != ui_NbBtnDejaAppui_LBR_Z1)
    {
      ui_NbBtnDejaAppui_LBR_Z1 = ui_NbBtnDejaAppui_LBR;
      ws.textAll("LBR@INFO:" + String(ui_NbBtnDejaAppui_LBR));
    }

    if((ui_Sequence == ((NB_PERIODE_SEQUENCEUR / 6) * 2)) && (b_Changement_PMP == true))
     {
        b_Changement_PMP = false;
        ws.textAll("PMP@INFO:" + String(ui_NbTour_PMP));
    }


    
    if(ui_Sequence == ((NB_PERIODE_SEQUENCEUR / 6) * 5))
    {
      b_Changement_SLD = false;
      for(i = 0; i < 3; i++)
      {
        b_BtnEtat_SLD_Z1[i] = b_BtnEtat_SLD[i];
        b_BtnEtat_SLD[i] = !digitalRead(ui_PinBtn_SLD[i]);
        if(b_BtnEtat_SLD_Z1[i] != b_BtnEtat_SLD[i])
        {
          b_Changement_SLD = true;
        }
      }
      if(b_Changement_SLD == true)
      {
        ws.textAll("SLD@INFO:" + String(b_BtnEtat_SLD[0]) + String(b_BtnEtat_SLD[1]) + String(b_BtnEtat_SLD[2]));
      }
    }

    for(i = 0; i < 3; i++)
    {
      digitalWrite(ui_PinBtn_LED_LBR[i], b_BtnDejaAppui_LBR[i]);
    }
    
    
    if(ui_Sequence < NB_PERIODE_SEQUENCEUR)
    {
      ui_Sequence++;      
    }
    else
    {
      ui_Sequence = 0;
    }
  }

}
