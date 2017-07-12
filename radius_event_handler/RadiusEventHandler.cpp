/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   radiusEventHandler.cpp
 * Author: timur
 *
 * Created on June 17, 2017, 12:47 PM
 */

//#include <cstdlib>
#include <iostream>
#include <signal.h>
#include "Configuration.hpp"
#include "PacketReceiver.hpp"
#include "ThriftSender.hpp"
#include "Worker.hpp"
#include "RadiusEventHandler.hpp"
#include <boost/ref.hpp>

using namespace reh;
using namespace worker;

bool isShutdownStarted = false;

void sigintHandler(int sig)
{
    if (sig == SIGINT)
    {
        isShutdownStarted = true;
        std::cout << std::endl << "Graceful shutdown started ..." << std::endl;
    }
}

RadiusEventHandler::RadiusEventHandler(reh::Configuration& config)
: m_config(config),
  m_unmatchedReqsNum(0)
{
    m_modulesArray.reserve(MODULES_NUMBER);
    m_PR2WorkersQueues.reserve(WORKERS_NUMBER);
}

RadiusEventHandler::~RadiusEventHandler() {
    m_modulesArray.clear();
    m_PR2WorkersQueues.clear();
    m_hashTable.clear();
}

void RadiusEventHandler::createQueues()
{
    // Create workers queues
    for(int i = 0; i < WORKERS_NUMBER; ++i)
    {
        WorkerQueuePtr wQueuePtr(new WorkerQueue);
        wQueuePtr->set_capacity(WORKER_QUEUE_CAPACITY);
        m_PR2WorkersQueues.push_back(wQueuePtr);
    }
    
    // Create Thrift structures sender queue
    m_thriftQueuePtr.reset(new ThriftQueue);
    m_thriftQueuePtr->set_capacity(THRIFT_QUEUE_CAPACITY);
}

void RadiusEventHandler::createModules() {
    createQueues();
    
    m_modulesArray.push_back(ProgramModulePtr(new ts::ThriftSender("ThriftSender", m_thriftQueuePtr, m_config, m_io)));
    for(int i = 0; i < WORKERS_NUMBER; ++i) {
        std::string name = "Worker_" + std::to_string(i);
        m_modulesArray.push_back( ProgramModulePtr(new Worker(name, m_unmatchedReqsNum, m_PR2WorkersQueues[i], m_thriftQueuePtr, m_hashTable, m_io)));
    }
    
    //@TODO: Create thread for graceful shutdown in case of io_service.run() blocking call
    //m_timersHandlerThread.reset(new std::thread(&RadiusEventHandler::timeoutsHandler, this, boost::ref(m_io)));
    
    m_packetReceiverPtr.reset(new pr::PacketReceiver("PacketReceiver", m_config, m_io, m_PR2WorkersQueues));
}

void RadiusEventHandler::startModules() 
{
    for(auto it = m_modulesArray.cbegin(); it != m_modulesArray.cend(); ++it) {
        (*it)->start();
    }    
    m_packetReceiverPtr->start();
}

void RadiusEventHandler::stopModules() 
{
    //stop_thread = true;
    //m_timersHandlerThread->join();
    
    // Stop modules in reverse order
    for(auto rIt = m_modulesArray.crbegin(); rIt != m_modulesArray.crend(); ++rIt) {
        (*rIt)->stop();
    } 
    std::cout << "Unmatched Req/Resp number = " << m_unmatchedReqsNum << endl;
}

/*void RadiusEventHandler::timeoutsHandler(boost::asio::io_service& io) {
        
        while(!stop_thread) {
           // io.run_one();
           // io.reset();
        }
        std::cout << "RadiusEventHandler::timeoutsHandler stopped" << std::endl;
    }*/

boost::asio::io_service& RadiusEventHandler::getIoService()
{
    return m_io;
}

void RadiusEventHandler::runIoService() {
    boost::system::error_code error;
    //@TODO: change to run()
    m_io.poll_one(error);
    if(error)
    {
        std::cout << "io_service error: " << error.message() << std::endl;
    }
    m_io.reset();
}

/*
 * 
 */
int main(int argc, char** argv) {
    
    signal(SIGINT, sigintHandler);
    
    reh::Configuration ownConfig;
    // Parse program options
    if(!ownConfig.getConfigFromProgramOptions(argc, argv))
    {
        exit(0);
    }
    
    //Create RadiusEventHandler
    RadiusEventHandler radiusEventHandler(ownConfig);
    try {
        radiusEventHandler.createModules();
        radiusEventHandler.startModules();
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        exit(0);
    }
    
    std::cout << "RADIUS event handler has been STARTED" << std::endl;
    while(!isShutdownStarted) {
        radiusEventHandler.runIoService();
    }
    
    std::cout << "Stopping all program modules..." << std::endl;
    radiusEventHandler.stopModules();
    
    std::cout << "RADIUS event handler has been STOPPED" << std::endl;
  
    return 0;
}

