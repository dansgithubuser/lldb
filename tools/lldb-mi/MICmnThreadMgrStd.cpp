//===-- MICmnThreadMgr.cpp --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:		MICmnThreadMgr.cpp
//
// Overview:	CMICmnThreadMgr implementation.
//
// Environment:	Compilers:	Visual C++ 12.
//							gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//				Libraries:	See MIReadmetxt. 
//
// Copyright:	None.
//--

// In-house headers:
#include "MICmnConfig.h"
#include "MICmnThreadMgrStd.h"
#include "MICmnLog.h"
#include "MICmnResources.h"
#include "MIUtilSingletonHelper.h"

//++ ------------------------------------------------------------------------------------
// Details:	CMICmnThreadMgr constructor.
// Type:	Method.
// Args:	None.
// Return:	None.
// Throws:	None.
//--
CMICmnThreadMgrStd::CMICmnThreadMgrStd( void )
{
}

//++ ------------------------------------------------------------------------------------
// Details:	CMICmnThreadMgr destructor.
// Type:	Method.
// Args:	None.
// Return:	None.
// Throws:	None.
//--
CMICmnThreadMgrStd::~CMICmnThreadMgrStd( void )
{
	Shutdown();
}

//++ ------------------------------------------------------------------------------------
// Details:	Initialize resources for *this thread manager.
// Type:	Method.
// Args:	None.
// Return:	MIstatus::success - Functional succeeded.
//			MIstatus::failure - Functional failed.
// Throws:	None.
//--
bool CMICmnThreadMgrStd::Initialize( void )
{
	m_clientUsageRefCnt++;

	if( m_bInitialized )
		return MIstatus::success;

	bool bOk = MIstatus::success;
	
	ClrErrorDescription();
	CMIUtilString errMsg;

	// Note initialization order is important here as some resources depend on previous
	MI::ModuleInit< CMICmnLog >      ( IDS_MI_INIT_ERR_LOG      , bOk, errMsg );
	MI::ModuleInit< CMICmnResources >( IDS_MI_INIT_ERR_RESOURCES, bOk, errMsg );

	m_bInitialized = bOk;
	
	if( !bOk )
	{
		CMIUtilString strInitError( CMIUtilString::Format( MIRSRC( IDS_MI_INIT_ERR_THREADMGR ), errMsg.c_str() ) );
		SetErrorDescription( strInitError );
		return MIstatus::failure;
	}

	return bOk;
}

//++ ------------------------------------------------------------------------------------
// Details:	Release resources for *this thread manager.
// Type:	Method.
// Args:	None.
// Return:	MIstatus::success - Functional succeeded.
//			MIstatus::failure - Functional failed.
// Throws:	None.
//--
bool CMICmnThreadMgrStd::Shutdown( void )
{
	if( --m_clientUsageRefCnt > 0 )
		return MIstatus::success;
	
	if( !m_bInitialized )
		return MIstatus::success;

	m_bInitialized = false;

	ClrErrorDescription();

	bool bOk = MIstatus::success;
	CMIUtilString errMsg;

	// Tidy up
	ThreadAllTerminate();
	
	// Note shutdown order is important here
	MI::ModuleShutdown< CMICmnResources >( IDE_MI_SHTDWN_ERR_RESOURCES, bOk, errMsg );
	MI::ModuleShutdown< CMICmnLog >      ( IDS_MI_SHTDWN_ERR_LOG      , bOk, errMsg );

	if( !bOk )
	{
		SetErrorDescriptionn( MIRSRC( IDS_MI_SHUTDOWN_ERR ), errMsg.c_str() );
	}

	return bOk;
}

//++ ------------------------------------------------------------------------------------
// Details:	Ask the thread manager to kill all threads and wait until they have died
// Type:	Method.
// Args:	None.
// Return:	MIstatus::success - Functional succeeded.
//			MIstatus::failure - Functional failed.
// Throws:	None.
//--
bool CMICmnThreadMgrStd::ThreadAllTerminate( void )
{
	// Find an iterator object for the list
	ThreadList_t::const_iterator it = m_threadList.begin();
	
	// Loop over all entries in the list
	for( ; it != m_threadList.end(); ++it )
	{
		// Get the thread object from the list
		CMIUtilThreadActiveObjBase * pThread = *it;
		
		// If the thread is still running
		if( pThread->ThreadIsActive() )
		{
			// Ask this thread to kill itself
			pThread->ThreadKill();
			
			// Wait for this thread to die
			pThread->ThreadJoin();
		}
	}

	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details:	Add a thread object to *this manager's list of thread objects. The list to 
//			used to manage thread objects centrally. 
// Type:	Method.
// Args:	vrObj	- (R) A thread object.
// Return:	MIstatus::success - Functional succeeded.
//			MIstatus::failure - Functional failed.
// Throws:	None.
//--
bool CMICmnThreadMgrStd::AddThread( const CMIUtilThreadActiveObjBase & vrObj )
{
	// Push this thread onto the thread list
	m_threadList.push_back( const_cast< CMIUtilThreadActiveObjBase * >( &vrObj ) );

	return MIstatus::success;
}