#ifndef EVENT_BUS_H_
#define EVENT_BUS_H_

#define EVENT_BUS_LIMIT 50     // Limit of pending events on Event Bus

/*
 * Event Types
 */
typedef enum {
  // Add event types here
  EXAMPLE_EVENT_TYPE,
  LAST_EVENT_TYPE = EXAMPLE_EVENT_TYPE
} EventType_t;

#define EVENT_TYPES (LAST_EVENT_TYPE + 1)

/*
 * Event Listeners
 */
typedef struct _eventListener {
  void (*func)( void* );
  struct _eventListener *next;          // Next in linked list
} EventListener_t;

/*
 * Event Queue
 */
typedef struct _event {
  EventType_t        event;
  uint32_t           eventTick;   // When event was created (based on SysTick)
  void              *data;
  struct _event     *next;        // Next in linked list
} Event_t;


void* CORE_AddEvent(EventType_t eventType, void *data, size_t size);
void CORE_ShiftEventQ( void );
void CORE_PopEventQ( void );
void CORE_RemoveEventByRef(Event_t *pEvent);

void CORE_AddListener(EventType_t eventType, void (*listener)(void *));
void CORE_ShiftEventListeners(EventType_t eventType);
void CORE_PopEventListeners(EventType_t eventType);
void CORE_RemoveListenerByRef(EventType_t eventType, void *pListener);
void CORE_RemoveEventListeners(EventType_t evType);

uint16_t CORE_GetEventCount(void);
 
void CORE_EventDispatcher(void);

#endif /* INC_EVENT_BUS_H_ */
