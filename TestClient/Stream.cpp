#include "Stream.h"

StreamManager::~StreamManager()
{
    managerThread->~thread();
}

void StreamManager::Start()
{
    streams.insert(std::pair<uint16, std::shared_ptr<LocalStream>>(11, std::make_shared<LocalStream>(11, new boost::asio::streambuf())));

    streams.insert(std::pair<uint16, std::shared_ptr<LocalStream>>(4, std::make_shared<LocalStream>(4, new boost::asio::streambuf())));


    if (!managerThread)
        managerThread = std::make_shared<std::thread>(&StreamManager::ExecuteCommands, this);
}

std::shared_ptr<LocalStream> StreamManager::GetStream(int port)
{
    if (auto stream = streams.find(port); stream != streams.end())
        return stream->second;
    else
        return nullptr;
}

void StreamManager::ProceedDataOnPort(int port, char *data, int length)
{
    auto stream = GetStream(port);
    stream->write(data, length);
    *stream << data;
    auto d = stream->gcount();

    stream->AddLength(length);
    if (stream->IsHandler() || stream->IsInQueue())
    {
        stream->Handle();
        handledStreams.push(stream);
    }
}
void StreamManager::ExecuteCommands()
{
    while (true)
    {
        if (!handledStreams.empty())
        {
            auto stream = handledStreams.front();
            //Handler
            handledStreams.pop();
            stream->Flush();
        }
    }
}

void LocalStream::Handle()
{
    IsHandled = true;
}
void LocalStream::Flush()
{
    IsHandled = false;
    this->clear();
    std::queue<uint32> q;
    packetSizes.swap(q);
}
const bool LocalStream::IsInQueue()
{
    return IsHandled;
}

const bool LocalStream::IsHandler()
{
    return IsHandable;
}

const uint16 LocalStream::GetOpcode()
{
    return opcode;
}

void LocalStream::AddLength(uint32 length)
{
    packetSizes.push(length);
}

LocalStream::LocalStream(uint16 opcode, std::streambuf* buf) : std::iostream(buf)
{
    this->opcode = opcode;
};