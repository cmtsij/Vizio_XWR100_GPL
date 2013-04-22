/*
Disabled					:none
WEP							:wep
WPA-Personal				:psk
WPA2-Personal+AES			:WPA2
WPA2-Personal+AES or TKIP	:WPA1WPA2
WPA-Enterprise				:wparadius
WPA2-Enterprise				:wpa2radius
RADIUS						:radius
*/
/*SecurityTypePage = new Array("WS_Wep.htm","WS_WPAPSK.htm","WPA2-Personal.htm","WS_WPARadius.htm","WS_WPA2Radius.htm","WS_Radius.htm","WS_None.htm","WPA2-Mixed.htm","WS_WPA2RadiusMixed.htm");
SecurityTypePage1 = new Array("WS_Wep_1.htm","WS_WPAPSK_1.htm","WPA2-Personal_1.htm","WS_WPARadius_1.htm","WS_WPA2Radius_1.htm","WS_Radius_1.htm","WS_None_1.htm","WPA2-Mixed_1.htm","WS_WPA2RadiusMixed_1.htm");
*/

SecurityTypePage = new Array("adv_WS_WEP.htm","adv_WS_WPAPSK.htm","adv_WS_WPA2_Personal.htm","adv_WS_WPA_Enterprise.htm","adv_WS_WPA2_Enterprise.htm","adv_WS_Radius.htm","adv_WS_None.htm","WPA2-Mixed.htm","WS_WPA2RadiusMixed.htm");
SecurityTypePage1 = new Array("adv_WS_WEP2.htm","adv_WS_WPAPSK2.htm","adv_WS_WPA2_Personal2.htm","adv_WS_WPA_Enterprise2.htm","adv_WS_WPA2_Enterprise2.htm","adv_WS_Radius2.htm","adv_WS_None2.htm","WPA2-Mixed_1.htm","WS_WPA2RadiusMixed_1.htm");
SecurityTypePage2 = new Array("adv_WS_WEP3.htm","adv_WS_WPAPSK3.htm","adv_WS_WPA2_Personal3.htm","adv_WS_WPA_Enterprise3.htm","adv_WS_WPA2_Enterprise3.htm","adv_WS_Radius3.htm","adv_WS_None3.htm","WPA2-Mixed_1.htm","WS_WPA2RadiusMixed_1.htm");

function getLayer(layerID) 
{
	if (self.parent.document.getElementById)
		return self.parent.document.getElementById(layerID);
	else if (self.parent.document.all)
		return self.parent.document.all[layerID];
	else 
		return null;
}

function WirelessSecurityType ( SecurityTypeMode,wps_page )
{	
	if ( SecurityTypeMode == "wparadius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_WPA_Enterprise.htm"; 
	}
    else if ( SecurityTypeMode == "wpa2radius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_WPA2_Enterprise.htm"; 
	}
    else if ( SecurityTypeMode == "radius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_Radius.htm"; 
	}    
    else if ( SecurityTypeMode == "wep" ) 
	{ 
		window.location.href ="adv_WS_WEP.htm"; 
	}
    else if ( SecurityTypeMode == "none") 
	{ 
		window.location.href ="adv_WS_None.htm"; 
	}
	else if ( SecurityTypeMode == "psk" ) { 
		window.location.href ="adv_WS_WPAPSK.htm"; 
	}
	else if ( SecurityTypeMode == "WPA2" ) 
	{ 
		window.location.href ="adv_WS_WPA2_Personal.htm"; 
	}
  else if ( SecurityTypeMode == "WPA1WPA2") 
	{ 
		window.location.href ="WPA2-Mixed.htm"; 
	}
	else if ( SecurityTypeMode == "WPA1WPA2Radius") 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="WS_WPA2RadiusMixed.htm"; 
	}
  else 
	{ 
		window.location.href ="adv_WS_None.htm"; 
	}
}

function WirelessSecurityType1 ( SecurityTypeMode,wps_page )
{	
	if ( SecurityTypeMode == "wparadius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_WPA_Enterprise2.htm"; 
	}
    else if ( SecurityTypeMode == "wpa2radius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_WPA2_Enterprise2.htm"; 
	}
    else if ( SecurityTypeMode == "radius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_Radius2.htm"; 
	}    
    else if ( SecurityTypeMode == "wep" ) 
	{ 
		window.location.href ="adv_WS_WEP2.htm"; 
	}
    else if ( SecurityTypeMode == "none") 
	{ 
		window.location.href ="adv_WS_None2.htm"; 
	}
	else if ( SecurityTypeMode == "psk" ) { 
		window.location.href ="adv_WS_WPAPSK2.htm"; 
	}
	else if ( SecurityTypeMode == "WPA2" ) 
	{ 
		window.location.href ="adv_WS_WPA2_Personal2.htm"; 
	}
  else if ( SecurityTypeMode == "WPA1WPA2") 
	{ 
		window.location.href ="WPA2-Mixed_1.htm"; 
	}
	else if ( SecurityTypeMode == "WPA1WPA2Radius") 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="WS_WPA2RadiusMixed_1.htm"; 
	}
  else 
	{ 
		window.location.href ="adv_WS_None2.htm"; 
	}
}


function WirelessSecurityType2 ( SecurityTypeMode,wps_page )
{	
	if ( SecurityTypeMode == "wparadius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_WPA_Enterprise3.htm"; 
	}
    else if ( SecurityTypeMode == "wpa2radius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_WPA2_Enterprise3.htm"; 
	}
    else if ( SecurityTypeMode == "radius" ) 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="adv_WS_Radius3.htm"; 
	}    
    else if ( SecurityTypeMode == "wep" ) 
	{ 
		window.location.href ="adv_WS_WEP3.htm"; 
	}
    else if ( SecurityTypeMode == "none") 
	{ 
		window.location.href ="adv_WS_None3.htm"; 
	}
	else if ( SecurityTypeMode == "psk" ) { 
		window.location.href ="adv_WS_WPAPSK3.htm"; 
	}
	else if ( SecurityTypeMode == "WPA2" ) 
	{ 
		window.location.href ="adv_WS_WPA2_Personal3.htm"; 
	}
  else if ( SecurityTypeMode == "WPA1WPA2") 
	{ 
		window.location.href ="WPA2-Mixed_1.htm"; 
	}
	else if ( SecurityTypeMode == "WPA1WPA2Radius") 
	{ 
		if(wps_page == '1')
			alert(translate_str("JavaScript",113));
		window.location.href ="WS_WPA2RadiusMixed_1.htm"; 
	}
  else 
	{ 
		window.location.href ="adv_WS_None3.htm"; 
	}
}

function GetAuthModePageIndex(AuthMode)
{
	var PageIndex;
	
	if ( AuthMode == "wep" )
	  PageIndex = 0;
	else if ( AuthMode == "psk" ) 
		PageIndex = 1;
	else if ( AuthMode == "WPA2" ) 
		PageIndex = 2;
  else if ( AuthMode == "wparadius" ) 
		PageIndex = 3;
  else if ( AuthMode == "wpa2radius" ) 
    PageIndex = 4;
  else if ( AuthMode == "radius" )
   	PageIndex = 5;
  else if ( AuthMode == "WPA1WPA2" ) 
    PageIndex = 2;
  else if ( AuthMode == "WPA1WPA2Radius" )
   	PageIndex = 4;
  else if ( AuthMode == "none" )
   	PageIndex = 6;
  else 
    PageIndex = 6;

	return PageIndex;
}

function GetSSIDName( SSIDnum )
{
	var get_result = "";
	var stringall = "";
	
	stringall = '<!--#set_get path="wireless/wl_ssid" -->' ;
	stringArray = stringall.split(";");
	
	if ( stringArray.length-1 < SSIDnum )
		get_result = "";
	else
		get_result = stringArray[SSIDnum];

	return get_result;
}
