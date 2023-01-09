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
        DBG_CB_HSOC("[HACCLIENT] Invalid port!");
        return;
    }
    
    ip_addr_t ip;
    if(!this->_parseIP(remoteIP, &ip))
    {
        DBG_CB_HSOC("[HACCLIENT] Invalid IP!");
        return;
    }
    
    this->_remotePort = remotePort;
    this->_remoteIP = &ip;
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

/**
     * Parse string IP and convert it to ip_addr_t
     * @param ipStr IP String
     * @param ip Converted IP pointer
     */
bool HaCClient::_parseIP(const char *ipStr, ip_addr_t *ip) 
{    
    uint16_t octet[4] = {0, 0, 0, 0};
    uint8_t i = 0;
    uint8_t validOctetCntr = 0;
    while(*ipStr)
    {
        if(!this->_isValidIPChar(*ipStr)) return false;   
        uint16_t tmp = 0, digitNo = 0;        
        bool isEnd = false;
        while(*ipStr != '.')    
        {
            if(!*ipStr) 
            {
                isEnd = true;
                break;              
            }            
            if(!this->_isValidIPChar(*ipStr)) return false;    
            tmp = tmp * 10 + (*ipStr - '0');            
            if(tmp > 255)
                return false;             
            digitNo++;
            ipStr++;
        }
        if(digitNo < 3) validOctetCntr++;
        octet[i] = tmp;            
        i++;                    

        if(isEnd)
            break;
        ipStr++;
    }
    if(validOctetCntr != 4)
        return false;
    
    *ip = IPADDR4_INIT_BYTES(octet[0], octet[1], octet[2], octet[3]);

    return true;     
}

/**
     * Check if it is a valid IP char
     * @param ipStr IP String
     * @param ip Converted IP pointer
     * @return True is the character is valid
     */
bool HaCClient::_isValidIPChar(char c)
{
    return ((c >= '0' && c <= '9') || c == '.');
}

/* #endregion */

/* #endregion */

