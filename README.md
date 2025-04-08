# stm32-eventbus
A simple event bus for STM32 MCUs

## Usage
1) Add event names to EventType_t enum in eventbus.h
2) Run CORE_EventDispatcher() in main while loop
3) Add events to bus using CORE_AddEvent
4) Add listeners to events with CORE_AddListener
