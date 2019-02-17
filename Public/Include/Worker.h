/*****************************************************************************
*  @file     Worker.h
*  @brief    执行体。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/03/20 14:55:50
*****************************************************************************/
#ifndef Worker_h__
#define Worker_h__

#pragma once
#include "Thread.h"
#include "KernelEvent.h"

class CWorker : public IRunnable
{
public:

	CWorker(bool bBlock, std::string strName = std::string());

	virtual ~CWorker();
		
	virtual bool Init() override;

	virtual bool Start() override;

	virtual bool Run() override;

	virtual bool Exit() override;

	virtual bool IsRuning() override;

	virtual void Wait(unsigned long dwMilliseconds) override;

private:
	bool m_bBlock;								///< 阻塞执行线程。
	CThread m_RunThread;						///< 线程对象。
	CKernelEvent m_ExitEvent;					///< 线程执行状态事件。
};

#endif // Worker_h__