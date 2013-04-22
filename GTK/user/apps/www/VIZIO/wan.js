function SplitWanDNS0 ( IPString )
{
	document.forms[0].wan_dns0_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].wan_dns0_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].wan_dns0_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].wan_dns0_4.value = GetIPSegment ( IPString , 4 );
}

function SplitWanDNS1 ( IPString )
{
	document.forms[0].wan_dns1_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].wan_dns1_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].wan_dns1_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].wan_dns1_4.value = GetIPSegment ( IPString , 4 );
}

function SplitWanDNS2 ( IPString )
{
	document.forms[0].wan_dns2_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].wan_dns2_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].wan_dns2_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].wan_dns2_4.value = GetIPSegment ( IPString , 4 );
}

function SplitWanSubnetMask ( IPString )
{
	document.forms[0].wan_netmask_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].wan_netmask_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].wan_netmask_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].wan_netmask_4.value = GetIPSegment ( IPString , 4 );
}

function SplitWanGateway ( IPString )
{
	document.forms[0].wan_gateway_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].wan_gateway_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].wan_gateway_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].wan_gateway_4.value = GetIPSegment ( IPString , 4 );
}
function AssembleWanIPAddress ()
{
	if ((document.forms[0].wan_ipaddr_1.value == "") && (document.forms[0].wan_ipaddr_2.value == "") &&(document.forms[0].wan_ipaddr_3.value == "") &&(document.forms[0].wan_ipaddr_4.value == ""))
	{
		alert(translate_str("JavaScript",62));
		return 0;		
	}	
	
	if ((document.forms[0].wan_ipaddr_1.value == 0) &&
	    (document.forms[0].wan_ipaddr_2.value == 0) &&
		(document.forms[0].wan_ipaddr_3.value == 0) &&
	    (document.forms[0].wan_ipaddr_4.value == 0)	    
	    )
	{
		alert(translate_str("JavaScript",2));
		return 0;		
	}		    
	
	
	if ( !(IPFirstCheck(document.forms[0].wan_ipaddr_1)) ) 
	{
    		document.forms[0].wan_ipaddr_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].wan_ipaddr_2)) ) 
	{
    		document.forms[0].wan_ipaddr_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].wan_ipaddr_3)) ) 
	{
    		document.forms[0].wan_ipaddr_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck(document.forms[0].wan_ipaddr_4)) ) 
	{
    		document.forms[0].wan_ipaddr_4.value = 0;
    		return false;
  	}

	d = (document.forms[0].wan_ipaddr_1.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_ipaddr_2.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_ipaddr_3.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_ipaddr_4.value * 1 );

	document.forms[0].wan_ipaddr.value = d;
	return true;
}

function AssemblePPTPServerIPAddress ()
{
	if ((document.forms[0].pptp_server_ipaddr_1.value == "") && (document.forms[0].pptp_server_ipaddr_2.value == "") &&(document.forms[0].pptp_server_ipaddr_3.value == "") &&(document.forms[0].pptp_server_ipaddr_4.value == ""))
	{
   alert(translate_str("JavaScript",62));
		return 0;		
	}	
	
	if ((document.forms[0].pptp_server_ipaddr_1.value == 0) &&
	    (document.forms[0].pptp_server_ipaddr_2.value == 0) &&
		(document.forms[0].pptp_server_ipaddr_3.value == 0) &&
	    (document.forms[0].pptp_server_ipaddr_4.value == 0)	    
	    )
	{
		alert(translate_str("JavaScript",2));		
		return 0;		
	}		    
	
	if ( !(IPFirstCheck(document.forms[0].pptp_server_ipaddr_1)) ) 
	{
    		document.forms[0].pptp_server_ipaddr_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].pptp_server_ipaddr_2)) ) 
	{
    		document.forms[0].pptp_server_ipaddr_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].pptp_server_ipaddr_3)) ) 
	{
    		document.forms[0].pptp_server_ipaddr_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck(document.forms[0].pptp_server_ipaddr_4)) ) 
	{
    		document.forms[0].pptp_server_ipaddr_4.value = 0;
    		return false;
  	}

	d = (document.forms[0].pptp_server_ipaddr_1.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_server_ipaddr_2.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_server_ipaddr_3.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_server_ipaddr_4.value * 1 );

	document.forms[0].pptp_server.value = d;
	return true;
}

function AssemblePPTPLocalIPAddress ()
{
	if ((document.forms[0].pptp_local_ipaddr_1.value == "") && (document.forms[0].pptp_local_ipaddr_2.value == "") &&(document.forms[0].pptp_local_ipaddr_3.value == "") &&(document.forms[0].pptp_local_ipaddr_4.value == ""))
	{
   alert(translate_str("JavaScript",62));
		return 0;		
	}	
	
	if ((document.forms[0].pptp_local_ipaddr_1.value == 0) &&
	    (document.forms[0].pptp_local_ipaddr_2.value == 0) &&
		(document.forms[0].pptp_local_ipaddr_3.value == 0) &&
	    (document.forms[0].pptp_local_ipaddr_4.value == 0)	    
	    )
	{
		alert(translate_str("JavaScript",2));		
		return 0;		
	}		    
	
	if ( !(IPFirstCheck(document.forms[0].pptp_local_ipaddr_1)) ) 
	{
    		document.forms[0].pptp_local_ipaddr_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].pptp_local_ipaddr_2)) ) 
	{
    		document.forms[0].pptp_local_ipaddr_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].pptp_local_ipaddr_3)) ) 
	{
    		document.forms[0].pptp_local_ipaddr_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck(document.forms[0].pptp_local_ipaddr_4)) ) 
	{
    		document.forms[0].pptp_local_ipaddr_4.value = 0;
    		return false;
  	}

	d = (document.forms[0].pptp_local_ipaddr_1.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_local_ipaddr_2.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_local_ipaddr_3.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_local_ipaddr_4.value * 1 );

	document.forms[0].wan_subipaddr.value = d;
	return true;
}

function AssembleWanSubnetMask ()
{
	if ((document.forms[0].wan_netmask_1.value == "") && (document.forms[0].wan_netmask_2.value == "") &&(document.forms[0].wan_netmask_3.value == "") &&(document.forms[0].wan_netmask_4.value == ""))
	{
		document.forms[0].wan_netmask.value = "0.0.0.0";						
		return 1;
	}	
	if ((document.forms[0].wan_netmask_1.value == "0") && (document.forms[0].wan_netmask_2.value == "0") &&(document.forms[0].wan_netmask_3.value == "0") &&(document.forms[0].wan_netmask_4.value == "0"))
	{
		document.forms[0].wan_netmask.value = "0.0.0.0";	
		document.forms[0].wan_netmask_1.focus();
		alert(translate_str("JavaScript",127));
		return false;
	}	
  d = document.forms[0].wan_netmask_1.value;
  if ( d>255 || d<0 || ( document.forms[0].wan_netmask_1.value == "" ) || !(IPCheck1(document.forms[0].wan_netmask_1)) ) {
   		document.forms[0].wan_netmask_1.value = 0;
   		return false;
  }
  	
	d = document.forms[0].wan_netmask_2.value;
  if ( d>255 || d<0 || ( document.forms[0].wan_netmask_2.value == "" ) || !(IPCheck1(document.forms[0].wan_netmask_2)) ) {
  		document.forms[0].wan_netmask_2.value = 0;
			return false;
  }

	d = document.forms[0].wan_netmask_3.value;
  if ( d>255 || d<0 || ( document.forms[0].wan_netmask_3.value == "" ) || !(IPCheck1(document.forms[0].wan_netmask_3)) ) {
   		document.forms[0].wan_netmask_3.value = 0;
   		return false;
  }

	d = document.forms[0].wan_netmask_4.value;
  if ( d>255 || d<0 || ( document.forms[0].wan_netmask_4.value == "" ) || !(IPCheck1(document.forms[0].wan_netmask_4)) ) {
   		document.forms[0].wan_netmask_4.value = 0;
   		return false;
  }

  d = (document.forms[0].wan_netmask_1.value * 1 ) + '.';
  d = d + (document.forms[0].wan_netmask_2.value * 1 ) + '.';
  d = d + (document.forms[0].wan_netmask_3.value * 1 ) + '.';
  d = d + (document.forms[0].wan_netmask_4.value * 1 );
	
	if (!CheckSubnetRule(d))	{
		alert(translate_str("JavaScript",127));
		return false;
	}
  document.forms[0].wan_netmask.value = d;
  return true;
}

function AssemblePPTPSubnetMask ()
{
	if ((document.forms[0].pptp_local_netmask_1.value == "") && (document.forms[0].pptp_local_netmask_2.value == "") &&(document.forms[0].pptp_local_netmask_3.value == "") &&(document.forms[0].pptp_local_netmask_4.value == ""))
	{
		document.forms[0].pptp_local_netmask.value = "0.0.0.0";						
		return 1;
	}	
	if ((document.forms[0].pptp_local_netmask_1.value == "0") && (document.forms[0].pptp_local_netmask_2.value == "0") &&(document.forms[0].pptp_local_netmask_3.value == "0") &&(document.forms[0].pptp_local_netmask_4.value == "0"))
	{
		document.forms[0].pptp_local_netmask.value = "0.0.0.0";	
		document.forms[0].pptp_local_netmask_1.focus();
		alert(translate_str("JavaScript",127));				
		return false;
	}	
  	d = document.forms[0].pptp_local_netmask_1.value;
  	if ( d>255 || d<0 || ( document.forms[0].pptp_local_netmask_1.value == "" ) || !(IPCheck1(document.forms[0].pptp_local_netmask_1)) ) {
    		document.forms[0].pptp_local_netmask_1.value = 0;
    		return false;
  	}
  	
	d = document.forms[0].pptp_local_netmask_2.value;
  	if ( d>255 || d<0 || ( document.forms[0].pptp_local_netmask_2.value == "" ) || !(IPCheck1(document.forms[0].pptp_local_netmask_2)) ) {
    		document.forms[0].pptp_local_netmask_2.value = 0;
			return false;
  	}

	d = document.forms[0].pptp_local_netmask_3.value;
  	if ( d>255 || d<0 || ( document.forms[0].pptp_local_netmask_3.value == "" ) || !(IPCheck1(document.forms[0].pptp_local_netmask_3)) ) {
    		document.forms[0].pptp_local_netmask_3.value = 0;
    		return false;
  	}

	d = document.forms[0].pptp_local_netmask_4.value;
  	if ( d>255 || d<0 || ( document.forms[0].pptp_local_netmask_4.value == "" ) || !(IPCheck1(document.forms[0].pptp_local_netmask_4)) ) {
    		document.forms[0].pptp_local_netmask_4.value = 0;
    		return false;
  	}

  	d = (document.forms[0].pptp_local_netmask_1.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_local_netmask_2.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_local_netmask_3.value * 1 ) + '.';
  	d = d + (document.forms[0].pptp_local_netmask_4.value * 1 );
	
	if (!CheckSubnetRule(d))	{
		alert(translate_str("JavaScript",127));
		return false;
	}
  	document.forms[0].wan_subnetmask.value = d;
  	return true;
}

function AssembleWanGateway ()
{
	if ((document.forms[0].wan_gateway_1.value == "") && (document.forms[0].wan_gateway_2.value == "") &&(document.forms[0].wan_gateway_3.value == "") &&(document.forms[0].wan_gateway_4.value == ""))
	{
		document.forms[0].wan_gateway.value = "0.0.0.0";
		return 1;
	}	
  	d = document.forms[0].wan_gateway_1.value;
  	if ( d>255 || d<0 || ( document.forms[0].wan_gateway_1.value == "" ) || !(IPFirstCheck(document.forms[0].wan_gateway_1)) ) {
    		document.forms[0].wan_gateway_1.value = 0;
    		return false;
  	}
  	
	d = document.forms[0].wan_gateway_2.value;
  	if ( d>255 || d<0 || ( document.forms[0].wan_gateway_2.value == "" ) || !(IPCheck1(document.forms[0].wan_gateway_2)) ) {
    		document.forms[0].wan_gateway_2.value = 0;
    		return false;
  	}

	d = document.forms[0].wan_gateway_3.value;
  	if ( d>255 || d<0 || ( document.forms[0].wan_gateway_3.value == "" ) || !(IPCheck1(document.forms[0].wan_gateway_3)) ) {
    		document.forms[0].wan_gateway_3.value = 0;
    		return false;
  	}

	d = document.forms[0].wan_gateway_4.value;
  	if ( d>255 || d<0 || ( document.forms[0].wan_gateway_4.value == "" ) || !(IPCheck(document.forms[0].wan_gateway_4)) ) {
    		document.forms[0].wan_gateway_4.value = 0;
    		return false;
  	}

  	d = (document.forms[0].wan_gateway_1.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_gateway_2.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_gateway_3.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_gateway_4.value * 1 );

  	document.forms[0].wan_gateway.value = d;
  	return true;
}
function AssembleWanDNS0 ()
{
	if ((document.forms[0].wan_dns0_1.value == "") && (document.forms[0].wan_dns0_2.value == "") &&(document.forms[0].wan_dns0_3.value == "") &&(document.forms[0].wan_dns0_4.value == ""))
	{
		alert(translate_str("JavaScript",143));
		document.forms[0].wan_dns0_1.focus();
    return false;
		//document.forms[0].wan_dns1.value = "0.0.0.0";
	}
	else {
	if ( !(IPFirstCheck(document.forms[0].wan_dns0_1)) ) {
    		document.forms[0].wan_dns0_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].wan_dns0_2)) ) {
    		document.forms[0].wan_dns0_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].wan_dns0_3)) ) {
    		document.forms[0].wan_dns0_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck2(document.forms[0].wan_dns0_4)) ) {
    		document.forms[0].wan_dns0_4.value = 0;
    		return false;
  	}
	}
	d = (document.forms[0].wan_dns0_1.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns0_2.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns0_3.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns0_4.value * 1 );

	document.forms[0].wan_dns1.value = d;
	return true;
}

function AssembleWanDNS1 ()
{
	if ((document.forms[0].wan_dns1_1.value == "") && (document.forms[0].wan_dns1_2.value == "") &&(document.forms[0].wan_dns1_3.value == "") &&(document.forms[0].wan_dns1_4.value == ""))
	{
		document.forms[0].wan_dns2.value = "0.0.0.0";
	}
	else	{
		
	if ( !(IPFirstCheck(document.forms[0].wan_dns1_1)) ) {
    		document.forms[0].wan_dns1_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].wan_dns1_2)) ) {
    		document.forms[0].wan_dns1_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].wan_dns1_3)) ) {
    		document.forms[0].wan_dns1_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck2(document.forms[0].wan_dns1_4)) ) {
    		document.forms[0].wan_dns1_4.value = 0;
    		return false;
  	}
	}
	d = (document.forms[0].wan_dns1_1.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns1_2.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns1_3.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns1_4.value * 1 );

	document.forms[0].wan_dns2.value = d;
	return true;
}
function AssembleWanDNS2 ()
{
	if ((document.forms[0].wan_dns2_1.value == "") && (document.forms[0].wan_dns2_2.value == "") &&(document.forms[0].wan_dns2_3.value == "") &&(document.forms[0].wan_dns2_4.value == ""))
	{
		document.forms[0].wan_dns3.value = "0.0.0.0";
	}
	else	{
	if ( !(IPFirstCheck(document.forms[0].wan_dns2_1)) ) {
    		document.forms[0].wan_dns2_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].wan_dns2_2)) ) {
    		document.forms[0].wan_dns2_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].wan_dns2_3)) ) {
    		document.forms[0].wan_dns2_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck2(document.forms[0].wan_dns2_4)) ) {
    		document.forms[0].wan_dns2_4.value = 0;
    		return false;
  	}
	}
	d = (document.forms[0].wan_dns2_1.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns2_2.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns2_3.value * 1 ) + '.';
  	d = d + (document.forms[0].wan_dns2_4.value * 1 );

	document.forms[0].wan_dns3.value = d;
	return true;
}

function SplitWanIPAddress ( IPString )
{
	document.forms[0].wan_ipaddr_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].wan_ipaddr_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].wan_ipaddr_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].wan_ipaddr_4.value = GetIPSegment ( IPString , 4 );
}

function SplitPPTPServerIPAddress ( IPString )
{
	document.forms[0].pptp_server_ipaddr_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].pptp_server_ipaddr_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].pptp_server_ipaddr_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].pptp_server_ipaddr_4.value = GetIPSegment ( IPString , 4 );
}

function SplitPPTPLocalIPAddress ( IPString )
{
	document.forms[0].pptp_local_ipaddr_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].pptp_local_ipaddr_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].pptp_local_ipaddr_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].pptp_local_ipaddr_4.value = GetIPSegment ( IPString , 4 );
}

function SplitPPTPSubnetMask ( IPString )
{
	document.forms[0].pptp_local_netmask_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].pptp_local_netmask_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].pptp_local_netmask_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].pptp_local_netmask_4.value = GetIPSegment ( IPString , 4 );
}
function CheckSubnetRule(subnet)
{
	var subnetarray=subnet.split('.');
	var i;
	var zeroflag=0;
	var d ;

	for(i=0;i<4;i++)
	{
		d=parseInt(subnetarray[i], 10);

		if(zeroflag==1)
		{
			if(d != 0)
				return false;
		}

		if(d != 255)
		{
			zeroflag=1;
			if(d != 254 && d != 252 && d != 248 && d != 240 && d != 224 && d != 192 && d != 128 && d != 0)
			{
				return false;
			}
		}
	}

	return true;

}

function SubnetCheck(ipaddr, netmask, gateway)
{
	 sn_octs = netmask.value.split('.'); 
   ref_octs = ipaddr.value.split('.');
   global_ip_octs = gateway.value.split('.');    
   global_ip_bin= (global_ip_octs[3]-0)+(256*(global_ip_octs[2]-0))+(256*256*(global_ip_octs[1]-0))+(256*256*256*(global_ip_octs[0]-0)); 
	 sn_bin=(sn_octs[3]-0)+(256*(sn_octs[2]-0))+(256*256*(sn_octs[1]-0))+(256*256*256*(sn_octs[0]-0));
   ref_bin=(ref_octs[3]-0)+(256*(ref_octs[2]-0))+(256*256*(ref_octs[1]-0))+(256*256*256*(ref_octs[0]-0));
	
	nwaddress = ref_bin & sn_bin;
	compare = global_ip_bin & sn_bin;
	if (nwaddress != compare)	{
		alert(translate_str("JavaScript",128));
		return 0;
	}
	else if (nwaddress == compare)	{
		return 1;
	}
}

function reboot(f)
{
	if(!confirm(translate_str("JavaScript",130)))
		return;
//	f.data1.value="";
//	f.redirect_timer.value="50000";
//	f.reboot.value=1;
	f.submit();
}