/**
 *
 * @file HaCEspSockets-impl.h
 * @date 29.12.2021
 * @author Harvy Aronales Costiniano
 *
 * Copyright (c) 2021 Harvy Aronales Costiniano. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


/* #region SELF_HEADER */
#include "HaCEspSockets.h"
/* #endregion */


/* #region CLASS_DEFINITION */

/* #region Public */
/**
     * Constructor.
     */
HaCEspSockets::HaCEspSockets()
{
}

/**
     * Destructor.
     */
HaCEspSockets::~HaCEspSockets()
{
     if(this->_socketServer != nullptr)     
          delete this->_socketServer;
     if(this->_socketClient != nullptr)     
          delete this->_socketClient;

}

/**
     * Setup Socket Server
     * @param port Socket server listening port
     */
void HaCEspSockets::setupServer(uint16_t port)
{
     DBG_CB_HSOC("[HACESPSOCKETS] Setting up the socket server..");
     this->_socketServer = new HaCServer();
     this->_serverListenPort = port;
     this->_socketServer->setup(this->_serverListenPort);       
     DBG_CB_HSOC("[HACESPSOCKETS] Server is listening..");

     this->_socketServer->onNewConnection(this->_server_onNewClientConnectionFn);
     this->_socketServer->onReceive(this->_server_clientOnDataArrivalFn);
     this->_socketServer->onClosed(this->_server_clientOnSocketClosedFn);
     this->_socketServer->onSent(this->_server_clientOnDataSentFn);
     this->_socketServer->onError(this->_server_clientOnSocketErrorFn);
     this->_socketServer->onPoll(this->_server_clientOnPollFn);
}

/**
     * Start Server function.
     */
void HaCEspSockets::startServer()
{     
     if(this->_socketServer)
          this->_socketServer->start();

     DBG_CB_HSOC("[HACESPSOCKETS] Server started..");
}

/**
     * Shutdown Socket Server
     */
void HaCEspSockets::shutdownServer()
{
     if(this->_socketServer)
          this->_socketServer->stop();
     
     DBG_CB_HSOC("[HACESPSOCKETS] Server Shutdown..");
}

/**
     * Server broadcast message to all connected client
     * @param message data message
     */
void HaCEspSockets::ServerBroadCast(const char *message)
{
     if(this->_socketServer)
          this->_socketServer->broadCastMessage(message);
}

/**
     * Server broadcast message to all connected client
     * @param message data message
     */
bool HaCEspSockets::setPingWatchdog(bool enable)
{
     if(this->_socketServer)
          return this->_socketServer->setPingWatchdog(enable);

     if(this->_socketClient)
          return this->_socketClient->setPingWatchdog(enable);

     return false;
}


/**
     * Setup a client socket
     * @param message data message
     */
void HaCEspSockets::setupClient(uint16_t remotePort, const char * remoteIP)
{    
     
     DBG_CB_HSOC("[HACESPSOCKETS] Setting up the client socket..");     
     this->_socketClient = new HaCClient(); 

     this->_socketClient->onReceive(this->_clientOnDataArrivalFn);
     this->_socketClient->onSent(this->_clientOnDataSentFn);
     this->_socketClient->onError(this->_server_clientOnSocketErrorFn);
     this->_socketClient->onPoll(this->_clientOnPollFn);
     this->_socketClient->onClosed(this->_clientOnSocketClosedFn);
     this->_socketClient->onConnected(this->_clientOnConnectedFn);
     
     this->_socketClient->setup(remotePort, remoteIP);
}

/**
     * Close client connection to remote server
     */
void HaCEspSockets::clientClose()
{    
     this->_socketClient->close(true);
}

/**
     * Client Send data
     * @param message data message
     * @return Send error state
     */
long HaCEspSockets::clientSend(const char *message)
{
     if(!this->_socketClient) return (long)0;

     return this->_socketClient->sendData(message);
     
}

/**
     * Client Connect
     * @param message data message  
     * @return True if connection is successful
     */
bool HaCEspSockets::clientConnect()
{
     return this->_socketClient->connect();     
}

/* #region Event functions(Client Events) */

/**
     * clientOnReceive Delegate function.           
     * @param fn clientOnReceive Callback function.
     */
void HaCEspSockets::clientOnDataArrival(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn)
{
     this->_clientOnDataArrivalFn = fn;
}

/**
     * clientOnDataSent Delegate function.           
     * @param fn clientOnDataSent Callback function.
     */
void HaCEspSockets::clientOnDataSent(std::function<void(uint16_t, HaCClientInfo*)> fn)
{
     this->_clientOnDataSentFn = fn;
}

/**
     * clientOnSocketError Delegate function.           
     * @param fn clientOnSocketError Callback function.
     */
void HaCEspSockets::clientOnSocketError(std::function<void(uint16_t, HaCClientInfo*)> fn)
{
     this->_clientOnSocketErrorFn = fn;
}

/**
     * clientOnPoll Delegate function.           
     * @param fn clientOnPoll Callback function.
     */
void HaCEspSockets::clientOnPoll(std::function<void(HaCClientInfo*)> fn)
{
     this->_clientOnPollFn = fn;
}

/**
     * clientOnSocketClosed Delegate function.           
     * @param fn clientOnSocketClosed Callback function.
     */
void HaCEspSockets::clientOnSocketClosed(std::function<void(HaCClientInfo*)> fn)
{
     this->_clientOnSocketClosedFn = fn;
}

/**
     * clientOnSocketClosed Delegate function.           
     * @param fn clientOnSocketClosed Callback function.
     */
void HaCEspSockets::clientOnConnected(std::function<void(HaCClientInfo*)> fn)
{
     this->_clientOnConnectedFn = fn;
}

/* #endregion */


/* #region Event functions(Server Events) */

/**
     * Server_clientOnReceive Delegate function.           
     * @param fn Server_clientOnReceive Callback function.
     */
void HaCEspSockets::Server_clientOnDataArrival(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn)
{
     this->_server_clientOnDataArrivalFn = fn;
}

/**
     * Server_clientOnSent Delegate function.           
     * @param fn Server_clientOnSent Callback function.
     */
void HaCEspSockets::Server_clientOnDataSent(std::function<void(uint16_t, HaCClientInfo*)> fn)
{
     this->_server_clientOnDataSentFn = fn;
}

/**
     * Server_clientOnError Delegate function.           
     * @param fn Server_clientOnError Callback function.
     */
void HaCEspSockets::Server_clientOnSocketError(std::function<void(uint16_t, HaCClientInfo*)> fn)
{
     this->_server_clientOnSocketErrorFn = fn;
}

/**
     * Server_clientOnPoll Delegate function.           
     * @param fn Server_clientOnPoll Callback function.
     */
void HaCEspSockets::Server_clientOnPoll(std::function<void(HaCClientInfo*)> fn)
{
     this->_server_clientOnPollFn = fn;
}

/**
     * Server_clientOnClosed Delegate function.           
     * @param fn Server_clientOnClosed Callback function.
     */
void HaCEspSockets::Server_clientOnSocketClosed(std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> fn)
{
     this->_server_clientOnSocketClosedFn = fn;
}

/**
     * Server_onNewClientConnection Delegate function.           
     * @param fn Server_onNewClientConnection Callback function.
     */
void HaCEspSockets::Server_onNewClientConnection(std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> fn)
{     
     this->_server_onNewClientConnectionFn = fn;
}

/* #endregion */

/* #endregion */

/* #region Private */
/* #endregion */

/* #endregion */




