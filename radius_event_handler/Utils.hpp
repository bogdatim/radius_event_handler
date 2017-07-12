/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Utils.hpp
 * Author: timur
 *
 * Created on June 20, 2017, 8:40 PM
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include  <iomanip>
#include <string>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <memory>
#include <sstream>
#include <vector>
#include <array>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>
#include <boost/functional/hash.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/archive/tmpdir.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <chrono>
#include <thread>

using namespace std::chrono;
using boost::asio::ip::udp;

namespace utils  {
    
    #define SOCK_PORT_HIGH_BOUND 1024
    
    #define WORKERS_NUMBER 4
    #define SENDERS_NUMBER 1
    #define MODULES_NUMBER (WORKERS_NUMBER+SENDERS_NUMBER)
    #define WORKER_QUEUE_CAPACITY 1024
    #define THRIFT_QUEUE_CAPACITY WORKER_QUEUE_CAPACITY*WORKERS_NUMBER

    #define RADIUS_MSG_CODE_LEN 1
    #define RADIUS_IDENTIFIER_LEN 1
    #define RADIUS_MSG_LEN 2
    #define RADIUS_AUTH_LEN 16
    #define RADIUS_HEADER_LEN (RADIUS_MSG_CODE_LEN + RADIUS_IDENTIFIER_LEN + RADIUS_MSG_LEN + RADIUS_AUTH_LEN)
    #define RADIUS_ACCOUNTING_PORT 1813
    #define RAD_ACC_MSG_CODE_OFFSET 0
    #define RAD_ACC_ID_OFFSET 1
    #define RAD_ACC_MSG_LEN_OFFSET 2
    #define RAD_ACC_AVP_TYPE_LEN 1
    #define RAD_ACC_AVP_LEN 1
    #define TIMEOUT_VALUE 2
    #define RADIUS_MSG_MAX_SIZE 4096
    #define HEXDUMP_ROW_LEN 16
    #define RAD_ACC_AVP_TYPE_LEN 1
    #define RAD_ACC_AVP_LEN_LEN 1
    #define RAD_ACC_AVP_TYPELEN_LEN 2
    
    typedef struct sockaddr_in IpV4Address;
    bool is_ipv4_address(const std::string& str);
    bool compareIPv4AddrsAndPorts( uint32_t src_addr, uint16_t src_prt, uint32_t dst_addr, uint16_t dst_prt);
    
    std::vector<std::string> split(const std::string &s, char delim);
    
    /* 5-tuple key for IPv4 */
     struct FlowKeyType {
        uint32_t ip_server;
        uint32_t ip_client;
        uint16_t port_server;
        uint16_t port_client;
        uint8_t protoField;
        
        FlowKeyType();
        friend std::ostream& operator<<(std::ostream& os, FlowKeyType& obj);
    };
    
    typedef struct FlowKeyType FlowKey;
    
    typedef boost::array<char, RADIUS_MSG_MAX_SIZE> RawDataArray;
    
    class RadiusRawMsg {
    private:
        RawDataArray m_radiusMsgData;
        FlowKey m_key;
        uint16_t m_msgLength;
    public:
        RadiusRawMsg();
        RawDataArray& getRawDataArray();
        void initFlowKey(uint32_t ipSrv, uint32_t ipClnt, uint16_t portSrv, uint16_t portClnt, uint8_t proto);
        FlowKey& getFlowKey();
        std::size_t size();
        bool empty();
        uint16_t getLength();
        void setLength(uint16_t len);
        friend std::ostream& operator<<(std::ostream& os, RadiusRawMsg& obj);
    };
    
    enum radAccMsgTypes {
        AccountingRequest = 4,
        AccountingResponse = 5
    };
    
    typedef std::shared_ptr<RadiusRawMsg> RadiusRawMsgPtr;
    typedef tbb::concurrent_bounded_queue<RadiusRawMsgPtr> WorkerQueue;
    typedef std::shared_ptr<WorkerQueue> WorkerQueuePtr;
    
    struct RadiusAvpType {
        uint8_t type;
        std::string value;

        friend class boost::serialization::access;
        friend std::ostream& operator<<(std::ostream& os, RadiusAvpType& obj);
        template<class Archive> void serialize(Archive & ar, const unsigned int)
        {
            ar & type;
            ar & value;
        }
    };
    
    typedef struct RadiusAvpType RadiusAvp;
    
    typedef std::vector<RadiusAvp> AvpList;
    typedef std::shared_ptr<AvpList> AvpListPtr;
    
    class ThriftStruct {
    private:
        uint64_t m_captureTimeStamp;
        uint16_t m_applId;
        AvpListPtr  m_reqAvpListPtr;
        AvpListPtr  m_respAvpListPtr;
        boost::asio::deadline_timer m_timer;
        
    public:
        ThriftStruct();
        ThriftStruct(boost::asio::io_service& io);
        void setTimeStamp(uint64_t captureTimeStamp);
        void setApplId(uint16_t applId);
        AvpListPtr requestAvpListPtr();
        AvpListPtr responseAvpListPtr();
        void setReqAvpListPtr(AvpListPtr avpListPtr);
        void setRespAvpListPtr(AvpListPtr avpListPtr);
        boost::asio::deadline_timer& getTimer();
        
        uint64_t& getTimeStamp();
        uint16_t& getApplId();
        friend class boost::serialization::access;
        friend std::ostream& operator<<(std::ostream& os, ThriftStruct& obj);
        template<class Archive> void serialize(Archive & ar, const unsigned int)
        {
            ar & m_captureTimeStamp;
            ar & m_applId;
            ar & (*m_reqAvpListPtr);
            ar & (*m_respAvpListPtr);
        }
    };
    
    struct PlainThriftStruct {
        uint64_t m_captureTimeStamp;
        uint16_t m_applId;
        AvpList  m_reqAvpList;
        AvpList  m_respAvpList;
        
        PlainThriftStruct(){};
        friend class boost::serialization::access;
        friend std::ostream& operator<<(std::ostream& os, PlainThriftStruct& obj);
        template<class Archive> void serialize(Archive & ar, const unsigned int)
        {
            ar & m_captureTimeStamp;
            ar & m_applId;
            ar & m_reqAvpList;
            ar & m_respAvpList;
        }
    };
    
    typedef std::shared_ptr<ThriftStruct> ThriftPtr;
    typedef tbb::concurrent_bounded_queue<ThriftPtr> ThriftQueue;
    typedef std::shared_ptr<ThriftQueue> ThriftQueuePtr;
    typedef std::shared_ptr<std::thread> ThreadPtr;
    
    class ProgramModule {
    private:
        std::string m_moduleName;
    public:
        ProgramModule(std::string moduleName);
        virtual void start();
        virtual void stop();
        std::string getModuleName();
    };
    
    typedef std::shared_ptr<ProgramModule> ProgramModulePtr;
    
    struct FlowKeyHashCompare {
        static size_t hash( const FlowKey& x ) {
            size_t h = 0;
            boost::hash_combine(h, x.ip_server);
            boost::hash_combine(h, x.ip_client);
            boost::hash_combine(h, x.port_server);
            boost::hash_combine(h, x.port_client);
            boost::hash_combine(h, x.protoField);
            return h;
        }
        //! True if FlowKey's are equal
        static bool equal( const FlowKey& x, const FlowKey& y ) {
            if(x.ip_server == y.ip_server && x.ip_client == y.ip_client &&
               x.port_server == y.port_server && x.port_client == y.port_client &&
               x.protoField == y.protoField )
            {
                return true;
            }
            else 
            {
                return false;
            }
        }
    };
 
    // A concurrent hash table that maps FlowKey's to ThriftPtr's.
    typedef tbb::concurrent_hash_map<FlowKey,ThriftPtr,FlowKeyHashCompare> ThriftHashTable;
}


#endif /* UTILS_HPP */

