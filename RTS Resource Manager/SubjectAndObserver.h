#pragma once
#include <memory>

class Observer
{
public:
	virtual void update() = 0;
};

class Subject
{
public:
	virtual void addObserver(Observer *observer) = 0;
	// virtual void removeObserver(const std::shared_ptr<Observer>& observer) = 0;
	virtual void notifyObservers() = 0;
};
