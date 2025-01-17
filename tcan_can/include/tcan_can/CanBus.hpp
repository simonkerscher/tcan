#pragma once

#include <stdint.h>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector>

#include "tcan/Bus.hpp"
#include "tcan_can/CanBusOptions.hpp"
#include "tcan_can/CanMsg.hpp"
#include "tcan_can/CanDevice.hpp"

namespace tcan_can {

class CanBus : public tcan::Bus<CanMsg> {
 public:

    struct CanFrameIdentifier {
        explicit CanFrameIdentifier(uint32_t id, uint32_t msk = 0xffffffffu) : identifier(id), mask(msk) {};
        uint32_t identifier;
        uint32_t mask;

        bool operator==(const CanFrameIdentifier& rhs) const {
            return identifier == rhs.identifier && mask == rhs.mask;
        }
    };

    struct CanFrameIdentifierHasher {
        size_t operator()(const CanFrameIdentifier& matcher) const {
            size_t h = 0;
            boost::hash_combine(h, matcher.identifier);
            boost::hash_combine(h, matcher.mask);
            return h;
        };
    };

    using CallbackPtr =  std::function<bool(const CanMsg&)>;
    using CanFrameIdentifierToFunctionMap = std::unordered_map<CanFrameIdentifier, std::pair<CanDevice*, CallbackPtr>, CanFrameIdentifierHasher>;
    using DeviceContainer = std::vector<CanDevice*>;

    CanBus() = delete;
    CanBus(std::unique_ptr<CanBusOptions>&& options);

    ~CanBus() override;

    /*!
     * in-place construction of a new device
     * @param options   pointer to the option class of the device
     * @return true if successful
     */
    template <class C, typename TOptions>
    inline std::pair<C*, bool> addDevice(std::unique_ptr<TOptions>&& options) {
        C* dev = new C(std::move(options));
        bool success = addDevice(dev);
        return std::make_pair(dev, success);
    }

    /*! Adds a device to the device vector and calls its initDevice function
     * @param device    Pointer to the device
     * @return true if init was successful
     */
    inline bool addDevice(CanDevice* device) {
        devices_.push_back(device);
        return device->initDeviceInternal(this);
    }

    /*! Adds a device and callback function for incoming messages identified by its CAN frame identifier. The timeout
     *  counter of the device is reset on reception of the message (treated as heartbeat).
     * @param canFrameId        29 or 11 bit frame ID of the message
     * @param device            pointer to the device
     * @param fp                pointer to the parse function
     * @return true if successful
     */
    template <class T>
    inline bool addCanMessage(const uint32_t canFrameId, T* device, bool(std::common_type<T>::type::*fp)(const CanMsg&), typename std::enable_if<!std::is_base_of<CanDevice, T>::value>::type* = 0)
    {
        return canFrameIdentifierToFunctionMap_.emplace(CanFrameIdentifier{canFrameId }, std::make_pair(nullptr, std::bind(fp, device, std::placeholders::_1))).second;
    }

    template <class T>
    inline bool addCanMessage(const uint32_t canFrameId, T* device, bool(std::common_type<T>::type::*fp)(const CanMsg&), typename std::enable_if<std::is_base_of<CanDevice, T>::value>::type* = 0)
    {
        return canFrameIdentifierToFunctionMap_.emplace(CanFrameIdentifier{canFrameId }, std::make_pair(device, std::bind(fp, device, std::placeholders::_1))).second;
    }

    /*! Like addCanMessage with a specific CanId, but matches against a range of CanIds through a mask.
    * To match all messages, 0x..FA..33, one would pass CanFrameIdentifier { 0x00FA0033, 0x00FF00FF }, i.e. the ID and the mask.
    * Bits in the ID that correspond to zeros in the mask are ignored.
    * @param matcher           CanFrameIdentifier for the message
    * @param device            pointer to the device
    * @param fp                pointer to the parse function
    * @return true if successful
    */
    template <class T>
    inline bool addCanMessage(const CanFrameIdentifier matcher, T* device, bool(std::common_type<T>::type::*fp)(const CanMsg&), typename std::enable_if<!std::is_base_of<CanDevice, T>::value>::type* = 0)
    {
        return canFrameIdentifierToFunctionMap_.emplace(matcher, std::make_pair(nullptr, std::bind(fp, device, std::placeholders::_1))).second;
    }

    template <class T>
    inline bool addCanMessage(const CanFrameIdentifier matcher, T* device, bool(std::common_type<T>::type::*fp)(const CanMsg&), typename std::enable_if<std::is_base_of<CanDevice, T>::value>::type* = 0)
    {
        return canFrameIdentifierToFunctionMap_.emplace(matcher, std::make_pair(device, std::bind(fp, device, std::placeholders::_1))).second;
    }

    /*! Send a sync message on the bus. Is called by BusManager::sendSyncOnAllBuses or directly.
     */
    inline void sendSync() {
        sendMessage(CanMsg(0x80, 0, nullptr));
    }

    /*!
     * @return  Container with all devices handled by this bus
     */
    const DeviceContainer& getDeviceContainer() const { return devices_; }

    /*!
     * Resets all devices handled by this bus to Initializing state and sends appropriate restart commands to the devices
     */
    void resetAllDevices();

    /*!
     * Set the callback function to be called for incoming messages with an id not found in the callback function map
     * @param callbackPtr std::function wrapper containing the callback function pointer
     */
    inline void setUnmappedMessageCallback(const CallbackPtr& callbackPtr) {
        // todo: protect this with a mutex? (also inside handleMessage)
        unmappedMessageCallbackFunction_ = callbackPtr;
    }

    bool defaultHandleUnmappedMessage(const CanMsg& msg);


 public:/// INTERNAL FUNCTIONS
    /*! Send a sync message on the bus without locking the queue.
     * This function is intended to be used by BusManager::sendSyncOnAllBuses, which locks the queue.
     */
    inline void sendSyncWithoutLock() {
        sendMessageWithoutLock(CanMsg(0x80, 0, nullptr));
    }

    /*! Is called after reception of a message. Routes the message to the callback and clears the errorMsgFlag_
     * @param cmsg	reference to the can message
     */
    void handleMessage(const CanMsg& cmsg) override;

    /*! Do a sanity check of all devices on this bus.
     */
    bool sanityCheck() override;

 protected:
    // vector containing all devices
    DeviceContainer devices_;

    // map mapping COB id to parse functions
    CanFrameIdentifierToFunctionMap canFrameIdentifierToFunctionMap_;

    // function pointer to be called for unmapped COB ids
    CallbackPtr unmappedMessageCallbackFunction_;
};

} /* namespace tcan_can */
