/**
 *
 * @file HaCClient.h
 * @date 05.01.2023
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

#ifndef __HAC_HACCLIENT_H_
#define __HAC_HACCLIENT_H_

/* #region CONSTANT_DEFINITION */

/* #region Debug */
/* #endregion */

/* #endregion */

/* #region INTERNAL_DEPENDENCY */
#include "HaCClientInfo.h"
#include "ESP8266WiFi.h"
/* #endregion */

/* #region EXTERNAL_DEPENDENCY */
#include <Arduino.h>
#include <lwip/tcp.h>
#include <IPAddress.h>
/* #endregion */

/* #region GLOBAL_DECLARATION */
/* #endregion */

/* #region CLASS_DECLARATION */
class HaCClient : public HaCClientInfo
{
public:
    HaCClient(); // Constructor
    ~HaCClient();   

    void setup(uint16_t remotePort, const char * remoteIP); // Constructor

    void onReceive(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn)
    {
        HaCClientInfo::onReceive(fn);
    }
    void onSent(std::function<void(uint16_t, HaCClientInfo*)> fn)
    {
        HaCClientInfo::onSent(fn);
    }
    void onError(std::function<void(uint16_t, HaCClientInfo*)> fn)
    {
        HaCClientInfo::onError(fn);
    }
    void onPoll(std::function<void(HaCClientInfo*)> fn)
    {
        HaCClientInfo::onPoll(fn);
    }
    void onClosed(std::function<void(HaCClientInfo*)> fn)
    {
        HaCClientInfo::onClosed(fn);
    }
    void onConnected(std::function<void(HaCClientInfo*)> fn)
    {
        HaCClientInfo::onConnected(fn);
    }    
    bool connect()
    {
        return HaCClientInfo::connect(this->_remoteIP, this->_remotePort);    
    }
    void close(bool forceClose = false)
    {
        HaCClientInfo::close(forceClose);
    }
    long sendData(const char *data)
    {
        return HaCClientInfo::sendData(data);
    }
    bool setPingWatchdog(bool enable = true)
    {
        return HaCClientInfo::setPingWatchdog(enable);
    }

private:    
    tcp_pcb *_soc = nullptr;
    IPAddress _remoteIP;
    uint16_t _remotePort;

};
/* #endregion */

#include "HaCClient-impl.h"

#endif
