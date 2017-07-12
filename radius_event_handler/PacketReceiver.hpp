/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PacketReceiver.hpp
 * Author: timur
 *
 * Created on June 26, 2017, 11:27 PM
 */

#ifndef PACKETRECEIVER_HPP
#define PACKETRECEIVER_HPP

#include "Configuration.hpp"

using boost::asio::ip::udp;
using namespace utils;

namespace pr {
    
    class PacketReceiver {
    private:
        std::string m_ownName;
        reh::Configuration& m_config;
        boost::asio::io_service& m_ioSrv;
        udp::socket m_socket;
        udp::endpoint m_localEndpoint;
        udp::endpoint m_remoteEndpoint;
        std::vector<WorkerQueuePtr>& m_workersInputQueuesRef;
        RadiusRawMsgPtr m_tmpRadiusRawMsgPtr;
        int m_workerIndex;
       
        bool isRadAccMsg(RadiusRawMsgPtr msg, udp::endpoint& r_endpoint, udp::endpoint& l_endpoint, std::size_t bytesNum);
        void setFlowKey(RadiusRawMsgPtr msg, udp::endpoint& remote_endpoint, udp::endpoint& local_endpoint);
        void handleRadiusEvent(RadiusRawMsgPtr msg);
        
    public:
        PacketReceiver(std::string ownName, reh::Configuration& cfg, boost::asio::io_service& io, std::vector<WorkerQueuePtr>& ref);
        std::string& getModuleName();
        void receiveRaduisMessage(const boost::system::error_code& error, std::size_t bytes_received);
        void start();
        
    };
    
    typedef std::shared_ptr<PacketReceiver> PacketReceiverPtr;
    
}

#endif /* PACKETRECEIVER_HPP */

