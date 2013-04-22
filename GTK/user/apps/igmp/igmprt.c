
#include <sys/types.h>
#include <unistd.h>    
#include <sys/wait.h>
#include "igmprt.h"

#define CLI_MAX_BUF_SZ 128

igmp_router_t router;
igmp_interface_t *downstream;
char upstream_interface[10][8];
//extern int inet_ntoa();
//extern int inet_addr();

int version,querier;


void igmp_info_print(igmp_router_t *router){
	
#if IGMP_DBG
	igmp_interface_t *ifp;
	igmp_group_t *gp;
	igmp_src_t *src;
	igmp_rep_t *rep;
	
	printf("\nIGMP Table\n");
	printf("-----------------\n");
	printf("\n%-14s %-9s %-14s %-5s %-14s %-14s\n","interface","version","groups","mode","source","Membres");
	for (ifp=router->igmprt_interfaces;ifp;ifp=(igmp_interface_t *)ifp->igmpi_next){
		printf("%-14s 0x%x\n",(char *)inet_ntoa(ifp->igmpi_addr),ifp->igmpi_version);
		if (ifp->igmpi_groups != NULL){
			for(gp=ifp->igmpi_groups;gp;gp=(igmp_group_t*)gp->igmpg_next){
				printf("%32s %11s\n",(char *)inet_ntoa(gp->igmpg_addr),(gp->igmpg_fmode == IGMP_FMODE_INCLUDE)? "INCLUDE":"EXCLUDE");
				if (gp->igmpg_sources != NULL)
				     for (src=gp->igmpg_sources;src;src=(igmp_src_t *)src->igmps_next)
					printf("%50s\n",(char *)inet_ntoa(src->igmps_source));
				if (gp->igmpg_members != NULL)
					for (rep=gp->igmpg_members;rep;rep=(igmp_rep_t *)rep->igmpr_next)
						/*if (gp->igmpg_sources != NULL)
							printf("%17s\n",inet_ntoa(rep->igmpr_addr));
						else*/ 
							printf("%70s\n",(char *)inet_ntoa(rep->igmpr_addr));
				else printf("\n");
			}
		}else
			printf("\n");
		
	}
#endif

}


/*
 * igmp_rep_t* igmp_rep_create()
 *
 */
igmp_rep_t*
igmp_rep_create(
	struct in_addr srcaddr)
{
	igmp_rep_t* rep;

	if ((rep = (igmp_rep_t*) malloc(sizeof(*rep)))) {
	  rep->igmpr_addr.s_addr = srcaddr.s_addr;
	  rep->igmpr_next  = NULL;
	}
	return rep;
}

int
igmp_rep_count(
	igmp_group_t *gp)
{
	igmp_rep_t *re;
	int count=0;
	dbg_printf("(%s->%s(%d)) group: %08x\n", __FILE__, __FUNCTION__, __LINE__, gp->igmpg_addr.s_addr);
	assert (gp != NULL);
	for (re=gp->igmpg_members;re != NULL;re=(igmp_rep_t *)re->igmpr_next)
		count++;
	dbg_printf("(%s->%s(%d)) count: %d\n", __FILE__, __FUNCTION__, __LINE__, count);

	return count;
	    
}
/*
 * void igmp_rep_cleanup()
 *
 */
void
igmp_rep_cleanup(
	igmp_group_t *gp,
	igmp_rep_t * rep)
{
	igmp_rep_t *re;

	assert(rep != NULL);
	assert (gp != NULL);
	if (rep != gp->igmpg_members){
	  for (re=gp->igmpg_members;(igmp_rep_t *)re->igmpr_next != rep;re=(igmp_rep_t *)re->igmpr_next);		
	  re->igmpr_next = rep->igmpr_next;
	  dbg_printf("(%s->%s(%d))*** free rep (0x%x)\n",__FILE__, __FUNCTION__, __LINE__, rep->igmpr_addr.s_addr);
	  free(rep); // Marked by Joshua
	}else{
	  /*delete the head*/
	  re=gp->igmpg_members;
	  gp->igmpg_members = (igmp_rep_t *)re->igmpr_next;
	  dbg_printf("(%s->%s(%d))*** free rep head (0x%x)\n",__FILE__, __FUNCTION__, __LINE__, re->igmpr_addr.s_addr);
	  free(re); // Marked by Joshua
	}
}

/*
 * void igmp_rep_print()
 *
 */
void
igmp_rep_print(
	igmp_rep_t* rep)
{
  printf("\t\tmembre:%-16s\n", 
	 (char *)inet_ntoa(rep->igmpr_addr));
}
/*
 * igmp_rep_t* igmp_group_rep_lookup()
 *
 * Lookup a membre in the sourcetable of a group
 */
igmp_rep_t*
igmp_group_rep_lookup(
	igmp_group_t *gp,
	struct in_addr srcaddr)
{
  igmp_rep_t *rep;
  dbg_printf("(%s->%s(%d)) get (%x):(%x)\n", __FILE__, __FUNCTION__, __LINE__,  gp->igmpg_addr.s_addr, srcaddr.s_addr);
  
  assert(gp != NULL);
  for (rep = (igmp_rep_t *) gp->igmpg_members; rep; rep = (igmp_rep_t *)rep->igmpr_next)
  {
    dbg_printf("(%s->%s(%d)) compare (0x%8x):(0x%8x)\n",__FILE__, __FUNCTION__, __LINE__,  rep->igmpr_addr.s_addr,  srcaddr.s_addr);
    if (rep->igmpr_addr.s_addr == srcaddr.s_addr)
    {
      dbg_printf("(%s->%s(%d)) compare (0x%8x):(0x%8x), get MATCH\n",__FILE__, __FUNCTION__, __LINE__,  rep->igmpr_addr.s_addr, srcaddr.s_addr);
      return rep;
    }
  }
  return NULL; 
}

/*
 * igmp_rep_t* igmp_group_rep_add()
 *
 * Add a member to the set of sources of a group
 */
igmp_rep_t*
igmp_group_rep_add(
	igmp_group_t *gp,
	struct in_addr srcaddr)
{
	igmp_rep_t* rep;

	assert(gp != NULL);
	/* Return the source if it's already present */
	if ((rep = igmp_group_rep_lookup(gp, srcaddr))) {
	  dbg_printf("(%s->%s(%d)) src(0x%x) already present \n", __FILE__, __FUNCTION__, __LINE__, srcaddr.s_addr);
	  return rep;
	}
	/* Create the source and add to the set */ 
	if ((rep = igmp_rep_create(srcaddr))) {
	  (igmp_rep_t *)rep->igmpr_next = gp->igmpg_members;
	  gp->igmpg_members = rep;
	  dbg_printf("(%s->%s(%d))  create the src(0x%x) and add to the set \n", __FILE__, __FUNCTION__, __LINE__, srcaddr.s_addr);
	}
	return rep;
}
/******************************************************************************
 *
 * igmp source routines 
 * 
 *****************************************************************************/
/*
 * igm_src_t* igmp_src_create()
 *
 */
igmp_src_t*
igmp_src_create(
	struct in_addr srcaddr)
{
	igmp_src_t* src;

	if ((src = (igmp_src_t*) malloc(sizeof(*src)))) {
	  src->igmps_source.s_addr = srcaddr.s_addr;
	  src->igmps_timer = 0;
	  src->igmps_fstate = TRUE;
	  src->igmps_next  = NULL;
	}
	return src;
}

/*
 * void igmp_src_cleanup()
 *
 */
void
igmp_src_cleanup(
	igmp_group_t *gp,
	igmp_src_t * src)
{
	igmp_src_t *sr;

	assert(src != NULL);
	assert (gp != NULL);
	if (src != gp->igmpg_sources){
	  for (sr=gp->igmpg_sources;(igmp_src_t *)sr->igmps_next != src;sr=(igmp_src_t *)sr->igmps_next);		
	  sr->igmps_next = src->igmps_next;
	  free(src);
	}else{
	  /*delete the head*/
	  sr=gp->igmpg_sources;
	  gp->igmpg_sources = (igmp_src_t *)sr->igmps_next;
	  free(sr);
	}
}

/*
 * void igmp_src_print()
 *
 */
void
igmp_src_print(
	igmp_src_t* src)
{
  printf("\t\tsource:%-16s  %d\n", 
	 (char *)inet_ntoa(src->igmps_source),src->igmps_timer);
}
/*
 * igmp_src_t* igmp_group_src_lookup()
 *
 * Lookup a source in the sourcetable of a group
 */
igmp_src_t*
igmp_group_src_lookup(
	igmp_group_t *gp,
	struct in_addr srcaddr)
{
  igmp_src_t *src;
  
 if (gp != NULL){
  for (src = gp->igmpg_sources; src != NULL; src = (igmp_src_t *)src->igmps_next){
    if (src->igmps_source.s_addr == srcaddr.s_addr)
      return src;
  }
  }
  return NULL; 
}

/*
 * igmp_src_t* igmp_group_src_add()
 *
 * Add a source to the set of sources of a group
 */
igmp_src_t*
igmp_group_src_add(
	igmp_group_t *gp,
	struct in_addr srcaddr)
{
	igmp_src_t* src;

	assert(gp != NULL);
	/* Return the source if it's already present */
	if ((src = igmp_group_src_lookup(gp, srcaddr)))
	  return src;
	/* Create the source and add to the set */ 
	if ((src = igmp_src_create(srcaddr))) {
	  (igmp_src_t *)src->igmps_next = gp->igmpg_sources;
	  gp->igmpg_sources = src;
	  dbg_printf ( "(%s->%s(%d)) igmp_group_src_add first member 0x%08x\n", __FILE__, __FUNCTION__, __LINE__, srcaddr.s_addr );
	}
	return src;
}



/******************************************************************************
 *
 * igmp group routines 
 *
 *****************************************************************************/

/*
 * igmp_group_t* igmp_group_create()
 *
 * Create a group record
 */
igmp_group_t*
igmp_group_create(
	struct in_addr groupaddr)
{
	igmp_group_t* gp;

	if ((gp = (igmp_group_t*) malloc(sizeof(*gp)))) {
		gp->igmpg_addr.s_addr = groupaddr.s_addr;
//eddie
		gp->igmpg_fmode = IGMP_FMODE_EXCLUDE;
		gp->igmpg_version = IGMP_VERSION_2;/*default version is V2*/
//eddie
		gp->igmpg_timer = 0;
		gp->igmpg_sources = NULL;
		gp->igmpg_members = NULL;
		gp->igmpg_next = NULL;
		return gp;
	}else
		return NULL;
	
}

/*
 * void igmp_group_cleanup()
 *
 * Cleanup a group record
 */
void
igmp_group_cleanup(
	igmp_interface_t *ifp,
	igmp_group_t* gp,
	igmp_router_t* router)
{
	char cmd[CLI_MAX_BUF_SZ];
	igmp_group_t *g;
	dbg_printf("(%s->%s(%d))\n", __FILE__, __FUNCTION__, __LINE__);

	assert(gp != NULL);
	assert(ifp != NULL);

	sprintf ( cmd, "/bin/iptables -t filter -D FORWARD -i %s -d %s -j ACCEPT 2>/dev/null", upstream_interface[0], (char *)inet_ntoa(gp->igmpg_addr) ); // Added by Joshua
	bcmSystemEx(cmd,1);
	dbg_printf("(%s->%s(%d)) execute: %s\n", __FILE__, __FUNCTION__, __LINE__, cmd);

	if (ifp->igmpi_groups != gp){
	  g=ifp->igmpi_groups;
	  while((igmp_group_t *)g->igmpg_next != gp) {		
	     g=(igmp_group_t *)g->igmpg_next;
	     if (g == NULL ) {
	 	//printf("BUG in igmp_group_cleanup sleep 100000\n"); 
		getchar();
	     }
	  }
	  g->igmpg_next=gp->igmpg_next;		
	  free(gp);
	}else{/*delete the head*/
	  g=ifp->igmpi_groups;
	  ifp->igmpi_groups = (igmp_group_t *)g->igmpg_next;
	  free(g);
	} 
}

/*
 * void igmp_group_print()
 *
 * Print a group record
 */
void
igmp_group_print(
	igmp_group_t* gp)
{
  igmp_src_t *src;
  igmp_rep_t *rep;
  printf("  %-16s %s %d\n", 
	 (char *)inet_ntoa(gp->igmpg_addr),
	 (gp->igmpg_fmode == IGMP_FMODE_EXCLUDE) ? "exclude" : "include",
	 gp->igmpg_timer);
  if (gp->igmpg_sources != NULL)
    for (src=gp->igmpg_sources;src;src=(igmp_src_t *)src->igmps_next)
      printf("source : %s timer : %d\n",(char *)inet_ntoa(src->igmps_source.s_addr),src->igmps_timer);
   if (gp->igmpg_members != NULL)
    for (rep=gp->igmpg_members;rep;rep=(igmp_rep_t *)rep->igmpr_next)
      printf("member : %s \n",(char *)inet_ntoa(rep->igmpr_addr.s_addr));
}

/******************************************************************************
 *
 * igmp interface routines 
 *
 *****************************************************************************/

/*
 * igmp_interface_t* igmp_interface_create()
 *
 * Create and initialize interface record
 */
igmp_interface_t*
igmp_interface_create(
	struct in_addr ifaddr,
	char *ifname,
	vifi_t index)
{
  igmp_interface_t* ifp;
  struct ip_mreq mreq;
  u_char i;
  short flags;
  
	if (((ifp = (igmp_interface_t*) malloc(sizeof(*ifp)))) == NULL)
		return NULL;

	/* Allocate a buffer to receive igmp messages */ 
	ifp->igmpi_bufsize = get_interface_mtu(ifname);

	if (ifp->igmpi_bufsize == -1)
		ifp->igmpi_bufsize = MAX_MSGBUFSIZE;
	/* XXX Should make buffer slightly bigger for ip header */
	if ((ifp->igmpi_buf = (char*) malloc(ifp->igmpi_bufsize)) == NULL) {
		free(ifp);
		return NULL;
	}	

	/* Initialize all fields */
	ifp->igmpi_addr.s_addr = ifaddr.s_addr;
	strncpy(ifp->igmpi_name, ifname, IFNAMSIZ);
	if ( strncmp(ifname,"br",2) == 0 )
	{
	   ifp->igmpi_type = DOWNSTREAM;
	   dbg_printf("(%s) get downstream: %s\n", __FUNCTION__, ifp->igmpi_name);
	   /* Create a raw igmp socket */
          ifp->igmpi_socket = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP);
	}
	else
	{
	   ifp->igmpi_type = UPSTREAM;
	   dbg_printf("(%s) get upstream: %s\n", __FUNCTION__, ifp->igmpi_name);
          ifp->igmpi_socket = socket(AF_INET, SOCK_DGRAM, 0);
	}


	if (ifp->igmpi_socket == -1) {
		printf("can't create socket \n");
 		free(ifp->igmpi_buf);
		free(ifp);
		return NULL;
	}
	ifp->igmpi_groups = NULL;
	ifp->sch_group_query = NULL;
	ifp->igmpi_isquerier = TRUE;
  	ifp->igmpi_version = version; /*IGMP_VERSION_3;*/
	ifp->igmpi_qi = IGMP_DEF_QI;
	ifp->igmpi_qri = IGMP_DEF_QRI;
	ifp->igmpi_rv = IGMP_DEF_RV;
	ifp->igmpi_gmi = ifp->igmpi_rv * ifp->igmpi_qi + ifp->igmpi_qri;
	ifp->igmpi_ti_qi = 0;
	ifp->igmpi_next = NULL;

#if 0
	/* Set router alert */
	ra[0] = 148;
	ra[1] = 4;
	ra[2] = 0;
	ra[3] = 0;
	setsockopt(ifp->igmpi_socket, IPPROTO_IP, IP_OPTIONS, ra, 4);
#endif

	int result;
	/* Set reuseaddr, ttl, loopback and set outgoing interface */
	i = 1;
	const int on=1;
	//result = setsockopt(ifp->igmpi_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&i, sizeof(i));
	result = setsockopt(ifp->igmpi_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	dbg_printf("SO_REUSEADDR: %d\n", result);
	i = 1;
	result = setsockopt(ifp->igmpi_socket, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&i, sizeof(i));
	dbg_printf("IP_MULTICAST_TTL: %d\n", result);
	//eddie disable LOOP
	i = 0;
	result = setsockopt(ifp->igmpi_socket, IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&i, sizeof(i));
	dbg_printf("IP_MULTICAST_LOOP: %d\n", result);
	result = setsockopt(ifp->igmpi_socket, IPPROTO_IP, IP_MULTICAST_IF, (void*)&ifaddr, sizeof(ifaddr));
	dbg_printf("IP_MULTICAST_IF: %d\n", result);

	//eddie In linux use IP_PKTINFO
	//IP_RECVIF returns the interface of received datagram
	//setsockopt(ifp->igmpi_socket, IPPROTO_IP, IP_RECVIF, &i, sizeof(i));
	i = 1;
	result = setsockopt(ifp->igmpi_socket, IPPROTO_IP, IP_PKTINFO, &i, sizeof(i));
	dbg_printf("IP_PKTINFO: %d\n", result);

	/* Add membership to ALL_ROUTERS and ALL_ROUTERS_V3 on this interface */
	mreq.imr_multiaddr.s_addr = inet_addr(IGMP_ALL_ROUTERS);
	mreq.imr_interface.s_addr = ifaddr.s_addr;/*htonl(0);*/
	if ( ifp->igmpi_type == DOWNSTREAM ) {
		result = setsockopt(ifp->igmpi_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		dbg_printf("IP_ADD_MEMBERSHIP: %d\n", result);
	}

	/* Tell the kernel this interface belongs to a multicast router */
	if ( ifp->igmpi_type == DOWNSTREAM )
	{
		result = mrouter_onoff(ifp->igmpi_socket,1);
		dbg_printf("mrouter_onoff result is %d\n", result);
	}
	ifp->igmpi_index = index;
	
	/* Set the interface flags to receive all multicast packets */
	ifp->igmpi_save_flags = get_interface_flags(ifname);
	if (ifp->igmpi_save_flags != -1) {
		set_interface_flags(ifname, ifp->igmpi_save_flags | IFF_ALLMULTI);
		/* If IFF_ALLMULTI didn't work, try IFF_PROMISC */
		flags = get_interface_flags(ifname);
		 if ((flags & IFF_ALLMULTI) != IFF_ALLMULTI) 
		    set_interface_flags(ifname, ifp->igmpi_save_flags | IFF_PROMISC);
	}
	else
		dbg_printf("can not get flags: %s\n", ifp->igmpi_name);


	return ifp;
}

/*
 * void igmp_interface_cleanup()
 *
 * Cleanup an interface 
 */
void
igmp_interface_cleanup(igmp_interface_t* ifp)
{
	igmp_group_t* gp,*gp2;

	assert(ifp != NULL);

	/* Cleanup all groups */
	gp=ifp->igmpi_groups;
	ifp->igmpi_groups = NULL;
	while(gp != NULL){
		gp2=gp;
		gp=gp->igmpg_next;
		free(gp2);
	}
	/* Tell the kernel the multicast router is no more */
	if ( ifp->igmpi_type == DOWNSTREAM )
	   k_stop_proxy(ifp->igmpi_socket);
	close(ifp->igmpi_socket);

 	free(ifp->igmpi_buf);

	/* If we managed to get/set the interface flags, revert */
	if (ifp->igmpi_save_flags != -1)
		set_interface_flags(ifp->igmpi_name, ifp->igmpi_save_flags);
	free(ifp);
}

/*
 * igmp_group_t* igmp_interface_group_add()
 *
 * Add a group to the set of groups of an interface
 */
igmp_group_t*
igmp_interface_group_add(
	igmp_router_t* router,
	igmp_interface_t *ifp,
	struct in_addr groupaddr)
{
	char cmd[CLI_MAX_BUF_SZ];
	igmp_group_t* gp;
	struct ip_mreq mreq;
	igmp_interface_t* upstream_interface;
	struct in_addr up;

	//printf("Add a group to the set of groups of an interface \n");
	assert(ifp != NULL);
	/* Return the group if it's already present */
	if ((gp = igmp_interface_group_lookup(ifp, groupaddr)))
		return gp;
	/* Create the group and add to the set */ 
	if ((gp = igmp_group_create(groupaddr))) {
		//printf("created  a group \n");
		gp->igmpg_next = ifp->igmpi_groups;
		ifp->igmpi_groups = gp;

		//printf("adding to all upstream interfaces \n");

	    for (ifp = router->igmprt_interfaces; ifp; ifp = ifp->igmpi_next) {
	      if (ifp->igmpi_type == UPSTREAM) {

		mreq.imr_multiaddr.s_addr=groupaddr.s_addr;
		mreq.imr_interface.s_addr=ifp->igmpi_addr.s_addr;
		if (VALID_ADDR(mreq.imr_multiaddr)) { /* only if the join is from downstream */
		  up.s_addr = ifp->igmpi_addr.s_addr;
		  upstream_interface = igmprt_interface_lookup(router,up);
		  if (igmp_interface_group_lookup(upstream_interface,mreq.imr_multiaddr) == NULL) {
		    if (setsockopt(router->igmprt_up_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mreq, sizeof(mreq)) < 0) { 
        	      ;//syslog(LOG_NOTICE,"igmp:IP_ADD_MEMBERSHIP Failed\n");
		    }
			// printf("* DONE IP_ADD_MEMBERSHIP gaddr %s iname %s upstiname %s rtiname %s rtiaddr %s* \n",
			//	(char *)inet_ntoa(groupaddr), ifp->igmpi_name, upstream_interface->igmpi_name,
			//	router->igmprt_interfaces->igmpi_name,
			//	(char *)inet_ntoa(router->igmprt_interfaces->igmpi_addr));

			sprintf ( cmd, "/bin/iptables -t filter -I FORWARD 1 -i %s -d %s -j ACCEPT 2>/dev/null", ifp->igmpi_name, (char *)inet_ntoa(groupaddr) );
			bcmSystemEx(cmd, 1);
			dbg_printf("(%s->%s(%d)) execute: %s\n", __FILE__, __FUNCTION__, __LINE__, cmd);
		  }
		} 
	      } 
	    } /* for */ 
	}
	return gp;
}

/*
 * igmp_group_t* igmp_interface_group_lookup()
 *
 * Lookup a group in the grouptable of an interface
 */
igmp_group_t*
igmp_interface_group_lookup(
	igmp_interface_t *ifp,
	struct in_addr groupaddr)
{
	igmp_group_t* gp;

	assert(ifp != NULL);
	for (gp = ifp->igmpi_groups; gp; gp = gp->igmpg_next)
		if (gp->igmpg_addr.s_addr == groupaddr.s_addr)
			return gp;
	dbg_printf("(%s->%s(%d)) igmp_interface_group_lookup return NULL \n", __FILE__, __FUNCTION__, __LINE__);
	return NULL; 
}
void
igmp_interface_membership_leave_v2(
	igmp_router_t* router,	
	igmp_interface_t* ifp,
	struct in_addr src,
	igmpr_t* report,
	int len)
{
	igmp_group_t* gp;
	igmp_rep_t *rep;
	igmp_interface_t* ifi;
  	struct ip_mreq mreq;
	int count=0;
	int needdrop=0;

	if (! IN_MULTICAST(ntohl(report->igmpr_group.s_addr))) {
		printf("Ignore non-multicast ...\n");
		return;
	}

	dbg_printf("(%s->%s(%d)) LEAVE GROUP=0x%x \n", __FILE__, __FUNCTION__, __LINE__,ntohl(report->igmpr_group.s_addr));
	for (ifi = router->igmprt_interfaces; ifi ; ifi = ifi->igmpi_next) {
		if ( ifi->igmpi_type == DOWNSTREAM) {
			for(gp = ifi->igmpi_groups;gp;gp = gp->igmpg_next) {
				dbg_printf("(%s->%s(%d)) compare %08x:%08x\n", __FILE__, __FUNCTION__, __LINE__, gp->igmpg_addr.s_addr, report->igmpr_group.s_addr);
				//if (gp->igmpg_addr.s_addr == ntohl(report->igmpr_group.s_addr)){ // Marked by Joshua
				if (gp->igmpg_addr.s_addr == report->igmpr_group.s_addr){ // Modified by Joshua - take off ntohl
						//if (strcmp(inet_ntoa(gp->igmpg_addr.s_addr),inet_ntoa(ntohl(report->igmpr_group.s_addr)))==0 ){ // Added by Joshua
					dbg_printf("(%s->%s(%d)) %08x=%08x\n", __FILE__, __FUNCTION__, __LINE__, gp->igmpg_addr.s_addr, report->igmpr_group.s_addr);
					rep = igmp_group_rep_lookup(gp,src);
					if (rep)
						igmp_rep_cleanup(gp,rep);
					count = igmp_rep_count(gp);
					if (count == 0) {
						igmp_group_cleanup(ifi,gp, router);
						dbg_printf("(%s->%s(%d)) LEAVE GROUP member rep count=%d rtiname %s \n", __FILE__, __FUNCTION__, __LINE__,count, router->igmprt_interfaces->igmpi_name);
						needdrop=1;
					}
				}
				else
					dbg_printf("(%s->%s(%d)) %08x != %08x\n", __FILE__, __FUNCTION__, __LINE__, gp->igmpg_addr.s_addr, report->igmpr_group.s_addr);

			} /*for groups */
		} /*DOWNSTREAM */
	} /*for interfaces*/

	if ( needdrop ) {
	  for (ifi = router->igmprt_interfaces; ifi ; ifi = ifi->igmpi_next) {
	    if ( ifi->igmpi_type == UPSTREAM ) {
            	mreq.imr_multiaddr.s_addr=ntohl(report->igmpr_group.s_addr);
             	mreq.imr_interface.s_addr=ifi->igmpi_addr.s_addr;
             if (VALID_ADDR(mreq.imr_multiaddr)) {
               if (setsockopt(router->igmprt_up_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void *) &mreq, sizeof(mreq)) < 0) {
        	      ;//syslog(LOG_NOTICE,"igmp:IP_DROP_MEMBERSHIP Failed\n");
                 }
                 dbg_printf("(%s->%s(%d)) IP_DROP_MERSHIP for(0x%x)\n",__FILE__, __FUNCTION__, __LINE__,ifi->igmpi_addr.s_addr);
             	}
             }
        }
	} /* needdrop */

}

/*
 * void igmp_interface_membership_report_v12()
 *
 * Process a v1/v2 membership report
 */
void
igmp_interface_membership_report_v12(
	igmp_router_t* router,	
	igmp_interface_t* ifp,
	struct in_addr src,
	igmpr_t* report,
	int len)
{
	igmp_group_t* gp;
	igmp_rep_t *rep;

	/* Ignore a report for a non-multicast address */ 
	if (! IN_MULTICAST(ntohl(report->igmpr_group.s_addr))) {
		printf("Ignore non-multicast ...\n");
		return;
	}
	/* Find the group, and if not present, add it */
	dbg_printf("(%s->%s(%d))  igmp_interface_group_add 0x%8x, src: 0x%8x\n",__FILE__, __FUNCTION__, __LINE__,report->igmpr_group.s_addr, src.s_addr);
	if ((gp = igmp_interface_group_add(router,ifp, report->igmpr_group)) == NULL)
		return;
    	/* find the member and add it if not present*/
	rep=igmp_group_rep_add(gp,src);
	dbg_printf("(%s->%s(%d))  igmp_group_rep_add call igmp_group_handle_isex\n",__FILE__, __FUNCTION__, __LINE__);

	/* Consider this to be a v3 is_ex{} report */
	//igmp_group_handle_isex(router,ifp, gp, 0, NULL);
	igmp_group_handle_isex(router,ifp, gp, 0, &src); //Joshua
}

/*
 * void igmp_interface_print()
 *
 * Print status of an interface 
 */
void
igmp_interface_print(
	igmp_interface_t* ifp)
{
	igmp_group_t* gp;

	/*syslog(LOG_NOTICE," interface %s, %s ver=0x%x name=%s index=%d\n",
		(char *)inet_ntoa(ifp->igmpi_addr),(ifp->igmpi_type == UPSTREAM)?"UPSTREAM":"DOWNSTREAM", ifp->igmpi_version,ifp->igmpi_name,ifp->igmpi_index);*/
	dbg_printf(" interface %s, %s ver=0x%x name=%s index=%d\n",
		(char *)inet_ntoa(ifp->igmpi_addr),(ifp->igmpi_type == UPSTREAM)?"UPSTREAM":"DOWNSTREAM", ifp->igmpi_version,ifp->igmpi_name,ifp->igmpi_index);
	for (gp = ifp->igmpi_groups; gp; gp = gp->igmpg_next)
		igmp_group_print(gp);
}

/******************************************************************************
 *
 * igmp router routines
 *
 *****************************************************************************/

/*
 * int igmprt_init()
 *
 * Initialize igmp router
 */
int
igmprt_init(
	igmp_router_t* igmprt)
{
	igmprt->igmprt_interfaces = NULL;
	//igmprt->igmprt_thr_timer = igmprt->igmprt_thr_input = NULL;
	igmprt->igmprt_flag_timer = 0;
	igmprt->igmprt_flag_input = 0;
/*create socket to tell BB-RAS about member reports from downstream */
	igmprt->igmprt_up_socket = socket( AF_INET, SOCK_DGRAM, 0 );
        if( igmprt->igmprt_up_socket < 0) {
	  perror("can't open upstream socket");
	  exit (1);
	}
	/*create raw socket to update mfc and vif table*/
	// Joshua
	igmprt->igmprt_socket = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP);
	//igmprt->igmprt_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (igmprt->igmprt_socket < 0)  {
	  perror("can't open igmp socket");
	  exit (1);
	}
        forward_upstream = 0;
	return 1;
}

/*
 * void igmprt_destroy()
 *
 * Cleanup the router 
 */
void
igmprt_destroy(igmp_router_t* igmprt)
{
	igmp_interface_t *ifp, *ifp2;

	for (ifp = igmprt->igmprt_interfaces; ifp;) {
 		k_proxy_del_vif(igmprt->igmprt_socket,ifp->igmpi_addr,ifp->igmpi_index);
		ifp2 = ifp;
		ifp = ifp->igmpi_next;
		igmp_interface_cleanup(ifp2);
	}
	igmprt_stop(igmprt);
	
}

/*
 * interface_t* igmprt_interface_lookup()
 *
 * Lookup a group, identified by the interface address
 */
igmp_interface_t*
igmprt_interface_lookup(
	igmp_router_t* igmprt, 
	struct in_addr ifaddr)
{
	igmp_interface_t *ifp;

	for (ifp = igmprt->igmprt_interfaces; ifp; ifp = ifp->igmpi_next)
		if (ifp->igmpi_addr.s_addr == ifaddr.s_addr)
			return ifp;
	return NULL;
}

/*
 * interface_t* igmprt_interface_lookup_index()
 *
 * Lookup a group, identified by the interface index
 */
igmp_interface_t*
igmprt_interface_lookup_index(
	igmp_router_t* igmprt, 
	int ifp_index)
{
	igmp_interface_t *ifp;

	for (ifp = igmprt->igmprt_interfaces; ifp; ifp = ifp->igmpi_next)
		if (ifp->igmpi_index == ifp_index)
			return ifp;
	return NULL;
}

/*
 * igmp_entry_t* igmprt_group_lookup() 
 *
 * Lookup a group, identified by the interface and group address
 */
igmp_group_t*
igmprt_group_lookup(
	igmp_router_t* igmprt, 
	struct in_addr ifaddr, 
	struct in_addr groupaddr)
{
	igmp_interface_t *ifp; 

	if ((ifp = igmprt_interface_lookup(igmprt, ifaddr)))
		return igmp_interface_group_lookup(ifp, groupaddr);
	return NULL;
}

/*
 * struct igmp_interface_t* igmprt_interface_add()
 *
 * Add an interface to the interfacetable
 */
igmp_interface_t*
igmprt_interface_add(
	igmp_router_t* igmprt, 
	struct in_addr ifaddr,
	char *ifname,
	vifi_t index)
{
	igmp_interface_t *ifp;

	/* Return the interface if it's already in the set */
	if ((ifp = igmprt_interface_lookup(igmprt, ifaddr)))
		return ifp;
	/* Create the interface and add to the set */
	if ((ifp = igmp_interface_create(ifaddr, ifname,index))) {
		ifp->igmpi_next = igmprt->igmprt_interfaces;
		igmprt->igmprt_interfaces = ifp;
	}
	return ifp;
}

/*
 * igmp_group_t* igmprt_group_add()
 *
 * Add a group to the grouptable of some interface
 */
igmp_group_t*
igmprt_group_add(
	igmp_router_t* igmprt, 
	struct in_addr ifaddr, 
	struct in_addr groupaddr)
{
	igmp_interface_t *ifp; 

	if ((ifp = igmprt_interface_lookup(igmprt, ifaddr)))
		return NULL;
	return igmp_interface_group_add(igmprt,ifp, groupaddr);
}

/*
 * void igmprt_timergroup(igmp_router_t* igmprt)
 *
 * Decrement timers and handle whatever has to be done when one expires
 */
void
igmprt_timer(igmp_router_t* igmprt)
{
  igmp_interface_t* ifp;
  struct in_addr zero;
  
  zero.s_addr = 0;
  
  /* Handle every interface */
  for (ifp = igmprt->igmprt_interfaces; ifp; ifp = ifp->igmpi_next) {
    /* If we're the querier for this network, handle all querier 
     * duties */
   if ( ifp->igmpi_type == DOWNSTREAM ) {
       if (ifp->igmpi_isquerier == TRUE ) {
         /* Deal with the general query */
         if (--ifp->igmpi_ti_qi <= 0) {
	   ifp->igmpi_ti_qi = ifp->igmpi_qi;
	   igmprt_membership_query(igmprt, ifp, &zero, NULL, 0, 0);
         }
       }else{
        /* If not the querier, deal with other-querier-present timer*/
        igmprt_timer_querier(ifp);	
       }
       /*handle group timer*/
       igmprt_timer_group(igmprt,ifp);
  } /* DOWNSTREAM */
 } /* for */
}

void*
igmprt_timer_thread(void* arg)
{
  igmp_router_t* igmprt = (igmp_router_t*) arg;

  
  if (igmprt->igmprt_flag_timer) {
    igmprt_timer(igmprt);
    /* Should be changed to take care of drifting */
    usleep(1000000);
  }
  return NULL;
}


/*
 * void igmprt_input()
 *
 * Process an incoming igmp message
 */
void
igmprt_input(igmp_router_t* igmprt, igmp_interface_t* ifp)
{

	struct msghdr msg;
	struct iovec iov;
	struct cmsghdr *cmsg;
	char *ctrl = (char *)malloc(MAXCTRLSIZE);
	int if_index=0;
	struct in_pktinfo *info;
	struct ip *iph;
	unsigned char *ptype;
	int n, igmplen;
	igmpv2q_t *query;
   	struct in_addr src;
	int srsp=1;
	int badmsg=0;
	igmp_interface_t *ifpi;
	struct sockaddr sa;

	dbg_printf("(%s), interface is %s, index: %d \n", __FUNCTION__, ifp->igmpi_name, ifp->igmpi_index);
	/* Read the igmp message */
	iov.iov_base = (char *)ifp->igmpi_buf;
	iov.iov_len = ifp->igmpi_bufsize;
	msg.msg_iov = &iov;
	msg.msg_iovlen= 1;
	msg.msg_control = ctrl;
	msg.msg_controllen = MAXCTRLSIZE;
	msg.msg_name = &sa;
	msg.msg_namelen = sizeof(struct sockaddr);

	//n=recvmsg(ifp->igmpi_socket,&msg,MSG_WAITALL);
	n=recvmsg(ifp->igmpi_socket,&msg,0); //Joshua
	if (n <= sizeof(*iph)) {
		printf("^^^^^^^ igmprt_input:BAD packet received n=%d \n",n);
		return;
	}
	//kernel: ip_sockglue.c
	//ip_cmsg_send(struct msghdr *msg,struct ipcm_cookie *ipc);
	//
	
	for(cmsg=CMSG_FIRSTHDR(&msg); cmsg != NULL;cmsg =CMSG_NXTHDR(&msg,cmsg)) {
		if (cmsg->cmsg_type == IP_PKTINFO){
			info = (struct in_pktinfo *)CMSG_DATA(cmsg);
			if_index = info->ipi_ifindex;
			dbg_printf("if_index is %d\n", if_index);
			if (if_index != ifp->igmpi_index) // Joshua
				return;
		}
		else  {
			printf("^^^^ BAD CMSG_HDR ^^^^^^^^\n");
			badmsg=1;
		}
	}
	free(ctrl);
	if ( badmsg ) {
		downstream->igmpi_ti_qi=10;
		downstream->igmpi_isquerier=TRUE;
		return;
	}
	/* Set pointer to start of report */
	iph = (struct ip*) ifp->igmpi_buf;
	if ((igmplen = n - (iph->ip_hl << 2)) < IGMP_MINLEN)
		return;
    //src=iph->ip_src; // marked by Joshua
	memcpy(&src,&iph->ip_src,sizeof(struct in_addr)); // Added by Joshua

 	ptype = ifp->igmpi_buf + (iph->ip_hl << 2);
	/*lookup the network interface from which the packet arrived*/
	ifpi = igmprt_interface_lookup_index(igmprt,if_index);
	if (ifpi == NULL)
		return;
	dbg_printf("(%s->%s(%d)) ifpi is %s\n",__FILE__, __FUNCTION__, __LINE__, ifpi->igmpi_name);

	/* Handle the message */
	switch (*ptype) {
	case IGMP_MEMBERSHIP_QUERY:
  		dbg_printf("source %s len=%d\n",(char *)inet_ntoa(src),igmplen);
		query = (igmpv2q_t *)(ifp->igmpi_buf + (iph->ip_hl << 2));
		if (query->igmpq_code == 0){
			/*version 1 query*/
			receive_membership_query(ifpi,query->igmpq_group,NULL,src.s_addr,0,srsp);
		}else if (igmplen == 8){
			/*version 2 query*/
			receive_membership_query(ifpi,query->igmpq_group,NULL,src.s_addr,0,TRUE);
		}else if (igmplen >= 12){
			/*version 3 query*/
			//srsp=IGMP_SRSP(query);
			//receive_membership_query(ifpi,query->igmpq_group,query->igmpq_sources,src.s_addr,query->igmpq_numsrc,srsp);
		}
		break;
	case IGMP_V1_MEMBERSHIP_REPORT:
	case IGMP_V2_MEMBERSHIP_REPORT:

		dbg_printf("(%s->%s(%d)) get v1 v2 membership report, src is 0x%08x\n",__FILE__, __FUNCTION__, __LINE__, src.s_addr);
		if (ifpi->igmpi_type == UPSTREAM)
			return;
		igmp_interface_membership_report_v12(igmprt,ifpi,src, (igmpr_t*) ptype, igmplen);
		igmp_info_print(&router);  /* but this more beautiful*/		
		break;
	case IGMP_V3_MEMBERSHIP_REPORT:
		dbg_printf("get v3 membership report\n");
	       break;
	case IGMP_V2_MEMBERSHIP_LEAVE:
		dbg_printf("get v2 membership leave\n");

		if (ifpi->igmpi_type != UPSTREAM) {
			igmp_interface_membership_leave_v2(igmprt,ifpi,src, (igmpr_t*) ptype, igmplen);
			igmp_info_print(&router);  /* Added by Joshua */
		}
		break;
	default:
		dbg_printf("get unknown\n");
		break;
	}
}

/*
 * void* igmprt_input_thread(void* arg)
 *
 * Wait on all interfaces for packets to arrive
 * igmp_router_t router;
 */
void*
igmprt_input_thread(void* arg)
{
	igmp_router_t* igmprt = (igmp_router_t*) arg;
	igmp_interface_t* ifp;
	fd_set allset, rset;
	int n, maxfd;
  	struct timeval to;

	/* Add the sockets from all interfaces to the set */
	FD_ZERO(&allset);
	FD_ZERO(&rset);
	maxfd = 0;
	for (ifp = igmprt->igmprt_interfaces; ifp; ifp = ifp->igmpi_next) {
		if ( ifp->igmpi_type == DOWNSTREAM) {
		  //dbg_printf("add to select: DOWNSTREAM: %s\n", ifp->igmpi_name);
		  	FD_SET(ifp->igmpi_socket, &allset);
		  	if (maxfd < ifp->igmpi_socket)
				maxfd = ifp->igmpi_socket;
		}
	}
	if (maxfd == 0) {
    	/*syslog(LOG_NOTICE,"igmp:no interface available\n");
		dbg_printf("igmp:no interface available\n");*/
		return NULL;
	}
	/* Wait for data on all sockets */
	memcpy(&rset,&allset,sizeof(rset));
	//bcopy(&allset, &rset,sizeof(rset));

  	to.tv_sec = 0;
  	to.tv_usec = 0;
	n = select(maxfd+1, &rset, NULL, NULL, &to);	
	//n--;
	//dbg_printf("igmprt_input_thread: select return n=%d\n",n);
		for (ifp = igmprt->igmprt_interfaces; ifp; ifp = ifp->igmpi_next) {
		  if ( ifp->igmpi_type == DOWNSTREAM) {
			if (FD_ISSET(ifp->igmpi_socket, &rset))
				igmprt_input(igmprt, ifp);
			if (--n == 0)
				break;
		} 
	} 
	return NULL;
}

/*
 * void igmprt_start(igmp_router_t* igmprt)
 *
 * Start the threads of this router 
 */
void
igmprt_start(igmp_router_t* igmprt)
{
	/* Return if already running */
	if (igmprt->igmprt_running)
		return;

	/* Create and start the timer handling (thread) */
	igmprt->igmprt_flag_timer = 1;

	/* Create and start input handling (thread) */
	igmprt->igmprt_flag_input = 1;
	
	igmprt->igmprt_running = 1;
		
}

/*
 * void igmprt_stop(igmp_router_t* igmprt)
 *
 * Stop the threads of this router 
 */
void
igmprt_stop(igmp_router_t* igmprt)
{

	/* Return if not running */ 
	if (! igmprt->igmprt_running)
		return;

	//k_stop_proxy(igmprt->igmprt_socket);

	/* Signal threads to stop */
	igmprt->igmprt_flag_timer = 0;
	igmprt->igmprt_flag_input = 0;

	/* Wait for the threads to finish */
	/* XXX Should use attach */ 
	igmprt->igmprt_running = 0;
	/* Make sure select of  input-thread wakes up */	
	/*if ((ifp = igmprt->igmprt_interfaces))
		write(ifp->igmpi_socket, &i, sizeof(i));*/
}

/*
 * void igmprt_print()
 *
 * Print the status of the igmpv3 proxy/router
 */
void
igmprt_print(igmp_router_t* igmprt)
{
	igmp_interface_t* ifp;
	dbg_printf("enter igmprt_print\n");
	assert(igmprt != NULL);
	for (ifp = igmprt->igmprt_interfaces; ifp; ifp = ifp->igmpi_next)
		igmp_interface_print(ifp);
}

/******************************************************************************
 *
 * The application
 *
 *****************************************************************************/

#define BUF_CMD	100

int go_on = 1;
char* pidfile = "/var/run/igmp_pid";

void
write_pid()
{
	FILE *fp = fopen(pidfile, "w+");
	if (fp) {
		fprintf(fp, "%d\n", getpid());
		fclose(fp);
	}
	else
	 	printf("Cannot create pid file\n");
}

void
done(int sig)
{
	igmprt_destroy(&router);
	unlink(pidfile);
	exit(0);
}

/***************************************************************************
// Function Name: bcmSystem().
// Description  : launch shell command in the child process.
// Parameters   : command - shell command to launch.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmSystemEx (char *command, int printFlag) {
   int pid = 0, status = 0;

   if ( command == 0 )
      return 1;

   pid = fork();
   if ( pid == -1 )
      return -1;

   if ( pid == 0 ) {
      char *argv[4];
      argv[0] = "sh";
      argv[1] = "-c";
      argv[2] = command;
      argv[3] = 0;
#ifdef BRCM_DEBUG
      if (printFlag)
         printf("app: %s\r\n", command);
#endif
      //if (printFlag)
        //syslog(LOG_DEBUG, command);
      //execve("/bin/sh", argv, environ);
      execve("/bin/sh", argv, NULL);
      exit(127);
   }

   /* wait for child process return */
   do {
      if ( waitpid(pid, &status, 0) == -1 ) {
         if ( errno != EINTR )
            return -1;
      } else
         return status;
   } while ( 1 );

   return status;
}

void ProcessExistProcedure (void)
{
	pid_t	pid = 0;
	int status = 0 , i = 0;

aaa:	
	if ( fork () == 0 ) 
	{
		fprintf(stderr, "IGMP, This is the child process, pid = %d\n" , getpid () );
	} 
	else 
	{
		fprintf(stderr, "IGMP, This is the parent process, pid = %d\n" , getpid () );
		pid = wait ( &status );
		i = WEXITSTATUS ( status );
		//sleep ( 10 );
		fprintf(stderr, "child's pid = %d , exit status = %d\n" , pid , i );	
		fprintf(stderr, "\n\n ***************************************\n\n" );
		goto aaa;
	}
} 

int main (int argc,char **argv)
{
	struct sockaddr_in *psin;
	interface_list_t *ifl, *ifp;
	vifi_t vifi;
	//struct sigaction sa;
	igmp_interface_t *ifc;
	int i;
	printf("Run igmp_proxy\n");

	//ProcessExistProcedure();
	
	write_pid();
	//printf("pid=%d \n",getpid());
	argc--; /* skip igmp */
	argv++;
	for (i=0; i<argc; i++) {
	   strcpy(upstream_interface[i],*argv++);	
	}
        //openlog("igmp",LOG_PID|LOG_NDELAY,LOG_USER);

        //syslog(LOG_NOTICE,"igmp started!\n");
	// a hack to wait for all ppp up

	//sleep(6);

	/* Initialize */
	signal(SIGUSR1, done);
	signal(SIGKILL, done);
	signal(SIGABRT, done);
	signal(SIGTERM, done);
	signal(SIGHUP, done);
	//sa.sa_handler = igmprt_timer_thread;
	//sa.sa_flags = SA_RESTART;
	//sigaction(SIGALRM, &sa,NULL);
	querier=1;
	version=22;
	/* Create and initialize the router */
	igmprt_init(&router);
	//k_init_proxy(((igmp_router_t *) &router)->igmprt_socket);
	numvifs = 0;
	/* Add all the multicast enabled ipv4 interfaces */
	ifl = get_interface_list(AF_INET, IFF_MULTICAST, IFF_LOOPBACK);
	for (vifi=0,ifp=ifl;ifp;ifp=ifp->ifl_next,vifi++) {
		psin = (struct sockaddr_in*) &ifp->ifl_addr;
		igmprt_interface_add(&router, psin->sin_addr, ifp->ifl_name,ifp->ifl_index);
		k_proxy_add_vif(((igmp_router_t *) &router)->igmprt_socket,psin->sin_addr.s_addr,ifp->ifl_index);
		numvifs++;
	}
	//free_interface_list(ifl); // Marked by Joshua
	/* Print the status of the router */
	igmprt_print(&router);
	/* Start the router */
	igmprt_start(&router);
	//system("/usr/sbin/iptables -I INPUT 1 -p 2 -j ACCEPT");
	for (ifc=(&router)->igmprt_interfaces;ifc;ifc=ifc->igmpi_next){
		if ( ifc->igmpi_type == DOWNSTREAM ) {
			downstream = ifc;
			break; // Added by Joshua
		}
	}
       while (go_on) {
		igmprt_timer_thread(&router);
	  	igmprt_input_thread(&router);
	}

	/* Done */
	free_interface_list(ifl); // Added by Joshua

	done(0);
	return 0;
}
