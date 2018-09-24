#ifndef MONZZA_WORKER_THREAD_HPP
#define MONZZA_WORKER_THREAD_HPP

#include <cstdint>
#include <map>
#include <vector>

#include "../../external/cpl/cpl_event_queue.hpp"

#include "../logger/log_functionality.hpp"
#include "../table/table.hpp"

#include "worker_settings.hpp"

class WorkerThreadServiceMessage {
public:
    enum class ResponseType {
        NOT,
        SUCCESSFUL,
        FAILURE,
    };

    enum class CommandType {
        NEW_CONNECTION,
        STOP
    };

    bool setResponseType ( ResponseType responseType );
    ResponseType getResponseType() const;

    bool setCommandType( CommandType commandType );
    CommandType getCommandType() const;

    bool setNewConnection( cpl::TcpServerExchangeSocket* tcpServerExchangeSocket );
    cpl::TcpServerExchangeSocket* getNewConnection() const;
private:
    ResponseType responseType_;
    CommandType  commandType_;

    cpl::TcpServerExchangeSocket* tcpServerExchangeSocket_;
};

class WorkerThread : public LogFunctionality {
public:
    WorkerThread();
    ~WorkerThread();

    bool initialize( Logger* logger, Table* table, WorkerSettings* workerSettings );

    cpl::Event* getNewOutputServiceMessageEvent();
    void addInputServiceMessage( WorkerThreadServiceMessage* workerThreadServiceMessage );
    WorkerThreadServiceMessage* getOutputServiceMessage();

    void operator()();
private:
    void processInputServiceMessage();
    void serviceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMessage );
    void newConnectionServiceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMessage );
    void stopServiceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMdessage );

    void processTcpServerExchangeSocketEvent( uint32_t index );

    bool breakThreadLoop_;
    std::vector<cpl::Event*> events_;
    WorkerSettings* workerSettings_;
    Table* table_;
    std::vector<std::pair<cpl::TcpServerExchangeSocket*, cpl::Event*>> tcpServerExchangeSockets_;

    uint8_t* bufPtr_;
    uint16_t bufSize_;

    cpl::EventQueue<WorkerThreadServiceMessage*> inputServiceMessages_;
    cpl::EventQueue<WorkerThreadServiceMessage*> outputServiceMessages_;
};

#endif // MONZZA_WORKER_THREAD_HPP