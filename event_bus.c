
static Event_t *eventQ = NULL;
static EventListener_t *pEventListeners[EVENT_TYPES] = { NULL };

static bool isDispatcherActive = false;

static uint16_t eventCnt = 0;


inline uint16_t CORE_GetEventCount(void) {
  return eventCnt;
}

/**
  * @brief  Adding an event to the end of the queue
  * @param  eventType EventType_t being pushed
  * @param  size size_t of data
  * @retval data Pointer to xxxEventData_t
  */
void* CORE_AddEvent(EventType_t eventType, void *data, size_t size) {

  void *dataPtr = NULL;

  // Only add events when dispatch is active
  if (!isDispatcherActive)
    return dataPtr;

  // Prevent overflowing event bus
  assert(eventCnt < EVENT_BUS_LIMIT);

  if (eventQ != NULL) {
    Event_t *ev = eventQ;

    // Find the end of the linked-list
    while (ev->next != NULL) {
        ev = ev->next;
    }

    /* now we can add a new variable */
    ev->next = (Event_t *) malloc(sizeof(Event_t));
    if(ev->next == NULL)
      return dataPtr;

    ev->next->event = eventType;
    ev->next->eventTick = HAL_GetTick();

    if (data != NULL) {
      ev->next->data = malloc(size);
      memcpy(ev->next->data, data, size);
      dataPtr = ev->next->data;

    } else {
      ev->next->data = NULL;
    }
    ev->next->next = NULL;

  } else {

    // eventQ head was NULL so create new linked-list head
    eventQ = (Event_t *) malloc(sizeof(Event_t));
    if (eventQ == NULL)
      return dataPtr;

    eventQ->event = eventType;
    eventQ->eventTick = HAL_GetTick();

    if (data != NULL) {
      eventQ->data = malloc(size);
      memcpy(eventQ->data, data, size);
      dataPtr = eventQ->data;

    } else {
      eventQ->data = NULL;
    }
    eventQ->next = NULL;
  }
  eventCnt++;
  return dataPtr;
}


/**
  * @brief  Removing the first event from queue
  * @retval None
  */
void CORE_ShiftEventQ(void) {

  if (eventQ == NULL) {
      return;
  }

  Event_t *nextEv = NULL;
  if ((eventQ)->next != NULL) {
    nextEv = (eventQ)->next;
  }

  if((eventQ)->data != NULL) {
    free((eventQ)->data);
  }

  free(eventQ);
  eventQ = nextEv;

  eventCnt--;
}


/**
  * @brief  Removing the last event of the queue
  * @retval None
  */
void CORE_PopEventQ(void) {
    /* if there is only one item in the list, remove it */
    if (eventQ->next == NULL) {
      if (eventQ->data != NULL) {
        free(eventQ->data);
      }
      free(eventQ);
      eventQ = NULL;
      eventCnt--;
      return;
    }

    /* get to the second to last node in the list */
    Event_t *current = eventQ;
    while (current->next->next != NULL) {
        current = current->next;
    }

    /* now current points to the second to last item of the list, so remove current->next */
    if (current->next->data != NULL) {
      free(current->next->data);
    }
    free(current->next);
    current->next = NULL;
    eventCnt--;
}


/**
  * @brief  Remove event from queue
  * @param  Event_t pointer to event
  * @retval None
  */
void CORE_RemoveEventByRef(Event_t *pEvent) {
  // first event so just use shift function
  if (pEvent == eventQ) {
    CORE_ShiftEventQ();
    return;
  }

  // Get the previous event to the searched event to update it's 'next' pointer
  Event_t *prevNode = eventQ;
  while (prevNode->next != NULL) {
    if ( prevNode->next == pEvent ) {
      break;
    }
    prevNode = prevNode->next;
  }

  // Unable to find node
  if ( prevNode == NULL ) {
    return;
  }

  // move next of searched event to next of previous event
  prevNode->next = pEvent->next;
  if ( pEvent->data != NULL ) {
    free(pEvent->data);
  }
  free(pEvent);
  eventCnt--;
}


/**
  * @brief  Adding listener to the end of event listeners
  * @param  eventType EventType_t list to push
  * @retval
  */
void CORE_AddListener(EventType_t eventType, void (*listener)(void *)) {

  // Check if event has existing listener
  if (pEventListeners[eventType] != NULL) {
    EventListener_t *el = pEventListeners[eventType];
    if (el->func == listener)
      return;  // Prevent listeners from getting added more than once

    // Find the end of the linked-list
    while (el->next != NULL) {
      if (el->next->func == listener)
        return;  // Prevent listeners from getting added more than once

      el = el->next;
    }

    /* now we can add a new variable */
    el->next = (EventListener_t *) calloc(1, sizeof(EventListener_t));
    if (el->next == NULL)
      return;

    el->next->func = listener;
    el->next->next = NULL;

  } else {
    // eventQ head was NULL so create new linked-list head
    pEventListeners[eventType] = (EventListener_t *) calloc(1, sizeof(EventListener_t));
    if (pEventListeners[eventType] == NULL)
      return;
    pEventListeners[eventType]->func = listener;
    pEventListeners[eventType]->next = NULL;
  }
}


/**
  * @brief  Removing the first listener from event listeners
  * @param  eventType EventType_t list to shift
  * @retval None
  */
void CORE_ShiftEventListeners(EventType_t eventType) {

    if (pEventListeners[eventType] == NULL) {
        return;
    }

    EventListener_t *nextEl = NULL;
    if ((pEventListeners[eventType])->next != NULL) {
      nextEl = (pEventListeners[eventType])->next;
    }

    free(pEventListeners[eventType]);
    pEventListeners[eventType] = nextEl;
}

/**
  * @brief  Removing the last listener of the event listeners
  * @param  eventType EventType_t list to pop
  * @retval None
  */
void CORE_PopEventListeners(EventType_t eventType) {
    /* if there is only one item in the list, remove it */
    if (pEventListeners[eventType]->next == NULL) {
      free(pEventListeners[eventType]);
      return;
    }

    /* get to the second to last node in the list */
    EventListener_t *current = pEventListeners[eventType];
    while (current->next->next != NULL) {
      current = current->next;
    }

    /* now current points to the second to last item of the list, so remove current->next */
    free(current->next);
    current->next = NULL;
}


/**
  * @brief  Removing listener matching pListener
  * @param  eventType EventType_t list to search
  * @retval None
  */
void CORE_RemoveListenerByRef(EventType_t eventType, void *pListener) {

  // first event so just use shift function
  if (pEventListeners[eventType]->func == pListener) {
    CORE_ShiftEventListeners(eventType);
    return;
  }

  // Get the previous event to the searched event to update it's 'next' pointer
  EventListener_t *prevNode = pEventListeners[eventType];
  while (prevNode->next != NULL) {
    if ( prevNode->next->func == pListener ) {
      break;
    }
    prevNode = prevNode->next;
  }

  // Unable to find node
  if ( prevNode->next == NULL ) {
    return;
  }

  // move next of searched to next of previous
  EventListener_t *delNode = prevNode;
  prevNode->next = prevNode->next->next;
  free(delNode);
}


/**
  * @brief  Removes all listeners from event type
  * @param  eventType EventType_t list to search
  * @retval None
  */
void CORE_RemoveEventListeners(EventType_t evType) {
  EventListener_t *temp = pEventListeners[evType];
  EventListener_t *next = NULL;

  while (temp != NULL) {
    next = temp->next;
    free(temp);
    temp = next;
  }

  if (pEventListeners[evType] != NULL) {
    free(pEventListeners[evType]);
    pEventListeners[evType] = NULL;
  }
}


/**
  * @brief  Event loop processor
  * @note   Should be called in main loop
  * @retval None
  */
void CORE_EventDispatcher(void) {

  if (!isDispatcherActive)
    isDispatcherActive = true;

  uint32_t now = HAL_GetTick();

  // Event Listener Dispatcher
  while(eventQ != NULL) {

    /* Prevent getting stuck in loop by not running any event entered after
     dispatcher started */
    if (eventQ->eventTick > now) {
      break;
    }

    EventListener_t *el = pEventListeners[eventQ->event];
    while(el != NULL) {
      el->func(eventQ->data);
      el = el->next;
    }
    CORE_ShiftEventQ();
  }
}
