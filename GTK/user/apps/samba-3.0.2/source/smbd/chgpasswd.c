/* 
   Unix SMB/CIFS implementation.
   Samba utility functions
   Copyright (C) Andrew Tridgell 1992-1998
   Copyright (C) Andrew Bartlett 2001-2002
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* fork a child process to exec passwd and write to its
 * tty to change a users password. This is running as the
 * user who is attempting to change the password.
 */

/* 
 * This code was copied/borrowed and stolen from various sources.
 * The primary source was the poppasswd.c from the authors of POPMail. This software
 * was included as a client to change passwords using the 'passwd' program
 * on the remote machine.
 *
 * This routine is called by set_user_password() in password.c only if ALLOW_PASSWORD_CHANGE
 * is defined in the compiler directives located in the Makefile.
 *
 * This code has been hacked by Bob Nance (nance@niehs.nih.gov) and Evan Patterson
 * (patters2@niehs.nih.gov) at the National Institute of Environmental Health Sciences
 * and rights to modify, distribute or incorporate this change to the CAP suite or
 * using it for any other reason are granted, so long as this disclaimer is left intact.
 */

/*
   This code was hacked considerably for inclusion in Samba, primarily
   by Andrew.Tridgell@anu.edu.au. The biggest change was the addition
   of the "password chat" option, which allows the easy runtime
   specification of the expected sequence of events to change a
   password.
   */

#include "includes.h"

extern struct passdb_ops pdb_ops;

static NTSTATUS check_oem_password(const char *user,
			       uchar * lmdata, const uchar * lmhash,
			       const uchar * ntdata, const uchar * nthash,
			       SAM_ACCOUNT **hnd, char *new_passwd,
			       int new_passwd_size);

#if ALLOW_CHANGE_PASSWORD

static int findpty(char **slave)
{
	int master;
	static fstring line;
	DIR *dirp;
	const char *dpname;

#if defined(HAVE_GRANTPT)
	/* Try to open /dev/ptmx. If that fails, fall through to old method. */
	if ((master = sys_open("/dev/ptmx", O_RDWR, 0)) >= 0)
	{
		grantpt(master);
		unlockpt(master);
		*slave = (char *)ptsname(master);
		if (*slave == NULL)
		{
			DEBUG(0,
			      ("findpty: Unable to create master/slave pty pair.\n"));
			/* Stop fd leak on error. */
			close(master);
			return -1;
		}
		else
		{
			DEBUG(10,
			      ("findpty: Allocated slave pty %s\n", *slave));
			return (master);
		}
	}
#endif /* HAVE_GRANTPT */

	fstrcpy(line, "/dev/ptyXX");

	dirp = opendir("/dev");
	if (!dirp)
		return (-1);
	while ((dpname = readdirname(dirp)) != NULL)
	{
		if (strncmp(dpname, "pty", 3) == 0 && strlen(dpname) == 5)
		{
			DEBUG(3,
			      ("pty: try to open %s, line was %s\n", dpname,
			       line));
			line[8] = dpname[3];
			line[9] = dpname[4];
			if ((master = sys_open(line, O_RDWR, 0)) >= 0)
			{
				DEBUG(3, ("pty: opened %s\n", line));
				line[5] = 't';
				*slave = line;
				closedir(dirp);
				return (master);
			}
		}
	}
	closedir(dirp);
	return (-1);
}

static int dochild(int master, const char *slavedev, const struct passwd *pass,
		   const char *passwordprogram, BOOL as_root)
{
	int slave;
	struct termios stermios;
	gid_t gid;
	uid_t uid;

	if (pass == NULL)
	{
		DEBUG(0,
		      ("dochild: user doesn't exist in the UNIX password database.\n"));
		return False;
	}

	gid = pass->pw_gid;
	uid = pass->pw_uid;

	gain_root_privilege();

	/* Start new session - gets rid of controlling terminal. */
	if (setsid() < 0)
	{
		DEBUG(3,
		      ("Weirdness, couldn't let go of controlling terminal\n"));
		return (False);
	}

	/* Open slave pty and acquire as new controlling terminal. */
	if ((slave = sys_open(slavedev, O_RDWR, 0)) < 0)
	{
		DEBUG(3, ("More weirdness, could not open %s\n", slavedev));
		return (False);
	}
#ifdef I_PUSH
	ioctl(slave, I_PUSH, "ptem");
	ioctl(slave, I_PUSH, "ldterm");
#elif defined(TIOCSCTTY)
	if (ioctl(slave, TIOCSCTTY, 0) < 0)
	{
		DEBUG(3, ("Error in ioctl call for slave pty\n"));
		/* return(False); */
	}
#endif

	/* Close master. */
	close(master);

	/* Make slave stdin/out/err of child. */

	if (sys_dup2(slave, STDIN_FILENO) != STDIN_FILENO)
	{
		DEBUG(3, ("Could not re-direct stdin\n"));
		return (False);
	}
	if (sys_dup2(slave, STDOUT_FILENO) != STDOUT_FILENO)
	{
		DEBUG(3, ("Could not re-direct stdout\n"));
		return (False);
	}
	if (sys_dup2(slave, STDERR_FILENO) != STDERR_FILENO)
	{
		DEBUG(3, ("Could not re-direct stderr\n"));
		return (False);
	}
	if (slave > 2)
		close(slave);

	/* Set proper terminal attributes - no echo, canonical input processing,
	   no map NL to CR/NL on output. */

	if (tcgetattr(0, &stermios) < 0)
	{
		DEBUG(3,
		      ("could not read default terminal attributes on pty\n"));
		return (False);
	}
	stermios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	stermios.c_lflag |= ICANON;
#ifdef ONLCR
 	stermios.c_oflag &= ~(ONLCR);
#endif
	if (tcsetattr(0, TCSANOW, &stermios) < 0)
	{
		DEBUG(3, ("could not set attributes of pty\n"));
		return (False);
	}

	/* make us completely into the right uid */
	if (!as_root)
	{
		become_user_permanently(uid, gid);
	}

	DEBUG(10,
	      ("Invoking '%s' as password change program.\n",
	       passwordprogram));

	/* execl() password-change application */
	if (execl("/bin/sh", "sh", "-c", passwordprogram, NULL) < 0)
	{
		DEBUG(3, ("Bad status returned from %s\n", passwordprogram));
		return (False);
	}
	return (True);
}

static int expect(int master, char *issue, char *expected)
{
	pstring buffer;
	int attempts, timeout, nread, len;
	BOOL match = False;

	for (attempts = 0; attempts < 2; attempts++) {
		if (!strequal(issue, ".")) {
			if (lp_passwd_chat_debug())
				DEBUG(100, ("expect: sending [%s]\n", issue));

			if ((len = write(master, issue, strlen(issue))) != strlen(issue)) {
				DEBUG(2,("expect: (short) write returned %d\n", len ));
				return False;
			}
		}

		if (strequal(expected, "."))
			return True;

		/* Initial timeout. */
		timeout = lp_passwd_chat_timeout() * 1000;
		nread = 0;
		buffer[nread] = 0;

		while ((len = read_socket_with_timeout(master, buffer + nread, 1,
						       sizeof(buffer) - nread - 1,
						       timeout)) > 0) {
			nread += len;
			buffer[nread] = 0;

			{
				/* Eat leading/trailing whitespace before match. */
				pstring str;
				pstrcpy( str, buffer);
				trim_char( str, ' ', ' ');

				if ((match = (unix_wild_match(expected, str) == 0))) {
					/* Now data has started to return, lower timeout. */
					timeout = lp_passwd_chat_timeout() * 100;
				}
			}
		}

		if (lp_passwd_chat_debug())
			DEBUG(100, ("expect: expected [%s] received [%s] match %s\n",
				    expected, buffer, match ? "yes" : "no" ));

		if (match)
			break;

		if (len < 0) {
			DEBUG(2, ("expect: %s\n", strerror(errno)));
			return False;
		}
	}

	DEBUG(10,("expect: returning %s\n", match ? "True" : "False" ));
	return match;
}

static void pwd_sub(char *buf)
{
	all_string_sub(buf, "\\n", "\n", 0);
	all_string_sub(buf, "\\r", "\r", 0);
	all_string_sub(buf, "\\s", " ", 0);
	all_string_sub(buf, "\\t", "\t", 0);
}

static int talktochild(int master, const char *seq)
{
	int count = 0;
	fstring issue, expected;

	fstrcpy(issue, ".");

	while (next_token(&seq, expected, NULL, sizeof(expected)))
	{
		pwd_sub(expected);
		count++;

		if (!expect(master, issue, expected))
		{
			DEBUG(3, ("Response %d incorrect\n", count));
			return False;
		}

		if (!next_token(&seq, issue, NULL, sizeof(issue)))
			fstrcpy(issue, ".");

		pwd_sub(issue);
	}
	if (!strequal(issue, ".")) {
		/* we have one final issue to send */
		fstrcpy(expected, ".");
		if (!expect(master, issue, expected))
			return False;
	}

	return (count > 0);
}

static BOOL chat_with_program(char *passwordprogram, struct passwd *pass,
			      char *chatsequence, BOOL as_root)
{
	char *slavedev;
	int master;
	pid_t pid, wpid;
	int wstat;
	BOOL chstat = False;

	if (pass == NULL) {
		DEBUG(0, ("chat_with_program: user doesn't exist in the UNIX password database.\n"));
		return False;
	}

	/* allocate a pseudo-terminal device */
	if ((master = findpty(&slavedev)) < 0) {
		DEBUG(3, ("chat_with_program: Cannot Allocate pty for password change: %s\n", pass->pw_name));
		return (False);
	}

	/*
	 * We need to temporarily stop CatchChild from eating
	 * SIGCLD signals as it also eats the exit status code. JRA.
	 */

	CatchChildLeaveStatus();

	if ((pid = sys_fork()) < 0) {
		DEBUG(3, ("chat_with_program: Cannot fork() child for password change: %s\n", pass->pw_name));
		close(master);
		CatchChild();
		return (False);
	}

	/* we now have a pty */
	if (pid > 0) {			/* This is the parent process */
		if ((chstat = talktochild(master, chatsequence)) == False) {
			DEBUG(3, ("chat_with_program: Child failed to change password: %s\n", pass->pw_name));
			kill(pid, SIGKILL);	/* be sure to end this process */
		}

		while ((wpid = sys_waitpid(pid, &wstat, 0)) < 0) {
			if (errno == EINTR) {
				errno = 0;
				continue;
			}
			break;
		}

		if (wpid < 0) {
			DEBUG(3, ("chat_with_program: The process is no longer waiting!\n\n"));
			close(master);
			CatchChild();
			return (False);
		}

		/*
		 * Go back to ignoring children.
		 */
		CatchChild();

		close(master);

		if (pid != wpid) {
			DEBUG(3, ("chat_with_program: We were waiting for the wrong process ID\n"));
			return (False);
		}
		if (WIFEXITED(wstat) && (WEXITSTATUS(wstat) != 0)) {
			DEBUG(3, ("chat_with_program: The process exited with status %d \
while we were waiting\n", WEXITSTATUS(wstat)));
			return (False);
		}
#if defined(WIFSIGNALLED) && defined(WTERMSIG)
		else if (WIFSIGNALLED(wstat)) {
                        DEBUG(3, ("chat_with_program: The process was killed by signal %d \
while we were waiting\n", WTERMSIG(wstat)));
			return (False);
		}
#endif
	} else {
		/* CHILD */

		/*
		 * Lose any oplock capabilities.
		 */
		oplock_set_capability(False, False);

		/* make sure it doesn't freeze */
		alarm(20);

		if (as_root)
			become_root();

		DEBUG(3, ("chat_with_program: Dochild for user %s (uid=%d,gid=%d) (as_root = %s)\n", pass->pw_name,
		       (int)getuid(), (int)getgid(), BOOLSTR(as_root) ));
		chstat = dochild(master, slavedev, pass, passwordprogram, as_root);

		if (as_root)
			unbecome_root();

		/*
		 * The child should never return from dochild() ....
		 */

		DEBUG(0, ("chat_with_program: Error: dochild() returned %d\n", chstat));
		exit(1);
	}

	if (chstat)
		DEBUG(3, ("chat_with_program: Password change %ssuccessful for user %s\n",
		       (chstat ? "" : "un"), pass->pw_name));
	return (chstat);
}

BOOL chgpasswd(const char *name, const char *oldpass, const char *newpass, BOOL as_root)
{
	pstring passwordprogram;
	pstring chatsequence;
	size_t i;
	size_t len;

	struct passwd *pass;

	if (!name) {
		DEBUG(1, ("chgpasswd: NULL username specfied !\n"));
	}
	
	pass = Get_Pwnam(name);
	if (!pass) {
		DEBUG(1, ("chgpasswd: Username does not exist in system !\n"));
		return False;
	}

	if (!oldpass) {
		oldpass = "";
	}

	DEBUG(3, ("chgpasswd: Password change (as_root=%s) for user: %s\n", BOOLSTR(as_root), name));

#if DEBUG_PASSWORD
	DEBUG(100, ("chgpasswd: Passwords: old=%s new=%s\n", oldpass, newpass));
#endif

	/* Take the passed information and test it for minimum criteria */
	/* Minimum password length */
	if (strlen(newpass) < lp_min_passwd_length()) {
		/* too short, must be at least MINPASSWDLENGTH */
		DEBUG(0, ("chgpasswd: Password Change: user %s, New password is shorter than minimum password length = %d\n",
		       name, lp_min_passwd_length()));
		return (False);	/* inform the user */
	}

	/* Password is same as old password */
	if (strcmp(oldpass, newpass) == 0) {
		/* don't allow same password */
		DEBUG(2, ("chgpasswd: Password Change: %s, New password is same as old\n", name));	/* log the attempt */
		return (False);	/* inform the user */
	}

	/* 
	 * Check the old and new passwords don't contain any control
	 * characters.
	 */

	len = strlen(oldpass);
	for (i = 0; i < len; i++) {
		if (iscntrl((int)oldpass[i])) {
			DEBUG(0, ("chgpasswd: oldpass contains control characters (disallowed).\n"));
			return False;
		}
	}

	len = strlen(newpass);
	for (i = 0; i < len; i++) {
		if (iscntrl((int)newpass[i])) {
			DEBUG(0, ("chgpasswd: newpass contains control characters (disallowed).\n"));
			return False;
		}
	}
	
#ifdef WITH_PAM
	if (lp_pam_password_change()) {
		BOOL ret;

		if (as_root)
			become_root();

		if (pass) {
			ret = smb_pam_passchange(pass->pw_name, oldpass, newpass);
		} else {
			ret = smb_pam_passchange(name, oldpass, newpass);
		}
			
		if (as_root)
			unbecome_root();

		return ret;
	}
#endif

	/* A non-PAM password change just doen't make sense without a valid local user */

	if (pass == NULL) {
		DEBUG(0, ("chgpasswd: user %s doesn't exist in the UNIX password database.\n", name));
		return False;
	}

	pstrcpy(passwordprogram, lp_passwd_program());
	pstrcpy(chatsequence, lp_passwd_chat());

	if (!*chatsequence) {
		DEBUG(2, ("chgpasswd: Null chat sequence - no password changing\n"));
		return (False);
	}

	if (!*passwordprogram) {
		DEBUG(2, ("chgpasswd: Null password program - no password changing\n"));
		return (False);
	}

	if (as_root) {
		/* The password program *must* contain the user name to work. Fail if not. */
		if (strstr(passwordprogram, "%u") == NULL) {
			DEBUG(0,("chgpasswd: Running as root the 'passwd program' parameter *MUST* contain \
the string %%u, and the given string %s does not.\n", passwordprogram ));
			return False;
		}
	}

	pstring_sub(passwordprogram, "%u", name);
	/* note that we do NOT substitute the %o and %n in the password program
	   as this would open up a security hole where the user could use
	   a new password containing shell escape characters */

	pstring_sub(chatsequence, "%u", name);
	all_string_sub(chatsequence, "%o", oldpass, sizeof(pstring));
	all_string_sub(chatsequence, "%n", newpass, sizeof(pstring));
	return (chat_with_program
		(passwordprogram, pass, chatsequence, as_root));
}

#else /* ALLOW_CHANGE_PASSWORD */

BOOL chgpasswd(const char *name, const char *oldpass, const char *newpass, BOOL as_root)
{
	DEBUG(0, ("chgpasswd: Password changing not compiled in (user=%s)\n", name));
	return (False);
}
#endif /* ALLOW_CHANGE_PASSWORD */

/***********************************************************
 Code to check the lanman hashed password.
************************************************************/

BOOL check_lanman_password(char *user, uchar * pass1,
			   uchar * pass2, SAM_ACCOUNT **hnd)
{
	uchar unenc_new_pw[16];
	uchar unenc_old_pw[16];
	SAM_ACCOUNT *sampass = NULL;
	uint16 acct_ctrl;
	const uint8 *lanman_pw;
	BOOL ret;
	
	become_root();
	ret = pdb_getsampwnam(sampass, user);
	unbecome_root();

	if (ret == False) {
		DEBUG(0,("check_lanman_password: getsampwnam returned NULL\n"));
		pdb_free_sam(&sampass);
		return False;
	}
	
	acct_ctrl = pdb_get_acct_ctrl     (sampass);
	lanman_pw = pdb_get_lanman_passwd (sampass);

	if (acct_ctrl & ACB_DISABLED) {
		DEBUG(0,("check_lanman_password: account %s disabled.\n", user));
		pdb_free_sam(&sampass);
		return False;
	}

	if (lanman_pw == NULL) {
		if (acct_ctrl & ACB_PWNOTREQ) {
			/* this saves the pointer for the caller */
			*hnd = sampass;
			return True;
		} else {
			DEBUG(0, ("check_lanman_password: no lanman password !\n"));
			pdb_free_sam(&sampass);
			return False;
		}
	}

	/* Get the new lanman hash. */
	D_P16(lanman_pw, pass2, unenc_new_pw);

	/* Use this to get the old lanman hash. */
	D_P16(unenc_new_pw, pass1, unenc_old_pw);

	/* Check that the two old passwords match. */
	if (memcmp(lanman_pw, unenc_old_pw, 16)) {
		DEBUG(0,("check_lanman_password: old password doesn't match.\n"));
		pdb_free_sam(&sampass);
		return False;
	}

	/* this saves the pointer for the caller */
	*hnd = sampass;
	return True;
}

/***********************************************************
 Code to change the lanman hashed password.
 It nulls out the NT hashed password as it will
 no longer be valid.
 NOTE this function is designed to be called as root. Check the old password
 is correct before calling. JRA.
************************************************************/

BOOL change_lanman_password(SAM_ACCOUNT *sampass, uchar *pass2)
{
	static uchar null_pw[16];
	uchar unenc_new_pw[16];
	BOOL ret;
	uint16 acct_ctrl;
	const uint8 *pwd;

	if (sampass == NULL) {
		DEBUG(0,("change_lanman_password: no smb password entry.\n"));
		return False;
	}
	
	acct_ctrl = pdb_get_acct_ctrl(sampass);
	pwd = pdb_get_lanman_passwd(sampass);

	if (acct_ctrl & ACB_DISABLED) {
		DEBUG(0,("change_lanman_password: account %s disabled.\n",
		       pdb_get_username(sampass)));
		return False;
	}

	if (pwd == NULL) { 
		if (acct_ctrl & ACB_PWNOTREQ) {
			uchar no_pw[14];
			memset(no_pw, '\0', 14);
			E_P16(no_pw, null_pw);

			/* Get the new lanman hash. */
			D_P16(null_pw, pass2, unenc_new_pw);
		} else {
			DEBUG(0,("change_lanman_password: no lanman password !\n"));
			return False;
		}
	} else {
		/* Get the new lanman hash. */
		D_P16(pwd, pass2, unenc_new_pw);
	}

	if (!pdb_set_lanman_passwd(sampass, unenc_new_pw, PDB_CHANGED)) {
		return False;
	}

	if (!pdb_set_nt_passwd    (sampass, NULL, PDB_CHANGED)) {
		return False;	/* We lose the NT hash. Sorry. */
	}

	if (!pdb_set_pass_changed_now  (sampass)) {
		pdb_free_sam(&sampass);
		/* Not quite sure what this one qualifies as, but this will do */
		return False; 
	}
 
	/* Now flush the sam_passwd struct to persistent storage */
	ret = pdb_update_sam_account (sampass);

	return ret;
}

/***********************************************************
 Code to check and change the OEM hashed password.
************************************************************/

NTSTATUS pass_oem_change(char *user,
			 uchar * lmdata, uchar * lmhash,
			 uchar * ntdata, uchar * nthash)
{
	fstring new_passwd;
	SAM_ACCOUNT *sampass = NULL;
	NTSTATUS nt_status = check_oem_password(user, lmdata, lmhash, ntdata, nthash,
				     &sampass, new_passwd, sizeof(new_passwd));

	if (!NT_STATUS_IS_OK(nt_status))
		return nt_status;

	/* We've already checked the old password here.... */
	become_root();
	nt_status = change_oem_password(sampass, NULL, new_passwd, True);
	unbecome_root();

	memset(new_passwd, 0, sizeof(new_passwd));

	pdb_free_sam(&sampass);

	return nt_status;
}

/***********************************************************
 Code to check the OEM hashed password.

 this function ignores the 516 byte nt OEM hashed password
 but does use the lm OEM password to check the nt hashed-hash.

************************************************************/

static NTSTATUS check_oem_password(const char *user,
			       uchar * lmdata, const uchar * lmhash,
			       const uchar * ntdata, const uchar * nthash,
			       SAM_ACCOUNT **hnd, char *new_passwd,
			       int new_passwd_size)
{
	static uchar null_pw[16];
	static uchar null_ntpw[16];
	SAM_ACCOUNT *sampass = NULL;
	const uint8 *lanman_pw, *nt_pw;
	uint16 acct_ctrl;
	int new_pw_len;
	uchar new_ntp16[16];
	uchar unenc_old_ntpw[16];
	uchar new_p16[16];
	uchar unenc_old_pw[16];
	char no_pw[2];
	BOOL ret;

	BOOL nt_pass_set = (ntdata != NULL && nthash != NULL);

	*hnd = NULL;

	pdb_init_sam(&sampass);

	become_root();
	ret = pdb_getsampwnam(sampass, user);
	unbecome_root();

	if (ret == False) {
		DEBUG(0, ("check_oem_password: getsmbpwnam returned NULL\n"));
		pdb_free_sam(&sampass);
		return NT_STATUS_WRONG_PASSWORD;
		/*
		  TODO: check what Win2k returns for this:
		  return NT_STATUS_NO_SUCH_USER; 
		*/
	}

	acct_ctrl = pdb_get_acct_ctrl(sampass);
	
	if (acct_ctrl & ACB_DISABLED) {
		DEBUG(0,("check_lanman_password: account %s disabled.\n", user));
		pdb_free_sam(&sampass);
		return NT_STATUS_ACCOUNT_DISABLED;
	}

	/* construct a null password (in case one is needed */
	no_pw[0] = 0;
	no_pw[1] = 0;
	nt_lm_owf_gen(no_pw, null_ntpw, null_pw);

	/* save pointers to passwords so we don't have to keep looking them up */
	lanman_pw = pdb_get_lanman_passwd(sampass);
	nt_pw = pdb_get_nt_passwd(sampass);

	/* check for null passwords */
	if (lanman_pw == NULL) {
		if (!(acct_ctrl & ACB_PWNOTREQ)) {
			DEBUG(0,("check_oem_password: no lanman password !\n"));
			pdb_free_sam(&sampass);
			return NT_STATUS_WRONG_PASSWORD;
		}
	}
	
	if (pdb_get_nt_passwd(sampass) == NULL && nt_pass_set) {
		if (!(acct_ctrl & ACB_PWNOTREQ)) {
			DEBUG(0,("check_oem_password: no ntlm password !\n"));
			pdb_free_sam(&sampass);
			return NT_STATUS_WRONG_PASSWORD;
		}
	}
	
	/* 
	 * Call the hash function to get the new password.
	 */
	SamOEMhash( lmdata, lanman_pw, 516);

	/* 
	 * The length of the new password is in the last 4 bytes of
	 * the data buffer.
	 */

	new_pw_len = IVAL(lmdata, 512);

	if (new_pw_len < 0 || new_pw_len > new_passwd_size - 1) {
		DEBUG(0,("check_oem_password: incorrect password length (%d).\n", new_pw_len));
		pdb_free_sam(&sampass);
		return NT_STATUS_WRONG_PASSWORD;
	}

	if (nt_pass_set) {
		/*
		 * nt passwords are in unicode
		 */
		pull_ucs2(NULL, new_passwd, 
			  (const smb_ucs2_t *)&lmdata[512 - new_pw_len],
			  new_passwd_size, new_pw_len, 0);
	} else {
		memcpy(new_passwd, &lmdata[512 - new_pw_len], new_pw_len);
		new_passwd[new_pw_len] = 0;
	}

	/*
	 * To ensure we got the correct new password, hash it and
	 * use it as a key to test the passed old password.
	 */

	nt_lm_owf_gen(new_passwd, new_ntp16, new_p16);

	if (!nt_pass_set) {
		/*
		 * Now use new_p16 as the key to see if the old
		 * password matches.
		 */
		D_P16(new_p16, lmhash, unenc_old_pw);

		if (memcmp(lanman_pw, unenc_old_pw, 16)) {
			DEBUG(0,("check_oem_password: old lm password doesn't match.\n"));
			pdb_free_sam(&sampass);
			return NT_STATUS_WRONG_PASSWORD;
		}

#ifdef DEBUG_PASSWORD
		DEBUG(100,
		      ("check_oem_password: password %s ok\n", new_passwd));
#endif
		*hnd = sampass;
		return NT_STATUS_OK;
	}

	/*
	 * Now use new_p16 as the key to see if the old
	 * password matches.
	 */
	D_P16(new_ntp16, lmhash, unenc_old_pw);
	D_P16(new_ntp16, nthash, unenc_old_ntpw);

	if (memcmp(lanman_pw, unenc_old_pw, 16)) {
		DEBUG(0,("check_oem_password: old lm password doesn't match.\n"));
		pdb_free_sam(&sampass);
		return NT_STATUS_WRONG_PASSWORD;
	}

	if (memcmp(nt_pw, unenc_old_ntpw, 16)) {
		DEBUG(0,("check_oem_password: old nt password doesn't match.\n"));
		pdb_free_sam(&sampass);
		return NT_STATUS_WRONG_PASSWORD;
	}
#ifdef DEBUG_PASSWORD
	DEBUG(100, ("check_oem_password: password %s ok\n", new_passwd));
#endif

	*hnd = sampass;
	return NT_STATUS_OK;
}

/***********************************************************
 Code to change the oem password. Changes both the lanman
 and NT hashes.  Old_passwd is almost always NULL.
 NOTE this function is designed to be called as root. Check the old password
 is correct before calling. JRA.
************************************************************/

NTSTATUS change_oem_password(SAM_ACCOUNT *hnd, char *old_passwd, char *new_passwd, BOOL as_root)
{
	BOOL ret;
	uint32 min_len;

	if (time(NULL) < pdb_get_pass_can_change_time(hnd)) {
		DEBUG(1, ("user %s cannot change password now, must wait until %s\n", 
			  pdb_get_username(hnd), http_timestring(pdb_get_pass_can_change_time(hnd))));
		return NT_STATUS_PASSWORD_RESTRICTION;
	}

	if (account_policy_get(AP_MIN_PASSWORD_LEN, &min_len) && (strlen(new_passwd) < min_len)) {
		DEBUG(1, ("user %s cannot change password - password too short\n", 
			  pdb_get_username(hnd)));
		DEBUGADD(1, (" account policy min password len = %d\n", min_len));
		return NT_STATUS_PASSWORD_RESTRICTION;
/* 		return NT_STATUS_PWD_TOO_SHORT; */
	}

	/* Take the passed information and test it for minimum criteria */
	/* Minimum password length */
	if (strlen(new_passwd) < lp_min_passwd_length()) {
		/* too short, must be at least MINPASSWDLENGTH */
		DEBUG(1, ("Password Change: user %s, New password is shorter than minimum password length = %d\n",
		       pdb_get_username(hnd), lp_min_passwd_length()));
		return NT_STATUS_PASSWORD_RESTRICTION;
/* 		return NT_STATUS_PWD_TOO_SHORT; */
	}

	/* TODO:  Add cracklib support here */

	/*
	 * If unix password sync was requested, attempt to change
	 * the /etc/passwd database first. Return failure if this cannot
	 * be done.
	 *
	 * This occurs before the oem change, because we don't want to
	 * update it if chgpasswd failed.
	 *
	 * Conditional on lp_unix_password_sync() because we don't want
	 * to touch the unix db unless we have admin permission.
	 */
	
	if(lp_unix_password_sync() &&
		!chgpasswd(pdb_get_username(hnd), old_passwd, new_passwd, as_root)) {
		return NT_STATUS_ACCESS_DENIED;
	}

	if (!pdb_set_plaintext_passwd (hnd, new_passwd)) {
		return NT_STATUS_ACCESS_DENIED;
	}

	/* Now write it into the file. */
	ret = pdb_update_sam_account (hnd);

	if (!ret) {
		return NT_STATUS_ACCESS_DENIED;
	}
	
	return NT_STATUS_OK;
}
