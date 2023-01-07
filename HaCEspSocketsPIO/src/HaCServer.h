/**
 *
 * @file HaCServer.h
 * @date 03.01.2023
 * @author Harvy Aronales Costiniano
 * 
 * Copyright (c) 2023 Harvy Aronales Costiniano. All rights reserved.
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

#ifndef __HAC_HACSERVER_H_
#define __HAC_HACSERVER_H_


/* #region CONSTANT_DEFINITION */

/* #region Debug */
/* #endregion */

/* #endregion */

/* #region INTERNAL_DEPENDENCY */
#include "HaCClientInfo.h"
/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
#include <memory>
#include <lwip/tcp.h>
#include <IPAddress.h>
/* #endregion */

/* #region GLOBAL_DECLARATION */
#ifndef HAC_SERVER_MAX_SOCKET_CLIENTS
#define HAC_SERVER_MAX_SOCKET_CLIENTS   5
#endif

#define HAC_SERVER_DEF_PORT 5000
/* #endregion */

/* #region CLASS_DECLARATION */
class HaCServer
{
    public:            
        HaCServer();
        ~HaCServer();

        void setup(uint16_t port);
        void start();
        void stop();
        void broadCastMessage(const char *message);
        bool setPingWatchdog(bool enable = true);
        
        /* #region Event functions(ClientInfo Events) */
        void onReceive(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn);
        void onSent(std::function<void(uint16_t, HaCClientInfo*)> fn);
        void onError(std::function<void(uint16_t, HaCClientInfo*)> fn);
        void onPoll(std::function<void(HaCClientInfo*)> fn);
        void onClosed(std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> fn); 
        void onNewConnection(std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> fn);       
        /* #endregion */
        
    private:
        bool _enablePingWatchdog = true;
        uint16_t _port = HAC_SERVER_DEF_PORT;
        tcp_pcb *_listenerSoc = nullptr;
        IPAddress _ipAddr;
        std::vector<HaCClientInfo*> _clientInfos = std::vector<HaCClientInfo*>();
        HaCClientInfo *_clientInfo = nullptr;

        std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> _onReceiveFn;
        std::function<void(uint16_t, HaCClientInfo*)> _onSentFn;
        std::function<void(uint16_t, HaCClientInfo*)> _onErrorFn;
        std::function<void(HaCClientInfo*)> _onPollFn;
        std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> _onClosedFn;
        std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> _onNewConnectionFn;

        /* #region Private Lambda functions(ClientInfo Events) */
        void _clientInfo_onClosed(HaCClientInfo * clientInfo);
        void _clientInfo_onAccepted(HaCClientInfo * clientInfo);
        /* #endregion */

        long _accept(tcp_pcb* newpcb, long err);
        static long _accept(void *arg, tcp_pcb* newpcb, long err);
        //long _onReceive(struct tcp_pcb *tpcb,
        //                struct pbuf *p, err_t err);
        
        //static long _onReceive(void *arg, struct tcp_pcb *tpcb,
        //                       struct pbuf *p, err_t err);
};


#include "HaCServer-impl.h"

#endif
