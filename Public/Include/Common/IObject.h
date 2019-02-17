/*****************************************************************************
*  @file     IObject.h
*  @brief    核心主对象基类，都会存在一个唯一的ID。
*  Details.                                                              
*                                                                           
*  @author   IDeath                                                  
*  @email    IDeath@operatorworld.com                                   
*  @version  0.0.0.1														
*  @date	 2017/02/18 16:36:56
*****************************************************************************/
#ifndef IObject_h__
#define IObject_h__

#pragma once
#include "Global.h"

class IObject
{
public:

	virtual ~IObject() {};

	virtual int GetObjectID() { return INVALID_OBJECTID; };

	virtual int SetObjectID(int nObjectID) { return INVALID_OBJECTID; };
};

#endif // IObject_h__