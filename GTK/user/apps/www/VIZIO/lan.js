function AssembleLanIPAddress ()
{

	if ( !(IPCheck_radius_1(document.forms[0].lan_ipaddr_1)) ) {
    		document.forms[0].lan_ipaddr_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].lan_ipaddr_2)) ) {
    		document.forms[0].lan_ipaddr_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].lan_ipaddr_3)) ) {
    		document.forms[0].lan_ipaddr_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck(document.forms[0].lan_ipaddr_4)) ) {
    		document.forms[0].lan_ipaddr_4.value = 0;
    		return false;
  	}
	if ((document.forms[0].lan_ipaddr_1.value==255) &&
	(document.forms[0].lan_ipaddr_2.value==255) &&
	(document.forms[0].lan_ipaddr_3.value==255) &&
	(document.forms[0].lan_ipaddr_4.value==255)
	)
	{
		alert(translate_str("JavaScript",3));
		document.forms[0].lan_ipaddr_1.focus();
		return 0;
	}  	
	if ((document.forms[0].lan_ipaddr_1.value >= 224) &&
	(document.forms[0].lan_ipaddr_1.value <= 239))	{
		alert(translate_str("JavaScript",8));
		document.forms[0].lan_ipaddr_1.focus();
		return 0;
	}  	
	if ((document.forms[0].lan_ipaddr_4.value==0) ||
		(document.forms[0].lan_ipaddr_4.value==255)
		)
	{
		alert(translate_str("JavaScript",4));
		document.forms[0].lan_ipaddr_4.focus();
		return 0;
	}
	if ((document.forms[0].lan_ipaddr_1.value==0) ||
		(document.forms[0].lan_ipaddr_1.value==255)
		)
	{
		alert(translate_str("JavaScript",4));
		document.forms[0].lan_ipaddr_1.focus();
		return 0;
	}
	d = (document.forms[0].lan_ipaddr_1.value * 1) + '.';
  	d = d + (document.forms[0].lan_ipaddr_2.value * 1) + '.';
  	d = d + (document.forms[0].lan_ipaddr_3.value * 1) + '.';
  	d = d + (document.forms[0].lan_ipaddr_4.value * 1);

	document.forms[0].lan_ipaddr.value = d;
	return true;
}

function SplitLanIPAddress ( IPString )
{
	document.forms[0].lan_ipaddr_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].lan_ipaddr_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].lan_ipaddr_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].lan_ipaddr_4.value = GetIPSegment ( IPString , 4 );
	return true;
}


