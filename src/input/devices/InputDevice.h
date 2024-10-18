//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_INPUTDEVICE_H
#define SURVIVING_SARNTAL_INPUTDEVICE_H

#include "../events/DeviceEvent.h"
#include "../events/GameEvent.h"
#include "Device.h"
#include <list>
#include <map>
#include <queue>
#include <unordered_map>
/**
 * An input device is characterised by its inputs and how they are mapped to
 * corresponding events in the game.
 */
class InputDevice {
  public:
    explicit InputDevice(int deviceId, Device device, const InputConstants &inputConstants);
    virtual ~InputDevice();
    /**
     * Checks for events currently happening on this device to control the game.
     *
     * @return list of events
     */
    virtual std::queue<GameEvent> getGameEvents() = 0;

    /**
     * Returns the ID of this device. The ID is only relevant for gamepads, as multiple such devices can be connected.
     * For all other devices, the ID is -1.
     *
     * @return The ID of this device
     */
    int getId() const;

    Device getDevice() const;

  protected:
    std::map<DeviceEvent, GameEvent, DeviceEventCompare> INPUT_MAPPINGS;

    /**
     * Maps a device event to the corresponding event for the game using the
     * INPUT_MAPPINGS.
     *
     * @param deviceEvent
     * @return corresponding game event
     */
    GameEvent getGameEvent(DeviceEvent deviceEvent) const;

    const InputConstants &inputConstants;

  private:
    const int deviceId;
    const Device device;
};

#endif // SURVIVING_SARNTAL_INPUTDEVICE_H
