
#ifndef __GMTK_PROCESS_H__
#define __GMTK_PROCESS_H__

/*
 * Check if the Process is alive
 * @param	proc_name, string, Process name
 * @return PID if the process is alive and 0 otherwise
 */
int GMTK_IsProcessAlive(const char *proc_name);


/*
 * Send the signal, sig to the process, proc_name 
 * @param	proc_name, string, Process name
 * @param sgi, signal number
 * @return 0 if sucess, -1 the process does not exist, errno otherwise
 */
int GMTK_killall(const char *proc_name, int sig);

/*
 * read /proc/reboot to trigger reboot from kernel 
 * @return >=0 if sucess, -1 the reboot action failed
 */
int GMTK_reboot();

#endif //__GMTK_PROCESS_H__
