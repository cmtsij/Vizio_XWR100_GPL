var checkns4 = (document.layers)? true:false ;
var checkie4 = (document.all)? true:false;
var checkie5  = (document.all && document.getElementById);
var checkns6 = (!document.all && document.getElementById);

function layerWrite(id,nestref,text) 
{
	if(checkns4) 
	{
		var lyr = (nestref)? eval('document.'+nestref+'.document.'+id+'.document') : document.layers[id].document ;
		lyr.open();
		lyr.write(text);
		lyr.close();
	}
	else if (checkie4)
		document.all[id].innerHTML = text ;
  else if(checkns6)
		document.getElementById(id).innerHTML = text ;
}

function sortByName(a, b) 
{
	var x = a.Name.toLowerCase();
	var y = b.Name.toLowerCase();
	var token = "1234567890" ;
	var m='', n='' ;
	var o, p ;
	var hit ;
	
	if( x.substring(0, 5) == 'vizio'  && y.substring(0, 5) == 'vizio')
	{
		hit = 0 ;
		for( var i=0 ; i<x.length ; i++ )
		{		
			for( var j=0 ; j<token.length ; j++ )
			{	
				if( x.charAt(i) == token.charAt(j) && hit == 0 )
				{
					hit = 1 ;
					break ;
				}
			}
			
			if( hit == 1 )
				m += x.charAt(i) ;
		}		
		o = m - 0 ;

		hit = 0 ;
		for( var i=0 ; i<y.length ; i++ )
		{		
			for( var j=0 ; j<token.length ; j++ )
			{	
				if( y.charAt(i) == token.charAt(j) && hit == 0 )
				{
					hit = 1 ;
					break ;
				}
			}
			
			if( hit == 1 )
				n += y.charAt(i) ;
		}		
		p = n - 0 ;	
		
		return ((o < p) ? -1 : ((o > p) ? 1 : 0));
	}
	else
		return 1 ;
}

function sortByInterface(a, b) 
{
	var x = a.Interface.toLowerCase();
	var y = b.Interface.toLowerCase();
	return ((x < y) ? -1 : ((x > y) ? 1 : 0));
}
function sortByStatus(a, b) 
{
	var x = a.STATUS.toLowerCase();
	var y = b.STATUS.toLowerCase();
	return ((x < y) ? -1 : ((x > y) ? 1 : 0));
}

function sortByIP(a, b) 
{
	var x = a.IP.toLowerCase();
	var y = b.IP.toLowerCase();
	var cnt = 0 ;
	var m='', n='' ;
	var o, p ;
	
	for( var i=0 ; i<x.length ; i++ )
	{		
		if( cnt == 3 )
			m += x.charAt(i) ;
		
		if( x.charAt(i) == '.' )
			cnt++ ;
	}
	o = m - 0 ;
	
	cnt = 0 ;
	for( var i=0 ; i<y.length ; i++ )
	{		
		if( cnt == 3 )
			n += y.charAt(i) ;
		
		if( y.charAt(i) == '.' )
			cnt++ ;
	}	
	p = n - 0 ;
	
	return ((o < p) ? -1 : ((o > p) ? 1 : 0));
}

function sortByMAC(a, b) 
{
	var x = a.MAC.toLowerCase();
	var y = b.MAC.toLowerCase();
	return ((x < y) ? -1 : ((x > y) ? 1 : 0));
}

function sData(port, sname) 
{
	this.Port  = port;
	this.Sname = sname;
}

function PortNumToServiceName(port)
{
	var pData = [
		["1", "tcpmux"], 		["5", "rje"], 				["7", "echo"], 				["9", "discard"],
		["11", "systat"], 		["13", "daytime"], 			["17", "qotd"], 			["18", "msp"],
		["19", "chargen"], 		["20", "ftp-data"], 		["21", "ftp"], 				["22", "ssh"],
		["23", "telnet"], 		["25", "smtp"], 			["37", "time"], 			["39", "rlp"],
		["42", "nameserver"], 	["43", "nicname"], 			["49", "tacacs"], 			["50", "re-mail-ck"],
		["53", "domain"], 		["63", "whois++"], 			["67", "bootps"], 			["68", "bootpc"],
		["69", "tftp"], 		["70", "gopher"], 			["71", "netrjs-1"], 		["72", "netrjs-2"],
		["88", "keberos"], 		["95", "supdup"], 			["101", "hostname"], 		["102", "iso-tsap"],
		["105", "csnet-ns"], 	["107", "rtelnet"], 		["109", "pop2"], 			["110", "pop3"],
		["111", "sunrpc"], 		["113", "auth"], 			["115", "sftp"], 			["117", "uucp-path"],
		["119", "nntp"], 		["123", "ntp"], 			["137", "netbios-ns"], 		["138", "netbios-dgm"],
		["139", "netbios-ssn"], ["143", "imap"], 			["161", "snmp"], 			["162", "snmptrap"],
		["163", "cmip-man"], 	["164", "cmip-agent"], 		["174", "mailq"], 			["177", "xdmcp"],
		["178", "nextstep"], 	["179", "bgp"], 			["191", "prospero"], 		["194", "irc"],
		["199", "smux"], 	 	["201", "at-rtmp"], 		["202", "at-nbp"], 			["204", "at-echo"],
		["206", "at-zis"], 	 	["209", "qmtp"], 			["210", "z39.50"], 			["213", "ipx"],
		["220", "imap3"], 	 	["245", "link"], 			["347", "fatserv"], 		["363", "rsvp_tunnel"],
		["369", "rpc2portmap"],	["370", "codaauth2"], 		["372", "ulistproc"],		["389", "ldap"],
		["427", "svrloc"], 	 	["434", "mobileip-agent"], 	["435", "mobilip-mn"], 		["443", "https"],
		["444", "snpp"], 	 	["445", "microsoft-ds"], 	["464", "kpasswd"], 		["468", "photuris"],
		["487", "saft"], 	 	["488", "gss-http"], 		["496", "pim-rp-disc"], 	["500", "isakmp"],
		["538", "gdomap"], 	 	["535", "iiop"], 			["546", "dhcpv6-client"], 	["547", "dhcpv6-server"],
		["554", "rtsp"], 	 	["563", "nntps"], 			["565", "whoami"], 			["587", "submission"],
		["610", "npmp-local"], 	["611", "npmp-gui"], 		["612", "hmmp-ind"],		["631", "ipp"],
		["636", "ldaps"], 	 	["674", "acap"], 			["694", "ha-culster"],		["749", "kerberos-adm"],
		["750", "kerberos-iv"], ["765", "webster"],			["767", "phonebook"], 		["873", "rsync"],
		["992", "telnets"],	 	["993", "imaps"], 			["994", "ircs"], 			["995", "pop3s"],
		["512", "exec"],	 	["513", "login"], 			["514", "syslog"], 			["515", "printer"],
		["517", "talk"],	 	["518", "ntalk"], 			["519", "utime"], 			["520", "router"],
		["521", "ripng"],	 	["525", "timed"], 			["526", "tempo"], 			["530", "Arial"],
		["531", "conference"],	["532", "netnews"], 		["533", "netwall"], 		["540", "uucp"],
		["543", "klogin"],		["544", "kshell"], 			["548", "afpovertcp"], 		["556", "remotefs"],
		["1080","socks"],		["1300","h323hostcallsc"],	["1433","ms-sql-s"], 		["1434", "ms-sql-m"],
		["1494","ica"],			["1512", "wins"], 			["1812", "radius"], 		["1813", "radius-acct"],
		["2049","nfs"],			["2401", "cvspserver"], 	["3306", "mysql"], 			["540", "uucp"],
		["465", "smtps"],		["80", "http"]		
	];
	var pDataArray = new Array();
	
	for( var i=0 ; i<pData.length ; i++ )
		pDataArray[pDataArray.length++] = new sData(pData[i][0], pData[i][1]);
		
	for (var i=0; i<pDataArray.length; i++) 
	{
		if( pDataArray[i].Port == port )
			return pDataArray[i].Sname ;
	}	
	
	return port ;
}

