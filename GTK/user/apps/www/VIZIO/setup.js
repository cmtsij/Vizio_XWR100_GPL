/*function connectionType ( connectionTypeMode )
{
	//Normal
    if ( connectionTypeMode == "static" ) { window.location.href ="SetupStatic.htm"; }
    else if ( connectionTypeMode == "pppoe" ) { window.location.href ="SetupPPPoE.htm"; }
    else if ( connectionTypeMode == "pptp" ) { window.location.href ="SetupPPTP.htm"; }    
    else if ( connectionTypeMode == "dhcp" ) { window.location.href ="SetupDHCP.htm"; }
    else if ( connectionTypeMode == "l2tp" ) { window.location.href ="SetupL2TP.htm"; }
	else if ( connectionTypeMode == "hbeat" ) { window.location.href ="SetupHBeat.htm"; }
    else { window.location.href ="SetupDHCP.htm"; }
}
*/
function CheckRTURL(I)
{
	var ch = new String(I.value);
	if (ch.indexOf(".") == -1)	{
		alert(translate_str("JavaScript",99));
		I.focus();
		return 0;
	}
	return 1;
}

function Setdaylightsaving(f,dayligh,time_zone_select){

	if (f.timeZone.selectedIndex==0 || f.timeZone.selectedIndex==1 || f.timeZone.selectedIndex==2 || f.timeZone.selectedIndex==5 || f.timeZone.selectedIndex==7 || f.timeZone.selectedIndex==9 || f.timeZone.selectedIndex==11 || f.timeZone.selectedIndex==13 || f.timeZone.selectedIndex==15 || f.timeZone.selectedIndex==17 || f.timeZone.selectedIndex==19 || f.timeZone.selectedIndex==21 || f.timeZone.selectedIndex==23 || f.timeZone.selectedIndex==24 || f.timeZone.selectedIndex==25 || f.timeZone.selectedIndex==26 || f.timeZone.selectedIndex==27 || f.timeZone.selectedIndex==28 || f.timeZone.selectedIndex==29 || f.timeZone.selectedIndex==30 || f.timeZone.selectedIndex==31 || f.timeZone.selectedIndex==33 || f.timeZone.selectedIndex==34){
		f.Daylight.disabled=true;
		f.Daylight.checked=false;
	}
	else{

		f.Daylight.disabled=false;
		
		if(f.timeZone.selectedIndex == time_zone_select){

			if(dayligh == '1')
				f.Daylight.checked=true;
			else
				f.Daylight.checked=false;
		}
		else
		f.Daylight.checked=true;
	}
}