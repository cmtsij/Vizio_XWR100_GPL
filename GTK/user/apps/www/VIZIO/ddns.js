function ShowDYNDDNS(status)
{
		/*if (status == '1')
			translate_strWrite("DDNS",6);
		else
			translate_strWrite("DDNS",5);	*/
		if (status != '0')
			document.write(status);
		else
			translate_strWrite("DDNS",5);
}
function ShowTZODDNS(tzo_switch,status)
{
	if (tzo_switch == '1')	{	
		if (status == '0')
			translate_strWrite("DDNS",28);
		else if(status == '1')
			translate_strWrite("DDNS",29);
		else
			document.write(status);	
	}
	else
			translate_strWrite("DDNS",5);
}

function ShowInternetAddr(wan_proto)
{
	var wan_ip = '<%nvram_get("wan_ipaddr");%>';
		dw(wan_ip);

}

function SelDDNS()
{
	if (document.forms[0].DDNSEnable.selectedIndex == 0)
		window.location.href ="AppDDNS.htm";
	else if (document.forms[0].DDNSEnable.selectedIndex == 1)
		window.location.href ="AppDDNSDYN.htm";
	else if (document.forms[0].DDNSEnable.selectedIndex == 2)
		window.location.href ="AppDDNSTZO.htm";


}