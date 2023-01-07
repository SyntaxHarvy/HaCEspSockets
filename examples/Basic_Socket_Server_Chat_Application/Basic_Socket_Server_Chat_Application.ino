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
     * On New Client Connection Callback Function Prototype.
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     * @param buffer Data buffer receive by the server from the remote end client
     * @param packLen Data packet length
     * @param totalLen Buffer total length
     */     
void Server_clientOnDataArrival_CB(HaCClientInfo * clientInfo, 
    const char* buffer, uint16_t packLen, uint32_t totalLen);  
    
    /**
     * On Data Arrival Callback Function Prototype.
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void Server_onNewClientConnection_CB(HaCClientInfo * clientInfo, std::vector<HaCClientInfo*> clientInfos);   
   
    /**
     * On Data Sent Callback Function Prototype.
     * @param len Data packet length sent to the client
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     */
void Server_clientOnDataSent_CB(uint16_t len, HaCClientInfo * clientInfo);

    /**
     * On Socket Closed Callback Function Prototype.     
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */    
void Server_clientOnSocketClosed_CB(HaCClientInfo * clientInfo, std::vector<HaCClientInfo*> clientInfos);

    /**
     * On Socket Error Callback Function Prototype.     
     * @param err Socket error for the associated client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void Server_clientOnError_CB(uint32_t err, HaCClientInfo * clientInfo);

    /**
     * On Poll Callback Function Prototype.     
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void Server_clientOnPoll_CB(HaCClientInfo * clientInfo);
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
  haCSockets.Server_onNewClientConnection(Server_onNewClientConnection_CB);
  haCSockets.Server_clientOnDataArrival(Server_clientOnDataArrival_CB);
  haCSockets.Server_clientOnDataSent(Server_clientOnDataSent_CB);
  haCSockets.Server_clientOnSocketClosed(Server_clientOnSocketClosed_CB);
  haCSockets.Server_clientOnSocketError(Server_clientOnError_CB);
  haCSockets.Server_clientOnPoll(Server_clientOnPoll_CB);
  
  /* #endregion */ 

  /* #region  Setting up and starting the socket server */
  //Change your_server_port to actual port value
  haCSockets.setupServer(your_server_port);   //Server setup
  //haCSockets.setPingWatchdog(false);    // To Disable the ping watchdog
  haCSockets.startServer();       //Starting the server
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
void Server_clientOnDataArrival_CB(HaCClientInfo * clientInfo, 
    const char* buffer, uint16_t packLen, uint32_t totalLen)
{

    DBG_CB_HSOC2("\n[Main] Incoming data from client connection id = %d", clientInfo->getConnectionId()); 
    DBG_CB_HSOC2("\n[Main] Data receive = %s", buffer);
    DBG_CB_HSOC2("\n[Main] Packet length = %d ", packLen);
    DBG_CB_HSOC2("\n[Main] Total Data length = %lu ", (unsigned long)totalLen);
    
    if(strcmp(buffer, "close") == 0)    //If data receive is equal to close
      clientInfo->close();              //Close the existing client
    if(strcmp(buffer, "send_all") == 0)    
        haCSockets.ServerBroadCast("[SERVER] I am instructed to broadcast a message, how are you today?");
    if(strcmp(buffer, "close_all") == 0)    
        haCSockets.ServerBroadCast("close");
}

/**
     * On New Client Connection Callback Function.
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void Server_onNewClientConnection_CB(HaCClientInfo * clientInfo, std::vector<HaCClientInfo*> clientInfos)
{
  DBG_CB_HSOC2("\n[Main] New Client with connection id = %d has been accepted..\n", clientInfo->getConnectionId());

  DBG_CB_HSOC("[Main] Active Connections \n");
  for(auto p : clientInfos)
    DBG_CB_HSOC2("\n[Main] Client with connection id = %d", p->getConnectionId());
}

/**
     * On Data Sent Callback Function.
     * @param len Data packet length sent to the client
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     */
void Server_clientOnDataSent_CB(uint16_t len, HaCClientInfo * clientInfo)
{
  DBG_CB_HSOC2("\n[Main] Sent to the client = %d with total length = %lu ..", clientInfo->getConnectionId(), (unsigned long)len);  
}

/**
     * On Socket Closed Callback Function.     
     * @param clientInfo HaCClientInfo unique pointer for each client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void Server_clientOnSocketClosed_CB(HaCClientInfo * clientInfo, std::vector<HaCClientInfo*> clientInfos)
{
  DBG_CB_HSOC2("\n[Main] Client with connection id = %d has been closed \n", clientInfo->getConnectionId());

  DBG_CB_HSOC("[Main] Active Connections:");
  for(auto p : clientInfos)
    DBG_CB_HSOC2("\n[Main] Client with connection id = %d", p->getConnectionId());
  
}

/**
     * On Socket Error Callback Function.     
     * @param err Socket error for the associated client connection
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void Server_clientOnError_CB(uint32_t err, HaCClientInfo * clientInfo)
{
  DBG_CB_HSOC2("\n[Main] Client = %d, has error = %lu ..", clientInfo->getConnectionId(), (unsigned long)err);
}

/**
     * On Poll Callback Function Prototype.     
     * @param clientInfos Array vectors of the HaCClientInfo connected to the server
     */
void Server_clientOnPoll_CB(HaCClientInfo * clientInfo)
{
  //DBG_CB_HSOC2("\n Client with connection id = %d is polling..", clientInfo->getConnectionId());
}

/* #endregion */ 
