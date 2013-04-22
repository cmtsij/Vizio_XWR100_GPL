/*
 * This wraps a few simple tasks that are different between platforms. 
 * Most of it focuses on the file system calls that are completely 
 * different on POSIX, Windows and Pocket PC.
 * PocketPC is probably the most different since all of the calls have
 * to be made using wide character arrays (16 bit chars). So code was
 * added to convert multibyte (UTF-8) to wide.
 */

#ifndef _FILE_IO_ABSTRACTION_H
#define _FILE_IO_ABSTRACTION_H

#define MAX_PATH_LENGTH 1024

enum ILibFileDir_Type
{
	ILibFileDir_Type_NOT_FOUND_ERROR	= 0,
	ILibFileDir_Type_FILE				= 1,
	ILibFileDir_Type_DIRECTORY			= 2
};


enum ILibFileDir_Type ILibFileDir_GetType(char* path);

/*
 *	Get the working directory.
 */
char* ILibFileDir_GetWorkingDir(char *path, size_t sizeOfBuf);

/*
 *	Closes a directory or specific path.
 *
 *	dirHandle returned from PCGetDirFirstFile.
 */
void ILibFileDir_CloseDir(void* dirHandle);

/*
 *	Opens a directory for enumeration or a specific file.
 *		Returns void* dirHandle.
 *
 *		directory		is the directory portion of the path
 *		filename		allocated byte array, contains the first filename in that directory path upon return
 *		filenamelength	the number of bytes available in filename
 *		fileSize		the size of the file (specified by dirName+filename)
 */
void* ILibFileDir_GetDirFirstFile(const char* directory, /*INOUT*/ char* filename, int filenamelength, /*INOUT*/ int* filesize);

/*
 *	Obtains the next entry in the directory.
 *		dirHandle		the void* returned in PCGetDirFirstFile
 *		dirName			the directory name
 *		filename		allocated byte array, contains the next filename in that directory path upon return
 *		filenamelength	the number of bytes available in filename
 *		fileSize		the size of the file (specified by dirName+filename)
 */
int ILibFileDir_GetDirNextFile(void* dirHandle, const char* dirName, /*INOUT*/char* filename, int filenamelength, int* fileSize);


/*
 *	Returns the number of entries specified in fullPath. 
 *
 *		dirDelimiter	single character to identify the character used to delimit directories within a path.
 *						Posix usually uses '/' whereas windows usually uses '\'
 */
int ILibFileDir_GetDirEntryCount(const char* fullPath, char *dirDelimiter);

int ILibFileDir_DeleteFile(char *FileName);
int ILibFileDir_DeleteDir(char *path);
int ILibFileDir_CreateDir(char *path);
int ILibFileDir_MoveFile(char *OldFileName, char *NewFileName);
long ILibFileDir_GetFileSize(char *FileName);
time_t ILibFileDir_GetFileTimeStamp(char *FileName);

int ILibFileDir_Delete(char* path);

#endif
