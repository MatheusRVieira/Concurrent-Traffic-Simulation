#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutexMsgQueue);
    _conditionMsgQueue.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> uLock(_mutexMsgQueue);
  	_queue.clear(); // Needed to vehicles stop in red light of surrounding traffic lights 
    _queue.emplace_back(std::move(msg));
    _conditionMsgQueue.notify_one();
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
  	_messages = std::make_shared<MessageQueue<TrafficLightPhase>>();
}



void TrafficLight::waitForGreen()
{
    while(true)
    {
      	if(_messages->receive() == TrafficLightPhase::green) return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{ 
	threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));  
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
  	// Random value generation logic below was based on https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
  	std::random_device rd; // obtain a random number from hardware
    static std::mt19937 genLight(rd()); // seed the generator
    static std::uniform_int_distribution<> distr(4000, 6000); // define the range
  
	double cycleDurationTraffic = distr(genLight); // duration of a single simulation cycle in ms  	
  	std::chrono::time_point<std::chrono::system_clock> lastUpdateTraffic;
  
  	lastUpdateTraffic = std::chrono::system_clock::now();
  	while(true)
    {
      
    	long timeSpent = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdateTraffic).count();
      
      	if (timeSpent >= cycleDurationTraffic) 
        {
           _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
           _messages->send(std::move(_currentPhase));	
           lastUpdateTraffic = std::chrono::system_clock::now();
        } 
      
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
 
    }
}

