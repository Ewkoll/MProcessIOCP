/*****************************************************************************
*  @file     Thread.h
*  @brief    线程封装。
*  Details.                                                                 
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/03/20 14:32:44
*****************************************************************************/
#ifndef Thread_h__
#define Thread_h__

#pragma once
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include "IRunable.h"

class CThread final
{
public:

	CThread();

	virtual ~CThread();

	void Attach(const std::shared_ptr<IRunnable>& pRunable);

	bool Run(bool bBlock);

	bool GetThreadPriority(int& nPriority);

	bool SetThreadPriority(int nPriority);
		
	bool SuspendThread();

	bool ResumeThread();

	bool TerminateThread(int nExitCode = 0);

	std::string GetName() const;

	void SetName(const std::string& strName);
				
protected:
	std::thread m_RunThread;				///< 线程对象。
	std::string m_strName;					///< 线程名称。
	std::atomic<bool> m_bRunStatus;			///< 运行状态。
	std::weak_ptr<IRunnable> m_pRunable;	///< 可执行对象引用。
};

#endif // Thread_h__