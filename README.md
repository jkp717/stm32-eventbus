# stm32-eventbus
A simple linked-list event bus for STM32 MCUs

# Setup
To use, add eventbus.c and eventbus.h to application.  Create event type names in EventType_t enum in eventbus.h.

# Usage
Add the CORE_EventDispatcher() in main while loop.  This function checks for pending events and calls corresponding listeners.  To fire events on the bus, use CORE_AddEvent.  To add listeners to a specific event use CORE_AddListener.  Several functions for removing events or listeners also are provided.
