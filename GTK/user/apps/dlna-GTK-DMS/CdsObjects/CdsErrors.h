

#ifndef _CDS_ERRORS_H
#define _CDS_ERRORS_H

/*! \file CdsErrors.h 
	\brief Common CDS error codes.
*/

/*! \defgroup CdsErrors CDS Helper - Errors
	\brief This module provides information about CDS error codes and error strings.
	\{
*/

/*!	\brief UPnP/CDS/SOAP error code for a failed request.
*/
#define CDS_EC_ACTION_FAILED					501

/*!	\brief UPnP/CDS/SOAP error message for a failed request.
*/
#define CDS_EM_ACTION_FAILED					"Action failed. Internal error encountered."

/*!	\brief UPnP/CDS/SOAP error code when a query involves an object ID that does not exist.
*/
#define CDS_EC_OBJECT_ID_NO_EXIST				701

/*!	\brief UPnP/CDS/SOAP error message when a query involves an object ID that does not exist.
*/
#define CDS_EM_OBJECT_ID_NO_EXIST				"ObjectID does not exist."

/*!	\brief UPnP/CDS/SOAP error code when a query involves a CDS item when the request expects a CDS container.
*/

#define CDS_EC_UNSUPPORTED_OR_INVALID_SORT_CRITERIA 709

#define CDS_EM_UNSUPPORTED_OR_INVALID_SORT_CRITERIA "Unsupported or invalid sort critreia error"

#define CDS_EC_NO_SUCH_CONTAINER				710

/*!	\brief UPnP/CDS/SOAP error message when a query involves a CDS item when the request expects a CDS container.
*/
#define CDS_EM_NO_SUCH_CONTAINER				"The specified ObjectID or ContainerID identifies an object that is not a container."

/*!	\brief These are UPnP layer error code, used in responding the UPnP actions.
 */
#define CDS_EC_INVALID_BROWSEFLAG				402

#define CDS_EM_INVALID_BROWSEFLAG				"Invalid value specified for BrowseFlag."

#define CDS_EC_INTERNAL_ERROR					500

#define	CDS_EM_INTERNAL_ERROR					"Unknown or internal error encountered."

#define CMS_EC_CONNECTION_DOES_NOT_EXIST		706

#define CMS_EM_CONNECTION_DOES_NOT_EXIST		"Connection does not exist."

/*!	\brief Enumeration of common CDS error codes.
 */
enum Enum_CdsErrors
{
	/*!	\brief No error encountered.
	*/
	CdsError_None = 0,
	
	/*!	\brief Use when the action failed for an unknown reason.
	*/
	CdsError_ActionFailed = CDS_EC_ACTION_FAILED,
	
	/*!	\brief Use when the CDS query specified an object ID that does not exist.
	*/
	CdsError_NoSuchObject = CDS_EC_OBJECT_ID_NO_EXIST,

	/*!	\brief Use when the CDS query required a CDS container but the specified object was a CDS item.
	*/
	CdsError_NoSuchContainer = CDS_EC_NO_SUCH_CONTAINER
};

/*!	\brief Static array of error message strings, such that the order corresponds
	to the order of error codes in \ref CDS_ErrorCodes.
*/
extern const char *CDS_ErrorStrings[];

/*!	\brief Static array of error message codes, such that the order corresponds
	to the order of error messages in \ref CDS_ErrorStrings.
*/
extern const int CDS_ErrorCodes[];

/*! \} */

#endif
