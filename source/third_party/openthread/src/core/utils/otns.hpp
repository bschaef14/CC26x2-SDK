/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file wraps the calls to platform OTNS abstrations.
 */

#ifndef UTILS_OTNS_HPP_
#define UTILS_OTNS_HPP_

#include "openthread-core-config.h"

#if (OPENTHREAD_MTD || OPENTHREAD_FTD) && OPENTHREAD_CONFIG_OTNS_ENABLE

#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/platform/otns.h>

#include "common/locator.hpp"
#include "common/notifier.hpp"
#include "mac/mac_types.hpp"
#include "net/ip6_address.hpp"
#include "thread/topology.hpp"

namespace ot {
namespace Utils {

/**
 * This class implements the OTNS Stub that interacts with OTNS.
 *
 */
class Otns : public InstanceLocator
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Otns(Instance &aInstance)
        : InstanceLocator(aInstance)
        , mNotifierCallback(aInstance, &Otns::HandleStateChanged, this)
    {
    }

    /**
     * This function emits radio short address to OTNS when changed.
     *
     * @param[in]  aShortAddress  The new short address.
     *
     */
    static void EmitShortAddress(uint16_t aShortAddress);

    /**
     * This function emits radio extended address to OTNS when changed.
     *
     * @param[in]  aExtAddress  The new extended address.
     *
     */
    static void EmitExtendedAddress(const Mac::ExtAddress &aExtAddress);

    /**
     * This function emits ping request information to OTNS when sending.
     *
     * @param[in]  aPeerAddress  The peer address of the ping request.
     * @param[in]  aPingLength   The data length of the ping request.
     * @param[in]  aTimestamp    The timestamp of the ping request.
     * @param[in]  aHopLimit     The hop limit of the ping request.
     *
     */
    static void EmitPingRequest(const Ip6::Address &aPeerAddress,
                                uint16_t            aPingLength,
                                uint32_t            aTimestamp,
                                uint8_t             aHopLimit);

    /**
     * This function emits ping reply information to OTNS when received.
     *
     * @param[in]  aPeerAddress  The peer address of the ping request.
     * @param[in]  aPingLength   The data length of the ping reply.
     * @param[in]  aTimestamp    The timestamp of the ping reply.
     * @param[in]  aHopLimit     The hop limit of the ping reply.
     *
     */
    static void EmitPingReply(const Ip6::Address &aPeerAddress,
                              uint16_t            aPingLength,
                              uint32_t            aTimestamp,
                              uint8_t             aHopLimit);

    /**
     * This function emits a neighbor table event to OTNS when a neighbor is added or removed.
     *
     * @param[in]  aEvent     The event type.
     * @param[in]  aNeighbor  The neighbor that is added or removed.
     *
     */
    static void EmitNeighborChange(otNeighborTableEvent aEvent, Neighbor &aNeighbor);

private:
    static void EmitStatus(const char *aFmt, ...);

    static void HandleStateChanged(Notifier::Callback &aCallback, otChangedFlags aFlags);
    void        HandleStateChanged(otChangedFlags aFlags);

    Notifier::Callback mNotifierCallback;
};

} // namespace Utils
} // namespace ot

#endif //(OPENTHREAD_MTD || OPENTHREAD_FTD) && OPENTHREAD_CONFIG_OTNS_ENABLE

#endif // UTILS_OTNS_HPP_
