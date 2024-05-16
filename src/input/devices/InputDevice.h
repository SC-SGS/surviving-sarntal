//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_INPUTDEVICE_H
#define SURVIVING_SARNTAL_INPUTDEVICE_H

#include "../events/DeviceEvent.h"
#include "../events/GameEvent.h"
#include <list>
#include <map>
#include <unordered_map>
/**
 * An input device is characterised by its inputs and how they are mapped to
 * corresponding events in the game.
 */
class InputDevice {
  public:
    virtual ~InputDevice();
    /**
     * Checks for events currently happening on this device to control the game.
     *
     * @return list of events
     */
    virtual std::list<GameEvent> getGameEvents() = 0;

  protected:
    std::map<DeviceEvent, GameEvent> INPUT_MAPPINGS;

    /**
     * Maps a device event to the corresponding event for the game using the
     * INPUT_MAPPINGS.
     *
     * @param deviceEvent
     * @return corresponding game event
     */
    GameEvent getGameEvent(DeviceEvent deviceEvent) const;
};

#endif // SURVIVING_SARNTAL_INPUTDEVICE_H
