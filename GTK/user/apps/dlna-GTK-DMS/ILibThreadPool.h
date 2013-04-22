

#ifndef __ILIBTHREADPOOL__
#define __ILIBTHREADPOOL__

/*! \file ILibThreadPool.h 
	\brief MicroStack APIs for platform independent threadpooling capabilities
*/

/*! \defgroup ILibThreadPool ILibThreadPool Module
	\{
*/

/*! \typedef ILibThreadPool
	\brief Handle to an ILibThreadPool module
*/
typedef void* ILibThreadPool;
/*! \typedef ILibThreadPool_Handler
	\brief Handler for a thread pool work item
	\param sender The ILibThreadPool handle
	\param var State object
*/
typedef void(*ILibThreadPool_Handler)(ILibThreadPool sender, void *var);

ILibThreadPool ILibThreadPool_Create();
void ILibThreadPool_AddThread(ILibThreadPool pool);
void ILibThreadPool_QueueUserWorkItem(ILibThreadPool pool, void *var, ILibThreadPool_Handler callback);
void ILibThreadPool_Destroy(ILibThreadPool pool);
int ILibThreadPool_GetThreadCount(ILibThreadPool pool);


/*! \} */
#endif
