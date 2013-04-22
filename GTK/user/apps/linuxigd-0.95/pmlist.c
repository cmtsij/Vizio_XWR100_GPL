#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
// +++ Gemtek
//#include <upnp/upnp.h>
#include <upnp.h>
// --- Gemtek
#include "globals.h"
#include "config.h"
#include "pmlist.h"
#include "gatedevice.h"
#include "util.h"

#if HAVE_LIBIPTC
#include "iptc.h"
#endif

struct portMap* pmlist_NewNode(int enabled, long int duration, char *remoteHost,
			       char *externalPort, char *internalPort,
			       char *protocol, char *internalClient, char *desc)
{
	struct portMap* temp = (struct portMap*) malloc(sizeof(struct portMap));

	temp->m_PortMappingEnabled = enabled;
	
	if (remoteHost && strlen(remoteHost) < sizeof(temp->m_RemoteHost)) strcpy(temp->m_RemoteHost, remoteHost);
		else strcpy(temp->m_RemoteHost, "");
	if (strlen(externalPort) < sizeof(temp->m_ExternalPort)) strcpy(temp->m_ExternalPort, externalPort);
		else strcpy(temp->m_ExternalPort, "");
	if (strlen(internalPort) < sizeof(temp->m_InternalPort)) strcpy(temp->m_InternalPort, internalPort);
		else strcpy(temp->m_InternalPort, "");
	if (strlen(protocol) < sizeof(temp->m_PortMappingProtocol)) strcpy(temp->m_PortMappingProtocol, protocol);
		else strcpy(temp->m_PortMappingProtocol, "");
	if (strlen(internalClient) < sizeof(temp->m_InternalClient)) strcpy(temp->m_InternalClient, internalClient);
		else strcpy(temp->m_InternalClient, "");
	if (strlen(desc) < sizeof(temp->m_PortMappingDescription)) strcpy(temp->m_PortMappingDescription, desc);
		else strcpy(temp->m_PortMappingDescription, "");
	temp->m_PortMappingLeaseDuration = duration;

	temp->next = NULL;
	temp->prev = NULL;
	
	return temp;
}
	
struct portMap* pmlist_Find(char *externalPort, char *proto, char *internalClient)
{
	struct portMap* temp;
	
	temp = pmlist_Head;
	if (temp == NULL)
		return NULL;
	
	do 
	{
		if ( (strcmp(temp->m_ExternalPort, externalPort) == 0) &&
				(strcmp(temp->m_PortMappingProtocol, proto) == 0) &&
				(strcmp(temp->m_InternalClient, internalClient) == 0) )
			return temp; // We found a match, return pointer to it
		else
			temp = temp->next;
	} while (temp != NULL);
	
	// If we made it here, we didn't find it, so return NULL
	return NULL;
}

// +++ Gemtek
struct portMap* pmlist_Find_SameExtPort(char *externalPort, char *proto, char *internalClient)
{
	struct portMap* temp;
	
	temp = pmlist_Head;
	if (temp == NULL)
		return NULL;
	
	do 
	{
		if ( (strcmp(temp->m_ExternalPort, externalPort) == 0) &&
				(strcmp(temp->m_PortMappingProtocol, proto) == 0)  &&
				(strcmp(temp->m_InternalClient, internalClient) != 0))
			return temp; // We found a match, return pointer to it
		else
			temp = temp->next;
	} while (temp != NULL);
	
	// If we made it here, we didn't find it, so return NULL
	return NULL;
}
// --- Gemtek

struct portMap* pmlist_FindByIndex(int index)
{
	int i=0;
	struct portMap* temp;

	temp = pmlist_Head;
	if (temp == NULL)
		return NULL;
	do
	{
		if (i == index)
			return temp;
		else
		{
			temp = temp->next;	
			i++;
		}
	} while (temp != NULL);

	return NULL;
}	

struct portMap* pmlist_FindSpecific(char *externalPort, char *protocol)
{
	struct portMap* temp;
	
	temp = pmlist_Head;
	if (temp == NULL)
		return NULL;
	
	do
	{
		if ( (strcmp(temp->m_ExternalPort, externalPort) == 0) &&
				(strcmp(temp->m_PortMappingProtocol, protocol) == 0))
			return temp;
		else
			temp = temp->next;
	} while (temp != NULL);

	return NULL;
}

int pmlist_IsEmtpy(void)
{
	if (pmlist_Head)
		return 0;
	else
		return 1;
}

int pmlist_Size(void)
{
	struct portMap* temp;
	int size = 0;
	
	temp = pmlist_Head;
	if (temp == NULL)
		return 0;
	
	while (temp->next)
	{
		size++;
		temp = temp->next;
	}
	size++;
	return size;
}	

int pmlist_FreeList(void)
{
  struct portMap *temp, *next;

  temp = pmlist_Head;
  while(temp) {
    CancelMappingExpiration(temp->expirationEventId);
// +++ Gemtek, CDRouter-190, 192
//    pmlist_DeletePortMapping(temp->m_PortMappingEnabled, temp->m_PortMappingProtocol, temp->m_ExternalPort,
//			     temp->m_InternalClient, temp->m_InternalPort);
    pmlist_DeletePortMapping(temp->m_PortMappingEnabled,temp->m_RemoteHost, temp->m_PortMappingProtocol, temp->m_ExternalPort,
			     temp->m_InternalClient, temp->m_InternalPort);
// --- Gemtek, CDRouter-190, 192
    next = temp->next;
    free(temp);
    temp = next;
  }
  pmlist_Head = pmlist_Tail = NULL;
  return 1;
}
		
int pmlist_PushBack(struct portMap* item)
{
	int action_succeeded = 0;

	if (pmlist_Tail) // We have a list, place on the end
	{
		pmlist_Tail->next = item;
		item->prev = pmlist_Tail;
		item->next = NULL;
		pmlist_Tail = item;
		action_succeeded = 1;
	}
	else // We obviously have no list, because we have no tail :D
	{
		pmlist_Head = pmlist_Tail = pmlist_Current = item;
		item->prev = NULL;
		item->next = NULL;
 		action_succeeded = 1;
		trace(3, "appended %d %s %s %s %s %ld", item->m_PortMappingEnabled, 
				    item->m_PortMappingProtocol, item->m_ExternalPort, item->m_InternalClient, item->m_InternalPort,
				    item->m_PortMappingLeaseDuration);
	}
	if (action_succeeded == 1)
	{
// +++ Gemtek, CDRouter-190, 192
//		pmlist_AddPortMapping(item->m_PortMappingEnabled, item->m_PortMappingProtocol,
//				      item->m_ExternalPort, item->m_InternalClient, item->m_InternalPort);
		pmlist_AddPortMapping(item->m_PortMappingEnabled,item->m_RemoteHost,item->m_PortMappingProtocol,
				      item->m_ExternalPort, item->m_InternalClient, item->m_InternalPort);
// --- Gemtek, CDRouter-190, 192
		return 1;
	}
	else
		return 0;
}

		
int pmlist_Delete(struct portMap* item)
{
	struct portMap *temp;
	int action_succeeded = 0;

	temp = pmlist_Find(item->m_ExternalPort, item->m_PortMappingProtocol, item->m_InternalClient);
	if (temp) // We found the item to delete
	{
	  CancelMappingExpiration(temp->expirationEventId);
// +++ Gemtek, CDRouter-190, 192
//		pmlist_DeletePortMapping(item->m_PortMappingEnabled, item->m_PortMappingProtocol, item->m_ExternalPort, 
//				item->m_InternalClient, item->m_InternalPort);
		pmlist_DeletePortMapping(item->m_PortMappingEnabled,item->m_RemoteHost, item->m_PortMappingProtocol, item->m_ExternalPort, 
				item->m_InternalClient, item->m_InternalPort);
// --- Gemtek, CDRouter-190, 192
		if (temp == pmlist_Head) // We are the head of the list
		{
			if (temp->next == NULL) // We're the only node in the list
			{
				pmlist_Head = pmlist_Tail = pmlist_Current = NULL;
				free (temp);
				action_succeeded = 1;
			}
			else // we have a next, so change head to point to it
			{
				pmlist_Head = temp->next;
				pmlist_Head->prev = NULL;
				free (temp);
				action_succeeded = 1;	
			}
		}
		else if (temp == pmlist_Tail) // We are the Tail, but not the Head so we have prev
		{
			pmlist_Tail = pmlist_Tail->prev;
			free (pmlist_Tail->next);
			pmlist_Tail->next = NULL;
			action_succeeded = 1;
		}
		else // We exist and we are between two nodes
		{
			temp->prev->next = temp->next;
			temp->next->prev = temp->prev;
			pmlist_Current = temp->next; // We put current to the right after a extraction
			free (temp);	
			action_succeeded = 1;
		}
	}
	else  // We're deleting something that's not there, so return 0
		action_succeeded = 0;

	return action_succeeded;
}

// +++ Gemtek, CDRouter-190,192

#if 0
int pmlist_AddPortMapping (int enabled, char *protocol, char *externalPort, char *internalClient, char *internalPort)
{
    if (enabled)
    {
#if HAVE_LIBIPTC
	char *buffer = malloc(strlen(internalClient) + strlen(internalPort) + 2);
	strcpy(buffer, internalClient);
	strcat(buffer, ":");
	strcat(buffer, internalPort);

	if (g_vars.forwardRules)
		iptc_add_rule("filter", g_vars.forwardChainName, protocol, NULL, NULL, NULL, internalClient, NULL, internalPort, "ACCEPT", NULL, FALSE);

	iptc_add_rule("nat", g_vars.preroutingChainName, protocol, g_vars.extInterfaceName, NULL, NULL, NULL, NULL, externalPort, "DNAT", buffer, TRUE);
	free(buffer);
#else
	char command[500];
	int status;
	
	{
	  char dest[100];
	  sprintf(dest,"%s:%s", internalClient, internalPort);
	  char *args[] = {"iptables", "-t", "nat", "-I", g_vars.preroutingChainName, "-i", g_vars.extInterfaceName, "-p", protocol, "--dport", externalPort, "-j", "DNAT", "--to", dest, NULL};
	  
	  sprintf(command, "%s -t nat -I %s -i %s -p %s --dport %s -j DNAT --to %s:%s", g_vars.iptables, g_vars.preroutingChainName, g_vars.extInterfaceName, protocol, externalPort, internalClient, internalPort);
	  trace(3, "%s", command);
	  if (!fork()) {
	    //system (command);
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	  } else {
	    wait(&status);		
	  }
	}

	if (g_vars.forwardRules)
	{
	  char *args[] = {"iptables", "-A", g_vars.forwardChainName, "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};
	  
	  sprintf(command,"%s -A %s -p %s -d %s --dport %s -j ACCEPT", g_vars.iptables,g_vars.forwardChainName, protocol, internalClient, internalPort);
	  trace(3, "%s", command);
	  if (!fork()) {
	    //system (command);
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	  } else {
	    wait(&status);		
	  }
	}
	
	// +++ Gemtek
	{
	  char *args[] = {"iptables", "-t", "nat", "-A", "PORT_FW", "-p", protocol, "-d", ExternalIPAddress, "--dport", internalPort, "-j", "DNAT", "--to", internalClient, NULL};

		if (!fork()) {
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	  } else {
	    wait(&status);		
	  }
	}
	
	{
	  char *args[] = {"iptables", "-A", "PORT_FW", "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};

		if (!fork()) {
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	  } else {
	    wait(&status);		
	  }
	}
			
	// --- Gemtek
#endif
    }
    return 1;
}

int pmlist_DeletePortMapping(int enabled, char *protocol, char *externalPort, char *internalClient, char *internalPort)
{
    if (enabled)
    {
#if HAVE_LIBIPTC
	char *buffer = malloc(strlen(internalClient) + strlen(internalPort) + 2);
	strcpy(buffer, internalClient);
	strcat(buffer, ":");
	strcat(buffer, internalPort);

	if (g_vars.forwardRules)
	    iptc_delete_rule("filter", g_vars.forwardChainName, protocol, NULL, NULL, NULL, internalClient, NULL, internalPort, "ACCEPT", NULL);

	iptc_delete_rule("nat", g_vars.preroutingChainName, protocol, g_vars.extInterfaceName, NULL, NULL, NULL, NULL, externalPort, "DNAT", buffer);
	free(buffer);
#else
	char command[500];
	int status;
	
	{
	  char dest[100];
	  sprintf(dest,"%s:%s", internalClient, internalPort);
	  char *args[] = {"iptables", "-t", "nat", "-D", g_vars.preroutingChainName, "-i", g_vars.extInterfaceName, "-p", protocol, "--dport", externalPort, "-j", "DNAT", "--to", dest, NULL};

	  sprintf(command, "%s -t nat -D %s -i %s -p %s --dport %s -j DNAT --to %s:%s",
		  g_vars.iptables, g_vars.preroutingChainName, g_vars.extInterfaceName, protocol, externalPort, internalClient, internalPort);
	  trace(3, "%s", command);
	  
	  if (!fork()) {
	    //system (command);
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	  } else {
	    wait(&status);		
	  }
	}

	if (g_vars.forwardRules)
	{
	  char *args[] = {"iptables", "-D", g_vars.forwardChainName, "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};
	  
	  sprintf(command,"%s -D %s -p %s -d %s --dport %s -j ACCEPT", g_vars.iptables, g_vars.forwardChainName, protocol, internalClient, internalPort);
	  trace(3, "%s", command);
	  if (!fork()) {
	    //system (command);
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	  } else {
	    wait(&status);		
	  }
	}
#endif
    }
    return 1;
}
#endif

int pmlist_AddPortMapping (int enabled, char *remoteHost,char *protocol, char *externalPort, char *internalClient, char *internalPort)
{
    if (enabled)
    {
#if HAVE_LIBIPTC
	char *buffer = malloc(strlen(internalClient) + strlen(internalPort) + 2);
	strcpy(buffer, internalClient);
	strcat(buffer, ":");
	strcat(buffer, internalPort);

	if (g_vars.forwardRules)
		iptc_add_rule("filter", g_vars.forwardChainName, protocol, NULL, NULL, NULL, internalClient, NULL, internalPort, "ACCEPT", NULL, FALSE);

	iptc_add_rule("nat", g_vars.preroutingChainName, protocol, g_vars.extInterfaceName, NULL, NULL, NULL, NULL, externalPort, "DNAT", buffer, TRUE);
	free(buffer);
#else
 //+++cdrouter-190,192
		char command[500];
	  int status;
	
	{
	  char dest[100];
	  
	  sprintf(dest,"%s:%s", internalClient, internalPort);
	 
	  	if(!strcmp(remoteHost,""))
	  	{
	      char *args[] = {"iptables", "-t", "nat", "-I", g_vars.preroutingChainName, "-i", g_vars.extInterfaceName,"-p", protocol, "--dport", externalPort, "-j", "DNAT", "--to-destination", dest, NULL};
	      sprintf(command, "%s -t nat -I %s -i %s -p %s --dport %s -j DNAT --to %s:%s", g_vars.iptables, g_vars.preroutingChainName, g_vars.extInterfaceName, protocol, externalPort, internalClient, internalPort);  
	   
	      if (!fork()) {
	       //system (command);
	        int rc = execv(g_vars.iptables, args);
	        exit(rc);
	        } else {
	          wait(&status);		
	        }
	     }
	     else
	     {
	        char *args[] = {"iptables", "-t", "nat", "-I", g_vars.preroutingChainName, "-i", g_vars.extInterfaceName,"-s", remoteHost, "-p", protocol, "--dport", externalPort, "-j", "DNAT", "--to-destination", dest, NULL};
	        if (!fork()) {
	         int rc = execv(g_vars.iptables, args);
	         exit(rc);
	          } else {
	         wait(&status);		
	         }
	      }
  }
  
	if (g_vars.forwardRules)
	{
		if(!strcmp(remoteHost,""))
		{
			char *args[] = {"iptables", "-A", g_vars.forwardChainName,"-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};
	    sprintf(command,"%s -A %s -p %s -d %s --dport %s -j ACCEPT", g_vars.iptables,g_vars.forwardChainName, protocol, internalClient, internalPort);
	    if (!fork()) {
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	    } else {
	    wait(&status);		
	    }
	  }
		else
		{	
	    char *args[] = {"iptables", "-A", g_vars.forwardChainName, "-s", remoteHost, "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};
	    if (!fork()) {
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	    } else {
	    wait(&status);		
	    }
    }
  }
	
	
	{
	  if(!strcmp(remoteHost,""))
	  {
//sam add +++ 2008.09.19 for remove unnecessary parameter
//origin	char *args[] = {"iptables", "-t", "nat", "-A", "PORT_FW","-i", g_vars.extInterfaceName, "-p", protocol,"--dport", internalPort, "-j", "DNAT", "--to-destination", internalClient, internalPort,NULL};
		char *args[] = {"iptables", "-t", "nat", "-A", "PORT_FW","-i", g_vars.extInterfaceName, "-p", protocol,"--dport", internalPort, "-j", "DNAT", "--to-destination", internalClient, NULL};
//sam add --- 2008.09.19
	   if (!fork()) {                            
		  int rc = execv(g_vars.iptables, args);  
		  exit(rc);                               
		  } else { 
		  wait(&status);	
		  }
	  }
	  else
	  {
//sam add +++ 2008.09.19 for remove unnecessary parameter
//origin		char *args[] = {"iptables", "-t", "nat", "-A", "PORT_FW","-i", g_vars.extInterfaceName, "-p", protocol, "-s", remoteHost, "--dport", internalPort, "-j", "DNAT", "--to-destination", internalClient, internalPort,NULL};
			char *args[] = {"iptables", "-t", "nat", "-A", "PORT_FW","-i", g_vars.extInterfaceName, "-p", protocol, "-s", remoteHost, "--dport", internalPort, "-j", "DNAT", "--to-destination", internalClient, NULL};
		  if (!fork()) {                            
		  int rc = execv(g_vars.iptables, args);  
		  exit(rc);                               
		  } else { 
			wait(&status);	
		  }
	  }
	}
	
	{
		if(!strcmp(remoteHost,""))
		{
		 char *args[] = {"iptables", "-A", "PORT_FW", "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};
		 if (!fork()) {                           
	    int rc = execv(g_vars.iptables, args); 
      exit(rc);                              
	   } else {                                 
	    wait(&status);		                     
	   }                                        
    }
		else
		{
		  char *args[] = {"iptables", "-A", "PORT_FW", "-s", remoteHost, "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};                                                                                                                                                                                                                                                        
	    if (!fork()) {                           
	    int rc = execv(g_vars.iptables, args); 
      exit(rc);                              
	    } else {                                 
	    wait(&status);		                     
	    }                                        
    }
  }
	
	//---cdrouter-190,192
#endif
   }
      return 1;
    
}  

int pmlist_DeletePortMapping(int enabled, char *remoteHost, char *protocol, char *externalPort, char *internalClient, char *internalPort)
{
    if (enabled)
    {
#if HAVE_LIBIPTC
	char *buffer = malloc(strlen(internalClient) + strlen(internalPort) + 2);
	strcpy(buffer, internalClient);
	strcat(buffer, ":");
	strcat(buffer, internalPort);

	if (g_vars.forwardRules)
	    iptc_delete_rule("filter", g_vars.forwardChainName, protocol, NULL, NULL, NULL, internalClient, NULL, internalPort, "ACCEPT", NULL);

	iptc_delete_rule("nat", g_vars.preroutingChainName, protocol, g_vars.extInterfaceName, NULL, NULL, NULL, NULL, externalPort, "DNAT", buffer);
	free(buffer);
#else
    //+++cdrouter-190,192
	char command[500];
	int status;
	
	{
	  char dest[100];
	  sprintf(dest,"%s:%s", internalClient, internalPort);
  	if(!strcmp(remoteHost,""))
  	{  		 
       char *args[] = {"iptables", "-t", "nat", "-D", g_vars.preroutingChainName, "-i", g_vars.extInterfaceName,"-p", protocol, "--dport", externalPort, "-j", "DNAT", "--to-destination", dest, NULL};
  		if (!fork()) {
	      int rc = execv(g_vars.iptables, args);
	    	exit(rc);
	  		} else {
	    		wait(&status);		
	  		}
  	}
  	else
  	 {
	    char *args[] = {"iptables", "-t", "nat", "-D", g_vars.preroutingChainName, "-i", g_vars.extInterfaceName, "-s", remoteHost, "-p", protocol, "--dport", externalPort, "-j", "DNAT", "--to-destination", dest, NULL};
       	if (!fork()) {
	     	int rc = execv(g_vars.iptables, args);
	      exit(rc);
	    	} else {
	    	wait(&status);		
	      }
     }
  }
	if (g_vars.forwardRules)
	{
	  if(!strcmp(remoteHost,""))
		{
		  char *args[] = {"iptables", "-D", g_vars.forwardChainName,"-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};
		  if (!fork()) 
		  {
	     int rc = execv(g_vars.iptables, args);
	     exit(rc);
	    } 
	    else 
	    wait(&status);		
	  }
		else
		{
	    char *args[] = {"iptables", "-D", g_vars.forwardChainName, "-s", remoteHost, "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};
	    if (!fork()) {
	    int rc = execv(g_vars.iptables, args);
	    exit(rc);
	    } else {
	    wait(&status);		
	  }
	  
	  
	  
	  }

  }
  //---Gemtek
  {
 		if(!strcmp(remoteHost,""))
	 		{
//sam add +++ 2008.09.19 for remove unnecessary parameter
//origin			char *args[] = {"iptables", "-t", "nat", "-D", "PORT_FW","-i", g_vars.extInterfaceName, "-p", protocol,"--dport", internalPort, "-j", "DNAT", "--to", internalClient, internalPort, NULL};
				char *args[] = {"iptables", "-t", "nat", "-D", "PORT_FW","-i", g_vars.extInterfaceName, "-p", protocol,"--dport", internalPort, "-j", "DNAT", "--to", internalClient, NULL};
//sam add --- 2008.09.19
				if (!fork()) {                               
		 	  int rc = execv(g_vars.iptables, args);     
		    exit(rc);                                  
		  	} else {                                     
		    wait(&status);		                         
		  	}                  	
	 		}
	 	else
	 	{
//sam add +++ 2008.09.19 for remove unnecessary parameter
//origin 	 	  char *args[] = {"iptables", "-t", "nat", "-D", "PORT_FW","-i", g_vars.extInterfaceName, "-p", protocol, "-s", remoteHost, "--dport", internalPort, "-j", "DNAT", "--to-destination", internalClient, internalPort, NULL};
	 	 	  char *args[] = {"iptables", "-t", "nat", "-D", "PORT_FW","-i", g_vars.extInterfaceName, "-p", protocol, "-s", remoteHost, "--dport", internalPort, "-j", "DNAT", "--to-destination", internalClient, NULL};
//sam add --- 2008.09.19
			  if (!fork()) {                               
		 	  int rc = execv(g_vars.iptables, args);     
		    exit(rc);                                  
		    } else {                                     
		    wait(&status);		                         
		    }                   
	  }                          
  }

	
  {
	  if(!strcmp(remoteHost,""))
  	{
//sam modify +++ 2008.03.28 for delete iptables rules in PORT_FW Chain. Here need to delete but add.
//origin	  char *args[] = {"iptables", "-A", "PORT_FW", "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};                                                                                                                                                                                            
		  char *args[] = {"iptables", "-D", "PORT_FW", "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};                                                                                                                                                                                            
//sam modify --- 2008.03.28
      if (!fork()) {                                 
      int rc = execv(g_vars.iptables, args);       
        exit(rc);                                    
      } 
      else
     wait(&status);		                           
    }
	  else
	  {
//sam modify +++ 2008.03.28 for delete iptables rules in PORT_FW Chain. Here need to delete but add.
//origin	char *args[] = {"iptables", "-A", "PORT_FW", "-s", remoteHost, "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};                                                                                                                                                                                            
		char *args[] = {"iptables", "-D", "PORT_FW", "-s", remoteHost, "-p", protocol, "-d", internalClient, "--dport", internalPort, "-j", "ACCEPT", NULL};                                                                                                                                                                                            
//sam modify --- 2008.03.28
      if (!fork()) 
      { 
      	int rc = execv(g_vars.iptables, args);       
        exit(rc);                                    
       } else 
        wait(&status);		                           
       
    }                                              	
  }
	//---cdrouter-190,192
#endif
  }
    return 1;
}
// --- Gemtek, CDRouter-190,192
