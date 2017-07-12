/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ThriftSender.hpp"
#include <vector>
#include <iostream>
#include <fstream>

namespace ts {
    
    ThriftSender::ThriftSender(std::string ownName, ThriftQueuePtr inputQueuePtr, reh::Configuration& cfg, boost::asio::io_service& io)
    : ProgramModule(ownName),
      stop_thread(false), 
      m_inputQueuePtr(inputQueuePtr),
      m_config(cfg),
      m_thriftStructsSent(0),
      m_filename(boost::archive::tmpdir()),
      m_socket(io, udp::endpoint(udp::v4(), 5555))
    {
        m_filename += "/archive.txt";
     
        m_remoteEndpoint.address(boost::asio::ip::address::from_string(m_config.getThriftIpAsString()));
        m_remoteEndpoint.port(m_config.getThriftPort());
    }
    
    void ThriftSender::start() {
        m_senderThreadPtr.reset(new std::thread(&ThriftSender::sendThriftStructures, this));
 
    }
    
    void ThriftSender::stop() {
        stop_thread = true;
        m_senderThreadPtr->join();
        std::cout << getModuleName() << " Number of sent structures = " << m_thriftStructsSent << std::endl;
    }
    
    void ThriftSender::sendThriftStructures() {
        
        while(!stop_thread) {
       
            ThriftPtr msgToSendPtr;
            //@TODO: Change to try()
            if(!m_inputQueuePtr->try_pop(msgToSendPtr))
                continue;
            
            if(!msgToSendPtr)
                continue;
                
            msgToSendPtr->setApplId(m_config.getApplId());
            msgToSendPtr->setTimeStamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
       
            try {
                //Do serialization and sending to thrift server
                PlainThriftStruct plainStructToSend;
                std::ostringstream archive_stream;
                std::string send_buf;
                
                //@TODO: Move to Worker thread
                fillPlainThriftStructure(msgToSendPtr, &plainStructToSend);
                boost::archive::text_oarchive archive(archive_stream);
                archive << plainStructToSend;
                send_buf = archive_stream.str();
                
                std::cout << "Thrift structure to send: " << std::endl;
                std::cout << plainStructToSend << endl;
                
                //@TODO: Change to TCP 
                m_socket.send_to(boost::asio::buffer(send_buf), m_remoteEndpoint);    
            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                continue;
            } 
            
            m_thriftStructsSent++;

          /*  save_thrift(plainStructToSend, m_filename.c_str());
            PlainThriftStruct restoredPlainThrift; 
            restore_thrift(restoredPlainThrift, m_filename.c_str());
            std::cout << "Restored PlainThriftStruct: " << std::endl;
            std::cout << restoredPlainThrift << endl;*/

        }
        std::cout << getModuleName() << " stopped" << std::endl;
        
    }
    
    void ThriftSender::save_thrift(const PlainThriftStruct &s, const char * filename)
    {
        // make an archive
        std::ofstream ofs(filename);
        boost::archive::text_oarchive oa(ofs);
        oa << s;
    }

    void ThriftSender::restore_thrift(PlainThriftStruct &s, const char * filename)
    {
        // open the archive
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ia(ifs);

        // restore the schedule from the archive
        ia >> s;
    }
    
    void ThriftSender::fillPlainThriftStructure(ThriftPtr thrift, PlainThriftStruct* plainThrift)
    {
        std::memcpy(&(plainThrift->m_captureTimeStamp), &(thrift->getTimeStamp()), sizeof(plainThrift->m_captureTimeStamp));
        std::memcpy(&(plainThrift->m_applId), &(thrift->getApplId()), sizeof(plainThrift->m_applId));
        //@TODO: Change to std::memcpy
        if(thrift->requestAvpListPtr())
        {
            plainThrift->m_reqAvpList = *(thrift->requestAvpListPtr());
        }
        
        if (thrift->responseAvpListPtr())
        {
            plainThrift->m_respAvpList = *(thrift->responseAvpListPtr());
        }
       
        
    }
    
}