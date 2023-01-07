/**
 *
 * @file HaCClientInfo-impl.h
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


/* #region SELF_HEADER */
#include "HaCClientInfo.h"
/* #endregion */


/* #region CLASS_DEFINITION */

/* #region Public */

/**
     * Constructor
     */
HaCClientInfo::HaCClientInfo() 
{     
}

/**
     * Constructor
     * @param soc Incoming socket from remote client
     * @param fn onAcceptedFn Callback function.
     */
HaCClientInfo::HaCClientInfo(tcp_pcb* soc, std::function<void(HaCClientInfo*, tcp_pcb*)> fn) 
{

     this->_soc = soc; 
     this->_onAcceptedFn = fn;     

     this->_setup();

     if(this->_onAcceptedFn) this->_onAcceptedFn(this, soc);

}

/**
     * Destructor
     */
HaCClientInfo::~HaCClientInfo() 
{
     DBG_CB_HSOC("\n[HACCLIENTINFO] Destroying HaCClientInfo..");
}

/**
     * Setup client socket
     * @param soc Client socket
     */
void HaCClientInfo::setupClientSocket(tcp_pcb* soc) 
{
     this->_soc = soc;
     this->_setup();
}


/**
     * Send data
     * @param buffer data to be sent
     */
long HaCClientInfo::sendData(const char * buffer) 
{
     //TO DO: Manage a chunk send data
     return tcp_write(this->_soc, buffer, strlen(buffer), TCP_WRITE_FLAG_COPY);     
}

/**
     * Set Connection ID
     * @param id Connection ID
     */
void HaCClientInfo::setConnectionId(uint8_t id)
{
     this->_connectionId = id;
}

/**
     * Get Connection ID
     * @return Connection ID     
     */
uint8_t HaCClientInfo::getConnectionId()
{
     return this->_connectionId;
}

/**
     * Set Ping watchdog to enable or disable
     * @param enable True to enable or Flase to Disable
     * @return Return ping watchdog enable state
     */
bool HaCClientInfo::setPingWatchdog(bool enable)
{    
     this->_enablePingWatchdog = enable;
     return this->_enablePingWatchdog;
}

/**
     * Abort client connection  
     */
void HaCClientInfo::abort() 
{
     if(this->_soc)
     {
          tcp_arg(this->_soc, NULL);
          tcp_sent(this->_soc, NULL);
          tcp_recv(this->_soc, NULL);
          tcp_err(this->_soc, NULL);
          tcp_poll(this->_soc, NULL, 0);  
          tcp_abort(this->_soc);        
          this->_soc = nullptr;
     }
}

/**
     * Close connection  
     * @param forceClose set to true if forcefully closing and aborting the socket connection  
     */
void HaCClientInfo::close(bool forceClose) 
{
     if(this->socketState()!= CLOSED || forceClose)
     {

          tcp_close(this->_soc);  
          //Dont delete the soc so that it can be recycled later on for future connection
          /*if(this->_soc)
          {
               Serial.println(3);
               this->_soc = nullptr;
          }*/

          if(this->_onClosedFn)
               this->_onClosedFn(this);               
          else 
               delete this;
     }          
}

/**
     * Socket state
     */
uint8_t HaCClientInfo::socketState() const
{
     return (!this->_soc || this->_soc->state == CLOSING || this->_soc->state == CLOSE_WAIT) ?
            CLOSED : this->_soc->state;
}

/**
     * Setup client socket
     * @param soc Client socket
     */
void HaCClientInfo::getRemoteIP(char *bufferIP) 
{     
     strcpy(bufferIP, &(IPAddress(this->_soc->remote_ip.addr).toString())[0]);
}

/**
     * Connect to remote server
     * @param ip address address of the remote host
     * @param port port of the remot host
     * @return True if connection is successful
     */
bool HaCClientInfo::connect(IPAddress ip, uint16_t port) 
{
     if(!this->_soc) return false;
     //TO DO: Resolve host before proceeding
     err_t err = tcp_connect(this->_soc, ip, port, &HaCClientInfo::_connected);
     
     if (err != ERR_OK) {
          Serial.println(3);
          return false;
     }
     
     return true;     
}


/**
     * onReceive Delegate function.           
     * @param fn onReceive Callback function
     */
void HaCClientInfo::onReceive(std::function<void(HaCClientInfo*, const char*, uint16_t, uint32_t)> fn) 
{     
     this->_onReceiveFn = fn;
}

/**
     * onSent Delegate function.           
     * @param fn onSent Callback function
     */
void HaCClientInfo::onSent(std::function<void(uint16_t, HaCClientInfo*)> fn) 
{
     this->_onSentFn = fn;
}

/**
     * onError Delegate function.           
     * @param fn onError Callback function
     */
void HaCClientInfo::onError(std::function<void(uint16_t, HaCClientInfo*)> fn) 
{
     this->_onErrorFn = fn;
}

/**
     * onPoll Delegate function.           
     * @param fn onPoll Callback function
     */
void HaCClientInfo::onPoll(std::function<void(HaCClientInfo*)> fn) 
{
     this->_onPollFn = fn;
}

/**
     * onClosed Delegate function.           
     * @param fn onClosed Callback function
     */
void HaCClientInfo::onClosed(std::function<void(HaCClientInfo*)> fn) 
{
     this->_onClosedFn = fn;
}

/**
     * onConnected Delegate function.           
     * @param fn onClosed Callback function
     */
void HaCClientInfo::onConnected(std::function<void(HaCClientInfo*)> fn) 
{
     this->_onConnectedFn = fn;
}

/* #endregion */

/* #region Private */

/**
     * Setup socket
     */
void HaCClientInfo::_setup()
{ 
     tcp_setprio(this->_soc, TCP_PRIO_MIN);
     tcp_arg(this->_soc, this);
     tcp_recv(this->_soc, &HaCClientInfo::_onReceive);
     tcp_sent(this->_soc, &HaCClientInfo::_onSent);
     tcp_err(this->_soc, &HaCClientInfo::_onError);
     tcp_poll(this->_soc, &HaCClientInfo::_onPoll, 1);   
}
/**
     * Internal library Calback function for on receive
     * @param tpcp Remote Client Socket Pointer
     * @param p Data buffer struct
     * @param err Socket error state
     * @return Socket error state
     */
long HaCClientInfo::_onReceive(struct tcp_pcb *tpcb,
                             struct pbuf *p, err_t err)
{ 
     
     if(p == 0) //Client close connection
     {
          Serial.println("\n[HACCLIENTINFO] Close...");          
          this->close(true);
          return ERR_CLSD;
     }
     /*
     this->_totalBytesReceive += p->tot_len;
     Serial.printf("\n Receive : p->len = %d p->tot_len = %d _totalBytesReceive = %llu err = %llu", 
                    p->len, p->tot_len, _totalBytesReceive, err);
     */
     if(this->_onReceiveFn)
     {
          //TO DO: Manage chunk packet
          const char *s = (const char*)p->payload;
          char *buffer = new char[p->tot_len + 1];
          memset(buffer, '\0', p->tot_len + 1);
         
          uint16_t i = 0;
          while(*s)
          {    
               if(_ignoreCRNLReceiveData)// Ignore carriage return and newline character
               {
                    if(*s != '\n' && *s != '\r')
                         buffer[i++] = *s;
               }
               else
                    buffer[i++] = *s;
               
               s++;             
               if(i >= p->tot_len) break; 
          }
          buffer[i++] = '\0';

          if(buffer[0])
          {
               this->_onReceiveFn(this, &buffer[0], p->len, p->tot_len);
          }
          delete[] buffer;
     }


     pbuf_free(p);     
     if(tpcb)
            tcp_recved(tpcb, p->tot_len);     

     return ERR_OK;
}

/**
     * Internal library Calback function for on sent
     * @param tpcp Remote Client Socket Pointer
     * @param len Data sent size
     * @return Socket error state
     */
long HaCClientInfo::_onSent(struct tcp_pcb *tpcb,
                              u16_t len)
{
     this->_isRemoteEndNotOk = false;
     if(this->_onSentFn)
          this->_onSentFn(len, this);

     return ERR_OK;
}

/**
     * Internal library Calback function for on error
     * @param err Socket error state
     */
void HaCClientInfo::_onError(err_t err)
{
     Serial.printf("\n[HACCLIENTINFO] Error %d \n", this->_connectionId);     
     if(this->_onErrorFn)
          this->_onErrorFn((uint32_t)err, this);    
}

/**
     * Internal library Calback function for on poll
     * @param tpcp Remote Client Socket Pointer
     * @return Socket error state
     */
long HaCClientInfo::_onPoll(struct tcp_pcb *tpcb)
{

     if(this->_pollingCounter > HAC_SOCCLIENT_POLL_INTVAL_PING && this->_enablePingWatchdog)
     {
          this->_pollingCounter = 0;
          long err = this->sendData("ping");
          tcp_output(this->_soc);
          Serial.printf("\n[HACCLIENTINFO] Send Err = %lu this->_connectionNotOkCntr = %d this->_isSendingPing = %d \n",
               err, this->_connectionNotOkCntr, this->_isRemoteEndNotOk);

          //If the sendData can't be acknowledge via the onSent event then isRemoteEndNotOk will
          //be keep unset. The not OK connection counter will start increasing until it will
          //reach to the max counter of 2, at this time it is confirm that end point has a problem 
          //and need to be closed
          if(this->_isRemoteEndNotOk)
               this->_connectionNotOkCntr++;

          if(this->_connectionNotOkCntr >= 2 || err != ERR_OK)
          {
               Serial.printf("\n[HACCLIENTINFO] Remote Client Closed connection...\n");
               tcp_output(this->_soc);
               this->close();

               //This kind of connection error need to close the soc so that the next 
               //connection attempt will be fresh

               this->abort();

               return ERR_CLSD;
          }
          //On Sent event wil toggle this bit to false, if the sendData won't get
          //any reply via onSent event means there is a problem on the remote end..
          this->_isRemoteEndNotOk = true;
     }
     this->_pollingCounter++;


     if(this->_onPollFn)
          this->_onPollFn(this);    

     return ERR_OK;
}

/**
     * Internal library Calback function for connected
     * @param pcb Socket pointer
     * @param err Socket error state
     * @return Socket error state
     */

long HaCClientInfo::_connected(struct tcp_pcb *pcb, err_t err)
{
    if(this->_onConnectedFn)
        this->_onConnectedFn(this);
    
    return ERR_OK;
}

/**
     * Native library Calback function for on receive
     * @param arg General Pointer
     * @param tpcp Remote Client Socket Pointer
     * @param p Data buffer struct
     * @param err Socket error state
     * @return Socket error state
     */
long HaCClientInfo::_onReceive(void *arg, struct tcp_pcb *tpcb,
                             struct pbuf *p, err_t err)
{
    return reinterpret_cast<HaCClientInfo*>(arg)->_onReceive(tpcb, p, err); 
}

/**
     * Native library Calback function for on sent
     * @param arg General Pointer
     * @param tpcp Remote Client Socket Pointer
     * @return Socket error state
     */
long HaCClientInfo::_onSent(void *arg, struct tcp_pcb *tpcb,
                              u16_t len)
{
    return reinterpret_cast<HaCClientInfo*>(arg)->_onSent(tpcb, len); 
}

/**
     * Native library Calback function for on error
     * @param arg General Pointer
     * @param err Socket error state
     */
void HaCClientInfo::_onError(void *arg, err_t err)
{
    reinterpret_cast<HaCClientInfo*>(arg)->_onError(err); 
}

/**
     * Native library Calback function for on poll
     * @param arg General Pointer
     * @param tpcp Remote Client Socket Pointer
     * @return Socket error state
     */
long HaCClientInfo::_onPoll(void *arg, struct tcp_pcb *tpcb)
{
    return reinterpret_cast<HaCClientInfo*>(arg)->_onPoll(tpcb); 
}

/**
     * Native library Calback function for connected
     * @param arg General Pointer
     * @param pcb Socket pointer
     * @param err Socket error state
     * @return Socket error state
     */
long HaCClientInfo::_connected(void* arg, struct tcp_pcb *pcb, err_t err)
{
    return reinterpret_cast<HaCClientInfo*>(arg)->_connected(pcb, err);
}

/* #endregion */

/* #endregion */

