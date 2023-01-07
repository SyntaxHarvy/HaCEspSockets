/**
 *
 * @file HaCServices-impl
 * @date 29.12.2021
 * @author Harvy Aronales Costiniano
 *
 * Copyright (c) 2021 Harvy Aronales Costiniano. All rights reserved.
 *
 * This library is free software; you can redisocstribute it and/or
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
#include "HaCServer.h"
/* #endregion */

/* #region CLASS_DEFINITION */

/* #region Public */
/**
     * Server Constructor.
     */
HaCServer::HaCServer()
{
}

/**
     * Server Destructor.
     */
HaCServer::~HaCServer()
{
    if(this->_clientInfo != nullptr)
        delete this->_clientInfo;
}

/**
     * Set Ping watchdog to enable or disable
     * @param enable True to enable or Flase to Disable
     * @return Return ping watchdog enable state
     */
bool HaCServer::setPingWatchdog(bool enable)
{    
     this->_enablePingWatchdog = enable;
     return this->_enablePingWatchdog;
}


/**
     * Socket Server Setup.
     * @param port Socket port number
     */
void HaCServer::setup(uint16_t port) 
{    
    this->_port = port;
    this->_ipAddr = IP_ANY_TYPE;
}

/**
     * Server start listening for incoming connection
     */
void HaCServer::start()
{
    this->stop();

    tcp_pcb * lsoc = tcp_new();
    if(!lsoc)
        return;
    

    lsoc->so_options |= SOF_REUSEADDR;

    if (tcp_bind(lsoc, this->_ipAddr, this->_port) != ERR_OK) {
        tcp_close(lsoc);
        return;
    }

    tcp_pcb *listenSoc = tcp_listen_with_backlog(lsoc, HAC_SERVER_MAX_SOCKET_CLIENTS);

    if(!listenSoc)
    {
        tcp_close(lsoc);
        return;
    }
    

    this->_listenerSoc = listenSoc;

    tcp_accept(this->_listenerSoc, &HaCServer::_accept);
    tcp_arg(this->_listenerSoc, (void*) this);

    DBG_CB_HSOC2("\n[HACSERVER] Server started and listening at port = %d \n", this->_port);
}

/**
     * Server stop from listening
     */
void HaCServer::stop()
{
    if(!this->_listenerSoc)
        return;
    
    tcp_close(this->_listenerSoc);
    this->_listenerSoc = nullptr;

    if(this->_clientInfos.size() >=1)
    {
        for(auto p : this->_clientInfos)
        {
            if(p != nullptr)
                delete p;
        }
    }

    this->_clientInfos.clear();
    this->_clientInfos = std::vector<HaCClientInfo*>();
}

/**
     * Broadcast message to all connected clients
     */
void HaCServer::broadCastMessage(const char * message)
{
    if(this->_clientInfos.size() <= 0 ) return;

    for(auto p : this->_clientInfos)
    {
        DBG_CB_HSOC2("\n[HACSERVER] Sending message from client connection id = %d", p->getConnectionId());
        p->sendData(message);
    }
}

/**
     * onReceive Delegate function.           
     * @param fn onReceive Callback function.
     */
void HaCServer::onReceive(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn)
{
     this->_onReceiveFn = fn;
}

/**
     * onSent Delegate function.           
     * @param fn onSent Callback function.
     */
void HaCServer::onSent(std::function<void(uint16_t, HaCClientInfo*)> fn)
{
     this->_onSentFn = fn;
}

/**
     * onError Delegate function.           
     * @param fn onError Callback function.
     */
void HaCServer::onError(std::function<void(uint16_t, HaCClientInfo*)> fn)
{
     this->_onErrorFn = fn;
}

/**
     * onPoll Delegate function.           
     * @param fn onPoll Callback function.
     */
void HaCServer::onPoll(std::function<void(HaCClientInfo*)> fn)
{
     this->_onPollFn = fn;
}

/**
     * onClosed Delegate function.           
     * @param fn onClosed Callback function.
     */
void HaCServer::onClosed(std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> fn)
{
     this->_onClosedFn = fn;
}

/**
     * onNewConnection Delegate function.           
     * @param fn onNewConnection Callback function.
     */
void HaCServer::onNewConnection(std::function<void(HaCClientInfo*, std::vector<HaCClientInfo*>)> fn)
{    
    this->_onNewConnectionFn = fn;
}

/* #endregion */

/* #region Private */
/**
     * Server start function
     * @param newSoc Incoming socket from remote client
     * @param err Socket error State
     * @return Socket error State
     */
long HaCServer::_accept(tcp_pcb* newSoc, long err)
{
    //Check if the connection is on the limit
    if(this->_clientInfos.size() >= HAC_SERVER_MAX_SOCKET_CLIENTS)
    {
        //Refuse the new connection
        tcp_close(newSoc);
        newSoc = nullptr;
        
        return ERR_ABRT;
    } 

    this->_clientInfo = new HaCClientInfo(newSoc,[&](HaCClientInfo * clInfo, tcp_pcb* soc)
        {
            tcp_backlog_delayed(soc);
            tcp_backlog_accepted(soc);
            clInfo->setPingWatchdog(this->_enablePingWatchdog);
            clInfo->onReceive(this->_onReceiveFn);
            clInfo->onSent(this->_onSentFn);
            clInfo->onError(this->_onErrorFn);
            clInfo->onPoll(this->_onPollFn);

            clInfo->onClosed(
                [&](HaCClientInfo * clientInfo)
                {
                    this->_clientInfo_onClosed(clientInfo);
                });

            this->_clientInfo_onAccepted(clInfo);
        }
    );


    return ERR_OK;
}

/**
     * Accept Incoming Connection
     * @param arg Universal pointer
     * @param newSoc Incoming socket from remote client
     * @param err Socket error State
     * @return Socket error State
     */
long HaCServer::_accept(void *arg, tcp_pcb* newSoc, long err)
{
    return reinterpret_cast<HaCServer*>(arg)->_accept(newSoc, err); 
}

/* #endregion */

/* #region Private Lambda functions */
/**
     * Client On Error CB(ClientInfo Events)
     * @param clientInfo Client connection information pointer
     */
void HaCServer::_clientInfo_onAccepted(HaCClientInfo * clientInfo)
{       
    clientInfo->setConnectionId(this->_clientInfos.size());
    DBG_CB_HSOC2("\n[HACSERVER] Client = %d connection has been accepted..\n", clientInfo->getConnectionId());
    this->_clientInfos.push_back(clientInfo);

    if(this->_onNewConnectionFn)
        this->_onNewConnectionFn(clientInfo, this->_clientInfos);

        
}


/**
     * Client On Closed CB(ClientInfo Events)
     * @param clientInfo Client connection information pointer
     */
void HaCServer::_clientInfo_onClosed(HaCClientInfo * clientInfo)
{    
    DBG_CB_HSOC2("\n[HACSERVER] Client with connection id = %d, closed connections..\n", clientInfo->getConnectionId());

    std::vector<HaCClientInfo*>::iterator end;
    
    //Save to temporary vector
    std::vector<HaCClientInfo*> tmp = std::vector<HaCClientInfo*>();
    for(auto p: this->_clientInfos)
    {
        if(p->getConnectionId() != clientInfo->getConnectionId())
            tmp.push_back(p);        
    }

    //Delete old vectors
    this->_clientInfos.clear();
    this->_clientInfos = std::vector<HaCClientInfo*>();

    //Assign tmp vector to newly cleared clientinfos vector
    for(auto p: tmp)
        this->_clientInfos.push_back(p);
    
    //Delete tmp vector
    tmp.clear();
    tmp = std::vector<HaCClientInfo*>();

    //Raised CB before deleting the clientinfo pointer
    if(this->_onClosedFn)
        this->_onClosedFn(clientInfo, this->_clientInfos);

    //Free clienInfo
    if(clientInfo != nullptr)
        delete clientInfo;
}

/* #endregion */

/* #endregion */

