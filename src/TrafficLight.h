#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum TrafficLightPhase
{
	red,
  	green,
};

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

	std::deque<T> _queue;
    
private:
	std::condition_variable _conditionMsgQueue;
    std::mutex _mutexMsgQueue;
    
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    // getters / setters
    TrafficLightPhase getCurrentPhase();
    // typical behaviour methods
	void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
	void cycleThroughPhases();

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
	std::shared_ptr<MessageQueue<TrafficLightPhase>> _messages;
};

#endif