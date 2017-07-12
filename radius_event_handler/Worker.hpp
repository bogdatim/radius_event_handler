/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Worker.hpp
 * Author: timur
 *
 * Created on July 4, 2017, 11:43 PM
 */

#ifndef WORKER_HPP
#define WORKER_HPP

#include "Utils.hpp"
#include <boost/asio.hpp>
using namespace utils;

namespace worker {
    
    class Worker : public ProgramModule {
    private:
        bool stop_thread = false;
        WorkerQueuePtr m_inputQueuePtr;
        ThriftQueuePtr m_outputQueuePtr;
        ThreadPtr m_msgHandlerThread;
        ThriftHashTable& m_hashTable;
        boost::asio::io_service& m_io;
        std::atomic<int>& m_unmatchedReqsNum;
        
        AvpListPtr getAllAvps(RadiusRawMsgPtr msgPtr);
        void handleRadiusMessage();
        void handleTimeout(FlowKey& key, const boost::system::error_code& e);
    public:
        Worker(std::string ownName, std::atomic<int>& cnt, WorkerQueuePtr wqPtr, ThriftQueuePtr tqPtr, ThriftHashTable& hashTable, boost::asio::io_service& io);
        void start();
        void stop();
        
    };
    
    typedef std::shared_ptr<Worker> WorkerPtr;
    
}

#endif /* WORKER_HPP */

