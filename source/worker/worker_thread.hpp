#ifndef MONZZA_WORKER_THREAD_HPP
#define MONZZA_WORKER_THREAD_HPP

#include <cstdint>
#include <map>
#include <vector>

#include "../../external/cpl/cpl_event_queue.hpp"

#include "../logger/log_functionality.hpp"
#include "../table/table.hpp"
#include "../http/http_request_parser.hpp"
#include "../http/http_request.hpp"
#include "../http/http_response.hpp"
#include "../http/http_response_serializer.hpp"
#include "../http/http_file_sender.hpp"

#include "worker_settings.hpp"

namespace monzza {
    namespace worker {
        // TODO: Add setters and getters.
        class TcpConnection {
        public:
            TcpConnection() {
                tcpServerExchangeSocket_ = nullptr;
                tcpServerExchangeSocketEvent_ = nullptr;
                httpRequestParser_ = nullptr;
                httpResponseSerializer_ = nullptr;
                sendingFile_ = false;
            }

            cpl::TcpServerExchangeSocket* tcpServerExchangeSocket_;
            cpl::Event* tcpServerExchangeSocketEvent_;
            monzza::http::HttpRequestParser* httpRequestParser_;
            monzza::http::HttpResponseSerializer* httpResponseSerializer_;

            bool sendingFile_;
            monzza::http::HttpFileDescription httpFileDescription_;
        };

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

        class WorkerThread : public monzza::logger::LogFunctionality {
        public:
            WorkerThread();
            ~WorkerThread();

            bool initialize( monzza::logger::Logger* logger,
                             monzza::table::Table* table,
                             WorkerSettings* workerSettings );

            cpl::Event* getNewOutputServiceMessageEvent();
            void addInputServiceMessage( WorkerThreadServiceMessage* workerThreadServiceMessage );
            WorkerThreadServiceMessage* getOutputServiceMessage();

            void operator()();
        private:
            void processInputServiceMessage();
            void serviceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMessage );
            void newConnectionServiceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMessage );
            void stopServiceMsgHandler( WorkerThreadServiceMessage* workerThreadServiceMessage );

            void processTcpServerExchangeSocketEvent( uint32_t index );
            bool processSocketData( uint32_t connectionIndex, int32_t readSize );
            bool processHttpRequestForConnection( uint32_t connectionIndex, monzza::http::HttpRequest* request );
            bool tryToSendTheRestOfTheFile( uint32_t connectionIndex );

            void clearDataForConnection( uint32_t connectionIndex );

            bool breakThreadLoop_;
            std::vector<cpl::Event*> events_;
            WorkerSettings* workerSettings_;
            monzza::table::Table* table_;

            std::vector<TcpConnection> tcpConnections_;
            monzza::http::HttpFileSender httpFileSender_;

            uint8_t* bufPtr_;
            uint16_t bufSize_;

            cpl::EventQueue<WorkerThreadServiceMessage*> inputServiceMessages_;
            cpl::EventQueue<WorkerThreadServiceMessage*> outputServiceMessages_;
        };
    }
}

#endif // MONZZA_WORKER_THREAD_HPP