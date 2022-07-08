#include "stream/seadStream.h"

namespace sead
{
class PrintWriteStream : public WriteStream
{
public:
    explicit PrintWriteStream(StreamFormat*);
    explicit PrintWriteStream(Modes, bool);
    ~PrintWriteStream();
};
}