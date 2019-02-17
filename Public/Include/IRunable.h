#ifndef IRunable_h__
#define IRunable_h__

#pragma once
#include <memory>

class IRunnable : public std::enable_shared_from_this<IRunnable>
{
public:

	virtual bool Init() = 0;

	virtual bool Start() = 0;

	virtual bool Run() = 0;

	virtual bool Exit() = 0;

	virtual bool IsRuning() = 0;

	virtual void Wait(unsigned long dwMilliseconds) = 0;
};

#endif // IRunable_h__