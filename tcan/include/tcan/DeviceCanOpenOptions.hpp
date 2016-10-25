/*
 * DeviceCanOpenOptions.hpp
 *
 *  Created on: Mar 27, 2016
 *      Author: Philipp Leemann
 */

#pragma once

#include "tcan/DeviceOptions.hpp"

namespace tcan {

class DeviceCanOpenOptions : public DeviceOptions {
 public:

    DeviceCanOpenOptions() = delete;

    DeviceCanOpenOptions(
        const uint32_t nodeId,
        const std::string name,
        const unsigned int maxSdoTimeoutCounter = 1,
        const unsigned int maxSdoSentCounter = 10,
        const uint16_t producerHeartBeatTime = 0,
        const unsigned int maxDeviceTimeoutCounter = 20):
        DeviceOptions(nodeId, name, maxDeviceTimeoutCounter),
        maxSdoTimeoutCounter_(maxSdoTimeoutCounter),
        maxSdoSentCounter_(maxSdoSentCounter),
        producerHeartBeatTime_(producerHeartBeatTime)
    {

    }

    virtual ~DeviceCanOpenOptions() { }

    inline void setSdoTimeoutCounter(const double timeout, const double looprate) {
        maxSdoTimeoutCounter_ = static_cast<unsigned int>(timeout*looprate);
    }

    //! counter limit at which an SDO is considered as timed out. Set 0 to disable.
    // maxSdoTimeoutCounter = timeout [s] * looprate [Hz] (looprate = rate of checkSanity(..) calls. In asynchrounous mode this is 1Hz by default (see BusOptions))
    unsigned int maxSdoTimeoutCounter_;

    //! number of tries of an SDO transmission
    unsigned int maxSdoSentCounter_;

    //! Heartbeat time interval [ms], produced by the device. Set to 0 to disable heartbeat message reception checking.
    uint16_t producerHeartBeatTime_;

};

} /* namespace tcan */
