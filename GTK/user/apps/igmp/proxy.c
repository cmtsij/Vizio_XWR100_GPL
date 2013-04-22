#include "igmprt.h"

vifi_t numvifs;
char buffer[500*16];
unsigned long upstream;

extern int inet_ntoa();
extern int inet_addr();

/*
 * Open and init the multicast routing in the kernel.
 */

void k_init_proxy(int socket)
{
    int v = 1;
    int result;
    if (result = setsockopt(socket, IPPROTO_IP, MRT_INIT, (char *)&v, sizeof(int)) < 0)
    {
	;//slog(LOG_NOTICE,"setsockopt- MRT_INIT\n");
    }
    dbg_printf("MRT_INIT: %d\n", result);

}



/*
 * Stops the multicast routing in the kernel.
 */

void k_stop_proxy(int socket)
{
    ;//if (setsockopt(socket, IPPROTO_IP, MRT_DONE, (char *)NULL, 0) < 0)
	//syslog(LOG_NOTICE,"setsockopt- MRT_DONE\n");
}

int k_proxy_del_vif (int socket,unsigned long vifaddr,vifi_t vifi)
{
	struct vifctl vc;
	int error;
	
	vc.vifc_vifi = vifi;
	vc.vifc_flags = 0;
	vc.vifc_threshold = 0;
	vc.vifc_rate_limit = 0;
	vc.vifc_lcl_addr.s_addr = vifaddr;
	vc.vifc_rmt_addr.s_addr = INADDR_ANY;
	if ((error=setsockopt(socket, IPPROTO_IP, MRT_DEL_VIF,(char *)&vc, sizeof(vc))) <0){
	  //syslog(LOG_NOTICE,"setsockopt- MRT_ADD_VIF\n");
	  return FALSE;
	}
	return TRUE;
}



/* 
 * Add a virtual interface to the kernel 
 * using the pimd API:MRT_ADD_VIF
 * 
 */

int k_proxy_add_vif (int socket,unsigned long vifaddr,vifi_t vifi)
{
	struct vifctl vc;
	int error;

	vc.vifc_vifi = vifi;
	vc.vifc_flags = 0;
	vc.vifc_threshold = 0;
	vc.vifc_rate_limit = 0;
	vc.vifc_lcl_addr.s_addr = vifaddr;
	vc.vifc_rmt_addr.s_addr = INADDR_ANY;
	if ((error=setsockopt(socket, IPPROTO_IP, MRT_ADD_VIF,(char *)&vc, sizeof(vc))) <0){
	  //syslog(LOG_NOTICE,"setsockopt- MRT_ADD_VIF\n");
	  dbg_printf("(%s->%s) MRT_ADD_VIF: %d (FALSE) %d\n", __FILE__, __FUNCTION__, vifi, error);
	  return FALSE;
	}
	dbg_printf("(%s->%s) MRT_ADD_VIF: %d (TRUE) %d\n", __FILE__, __FUNCTION__, vifi, error);
	return TRUE;
}


/*
 * Del an MFC entry from the kernel
 * using pimd API:MRT_DEL_MFC
 */

int k_proxy_del_mfc (int socket, u_long source, u_long group)
{
	struct mfcctl mc;
	
	mc.mfcc_origin.s_addr   = source;
	mc.mfcc_mcastgrp.s_addr = group;
	if (setsockopt(socket, IPPROTO_IP, MRT_DEL_MFC, (char *)&mc, sizeof(mc)) <0){
	  //syslog(LOG_NOTICE,"setsockopt- MRT_DEL_MFC\n");
	  return FALSE;
	}
	return TRUE;
}



/*
 * Install and modify a MFC entry in the kernel (S,G,interface address)
 * using pimd API: MRT_AD_MFC
 */

extern igmp_router_t router;

int k_proxy_chg_mfc(int socket,u_long source,u_long group,vifi_t outvif,int fstate)
{
    struct mfcctl mc;
    igmp_interface_t *ifp;
    igmp_router_t* igmprt=&router;


    /*
     ** mfcc_parent holds the inputs(Upstreams) interfaces
     ** in a multiple PVC case, the inputs can have up to 8
     ** interfaces. Sean Lee is going to provide info on which PVC's have IGMP enabled
     */

    memset(&mc,0,sizeof(struct mfcctl));
    for (ifp = igmprt->igmprt_interfaces; ifp; ifp = ifp->igmpi_next)
        if (ifp->igmpi_type == UPSTREAM /* && ifp->igmpi_multienabled */) {
              mc.mfcc_origin.s_addr = source;
              mc.mfcc_mcastgrp.s_addr = group;
	      mc.mfcc_parent = ifp->igmpi_index;
	      mc.mfcc_ttls[outvif] = fstate;
	      if (setsockopt(socket, IPPROTO_IP, MRT_ADD_MFC, (char *)&mc, sizeof(mc)) < 0) {
	  	    //syslog(LOG_NOTICE,"setsockopt- MRT_ADD_MFC\n");
	            return(FALSE);
	      }
         }
     return(TRUE);

}

/*
 * create entry in the membership database
 */

membership_db*
create_membership(struct in_addr group,int fmode,int numsources,struct in_addr sources[MAX_ADDRS])
{
        membership_db* member;
        int i;
        if ((member = (membership_db*) malloc(sizeof(*member)))) {
                member->membership.group = group;
                member->membership.fmode = fmode;
                member->membership.numsources = numsources;
                for(i=0;i<numsources;i++)
                  member->membership.sources[i].s_addr = sources[i].s_addr;
                member->next = NULL;
                return member;
        }else
                return NULL;
}


/*
 * lookup for a group entry in the membership database
 */

membership_db*
find_membership(membership_db *membership,struct in_addr group)
{
  membership_db* memb;

        for(memb=membership;memb;memb=memb->next)
          if (memb->membership.group.s_addr == group.s_addr)
            return memb;
        return NULL;
}

/*
 * find a source in a in a source list
 */

int find_source(struct in_addr sr,int nsources,struct in_addr *sources)
{
       int i;

       for(i=0;i<nsources;i++)
         if (sources[i].s_addr == sr.s_addr)
           return TRUE;
       return FALSE;
}
/*
 * add multicast group to the membership database
 */

membership_db*
update_multi(igmp_router_t *igmprt,struct in_addr group,int fmode,int nsources,struct in_addr sources[MAX_ADDRS])
{

       int i,k;
       membership_db* member;
       struct in_addr sr[MAX_ADDRS];

       /*find corresponding group*/
       if ((member = find_membership(igmprt->igmprt_membership_db,group))) {
           /*update group status using merging rules*/
           member->membership.fmode = (int)member->membership.fmode && (int)fmode;
           if (member->membership.fmode == IGMP_FMODE_INCLUDE) {
                if (fmode == IGMP_FMODE_INCLUDE) {
                     for(i=0;i<nsources;i++)
                       if (find_source(sources[i],member->membership.numsources,member->membership.sources) == FALSE){
                         member->membership.numsources = member->membership.numsources + 1;
                         member->membership.sources[member->membership.numsources].s_addr = sources[i].s_addr;
                       }
                }
                else{
                     k = 0;
                     for(i=0;i<nsources;i++)
                         if (find_source(sources[i],member->membership.numsources,member->membership.sources) == FALSE){
                             sr[k].s_addr = sources[i].s_addr;
                             k = k+1;
                         }
                     member->membership.numsources = k;
                     for(i=0;i<k;i++)
                         member->membership.sources[i].s_addr = sr[i].s_addr;
                }
           }
           else {
                if (fmode == IGMP_FMODE_INCLUDE) {
                     k = 0;
                     for(i=0;i<member->membership.numsources;i++)
                         if (find_source(member->membership.sources[i],nsources,sources) == FALSE){
                             sr[k].s_addr = member->membership.sources[i].s_addr;
                             k = k+1;
                         }
                     member->membership.numsources = k;
                     for(i=0;i<k;i++)
                       member->membership.sources[i].s_addr = sr[i].s_addr;
                }
                else{
                     k = 0;
                     for(i=0;i<member->membership.numsources;i++)
                         if (find_source(member->membership.sources[i],nsources,sources) == TRUE){
                             sr[k].s_addr = member->membership.sources[i].s_addr;
                             k = k+1;
                         }
                     member->membership.numsources = k;
                     for(i=0;i<k;i++)
                       member->membership.sources[i].s_addr = sr[i].s_addr;
                }
           }
       }
       else {
            /*create new entry in the membership database*/
            member = create_membership(group,fmode,nsources,sources);
            member->next = igmprt->igmprt_membership_db;
            igmprt->igmprt_membership_db = member;
       }
       return member;
}

