#include "Streams.h"
#include "ServerClient.h"
#include "Server.h"

StreamManager::~StreamManager()
{
    managerThread->~thread();
}

void StreamManager::Start(std::shared_ptr<ServerClient> cptr)
{
    for (int i = 1; i < marxp::OP_CODE_LAST; ++i)
        streams.insert(std::pair<uint16, std::shared_ptr<LocalStream>>(i, std::make_shared<LocalStream>(i)));
    
    if (!managerThread)
        managerThread = std::make_shared<std::thread>(&StreamManager::ExecuteCommands, this);

    client_ptr = cptr;
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
    if (stream)
    {
        if (length != 0) {
            stream->write(data, length);
            stream->AddLength(length);
        }
        if (stream->IsHandler() || !stream->IsInQueue())
        {
            stream->Handle();
            handledStreams.push(stream);
        }
    }
}
void StreamManager::ExecuteCommands()
{
    while (true)
    {
        if (!handledStreams.empty())
        {
            auto stream = handledStreams.front();
            marxp::CoordinatorServer::Get().CallHandler(static_cast<marxp::OP_CODES>(stream->GetOpcode()), client_ptr);
            handledStreams.pop();
            stream->Flush();
        }
    }
}

LocalStream::LocalStream(uint16 opcode) : std::iostream(new streambuf)
{
    this->opcode = opcode;
};

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