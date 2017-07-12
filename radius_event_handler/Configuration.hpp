/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Configuration.hpp
 * Author: timur
 *
 * Created on June 20, 2017, 12:16 AM
 */

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <cstdint>
#include "Utils.hpp"

using namespace utils;
using namespace std;

namespace reh {
    
class Configuration  {
private:
    uint16_t m_applId;
    
    unsigned short m_ownPort;
    string m_ownIpaddr;
    
    unsigned short m_thriftPort;
    string m_thriftIpAddr;
    
    IpV4Address m_ownAddress;
    IpV4Address m_thriftServerAddress;    
    
public:
    
    Configuration();
    
    bool getConfigFromProgramOptions(int argc, char** argv);
    
    inline uint16_t getApplId() {return m_applId;}
    inline IpV4Address& getOwnAddress() {return m_ownAddress;}
    inline IpV4Address& getThriftAddress() {return m_thriftServerAddress;} 
    inline string& getOwnIpAsString(){return m_ownIpaddr;}
    inline unsigned short getOwnPort(){return m_ownPort;}
    inline string& getThriftIpAsString(){return m_thriftIpAddr;}
    inline unsigned short getThriftPort(){return m_thriftPort;}
};




}
#endif /* CONFIGURATION_HPP */

