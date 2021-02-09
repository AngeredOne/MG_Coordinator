#pragma once
#include <iostream>
#include <list>
#include <queue>
#include <memory>
#include <map>
#include <thread>

class LocalStream;
class ServerClient;

typedef u_int16_t uint16;
typedef u_int32_t uint32;

struct StructBroken : public std::exception
{
    const char *what() const throw()
    {
        return "The available number of bytes does not match the size of the structure.";
    }
};

class StreamManager
{
public:
    void Start(std::shared_ptr<ServerClient>);
    std::shared_ptr<LocalStream> GetStream(int port);
    void ProceedDataOnPort(int port, char *data, int length);
    void ExecuteCommands();

    ~StreamManager();

private:
    std::queue<std::shared_ptr<LocalStream>> handledStreams;
    std::map<uint16, std::shared_ptr<LocalStream>> streams;
    std::shared_ptr<std::thread> managerThread = nullptr;
    std::shared_ptr<ServerClient> client_ptr;
};

class LocalStream : public std::iostream
{
public:
    LocalStream(uint16 opcode);
    void AddLength(uint32);
    void Handle();
    void Flush();
    const bool IsHandler();
    const bool IsInQueue();
    const uint16 GetOpcode();

    template <class T>
    std::shared_ptr<T> GetStruct()
    {
        while (packetSizes.empty());
        char *data = new char[packetSizes.front()];
        packetSizes.pop();
        return std::shared_ptr<T>(reinterpret_cast<T *>(data));
    }

    template <class T>
    std::vector<std::shared_ptr<T>> GetAllStructs()
    {
        while (packetSizes.empty());
        // Save size of data block
        auto size = packetSizes.front();
        // Get items count by block's size and size of struct
        auto count = size / sizeof(T);
        //Storage
        std::vector<std::shared_ptr<T>> out;

        if (size % sizeof(T) != 0)
        {
            throw StructBroken();
        }
        //Allocate new block for data
        char *data = new char[size];
        packetSizes.pop();

        //Fill allocated memory block with data from local netStream
        read(data, size);

        //Make collection(T*) of T structs from raw bytes(filled data)
        T *collection = reinterpret_cast<T *>(data);

        //Aggregate all elements
        for (int i = 0; i < count; i++)
        {
            out.push_back(std::make_shared<T>(collection[i]));
        }
        return out;
    }

private:
    uint16 opcode = 65535;
    std::queue<uint32> packetSizes;
    //Is in Queue of stream manager
    bool IsHandled = false;
    //Is needed to execute handler
    bool IsHandable = true;
};