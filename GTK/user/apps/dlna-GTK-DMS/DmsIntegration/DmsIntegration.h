

#ifndef DMS_INTEGRATION_H
#define DMS_INTEGRATION_H

#include "ILibThreadPool.h"

/*! \file DmsIntegration.h 
	\brief Integration Layer - DMS (Sample)
*/

/*! \defgroup DmsIntegration Integration Layer - DMS (Sample)
	\brief Integrates the necessary components for representing
	a DMS implementation that can serve HTTP content and receive HTTP uploads.
	\{
*/

/*!	\brief Represents a DMS device class with an HTTP server.
*/
typedef void* DMS_StateObj;

/*!	\brief Represents the input parameters needed to initialize the back-end metadata system.

	\note \b TODO: Customize this according to your needs.
*/
struct DMS_BackEndInit
{
	const char* Path;
};


/*	brief Creates a DMS.

	The  sortable_fields and  searchable_fields are bit-strings mapped out by
	CdsSortableSearchableFields. Applications simply perform a
	bitwise OR operation to create a set of fields for each of these arguments.
	
	param[in] thread_chain		A thread-chain created by  ILibCreateChain().
	param[in] thread_pool		A thread-pool object created by  ILibThreadPool_Create().
	param[in] port				The socket port where the Server is created, use 0 to select a random port.
	param[in] friendly_name	The friendly name of the DMS.
	param[in] udn_uuid			The UUID value that is used for the UDN.
	param[in] serial_num		Serial number string that is null-terminated.
	param[in] sortable_fields		The metadata properties that can be specified in a  SortCriteria argument of a CDS:Browse or CDS:Search request.
	param[in] searchable_fields	The metadata properties that can be specified in a  SearchCriteria argument of a CDS:Search request.
	param[in] sink_protocol_info	Comma-separated list of protocolInfo values. Usually "" for a DMS.
	param[in] source_protocol_info	Comma-separated list of protocolInfo values for content that the DMS will serve.
	param[in] back_end_init	Input parameters need to initialize the back end metadata system.
	param[in] dms_user_obj		An application-defined pointer that is associated with the returned   DMS_StateObj.
	returns A  DMS_StateObj that represents the DMS.
*/
DMS_StateObj DMS_Create(void* thread_chain,
						ILibThreadPool thread_pool,
						unsigned short port,
						const char* friendly_name, 
						const char* udn_uuid,
						const char* serial_num, 
						unsigned int sortable_fields, 
						unsigned int searchable_fields,
						const char* sink_protocol_info, 
						const char* source_protocol_info, 
						struct DMS_BackEndInit* back_end_init,
						void* dms_user_obj);

/*   Associates an app-defined pointer with the   DMS_StateObj.
	param[in] dms_obj			The   DMS_StateObj obtained from   DMS_Create().
	param[in] dms_user_obj		An application-defined pointer that is associated with the returned   DMS_StateObj.
*/
void DMS_SetUserObj(DMS_StateObj dms_obj, void* dms_user_obj);

/*!	\brief Gets the app-defined pointer value associated with the   DMS_StateObj.

	\param[in] dms_obj			The   DMS_StateObj obtained from   DMS_Create().
	\returns The pointer value that was set in   DMS_SetUserObj().
*/
void* DMS_GetUserObj(DMS_StateObj dms_obj);

/*!	\brief Locks   DMS_StateObj for thread-safe access.

	\param[in] dms_obj			The   DMS_StateObj obtained from   DMS_Create().
*/
void DMS_Lock(DMS_StateObj dms_obj);

/*!	\brief Unlock   DMS_StateObj after a call to   DMS_Lock().

	\param[in] dms_obj			The   DMS_StateObj obtained from   DMS_Create().
*/
void DMS_UnLock(DMS_StateObj dms_obj);

#if defined (INCLUDE_FEATURE_UPLOAD)
/*!	\brief A hashtable of all upload requests, only POST request created from upload requests will be accepted.
 */
typedef void* DMS_UploadRequest;

/*!	\brief Allows the application to get the current uploaded progress in number of bytes received.
	\param[in] request The upload request created in _DMS_MsaHandler_OnCreateObject.

	\returns The number of bytes uploaded.
*/
int DMS_GetBytesReceived(DMS_UploadRequest request);

/*!	\brief Allows the application to get the total number of bytes needed to be uploaded.
	\param[in] request The upload request created in _DMS_MsaHandler_OnCreateObject.

	\returns The number of total bytes going to be uploaded, which is the file size. If size is unknown
	it will be -1.
*/
int DMS_GetTotalBytesExpected(DMS_UploadRequest request);
#endif

/*!	\brief Call this method if the IP address of the system running the stack has been changed.
	This wil allow the DMS to advertise its presense on the new IP addresses.
*/
void DMS_NotifyIPAddressChange(DMS_StateObj dms_obj);

#endif

