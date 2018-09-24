#ifndef MONZZA_MASTER_THREAD_HPP
#define MONZZA_MASTER_THREAD_HPP

#include <map>
#include <vector>

#include "../../external/cpl/cpl.hpp"
#include "../../external/cpl/cpl_event_queue.hpp"

#include "../worker/worker.hpp"
#include "../logger/log_functionality.hpp"

#include "master_settings.hpp"

class MasterThreadServiceMessage {
public:
    enum class ResponseType {
        NOT,
        SUCCESSFUL,
        FAILURE,
    };

    enum class CommandType {
        STOP
    };

    bool setResponseType ( ResponseType responseType );
    ResponseType getResponseType() const;

    bool setCommandType( CommandType commandType );
    CommandType getCommandType() const;
private:
    ResponseType responseType_;
    CommandType  commandType_;
};

class MasterThread : public LogFunctionality {
public:
    MasterThread();
    ~MasterThread();

    bool initialize( Logger* logger, MasterSettings* masterSettings );

    cpl::Event* getNewOutputServiceMessageEvent();
    void addInputServiceMessage( MasterThreadServiceMessage* masterThreadServiceMessage );
    MasterThreadServiceMessage* getOutputServiceMessage();

    void operator()();
private:
    void processInputServiceMessage();
    void serviceMsgHandler( MasterThreadServiceMessage* masterThreadServiceMessage );
    void stopServiceMsgHandler( MasterThreadServiceMessage* masterThreadServiceMessage );

    void updateOptimalWorkerIdToUse();

    void processNewAcceptConnection();

    bool breakThreadLoop_;
    std::vector<cpl::Event*> events_;

    Logger* logger_;
    Table* table_;
    MasterSettings* masterSettings_;

    cpl::TcpServerListenSocket listenSocket_;
    cpl::Event listenSocketEvent_;

    uint32_t workerIdToUse_;
    std::map<uint32_t, std::pair<uint32_t, uint32_t>> workersInformation_;
    std::map<uint32_t, std::pair<Worker*, WorkerSettings*>> workers_;

    cpl::EventQueue<MasterThreadServiceMessage*> inputServiceMessages_;
    cpl::EventQueue<MasterThreadServiceMessage*> outputServiceMessages_;
};

#endif // MONZZA_MASTER_THREAD_HPP
