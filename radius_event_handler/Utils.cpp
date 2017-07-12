/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <boost/asio/basic_deadline_timer.hpp>

#include "Utils.hpp"

using namespace std;

namespace utils {

    FlowKeyType::FlowKeyType()
    : ip_server(0),
      ip_client(0),
      port_server(0),
      port_client(0),
      protoField(0)
    {
        
    }
    
    ostream& operator<<(ostream& os, FlowKeyType& obj)
    {
        uint32_t IpSrv = htonl(obj.ip_server);
        uint32_t IpClnt = htonl(obj.ip_client);
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &IpSrv, str, INET_ADDRSTRLEN);
        os << "FlowKey:" << endl;
        os << "         Server IP  : " << str << endl;
        inet_ntop(AF_INET, &IpClnt, str, INET_ADDRSTRLEN);
        os << "         Client IP  : " << str << endl;    
        os << "         Server port: " << obj.port_server << endl;
        os << "         Client port: " << obj.port_client << endl;
        os << "         Identifier : " << (uint16_t)obj.protoField << endl;
        
        return os;
    }

    std::vector<std::string> split(const std::string &s, char delim) {
        std::stringstream ss(s);
        std::string item;
        std::vector<std::string> elems;
        while (std::getline(ss, item, delim)) {
            elems.push_back(std::move(item)); 
        }
        return elems;
    }
    
    RawDataArray& RadiusRawMsg::getRawDataArray() 
    {
        return m_radiusMsgData;
    }
    
    void RadiusRawMsg::initFlowKey(uint32_t ipSrv, uint32_t ipClnt, uint16_t portSrv, uint16_t portClnt, uint8_t proto)
    {
        m_key.ip_server = ipSrv;
        m_key.ip_client = ipClnt;
        m_key.port_server = portSrv;
        m_key.port_client = portClnt;
        m_key.protoField = proto;
    }
    
    FlowKey& RadiusRawMsg::getFlowKey() 
    {
        return m_key;
    }
    
    std::size_t RadiusRawMsg::size()
    {
        return m_radiusMsgData.size();
    }
    
    RadiusRawMsg::RadiusRawMsg():
    m_msgLength(0)
    {
        m_radiusMsgData.fill(0);
    }
    
    bool RadiusRawMsg::empty()
    {
        return m_radiusMsgData.empty();
    }
    
    uint16_t RadiusRawMsg::getLength()
    {
        return m_msgLength;
    }
    
    void RadiusRawMsg::setLength(uint16_t len)
    {
        m_msgLength = len;
    }
    
    std::ostream& operator<<(std::ostream& os, RadiusRawMsg& obj)
    {
        int bytesInRowCnt = 0;
        int msgBytesCnt = 0;      
        for(auto it = obj.m_radiusMsgData.begin(); it != obj.m_radiusMsgData.end(); it++)
        {
            os <<"0x"<<hex<<setfill('0')<<setw(2)<<right<<(((uint16_t)(*it))&0xff)<< " ";
            bytesInRowCnt++;
            msgBytesCnt++;
            if(HEXDUMP_ROW_LEN == bytesInRowCnt)
            {
                bytesInRowCnt = 0;
                cout << endl;
            }
            else if(msgBytesCnt == obj.m_msgLength)
            {
                break;
            }
            os << dec;
        }
        return os;
    }
    
    std::ostream& operator<<(std::ostream& os, RadiusAvpType& obj) 
    {
        os << "                       AVP: " << endl;
        os << "                           Type: " << (((uint16_t)(obj.type))&0xff)<< endl;
        os << "                           Value:" << obj.value << endl;
        
        return os;
    }
    
    ThriftStruct::ThriftStruct(boost::asio::io_service& io) 
    : m_captureTimeStamp(0),
      m_applId(0),
      m_timer(io)
    {
        
    }
    
    void ThriftStruct::setApplId(uint16_t applId) 
    {
        m_applId = applId;
    }
    
    void ThriftStruct::setTimeStamp(uint64_t captureTimeStamp) 
    {
        m_captureTimeStamp = captureTimeStamp;
    }
    
    AvpListPtr ThriftStruct::requestAvpListPtr() 
    {
        return m_reqAvpListPtr;
    }
    
    AvpListPtr ThriftStruct::responseAvpListPtr() 
    {
        return m_respAvpListPtr;
    }
    
    void ThriftStruct::setReqAvpListPtr(AvpListPtr avpListPtr)
    {
        m_reqAvpListPtr = avpListPtr;
    }
    void ThriftStruct::setRespAvpListPtr(AvpListPtr avpListPtr) 
    {
        m_respAvpListPtr = avpListPtr;
    }
    
    uint64_t& ThriftStruct::getTimeStamp()
    {
        return m_captureTimeStamp;
    }
    
    uint16_t& ThriftStruct::getApplId()
    {
        return m_applId;
    }
    
    boost::asio::deadline_timer& ThriftStruct::getTimer()
    {
        return m_timer;
    }
    
    std::ostream& operator<<(std::ostream& os, ThriftStruct& obj)
    {
        os << "Thrift: " << endl;
        os << "        Time: " << obj.m_captureTimeStamp << endl;
        os << "        ApplId: " << obj.m_applId << endl;
        os << "        RequestMsgAvpsList: " << endl;
        if(obj.m_reqAvpListPtr)
        {
            for(auto it = obj.m_reqAvpListPtr->begin(); it != obj.m_reqAvpListPtr->end(); ++it)
            {
                os << (*it) << endl;
            }
        }
        
        os << "        ResponseMsgAvpsList: " << endl;
        if(obj.m_respAvpListPtr)
        {
            for(auto it = obj.m_respAvpListPtr->begin(); it != obj.m_respAvpListPtr->end(); ++it)
            {
                os << (*it) << endl;
            }
        }
        
        return os;
    }
    
    std::ostream& operator<<(std::ostream& os, PlainThriftStruct& obj)
    {
        os << "Thrift: " << endl;
        os << "        Time: " << obj.m_captureTimeStamp << endl;
        os << "        ApplId: " << obj.m_applId << endl;
        os << "        RequestMsgAvpsList: " << endl;
        for(auto it = obj.m_reqAvpList.begin(); it != obj.m_reqAvpList.end(); ++it)
        {
            os << (*it) << endl;
        }
  
        os << "        ResponseMsgAvpsList: " << endl;
        for(auto it = obj.m_respAvpList.begin(); it != obj.m_respAvpList.end(); ++it)
        {
            os << (*it) << endl;
        }
 
        return os;
    }
    
    ProgramModule::ProgramModule(std::string moduleName)
    : m_moduleName(moduleName)
    {
        
    }
    
    std::string ProgramModule::getModuleName() {
        return m_moduleName;
    }
    
    void ProgramModule::start() {
        
    }
    
    void ProgramModule::stop() {
        
    }

}