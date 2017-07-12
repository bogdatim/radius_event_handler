/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RadiusEventHandler.hpp
 * Author: timur
 *
 * Created on July 6, 2017, 8:21 PM
 */

#ifndef RADIUSEVENTHANDLER_HPP
#define RADIUSEVENTHANDLER_HPP

#include "Configuration.hpp"
#include "Worker.hpp"
#include "PacketReceiver.hpp"
#include <atomic>
#include <boost/asio.hpp>

using namespace utils;
using namespace std;
using namespace worker;

namespace reh {
    
    class RadiusEventHandler {
    private:
        reh::Configuration& m_config;
        std::atomic<int> m_unmatchedReqsNum;
        std::vector<WorkerQueuePtr> m_PR2WorkersQueues;
        ThriftQueuePtr m_thriftQueuePtr;
        pr::PacketReceiverPtr m_packetReceiverPtr;
        std::vector<ProgramModulePtr> m_modulesArray;
        ThriftHashTable m_hashTable;
        boost::asio::io_service m_io;
        ThreadPtr m_timersHandlerThread;
        //bool stop_thread = false;
        
        void createQueues();
        //void timeoutsHandler(boost::asio::io_service& io);
    public:
        RadiusEventHandler(reh::Configuration& config);
        ~RadiusEventHandler();
        void createModules();
        void startModules();
        void stopModules();
        boost::asio::io_service& getIoService();
        void runIoService();
        
        inline void incUnmatchedReqCounter() {++m_unmatchedReqsNum;}
        inline ThriftQueuePtr getThriftQueuePtr(){return m_thriftQueuePtr;}
        inline std::vector<WorkerQueuePtr>& getWorkersQueueVec(){return m_PR2WorkersQueues;}
        inline pr::PacketReceiverPtr getPacketReceiverPtr() {return m_packetReceiverPtr;}
        
    };
    
}


#endif /* RADIUSEVENTHANDLER_HPP */

