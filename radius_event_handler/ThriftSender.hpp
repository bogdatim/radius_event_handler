/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ThriftSender.hpp
 * Author: timur
 *
 * Created on July 5, 2017, 11:05 PM
 */

#ifndef THRIFTSENDER_HPP
#define THRIFTSENDER_HPP

#include <atomic>
#include "Configuration.hpp"

using namespace utils;

namespace ts {
    
    class ThriftSender : public ProgramModule {
    private:
        bool stop_thread;
        ThriftQueuePtr m_inputQueuePtr;
        ThreadPtr m_senderThreadPtr;
        reh::Configuration& m_config;
        std::atomic<int> m_thriftStructsSent;
        std::string m_filename;
        boost::asio::ip::udp::socket m_socket;
        boost::asio::ip::udp::endpoint m_remoteEndpoint;
        
        void sendThriftStructures();
        void save_thrift(const PlainThriftStruct &s, const char * filename);
        void restore_thrift(PlainThriftStruct &s, const char * filename);
        void fillPlainThriftStructure(ThriftPtr thrift, PlainThriftStruct* plainThrift);
    public:
        ThriftSender(std::string ownName, ThriftQueuePtr inputQueuePtr, reh::Configuration& cfg, boost::asio::io_service& io);
        void start();
        void stop();
    };
    
}


#endif /* THRIFTSENDER_HPP */

