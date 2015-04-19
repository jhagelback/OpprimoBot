/*
*/

//project dependencies
#include "cthread.h"
//end project dependencies

//*********************************************************
//default constructor for class CThread
//*********************************************************
CThread::CThread ()
{
	this->hThread		= NULL;
	this->hThreadId		= 0;
	this->hMainThread	= ::GetCurrentThread ();
	this->hMainThreadId = ::GetCurrentThreadId ();
	this->Timeout		= 2000; //milliseconds
}

//*********************************************************
//destructor for class CObject
//*********************************************************
CThread::~CThread ()
{
	//waiting for the thread to terminate
	if (this->hThread)
	{
		if (::WaitForSingleObject (this->hThread, this->Timeout) == WAIT_TIMEOUT)
			::TerminateThread (this->hThread, 1);

		::CloseHandle (this->hThread);
	}
}

//*********************************************************
//working method
//*********************************************************
unsigned long CThread::Process (void* parameter)
{

	//a mechanism for terminating thread should be implemented
	//not allowing the method to be run from the main thread
	if (::GetCurrentThreadId () == this->hMainThreadId)
		return 0;
	else
	{

		return 0;
	}

}

//*********************************************************
//creates the thread
//*********************************************************
bool CThread::CreateThread ()
{

	if (!this->IsCreated ())
	{
		param*	this_param = new param;
		this_param->pThread	= this;
		this->hThread = ::CreateThread (NULL, 0, (unsigned long (__stdcall *)(void *))this->runProcess, (void *)(this_param), 0, &this->hThreadId);
		return this->hThread ? true : false;
	}
	return false;

}

void CThread::StartThread()
{
	while (ResumeThread(hThread) == -1);
}

void CThread::PauseThread()
{
	while (SuspendThread(hThread) == -1);
}

//*********************************************************
//creates the thread
//*********************************************************
int CThread::runProcess (void* Param)
{
	CThread*	thread;
	thread			= (CThread*)((param*)Param)->pThread;
	delete	((param*)Param);
	return thread->Process (0);
}
