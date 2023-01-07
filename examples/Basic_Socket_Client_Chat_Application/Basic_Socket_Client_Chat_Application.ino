/* #region Debug */
/*
  This will be useful if you want to see the different defined serial print debug on the library for tracing and troubleshootng purposes. 
  Warning : Make sure to write this piece of code before the library headers.
  Note : During production and if you want to disable the serial print debug then just comment out this piece of code in your main sketch program.     
*/
#define HAC_ENABLE_DEBUG
/* #endregion */

/* #region Including the Library */
/*
  - Include ESP8266WiFi.h for esp8266 wifi and for the HaCEspSockets library dependency
  - Inlude the library header HaCEspSockets.h (https://github.com/SyntaxHarvy/HaCEspSockets)
*/
#include <Arduino.h>
#include <HaCEspSockets.h>
#include "ESP8266WiFi.h"
/* #endregion */

/* #region Instantiate the library */
HaCEspSockets haCSockets;
/* #endregion */ 

/* #region Declaration of Callback Function Prototypes */
    /**
     * On Data Arrival Callback Function Prototype.
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     * @param buffer Data buffer receive by the server from the remote end client
     * @param packLen Data packet length
     * @param totalLen Buffer total length
     */
void clientOnDataArrival_CB(HaCClientInfo* clientInfo, 
  const char* buffer, uint16_t packetLength, uint32_t totalLength);    
    
    /**
     * On Connected Callback Function Prototype.
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void clientOnConnected_CB(HaCClientInfo * clientInfo);
    
    /**
     * On Data Sent Callback Function Prototype.
     * @param len Data packet length sent to the client
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     */
void clientOnDataSent_CB(uint16_t len, HaCClientInfo * clientInfo);
    
    /**
     * On Socket Closed Callback Function Prototype.     
     * @param clientInfo HaCClientInfo unique pointer for each client connection     
     */    
void clientOnSocketClosed_CB(HaCClientInfo * clientInfo);
    
    /**
     * On Socket Error Callback Function Prototype.     
     * @param err Socket error for the associated client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void clientOnSocketError_CB(uint32_t err, HaCClientInfo * clientInfo);

    /**
     * On Poll Callback Function Prototype.     
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void clientOnPoll_CB(HaCClientInfo * clientInfo);
/* #endregion */ 

/* #region Global Wifi Access Definition */
const char* ssid = "myssid";              //Change this to your correct wifi ssid
const char* password =  "mypassword";     //Change this to your correct wifi password
/* #endregion */ 

/* #region Buildng the setup Function */
void setup() {  
  /* #region Wifi typical initialization */
  Serial.begin(115200);  
  delay(1000);


  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    DBG_CB_HSOC("[Main] Connecting..");
  }
  DBG_CB_HSOC2("[Main] Connected to WiFi. IP: %s \n", WiFi.localIP().toString().c_str())
  /* #endregion */ 

  /* #region HaCEspSockets events initialization */
  haCSockets.clientOnDataArrival(clientOnDataArrival_CB);
  haCSockets.clientOnDataSent(clientOnDataSent_CB);
  haCSockets.clientOnSocketError(clientOnSocketError_CB);
  haCSockets.clientOnPoll(clientOnPoll_CB);
  haCSockets.clientOnSocketClosed(clientOnSocketClosed_CB);
  haCSockets.clientOnConnected(clientOnConnected_CB);  
  /* #endregion */ 

  /* #region  Setting up and connecting with the client socket */
  //Change your_server_port to actual port value and your_server_ip to your actual server ip
  haCSockets.setupClient(your_server_port, "your_server_ip");      
  haCSockets.setPingWatchdog(false);                               // To Disable the ping watchdog
  haCSockets.clientConnect();                                      //Initiate connection to the server
  /* #endregion */ 

}
/* #endregion */ 

void loop() {}

/* #region Definition of Callback Function Prototypes */

/**
     * On Data Arrival Callback Function.
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     * @param buffer Data buffer receive by the server from the remote end client
     * @param packLen Data packet length
     * @param totalLen Buffer total length
     */
void clientOnDataArrival_CB(HaCClientInfo* clientInfo, const char* data, uint16_t packetLength, uint32_t totalLength)
{
  char ip[50];
  memset(ip, 0, 50);
  clientInfo->getRemoteIP(ip);
  Serial.printf("\n[Main] Remote IP = %s ", &ip[0]);
  Serial.printf("\n[Main] Data receive = %s ", &data[0]);

  if(strcmp(data, "close") == 0)  //If data receive is equal to close
    haCSockets.clientClose();     //Close the connection to the server
}

/**
     * On Connected Callback Function.
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     */
void clientOnConnected_CB(HaCClientInfo * clientInfo)
{     
  char ip[50];
  memset(ip, 0, 50);
  clientInfo->getRemoteIP(ip);

  DBG_CB_HSOC2("\n[Main] Client is connected to remote ip = %s", &ip[0]);  
  haCSockets.clientSend("From client");
}

/**
     * On Data Sent Callback Function Prototype.
     * @param len Data packet length sent to the client
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     */
void clientOnDataSent_CB(uint16_t len, HaCClientInfo * clientInfo)
{
  char ip[50];
  memset(ip, 0, 50);
  clientInfo->getRemoteIP(ip);
  
  DBG_CB_HSOC2("\n[Main] Sent to the server = %s with total length = %lu ..", &ip[0], (unsigned long)len);  
}

/**
     * On Socket Closed Callback Function Prototype.     
     * @param clientInfo HaCClientInfo unique pointer for each client connection     
     */    
void clientOnSocketClosed_CB(HaCClientInfo * clientInfo)
{
  char ip[50];
  memset(ip, 0, 50);
  clientInfo->getRemoteIP(ip);

  DBG_CB_HSOC2("\n[Main] Client has been disconnected from remote ip = %s", &ip[0]);  
}

/**
     * On Socket Error Callback Function Prototype.     
     * @param err Socket error for the associated client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void clientOnSocketError_CB(uint32_t err, HaCClientInfo * clientInfo)
{
  char ip[50];
  memset(ip, 0, 50);
  clientInfo->getRemoteIP(ip);

  DBG_CB_HSOC2("\n[Main] Client connecting to remote ip = %s has an error = %lu ..", &ip[0], (unsigned long)err);  
}

/**
     * On Poll Callback Function Prototype.     
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void clientOnPoll_CB(HaCClientInfo * clientInfo)
{
  //Take note that this event will be fire every 1 second once the connection to the server 
  //will be established
  char ip[50];
  memset(ip, 0, 50);
  clientInfo->getRemoteIP(ip);

  //DBG_CB_HSOC2("\nClient is polling from remote ip = %s", &ip[0]);  
}
/* #endregion */ 
