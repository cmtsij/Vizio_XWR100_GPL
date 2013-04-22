VPNClientListURL = new Array("StatusVPNClientList1.htm", "StatusVPNClientList2.htm");

function ShowDHCPServerStatus(dhcps)
{
	if (dhcps == '0')
		document.write("" + translate_str("CheckBox",1) + "");
	else
		document.write("" + translate_str("CheckBox",0) + "");	
		
}

function ShowWEPStatus(wep)
{
	if (wep == 'none')
		document.write(translate_str("SingalPortFW",1));
	else if (wep == 'wep')
		document.write(translate_str("WirelessSecurity",3));
	else if (wep == 'psk')
		document.write(translate_str("WirelessSecurity",4));
	else if (wep == 'radius')
		document.write(translate_str("WirelessSecurity",9));
	else if (wep == 'wparadius')
		document.write(translate_str("WirelessSecurity",7));
	else if (wep == 'wpa2radius')
		document.write(translate_str("WirelessSecurity",8));
	else if (wep == 'WPA2')
		document.write(translate_str("WirelessSecurity",5));
	else if (wep == 'WPA1WPA2')
		document.write(translate_str("WirelessSecurity",5));
	else if (wep == 'WPA1WPA2Radius')
		document.write(translate_str("WirelessSecurity",8));
	else 
		document.write(''+translate_str("BasicWireless",28)+'');
}

function ShowWLMode(wl)
{
//		alert("wl="+wl);
	if (wl == '0')
		document.write(translate_str("BasicWireless",27));
	else if (wl == '1')
		document.write(translate_str("BasicWireless",3));
	else if (wl == '2')
		document.write(translate_str("BasicWireless",5));
	else if (wl == '3')
		document.write(translate_str("BasicWireless",4));
	else
		document.write(translate_str("BasicWireless",26));
}

function ShowWLMode1(wl)
{
	if (wl == '1')
		document.write(translate_str("BasicWireless",3));
	else if (wl == '2')
		document.write(translate_str("BasicWireless",40));
	else if (wl == '3')
		document.write(translate_str("BasicWireless",26));
	else if (wl == '0')
		document.write(translate_str("BasicWireless",27));
	else
		document.write(translate_str("BasicWireless",28));
}
function ShowSSIDBroadcast(wl)
{
	if (wl == '1')
		document.write("" + translate_str("CheckBox",0) + "");
	else if (wl == '0')
		document.write("" + translate_str("CheckBox",1) + "");
}

function ShowConnType(type)
{
	if (type=="dhcp")
		document.write(''+translate_str("BasicSetup",9)+'');
	else if (type=="static")
		document.write(''+translate_str("BasicSetup",10)+'');
	else if (type=="pppoe")
		document.write(translate_str("BasicSetup",11));
	else if (type=="pptp")
		document.write(translate_str("BasicSetup",12));
	else if (type=="l2tp")
		document.write(translate_str("BasicSetup",109));
	else if (type=="7")
		document.write(translate_str("BasicSetup",125));
	else if (type=="hbeat")
		document.write(translate_str("BasicSetup",13));
	else
		document.write(''+translate_str("BasicSetup",9)+'');	
	
}

function ShowRadioBand(wl)
{
	if (wl == '0')
		document.write(translate_str("BasicWireless",29));
	else if (wl == '2')
		document.write(translate_str("BasicWireless",30));
	else
		document.write(translate_str("BasicWireless",31));
}

function ShowWideChannel(wl)
{

	if (wl == '1')
		document.write('3');
	else if (wl == '2')
		document.write('4');
	else if (wl == '3')
		document.write('5');
	else if (wl == '4')
		document.write('6');
	else if (wl == '5')
		document.write('7');
	else if (wl == '6')
		document.write('8');
	else if (wl == '7')
		document.write('9');
	else
	document.write(translate_str("BasicSetup",18));
}

function ShowWideChannel1(wl)
{
	if (wl == '1')
		document.write('38');
	else if (wl == '2')
		document.write('46');
	else if (wl == '3')
		document.write('151');
	else if (wl == '4')
		document.write('159');
	else
		document.write(translate_str("BasicSetup",18));
}
function ShowChannel(wl)
{
	if (wl == '0')
		document.write('Auto');
	else if (wl == "-1")
		document.write('N/A');
	else
		document.write(wl);
}
function ShowLeaseTime(sec, cur_time, up_time)
{
	var past_time = cur_time - up_time;
	var second,minute,hour,day;
	var cur_lease = sec - past_time;
	second = parseInt(cur_lease);
	minute = parseInt(second/60);
	second = second%60;
	hour = parseInt(minute/60);
	minute = minute%60;
	day = parseInt(hour/24);
	hour = hour%24;
	if (sec =='0' || sec =="")
		document.write("&nbsp;");
	else
		document.write(day+" Days "+hour+" Hours "+minute+" Minutes "+second+" Seconds ");	
}