// 
// (c)1998-2006 Tzolkin Corporation 
// All Tzolkin Software is copyrighted with all rights reserved.
//
#define ERR_MEMCREATE_FAILED		100		// Internal error allocating memory
#define ERR_FETCHIPADDRESS_FAILED	101		// Could not fetch IP address for domain
#define ERR_OPENCONNECTION_FAILED	102		// Could not connect to server
#define ERR_SENDTOSOCKET_FAILED		103		// Could not send data to server
#define ERR_READFROMSOCKET_FAILED	104		// Could not read from server
#define ERR_BAD_PACKET_RETURNED		105		// Server responed with an invalid packet

#define UPDATE_SUCCESS				200		// The update was successful, and the hostname is now updated
#define HOST_CREATED				201		// New Domain Created for Existing TZO Key
#define UPDATE_NOCHANGE				304		// No Change in the IP Address

#define ERR_BAD_AUTH				401		// Bad Authentication - Username or Password
#define ERR_NOT_AVAIL_CU			402		// An option available only to credited users
#define ERR_BLOCKED_UPDATES			403		// The hostname specified is blocked for update abuse, please wait 1 minute
#define ERR_NO_HOST_EXIST			404		// The hostname specified does not exist
#define ERR_BLOCKED_AGENT			405		// The user agent that was sent has been blocked for not following specifications
#define ERR_BAD_HOST_NAME			406		// The hostname specified is not a fully-qualified domain name
#define ERR_HOST_MISMATCH			409		// The hostname specified exists, but not under the username specified
#define ERR_SYSTEM_TYPE				412		// Bad System type
#define ERR_HOST_COUNT				413		// Too many or too few hosts found
#define ERR_ACCOUNT_TEMP_BLOCK      414     // Same IP address update in less than a minute
#define ERR_ACCOUNT_BLOCKED			415		// Blocked from updating
#define ERR_ACCOUNT_EXPIRED			480		// The TZO account has expired

#define ERR_SERVER_DATABASE			500		// TZO Server Database 
#define ERR_DNS_ERROR				506		// DNS Error
