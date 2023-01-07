/**
 *
 * @file HaCClient-impl.h
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


/* #region SELF_HEADER */
#include "HaCClient.h"
/* #endregion */


/* #region CLASS_DEFINITION */

/* #region Public */
/**
     * Constructor.
     */
HaCClient::HaCClient()
{

}

/**
     * Setup client.
     * @param remotePort server remote port
     * @param remoteIP server remote IP
     */
void HaCClient::setup(uint16_t remotePort, const char * remoteIP)
{
    
    tcp_pcb *soc = tcp_new();         
    if(!soc)
        return;
    
    if(remotePort <= 0) 
    {   
        DBG_CB_HSOC("[HACCLIENT] Invalid IP port!");
        return;
    }
        
    this->_remotePort = remotePort;
    
    if (!WiFi.hostByName(remoteIP, this->_remoteIP, 1000))
    {
        DBG_CB_HSOC("[HACCLIENT] Invalid IP address!");
        return;
    }
    
    soc->remote_ip = *this->_remoteIP;
    soc->remote_port = this->_remotePort;
    
    HaCClientInfo::setupClientSocket(soc);
    DBG_CB_HSOC("[HACCLIENT] Client socket setup successfully.."); 
}

/**
     * Destructor
     */
HaCClient::~HaCClient() 
{
     DBG_CB_HSOC("[HACCLIENT] Destroying HaCClient..");          
}

/* #endregion */

/* #region Private */



/* #endregion */

/* #endregion */

