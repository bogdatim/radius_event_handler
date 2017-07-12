/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "PacketReceiver.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <boost/asio.hpp>
#include <netinet/in.h>
#include <boost/bind.hpp>

using namespace std;
using namespace utils;

namespace pr {
    
    PacketReceiver::PacketReceiver(std::string ownName, reh::Configuration& cfg, boost::asio::io_service& io, std::vector<WorkerQueuePtr>& ref)
    : m_ownName(ownName), 
    m_config(cfg),
    m_ioSrv(io),
    m_socket(io),
    m_workersInputQueuesRef(ref),
    m_workerIndex(0)
    {
        m_tmpRadiusRawMsgPtr.reset(new RadiusRawMsg());
    }
      
    std::string& PacketReceiver::getModuleName()
    {
        return m_ownName;
    }
       
    void PacketReceiver::start() {

        boost::asio::ip::udp::endpoint listen_endpoint(boost::asio::ip::address::from_string(m_config.getOwnIpAsString()), m_config.getOwnPort());
        m_socket.open(listen_endpoint.protocol());
        m_socket.bind(listen_endpoint);
        m_localEndpoint = m_socket.local_endpoint();

        m_socket.async_receive_from(boost::asio::buffer(m_tmpRadiusRawMsgPtr->getRawDataArray() ), 
                                            m_remoteEndpoint,
                                            boost::bind(&PacketReceiver::receiveRaduisMessage, this,
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred));
        
    }
    
    void PacketReceiver::receiveRaduisMessage(const boost::system::error_code& error, std::size_t bytesReceived)
    {
        if(!error)
        {
            RadiusRawMsgPtr radiusRawMsgPtr(new RadiusRawMsg());
            radiusRawMsgPtr.swap(m_tmpRadiusRawMsgPtr);

            if(isRadAccMsg(radiusRawMsgPtr, m_remoteEndpoint, m_localEndpoint, bytesReceived)) 
            {
                radiusRawMsgPtr->setLength(bytesReceived);
                setFlowKey(radiusRawMsgPtr, m_remoteEndpoint, m_localEndpoint);
                handleRadiusEvent(radiusRawMsgPtr);
            }
            
            
          
            m_socket.async_receive_from(boost::asio::buffer(m_tmpRadiusRawMsgPtr->getRawDataArray() ), 
                                        m_remoteEndpoint,
                                        boost::bind(&PacketReceiver::receiveRaduisMessage, this,
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));

        }
        else
        {
            std::cout << "Error in receiveRaduisMessage: " << error.message() << std::endl;
        }
   
    }
    
    bool PacketReceiver::isRadAccMsg(RadiusRawMsgPtr msg, udp::endpoint& r_endpoint, udp::endpoint& l_endpoint, std::size_t bytesNum) {
        bool result = false;
        
        uint16_t radMsgLen;
        std::memcpy(&radMsgLen, msg->getRawDataArray().data() + RAD_ACC_MSG_LEN_OFFSET, RADIUS_MSG_LEN);
        radMsgLen = ntohs(radMsgLen);
        // Enough bytes for RADIUS message?
        // Number of bytesNum is equal to message length field value?
        if(RADIUS_HEADER_LEN <= radMsgLen && RADIUS_MSG_MAX_SIZE > radMsgLen) {
            // Accept only messages with IANA port for Radius accounting (1813)
            if(RADIUS_ACCOUNTING_PORT == l_endpoint.port() || RADIUS_ACCOUNTING_PORT == r_endpoint.port()) {
                //get radius message code
                try {
                    auto radMsgCode = msg->getRawDataArray().at(RAD_ACC_MSG_CODE_OFFSET);
                    
                    // Accept only RADIUS accounting requests and response messages
                    if (radMsgCode == radAccMsgTypes::AccountingRequest || radMsgCode == radAccMsgTypes::AccountingResponse ) {
                        cout << "Got RADIUS accounting message of type " << (int)radMsgCode << endl;
                        result = true;
                    }
                }
                catch (std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        }
            
        return result;
    }
    
    // Pushes a Raw RADIUS message to workers queues using round-robin algorithm
    // with simple overload protection.
    void PacketReceiver::handleRadiusEvent(RadiusRawMsgPtr msg) {
    
        auto numQueues = m_workersInputQueuesRef.size();
        m_workerIndex = m_workerIndex % numQueues;
        if(!m_workersInputQueuesRef[m_workerIndex++]->try_push(msg)) {
            //Try to push to queue of the next worker
            std::cout << "Worker " << (m_workerIndex -1)  << " queue overloaded, trying another one... " << std::endl;
            bool pushed = false;
            uint8_t numberOfTries = 0;
            while(!pushed && numberOfTries < (numQueues - 1)) {
                m_workerIndex = m_workerIndex % numQueues;
                pushed = m_workersInputQueuesRef[m_workerIndex++]->try_push(msg);
                numberOfTries++;
            }
        
            if(!pushed) {
                cout << "All workers are overloaded, message dropped!!!" << endl;
            }
        }
    }
    
    void PacketReceiver::setFlowKey(RadiusRawMsgPtr msg, udp::endpoint& remote_endpoint, udp::endpoint& local_endpoint) {
        uint32_t ipSrv, ipClnt, srcIp, dstIp;
        uint16_t portSrv, portClnt, srcPort, dstPort;
                                       
        dstIp = (uint32_t)local_endpoint.address().to_v4().to_ulong();
        dstPort = (uint16_t)local_endpoint.port();
        srcIp = (uint32_t)remote_endpoint.address().to_v4().to_ulong();
        srcPort = (uint16_t)remote_endpoint.port();

        if(srcPort >= dstPort) {
            portSrv = srcPort;
            portClnt = dstPort;
        }
        else {
            portSrv = dstPort;
            portClnt = srcPort;
        }
                            
        if(srcIp >= dstIp) {
           ipSrv = srcIp;
           ipClnt = dstIp;
        }
        else {
            ipSrv = dstIp;
            ipClnt = srcIp;
        }
                     
        uint8_t identifier = (uint8_t)msg->getRawDataArray().at(RAD_ACC_ID_OFFSET);        
        msg->initFlowKey(ipSrv, ipClnt, portSrv, portClnt, identifier);
        std::cout << msg->getFlowKey() << endl;
    }
    
}