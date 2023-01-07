/**
 *
 * @file HaCClientInfo.h
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

#ifndef __HAC_CLIENTINFO_H_
#define __HAC_CLIENTINFO_H_


/* #region CONSTANT_DEFINITION */

/* #region Debug */
/* #endregion */

/* #endregion */

/* #region INTERNAL_DEPENDENCY */
#include "HaCEspSockets.h"
/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
#include <lwip/tcp.h>
#include <IPAddress.h>
/* #endregion */

/* #region GLOBAL_DECLARATION */
#define HAC_SOCCLIENT_POLL_INTVAL_PING 10
/* #endregion */

/* #region CLASS_DECLARATION */
class HaCClientInfo
{    
    public:
        HaCClientInfo();
        HaCClientInfo(tcp_pcb* pcb, std::function<void(HaCClientInfo*, tcp_pcb*)> fn);
        ~HaCClientInfo();
        void onReceive(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn);
        void onSent(std::function<void(uint16_t, HaCClientInfo*)> fn);
        void onError(std::function<void(uint16_t, HaCClientInfo*)> fn);
        void onPoll(std::function<void(HaCClientInfo*)> fn);
        void onClosed(std::function<void(HaCClientInfo*)> fn);        
        void onConnected(std::function<void(HaCClientInfo*)> fn);  
        void setConnectionId(uint8_t id);
        uint8_t getConnectionId();
        
        bool setPingWatchdog(bool enable = true);
        bool connect(IPAddress ip, uint16_t port);        
        uint8_t socketState() const;
        long sendData(const char * buffer);
        void close(bool forceClose = false);
        void abort();
        void getRemoteIP(char *bufferIP);
        void setupClientSocket(tcp_pcb* soc);
        

    private:
        tcp_pcb *_soc = nullptr;
        bool _ignoreCRNLReceiveData = true;
        bool _isRemoteEndNotOk = false;
        bool _enablePingWatchdog = true;
        uint8_t _connectionNotOkCntr = 0;
        uint8_t _pollingCounter = 0;  
        
        uint8_t _connectionId = 254; // Unasssigned ID
        //uint64_t _totalBytesReceive = 0;
        std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> _onReceiveFn;
        std::function<void(uint16_t, HaCClientInfo*)> _onSentFn;
        std::function<void(uint16_t, HaCClientInfo*)> _onErrorFn;
        std::function<void(HaCClientInfo*)> _onPollFn;
        std::function<void(HaCClientInfo*)> _onClosedFn;
        std::function<void(HaCClientInfo*, tcp_pcb*)> _onAcceptedFn;
        std::function<void(HaCClientInfo*)> _onConnectedFn;

        void _setup();

        static long _onReceive(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err);
        long _onReceive(struct tcp_pcb *tpcb,
                        struct pbuf *p, err_t err);
        long _onSent(struct tcp_pcb *tpcb,
                              u16_t len);
        static long _onSent(void *arg, struct tcp_pcb *tpcb,
                              u16_t len);
        void _onError(err_t err);
        static void _onError(void *arg, err_t err);
        long _onPoll(struct tcp_pcb *tpcb);
        static long _onPoll(void *arg, struct tcp_pcb *tpcb);
        long _connected(struct tcp_pcb *pcb, err_t err);
        static long _connected(void* arg, struct tcp_pcb *pcb, err_t err);
        
        
};

#include "HaCClientInfo-impl.h"

#endif
