/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Worker.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <atomic>

namespace worker {
    
    Worker::Worker(std::string ownName, std::atomic<int>& cnt, WorkerQueuePtr wqPtr, ThriftQueuePtr tqPtr, ThriftHashTable& hashTable, boost::asio::io_service& io)
    : ProgramModule(ownName),
    m_inputQueuePtr(wqPtr),
    m_outputQueuePtr(tqPtr),
    m_hashTable(hashTable),
    m_io(io),
    m_unmatchedReqsNum(cnt)
    {
         
    }
    
    void Worker::start() {
        m_msgHandlerThread.reset(new std::thread(&Worker::handleRadiusMessage, this));
    }
    
    void Worker::stop() {
        std::cout << "stopping " << getModuleName() << std::endl; 
        stop_thread = true;
        m_msgHandlerThread->join();
    }
    
    void Worker::handleRadiusMessage() {

        while(!stop_thread) {
            
            RadiusRawMsgPtr msgPtr;
              
            //Get one message from the queue
            //@TODO: Change to try()
            if(!m_inputQueuePtr->try_pop(msgPtr))
                continue;
            
            if(!msgPtr)
                continue;
   
            //Get message code
            uint16_t radMsgCode = msgPtr->getRawDataArray().at(RAD_ACC_MSG_CODE_OFFSET);
            AvpListPtr avpListPtr = getAllAvps(msgPtr);
            
            ThriftPtr thriftPtr;
            
            {
                ThriftHashTable::accessor hashTableAccessor;
                if(m_hashTable.find(hashTableAccessor, msgPtr->getFlowKey())) 
                {
                    thriftPtr = hashTableAccessor->second; 
                    thriftPtr->getTimer().cancel();
                    if(radMsgCode == radAccMsgTypes::AccountingRequest && thriftPtr->responseAvpListPtr()) {
                        thriftPtr->setReqAvpListPtr(avpListPtr);
                    }
                    else if(radMsgCode == radAccMsgTypes::AccountingResponse && thriftPtr->requestAvpListPtr())
                    {
                        thriftPtr->setRespAvpListPtr(avpListPtr);
                    }
                    else
                    {
                        std::cout << "Logical error processing message" << std::endl;
                    }
 
                    m_outputQueuePtr->try_push(thriftPtr);
 
                    m_hashTable.erase(hashTableAccessor);
                }
                else 
                {
                    // There is no mate in the hash table
                    thriftPtr.reset(new ThriftStruct(m_io));
                    if(radMsgCode == radAccMsgTypes::AccountingRequest) {
                        thriftPtr->setReqAvpListPtr(avpListPtr);
                    }
                    else {
                        thriftPtr->setRespAvpListPtr(avpListPtr);
                    }
                    
                    thriftPtr->getTimer().expires_from_now(boost::posix_time::seconds(TIMEOUT_VALUE));
                    thriftPtr->getTimer().async_wait(boost::bind(&Worker::handleTimeout, this, msgPtr->getFlowKey(), boost::asio::placeholders::error));
                    
                    m_hashTable.insert(hashTableAccessor, msgPtr->getFlowKey());
                    hashTableAccessor->second = thriftPtr;
                }
            }
            
            
        }
        std::cout << getModuleName() << " stopped" << std::endl;
    }
    
    void Worker::handleTimeout(FlowKey& key, const boost::system::error_code& ec) 
    {
        if (ec && ec == boost::asio::error::operation_aborted) {
            return;
        } 
        else
        {
        
            ThriftHashTable::accessor hashTableAccessor;
            if(m_hashTable.find(hashTableAccessor, key)) 
            {
                ThriftPtr thriftPtr = hashTableAccessor->second; 
                if(thriftPtr)
                {
                    m_outputQueuePtr->try_push(thriftPtr);
                    m_hashTable.erase(hashTableAccessor);
                    m_unmatchedReqsNum++;
                }
                else
                {
                    std::cout << getModuleName() << " handleTimeout ERROR, got NULL pointer from hash table" << std::endl;
                }
            }
        }
    }
    
    AvpListPtr Worker::getAllAvps(RadiusRawMsgPtr msgPtr)
    {
        AvpListPtr avpListPtr(new AvpList);
        uint16_t radMsgLen;
        uint16_t offset = RAD_ACC_MSG_CODE_OFFSET;
        uint8_t avpLength;
        
        //Get message length
        std::memcpy(&radMsgLen, msgPtr->getRawDataArray().data() + RAD_ACC_MSG_LEN_OFFSET, RADIUS_MSG_LEN);
        radMsgLen = ntohs(radMsgLen);
        
        //Return empty pointer if no AVP's are present
        if(radMsgLen == RADIUS_HEADER_LEN) {
            return avpListPtr;
        }
        
        offset += RADIUS_HEADER_LEN;
        while(offset < radMsgLen)
        {
            RadiusAvp avp;
            
            if((offset + RAD_ACC_AVP_TYPELEN_LEN )< radMsgLen)
            {
                avp.type = msgPtr->getRawDataArray().at(offset);
                avpLength = msgPtr->getRawDataArray().at(offset+RAD_ACC_AVP_TYPE_LEN);
                                
                if((offset + avpLength) <= radMsgLen)
                {
                    avp.value.assign((msgPtr->getRawDataArray().data() + offset + RAD_ACC_AVP_TYPELEN_LEN), (avpLength - RAD_ACC_AVP_TYPELEN_LEN));
                    avpListPtr->push_back(avp);
                    offset += avpLength;

                }
            }
        }
        return avpListPtr;   
    }
    
    
}