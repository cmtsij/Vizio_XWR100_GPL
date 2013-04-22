LanDHCPURL = new Array("DHCPDisable.htm", "DHCPEnable.htm", "DHCPRelay.htm");

function SelLanDHCP(obj)
{
	n = obj.selectedIndex;
	parent.LanDHCP.location.href = LanDHCPURL[n];	
}

function FillDhcpReleaseTimeIn ()
{
	var	d = document.forms[0].DhcpReleaseTime.value;

	if ( d == '' ) { document.forms[0].DhcpReleaseTime.value = '0'; }
	else { document.forms[0].DhcpReleaseTime.value = ( parseInt ( d , 10 ) ); }
}

function ValidatingDhcpReleaseTime ()
{
	var	d = parseInt ( document.forms[0].Time.value , 10 );

	if (isNaN(d)) {
		alert(translate_str("JavaScript",53));
		document.forms[0].Time.focus();		
		return 0;
	}
	if ( (d < 0) || (d > 86400) ) {
    		document.forms[0].Time.value = '0';
		alert(translate_str("JavaScript",54) + ' [0 - 86400]');
		document.forms[0].Time.focus();
    		return false;
	}
	return true;
}

function SplitDhcpStartEnd ( IPStringStart, IPStringEnd)
{
	document.forms[0].StartIP.value = GetIPSegment ( IPStringStart , 4 );
	document.forms[0].EndIP.value = GetIPSegment ( IPStringEnd , 4 );

	return true;
}

var	LanIPAddress_1 = GetIPSegment ( '<%nvram_get("lan_ipaddr");%>' , 1 );
var	LanIPAddress_2 = GetIPSegment ( '<%nvram_get("lan_ipaddr");%>' , 2 );
var	LanIPAddress_3 = GetIPSegment ( '<%nvram_get("lan_ipaddr");%>' , 3 );

function AssembleDhcpStartEndAddress ()
{
	if ( !(IPCheck(document.forms[0].StartIP)) ) 
	{
		document.forms[0].StartIP.value = 0;
   		return false;
  	}
	
	if ( !(IPCheck(document.forms[0].EndIP)) ) 
	{
    		document.forms[0].EndIP.value = 0;
   		return false;
  	}  	

	document.forms[0].dhcp_start.value = LanIPAddress_1 + '.' +
				        LanIPAddress_2 + '.' +
				        LanIPAddress_3 + '.' +
				        (document.forms[0].StartIP.value * 1);
	document.forms[0].dhcp_end.value = LanIPAddress_1 + '.' +
				        LanIPAddress_2 + '.' +
				        LanIPAddress_3 + '.' +
				        (document.forms[0].EndIP.value * 1 );				        
	return true;
}

function GettingNumOfUser ()
{
	var DhcpEnd =  parseInt(document.forms[0].dhcp_end.value,10);
	var DhcpStart = parseInt(document.forms[0].dhcp_start.value,10);
	document.forms[0].NumOfUsers.value = parseInt ( DhcpEnd , 10 ) - parseInt (DhcpStart , 10 ) + 1;
}

function AssembleDhcpStartAddress (f)
{
	if ( !(IPCheck(document.forms[0].StartIP)) ) {
    		document.forms[0].StartIP.value = 0;
    		return false;
  	}
	document.forms[0].dhcp_start.value = document.forms[0].StartIP.value;
	return true;
}

function AssembleDhcpEndAddress (f)
{
	var netmask = parseInt(GetIPSegment (f.sel_netmask.value, 4));
	var client = 255-netmask;
	var localip = parseInt(f.lan_ipaddr_4.value);
	var net_start = (netmask & localip) + 1;
	var net_end = (net_start + client) - 1;
	var startIP = f.StartIP.value;	
	var	d = parseInt ( document.forms[0].NumOfUsers.value , 10 );
	var tmp = parseInt(startIP) + d;

	if (isNaN(d)) {
		alert(translate_str("JavaScript",53));
		document.forms[0].NumOfUsers.focus();
		return 0;
	}
//Willes add 20090225	
	if(tmp > net_end)
	{
		document.forms[0].NumOfUsers.value = 0;
		alert(translate_str("JavaScript",56) + ' [1 - ' + net_end + ']');
		document.forms[0].NumOfUsers.focus();
    	return false;
	}

/*	
	if ( ( d <= 0 ) || ( d > 255 ) ) {
		document.forms[0].NumOfUsers.value = 0;
		alert(translate_str("JavaScript",56) + ' [1 - 255]');
		document.forms[0].NumOfUsers.focus();
    		return false;
	}
*/
	
//	d = parseInt ( document.forms[0].NumOfUsers.value , 10 ) + parseInt ( document.forms[0].StartIP.value , 10 ) - 1;
//	if (d > 254)
//		d = 254;
	d = tmp - 1;
	if(d < startIP)
		d = parseInt(startIP);
	document.forms[0].dhcp_end.value = d;
//20090225
	return true;
}


function ShowDHCPRange(f)
{
	dw(document.forms[0].dhcp_start.value + " ~ ");
	dw(GetIPSegment(document.forms[0].dhcp_end.value, 4));
}

function SplitDHCPDNS1Address ( IPString )
{
	document.forms[0].DHCP_DNS1_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].DHCP_DNS1_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].DHCP_DNS1_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].DHCP_DNS1_4.value = GetIPSegment ( IPString , 4 );
	return true;
}

function SplitDHCPDNS2Address ( IPString )
{
	document.forms[0].DHCP_DNS2_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].DHCP_DNS2_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].DHCP_DNS2_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].DHCP_DNS2_4.value = GetIPSegment ( IPString , 4 );
	return true;
}

function SplitDHCPDNS3Address ( IPString )
{
	document.forms[0].DHCP_DNS3_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].DHCP_DNS3_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].DHCP_DNS3_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].DHCP_DNS3_4.value = GetIPSegment ( IPString , 4 );
	return true;
}

function SplitDHCPWINSAddress ( IPString )
{
	document.forms[0].WINS_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].WINS_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].WINS_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].WINS_4.value = GetIPSegment ( IPString , 4 );
	return true;
}
function AssembleDHCPDNS1Address ()
{
	if (document.forms[0].DHCP_DNS1_1.value == 0 && document.forms[0].DHCP_DNS1_2.value == 0 && document.forms[0].DHCP_DNS1_3.value == 0 && document.forms[0].DHCP_DNS1_4.value == 0)
		d = "0.0.0.0";	
	else {
		if ( !(IPCheck(document.forms[0].DHCP_DNS1_1)) ) {
	    		document.forms[0].DHCP_DNS1_1.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck2(document.forms[0].DHCP_DNS1_2)) ) {
	    		document.forms[0].DHCP_DNS1_2.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck2(document.forms[0].DHCP_DNS1_3)) ) {
	    		document.forms[0].DHCP_DNS1_3.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck(document.forms[0].DHCP_DNS1_4)) ) {
	    		document.forms[0].DHCP_DNS1_4.value = 0;
	    		return false;
	  	}
		if ((document.forms[0].DHCP_DNS1_1.value==255) &&
		    (document.forms[0].DHCP_DNS1_2.value==255) &&
		    (document.forms[0].DHCP_DNS1_3.value==255) &&
		    (document.forms[0].DHCP_DNS1_4.value==255)
		   )
		{
			alert(translate_str("JavaScript",3));
			document.forms[0].DHCP_DNS1_1.focus();
			return 0;
		}  	
		if ((document.forms[0].DHCP_DNS1_4.value==0) ||
			(document.forms[0].DHCP_DNS1_4.value==255)
			)
		{
			alert(translate_str("JavaScript",4));
			document.forms[0].DHCP_DNS1_4.focus();
			return 0;
		}
		d = (document.forms[0].DHCP_DNS1_1.value)* 1 + '.';
	  	d = d + (document.forms[0].DHCP_DNS1_2.value)* 1 + '.';
	  	d = d + (document.forms[0].DHCP_DNS1_3.value)* 1 + '.';
	  	d = d + (document.forms[0].DHCP_DNS1_4.value)* 1;
	}
	document.forms[0].lan_dns1.value = d;
	return true;
}

function AssembleDHCPDNS2Address ()
{
	if (document.forms[0].DHCP_DNS2_1.value == 0 && document.forms[0].DHCP_DNS2_2.value == 0 && document.forms[0].DHCP_DNS2_3.value == 0 && document.forms[0].DHCP_DNS2_4.value == 0)
		d = "0.0.0.0";	
	else {
		if ( !(IPCheck(document.forms[0].DHCP_DNS2_1)) ) {
	    		document.forms[0].DHCP_DNS2_1.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck2(document.forms[0].DHCP_DNS2_2)) ) {
	    		document.forms[0].DHCP_DNS2_2.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck2(document.forms[0].DHCP_DNS2_3)) ) {
	    		document.forms[0].DHCP_DNS2_3.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck(document.forms[0].DHCP_DNS2_4)) ) {
	    		document.forms[0].DHCP_DNS2_4.value = 0;
	    		return false;
	  	}
		if ((document.forms[0].DHCP_DNS2_1.value==255) &&
		    (document.forms[0].DHCP_DNS2_2.value==255) &&
		    (document.forms[0].DHCP_DNS2_3.value==255) &&
		    (document.forms[0].DHCP_DNS2_4.value==255)
		   )
		{
			alert(translate_str("JavaScript",3));
			document.forms[0].DHCP_DNS2_1.focus();
			return 0;
		}  	
		if ((document.forms[0].DHCP_DNS2_4.value==0) ||
			(document.forms[0].DHCP_DNS2_4.value==255)
			)
		{
			alert(translate_str("JavaScript",4));
			document.forms[0].DHCP_DNS2_4.focus();
			return 0;
		}
		d = (document.forms[0].DHCP_DNS2_1.value * 1)+ '.';
	  	d = d + (document.forms[0].DHCP_DNS2_2.value * 1)+ '.';
	  	d = d + (document.forms[0].DHCP_DNS2_3.value * 1)+ '.';
	  	d = d + (document.forms[0].DHCP_DNS2_4.value * 1); 
	}
	document.forms[0].lan_dns2.value = d;
	return true;
}
function AssembleDHCPDNS3Address ()
{
	if (document.forms[0].DHCP_DNS3_1.value == 0 && document.forms[0].DHCP_DNS3_2.value == 0 && document.forms[0].DHCP_DNS3_3.value == 0 && document.forms[0].DHCP_DNS3_4.value == 0)
		d = "0.0.0.0";	
	else {
		if ( !(IPCheck(document.forms[0].DHCP_DNS3_1)) ) {
	    		document.forms[0].DHCP_DNS3_1.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck2(document.forms[0].DHCP_DNS3_2)) ) {
	    		document.forms[0].DHCP_DNS3_2.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck2(document.forms[0].DHCP_DNS3_3)) ) {
	    		document.forms[0].DHCP_DNS3_3.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck(document.forms[0].DHCP_DNS3_4)) ) {
	    		document.forms[0].DHCP_DNS3_4.value = 0;
	    		return false;
	  	}
		if ((document.forms[0].DHCP_DNS3_1.value==255) &&
		    (document.forms[0].DHCP_DNS3_2.value==255) &&
		    (document.forms[0].DHCP_DNS3_3.value==255) &&
		    (document.forms[0].DHCP_DNS3_4.value==255)
		   )
		{
			alert(translate_str("JavaScript",3));
			document.forms[0].DHCP_DNS3_1.focus();
			return 0;
		}  	
		if ((document.forms[0].DHCP_DNS3_4.value==0) ||
			(document.forms[0].DHCP_DNS3_4.value==255)
			)
		{
			alert(translate_str("JavaScript",4));
			document.forms[0].DHCP_DNS3_4.focus();
			return 0;
		}
		d = (document.forms[0].DHCP_DNS3_1.value * 1)+ '.';
	  	d = d + (document.forms[0].DHCP_DNS3_2.value * 1)+ '.';
	  	d = d + (document.forms[0].DHCP_DNS3_3.value * 1)+ '.';
	  	d = d + (document.forms[0].DHCP_DNS3_4.value * 1); 
	}
	document.forms[0].lan_dns3.value = d;
	return true;
}
function AssembleDHCPWINSAddress ()
{
	if (document.forms[0].WINS_1.value == 0 && document.forms[0].WINS_2.value == 0 && document.forms[0].WINS_3.value == 0 && document.forms[0].WINS_4.value == 0)
		d = "0.0.0.0";	
	else {
		if ( !(IPCheck(document.forms[0].WINS_1)) ) {
	    		document.forms[0].WINS_1.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck2(document.forms[0].WINS_2)) ) {
	    		document.forms[0].WINS_2.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck2(document.forms[0].WINS_3)) ) {
	    		document.forms[0].WINS_3.value = 0;
	    		return false;
	  	}
	
		if ( !(IPCheck(document.forms[0].WINS_4)) ) {
	    		document.forms[0].WINS_4.value = 0;
	    		return false;
	  	}
		if ((document.forms[0].WINS_1.value==255) &&
		    (document.forms[0].WINS_2.value==255) &&
		    (document.forms[0].WINS_3.value==255) &&
		    (document.forms[0].WINS_4.value==255)
		   )
		{
			alert(translate_str("JavaScript",3));
			document.forms[0].WINS_1.focus();
			return 0;
		}  	
		if ((document.forms[0].WINS_4.value==0) ||
			(document.forms[0].WINS_4.value==255)
			)
		{
			alert(translate_str("JavaScript",4));
			document.forms[0].WINS_4.focus();
			return 0;
		}
		d = (document.forms[0].WINS_1.value * 1) + '.';
	  	d = d + (document.forms[0].WINS_2.value * 1) + '.';
	  	d = d + (document.forms[0].WINS_3.value * 1) + '.';
	  	d = d + (document.forms[0].WINS_4.value * 1);
	}
	document.forms[0].lan_wins.value = d;
	return true;
}
function CheckRealDNS ()
{
	if (document.forms[0].real_dns.checked == true)
		return true;
	else
		return false;
}
function check_clientNumber(f)
{
	var netmask = parseInt(GetIPSegment (f.sel_netmask.value, 4));
	var net_number = 256 / (256 - netmask);
	var client = 255-netmask;
	var localip = parseInt(f.lan_ipaddr_4.value);
	var net_start = (netmask & localip) + 1;
	var net_end = (net_start + client) - 1;

	if (localip < 1 || localip > 254) {
		alert(translate_str("JavaScript",125)+" [1-254]!");
		f.lan_ipaddr_4.focus();
		return false;
	}

	if (localip == (net_start-1)) {
		alert(translate_str("JavaScript",57) + " [" + localip + "]");
		f.lan_ipaddr_4.focus();
		return false;
	}
	if (localip == (net_end)) {
		alert(translate_str("JavaScript",59) + " [" + localip + "]");
		f.lan_ipaddr_4.focus();
		return false;
	}
/*
	if (localip == (net_end+1)) {
		alert(translate_str("JavaScript",59) + localip);
		f.lan_ipaddr_4.focus();
		return false;
	}
*/
	if (parseInt(f.StartIP.value) == localip) {
		alert(translate_str("JavaScript",55));
		f.StartIP.focus();
		return false;
	}
	if ((parseInt(f.StartIP.value) < net_start) || (parseInt(f.StartIP.value) > net_end)) {
		alert(translate_str("JavaScript",60) + " [" + net_start + "~" + (net_end-1) + "]");
		f.StartIP.focus();
		return false;
	}
	if ((parseInt(f.StartIP.value) + parseInt(f.NumOfUsers.value)) > net_end) {
		client = net_end - parseInt(f.StartIP.value);
		alert(translate_str("JavaScript",61) + " [" + client+ "]");
		f.NumOfUsers.focus();
		return false;
	}
	if( parseInt(f.NumOfUsers.value) < 1 || parseInt(f.NumOfUsers.value) > 254)
	{
		client = net_end - parseInt(f.StartIP.value);
		alert(translate_str("JavaScript",125)+" [1-" + client + "]!");
		f.NumOfUsers.focus();
		return false;
	}
	return true;
}
