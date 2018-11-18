#include "buffer.hpp"

using namespace monzza::buffer;

Buffer::Buffer( uint32_t bufSize ) {
    capacity_ = bufSize;
    buf_ = new uint8_t[ capacity_ ];
    memset( buf_, 0, capacity_ );

    currentPos_ = 0;
    endDataPos_ = -1;
}

Buffer::~Buffer() {
    delete[] buf_;
}

bool Buffer::write( uint8_t* data, uint32_t dataLen ) {
    while ( !checkAvailableBufferSpace( dataLen ) ) {
        // TODO: add check (i don't need endless loop)
        increaseBufferCapacity( dataLen );
    }

    memcpy( buf_ + endDataPos_ + 1, data, dataLen );
    endDataPos_ += dataLen;

    return true;
}

bool Buffer::read( uint8_t* data, uint32_t dataLen ) {
    if ( !checkAvailableDataSize( dataLen ) ) {
        return false;
    }

    memcpy( data, buf_ + currentPos_, dataLen );
    currentPos_ += dataLen;

    return true;
}

bool Buffer::clean() {
    memset( buf_, 0, capacity_ );
    currentPos_ = 0;
    endDataPos_ = -1;
}

void Buffer::setCurrentPos( uint32_t newPos ) {
    currentPos_ = newPos;
}

uint32_t Buffer::getCurrentPos() {
    return currentPos_;
}

void Buffer::setCurrentPosChar( uint8_t newChar ) {
    buf_[ currentPos_ ] = newChar;
}

uint8_t Buffer::getCurrentPosChar() {
    return buf_[ currentPos_ ];
}

uint8_t Buffer::getPosChar( uint32_t pos ) {
    return buf_[ pos ];
}

bool Buffer::noMoreData() {
    return ( currentPos_ == ( endDataPos_ + 1 ) );
}

void Buffer::increaseCurrentPos( uint32_t numBytes ) {
    currentPos_ = currentPos_ + numBytes;
    // TODO: increase buffer capacity?
}

void Buffer::decreaseCurrentPos( uint32_t numBytes ) {
    currentPos_ = currentPos_ - numBytes;
    // TODO: decrease buffer capacity?
}

bool Buffer::checkAvailableBufferSpace( uint32_t dataLen ) {
    return ( capacity_ != ( endDataPos_ + 1 ) );
}

bool Buffer::increaseBufferCapacity( uint32_t dataLen ) {
    capacity_ = capacity_ * 2; // TODO add dataLen check
    auto* newBuf = new uint8_t[ capacity_ ];
    memcpy(newBuf, buf_, sizeof( *( buf_ ) ) );
    delete[] buf_;
    buf_ = newBuf;
    return true;
}

uint32_t Buffer::getWrittenDataSize() {
    return static_cast<uint32_t > ( ( endDataPos_ >= 0 ) ? endDataPos_+ 1 : 0 );
}

bool Buffer::checkAvailableDataSize( uint32_t dataLen ) {
    return ( endDataPos_ != 0 && ( endDataPos_ + 1 - currentPos_ >= dataLen ) );
}

bool Buffer::checkAvailableDataSizeFromPos( uint32_t dataLen, uint32_t pos ) {
    return ( endDataPos_ != 0 && ( endDataPos_ - pos >= dataLen ) );
}

int32_t Buffer::compareToString( int32_t bufPosToCompareFrom,
                                 std::string stringToCompare,
                                 uint32_t numBytesToCompare )
{
    std::string bufString;
    copyToString( bufString, bufPosToCompareFrom, numBytesToCompare );
    return ( strncmp( bufString.c_str(), stringToCompare.c_str(), numBytesToCompare ) );
}

bool Buffer::copyToString( std::string& str, int32_t bufStartPos, uint32_t bytesToCopy ) {
    auto bufToCopy = new uint8_t[ bytesToCopy + 1 ];
    memset( bufToCopy, 0, ( bytesToCopy + 1 ) );
    memcpy( bufToCopy, ( buf_ + bufStartPos ), bytesToCopy );
    bufToCopy[ bytesToCopy ] = '\0';

    str = std::string( ( char* ) bufToCopy, ( bytesToCopy ) ); // TODO: memory leak + fix

    delete[] bufToCopy;
    return true;
}
