#include "stream/seadStream.h"
#include "filedevice/seadFileDevice.h"

namespace sead
{
class FileDeviceWriteStream : public WriteStream
{
public:
    explicit FileDeviceWriteStream(FileHandle*, Modes);
    ~FileDeviceWriteStream();
};
}