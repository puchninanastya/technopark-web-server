#ifndef MONZZA_BUFFER_HPP
#define MONZZA_BUFFER_HPP

#include <cstdint>
#include <string>
#include <cstring>

#define MONZZA_BUFFER_CAPACITY_DEFAULT 512

namespace monzza {
    namespace buffer {
        class Buffer {
        public:
            explicit Buffer( uint32_t bufSize = MONZZA_BUFFER_CAPACITY_DEFAULT );
            ~Buffer();

            bool write( uint8_t *data, uint32_t dataLen );
            bool read( uint8_t *data, uint32_t dataLen );
            bool clean();

            void setCurrentPos( uint32_t newPos );
            uint32_t getCurrentPos();

            void setCurrentPosChar( uint8_t newChar );
            uint8_t getCurrentPosChar();
            uint8_t getPosChar( uint32_t pos );

            bool noMoreData();

            void increaseCurrentPos( uint32_t numBytes = 1);
            void decreaseCurrentPos( uint32_t numBytes = 1);

            uint32_t getWrittenDataSize();
            bool checkAvailableBufferSpace( uint32_t dataLen = 1 );
            bool checkAvailableDataSize( uint32_t dataLen = 1 );
            bool checkAvailableDataSizeFromPos( uint32_t pos, uint32_t dataLen = 1 );

            bool copyToString( std::string& str, int32_t bufStartPos, uint32_t bytesToCopy );

            int32_t compareToString( int32_t bufPosToCompareFrom,
                                     std::string stringToCompare,
                                     uint32_t numBytesToCompare );

        protected:
            bool increaseBufferCapacity( uint32_t dataLen = 0 );

        private:
            uint8_t* buf_;
            uint32_t capacity_;
            uint32_t currentPos_;
            int32_t  endDataPos_;
        };
    }
}

#endif //MONZZA_BUFFER_HPP
