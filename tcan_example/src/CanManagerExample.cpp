#include "tcan_example/CanManagerExample.hpp"

namespace tcan_example {

CanManagerExample::~CanManagerExample()
{
    // close Buses (especially their threads!) here, so that the receiveThread does not try to call a callback of a already destructed object (parseIncomingSync(..) in this case)
    closeBuses();
}

void CanManagerExample::init() {
    // add a CAN bus, asynchronous
    tcan_can::SocketBusOptions options;
    options.mode_ = tcan::BusOptions::Mode::Asynchronous;
    options.name_ = "can0";
    options.loopback_ = true;
    options.canErrorMask_ = CAN_ERR_MASK & ~CAN_ERR_LOSTARB; // report all errors but 'arbitration lost'
    // add (multiple) can filters like this {can_id, can_msg}:
    // options->canFilters.push_back({0x123, CAN_SFF_MASK});

    addSocketBus(BusId::BUS1, std::unique_ptr<tcan_can::SocketBusOptions>(new tcan_can::SocketBusOptions(options)));
    addDeviceExample(BusId::BUS1, static_cast<DeviceExampleId>(0), static_cast<NodeId>(1));

    // start the threads for semi-synchronous and asynchronous buses
    startThreads();
}

void CanManagerExample::addDeviceExample(const BusId busId, const DeviceExampleId deviceId, const NodeId nodeId) {
    const std::string name = "EXAMPLE_DEVICE" + std::to_string(static_cast<unsigned int>(deviceId));

    std::unique_ptr<CanDeviceExampleOptions> options(new CanDeviceExampleOptions(static_cast<uint32_t>(nodeId), name));
    options->someParameter = 37;
    options->maxDeviceTimeoutCounter_ = 10;

    auto ret_pair = getCanBus(static_cast<unsigned int>(busId))->addDevice<CanDeviceExample>( std::move(options) );
    deviceExampleContainer_.insert({static_cast<unsigned int>(deviceId), ret_pair.first});
}

void CanManagerExample::addSocketBus(const BusId busId, std::unique_ptr<tcan_can::SocketBusOptions>&& options) {

    auto bus = new tcan_can::SocketBus(std::move(options));
    if(!addBus( bus )) {
        MELO_FATAL_STREAM("failed to add bus " << bus->getName());
    }

    busContainer_.insert({static_cast<unsigned int>(busId), bus});
}

bool CanManagerExample::parseIncomingSyncBus1(const tcan_can::CanMsg& cmsg) {
    std::cout << "Bus1: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
    return true;
}

bool CanManagerExample::parseIncomingSyncBus2(const tcan_can::CanMsg& cmsg) {
    std::cout << "Bus2: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
    return true;
}

bool CanManagerExample::parseIncomingSyncBus3(const tcan_can::CanMsg& cmsg) {
    std::cout << "Bus3: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
    return true;
}

} /* namespace tcan_example */
