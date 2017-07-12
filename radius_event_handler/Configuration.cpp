/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Configuration.hpp"
#include "cxxopts.hpp"

using namespace utils;

namespace reh {

Configuration::Configuration()
: m_applId(0)
{
    m_ownAddress = {0};
    m_thriftServerAddress = {0};
}
    
bool Configuration::getConfigFromProgramOptions(int argc, char** argv) 
{
    bool result = false;
    
    cout << "Parsing program options ..." << endl;
    cout << "argc = " << argc << endl;
    
    try {
        
        cxxopts::Options options(argv[0], " command line options");
        options.add_options("Params")
        ("h,help", "Print help")
        ("id", "Application ID", cxxopts::value<uint16_t>())
        ("own-addr", "Own address", cxxopts::value<std::string>())
        ("thrift-addr", "Thrift server address", cxxopts::value<std::string>())
        ;
        
        options.parse(argc, argv);
        
        if (options.count("help"))
        {
            std::cout << options.help({"Params"}) << std::endl;
            return result;
        }
        
        if(!options.count("id") || !options.count("own-addr") || !options.count("thrift-addr"))
        {
            std::cout << "One or more of mandatory program parameters is missing" << std::endl;
            std::cout << options.help({"Params"}) << std::endl;
            return result;
        }
        
        m_applId = options["id"].as<uint16_t>();
        std::cout << "Own application id = " << m_applId << std::endl;
        
        //Get own IP address and port
        std::string ownAddress = options["own-addr"].as<std::string>();
        std::cout << "Own address = " << ownAddress << std::endl;
        std::vector<std::string> tokens = split(ownAddress, ':');
        if(tokens.size() != 2)
        {
            std::cout << "Wrong format of own address parameter" << std::endl;
            return result;
        }
        
        int ownPort = std::stoi(tokens.back());   
        if(ownPort < SOCK_PORT_HIGH_BOUND)
        {
            std::cout << "Wrong own port parameter" << std::endl;
            return result;
        }
        std::cout << "Own port = " << tokens.back() << std::endl;
        
        if(inet_pton(AF_INET, tokens.front().c_str(), &(m_ownAddress.sin_addr)) != 1)
        {
            std::cout << "Wrong own IP address " << tokens.front() << std::endl;
            return result;
        }
        
        m_ownPort = (unsigned short)ownPort;
        m_ownIpaddr = tokens.front();
        std::cout << "Own IP address = " << m_ownIpaddr << std::endl;
        
        //Get Thrift IP address and port
        std::string thriftAddress = options["thrift-addr"].as<std::string>();
        std::cout << "Thrift server address = " << thriftAddress << std::endl;
        tokens = split(thriftAddress, ':');
        if(tokens.size() != 2)
        {
            std::cout << "Wrong format of Thrift address parameter" << std::endl;
            return result;
        }
        
        int thriftPort = std::stoi(tokens.back());   
        if(thriftPort < SOCK_PORT_HIGH_BOUND)
        {
            std::cout << "Wrong thrift port parameter" << std::endl;
            return result;
        }
        
        m_thriftPort = (unsigned short)thriftPort;
        std::cout << "Thrift port = " << m_thriftPort << std::endl;
        
        if(inet_pton(AF_INET, tokens.front().c_str(), &(m_ownAddress.sin_addr)) != 1)
        {
            std::cout << "Wrong thrift IP address " << tokens.front() << std::endl;
            return result;
        }
        
        m_thriftIpAddr = tokens.front();
        std::cout << "Thrift IP address = " << m_thriftIpAddr << std::endl;
        
        result = true;
    
    } catch (const cxxopts::OptionException& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
    } catch (std::exception& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
    } 
    
    return result;
}

}