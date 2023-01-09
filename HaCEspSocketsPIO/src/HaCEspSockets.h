/**
 *
 * @file HaCEspSockets.h
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

#ifndef __HAC_SOCKETS_H_
#define __HAC_SOCKETS_H_


/* #region CONSTANT_DEFINITION */

/* #region Debug */
//#if defined(DEBUG_ESP_PORT) || defined(HAC_ENABLE_DEBUG)
#ifdef HAC_ENABLE_DEBUG
#define DBG_CB_HSOC(msg)      \
    {                                \
        Serial.println(msg); \
    }
#define DBG_CB_HSOC2(msg, ...)               \
    {                                               \
        Serial.printf(msg, ##__VA_ARGS__); \
    }
#else

#define DBG_CB_HSOC(msg)
#define DBG_CB_HSOC2(msg, ...)

#endif

/* #endregion */

/* #endregion */

/* #region INTERNAL_DEPENDENCY */
#include "HaCServer.h"
#include "HaCClientInfo.h"
#include "HaCClient.h"
/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
/* #endregion */

/* #region GLOBAL_DECLARATION */

/* #endregion */

/* #region CLASS_DECLARATION */


class HaCEspSockets
{
public:

    HaCEspSockets(); // Constructor
    ~HaCEspSockets();

    void setupServer(uint16_t port);
    void startServer();
    void shutdownServer();
    void ServerBroadCast(const char *message);
    bool setPingWatchdog(bool enable = true);
    /* #region Event functions(Server Events) */
    void Server_clientOnDataArrival(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn);
    void Server_clientOnDataSent(std::function<void(uint16_t, HaCClientInfo*)> fn);
    void Server_clientOnSocketError(std::function<void(uint16_t, HaCClientInfo*)> fn);
    void Server_clientOnPoll(std::function<void(HaCClientInfo*)> fn);
    void Server_clientOnSocketClosed(std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> fn); 
    void Server_onNewClientConnection(std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> fn);       
    /* #endregion */

    void setupClient(uint16_t remotePort, const char * remoteIP);
    long clientSend(const char *message);
    bool clientConnect();
    void clientClose();    

    /* #region Event functions(Client Events) */
    void clientOnDataArrival(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn);
    void clientOnDataSent(std::function<void(uint16_t, HaCClientInfo*)> fn);
    void clientOnSocketError(std::function<void(uint16_t, HaCClientInfo*)> fn);
    void clientOnPoll(std::function<void(HaCClientInfo*)> fn);
    void clientOnSocketClosed(std::function<void(HaCClientInfo*)> fn);     
    void clientOnConnected(std::function<void(HaCClientInfo*)> fn);
    /* #endregion */


private: 
    uint16_t _serverListenPort;
    HaCServer *_socketServer = nullptr;
    HaCClient *_socketClient = nullptr;
    
    std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> _server_clientOnDataArrivalFn;
    std::function<void(uint16_t, HaCClientInfo*)> _server_clientOnDataSentFn;
    std::function<void(uint16_t, HaCClientInfo*)> _server_clientOnSocketErrorFn;
    std::function<void(HaCClientInfo*)> _server_clientOnPollFn;
    std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> _server_clientOnSocketClosedFn;
    std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> _server_onNewClientConnectionFn;

    std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> _clientOnDataArrivalFn;
    std::function<void(uint16_t, HaCClientInfo*)> _clientOnDataSentFn;
    std::function<void(uint16_t, HaCClientInfo*)> _clientOnSocketErrorFn;
    std::function<void(HaCClientInfo*)> _clientOnPollFn;
    std::function<void(HaCClientInfo*)> _clientOnSocketClosedFn;
    std::function<void(HaCClientInfo*)> _clientOnConnectedFn;
};


/* #endregion */


#include "HaCEspSockets-impl.h"

#endif
