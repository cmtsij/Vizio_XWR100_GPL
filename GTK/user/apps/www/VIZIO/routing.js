function IPCheck_0 ( I )	//1-254
{
	if ( isNaN( I.value ))
	{
//		alert(translate_str("JavaScript",8));
//		I.focus();
		return false;
	}	
	d = I.value;
	data = d.match(/[^0-9]/);
	if (data || !d) {
//		alert(translate_str("JavaScript",9));
//		I.focus();
		return false;
	}

	if ( !(d<255 && d>0)) 
	{
//		alert(translate_str("JavaScript",10) + " [1-254] !");
//		I.focus();	
		return false;
	}
	return true;
}

function IPCheck_1 ( I )	//0-255
{
	d = I.value;
	if(I.value=="")
		return true;	
	else {
		data = d.match(/[^0-9]/);
		if (data || !d) {
//			alert(translate_str("JavaScript",9));
//			I.focus();
			return false;
		}
	
		if ( !(d<256 && d>=0)) 
		{
//			alert(translate_str("JavaScript",10) + " [0-255] !");
//			I.focus();	
			return false;
		}
		return true;
	}
}

function IPCheck_2 ( I )
{
	d = I.value;
	if(I.value=="")
		return true;	
	else {
		data = d.match(/[^0-9]/);
		if (data || !d) {
//			alert(translate_str("JavaScript",9));
//			I.focus();
			return false;
		}
	
		if ( !(d<255 && d>=0)) 
		{
//			alert(translate_str("JavaScript",10) + " [0-254] !");
//			I.focus();	
			return false;
		}
		return true;
	}
}

function CheckIPDomain(f)
{
	if(f.sr_ipaddr_4.value != '0')
	{
		if(f.sr_netmask_1.value != '255' || f.sr_netmask_2.value != '255' || f.sr_netmask_3.value != '255' || f.sr_netmask_4.value != '255')
		{
			alert(translate_str("JavaScript",32) + ' 255.255.255.255!');
			f.sr_netmask_1.focus();
			return false;			
		}
	}
	else
	{
	  if(f.sr_ipaddr_4.value != '0' && f.sr_netmask_4.value == '0') 
		{
			alert(translate_str("JavaScript",33) + ' 0');
			f.sr_ipaddr_4.focus();
			return false;
		}
	  if(f.sr_ipaddr_3.value != '0' && f.sr_netmask_3.value == '0') 
		{
			alert(translate_str("JavaScript",34) + ' 0');
			f.sr_ipaddr_3.focus();			
			return false;			
		}		
	  if(f.sr_ipaddr_2.value != '0' && f.sr_netmask_2.value == '0') 
		{
			alert(translate_str("JavaScript",35) + ' 0');
			f.sr_ipaddr_2.focus();	
			return false;					
		}		
		
		if(f.sr_ipaddr_4.value == '0' && f.sr_netmask_4.value != '0') 
		{ 
			alert(translate_str("JavaScript",36) + ' 0');
			f.sr_netmask_4.focus();
			return false;
		}
	  if(f.sr_ipaddr_3.value == '0' && f.sr_netmask_3.value != '0') 
		{ 
			if((f.sr_netmask_3.value == '255') && (f.sr_netmask_2.value == '255'))	{
				return true;
			}
			alert(translate_str("JavaScript",37) + ' 0');
			f.sr_netmask_3.focus();			
			return false;			
		}		
	  if(f.sr_ipaddr_2.value == '0' && f.sr_netmask_2.value != '0') 
		{
			if((f.sr_netmask_2.value == '255') && (f.sr_netmask_1.value == '255'))	{
				return true;
			}
			alert(translate_str("JavaScript",38) + ' 0');
			f.sr_netmask_2.focus();	
			return false;					
		}		

		if(f.sr_ipaddr_2.value == '0' && f.sr_ipaddr_3.value != '0' && f.sr_ipaddr_4.value == '0')
		{
			alert(translate_str("JavaScript",39));
			f.sr_ipaddr_3.focus();	
			return false;			
		}
		
		
	}
	return true;
}

function SpiltIP(IPAddress,SegmentSeq)
{
	var	DotPos_1 = 0;
	var	DotPos_2 = 0;
	var	DotPos_3 = 0;
	var	StartPos = 0;
	var	EndPos = 0;
	var	SegmentIP = '';

	for ( iln = 0 ; iln < IPAddress.length ; iln++ ) {
      		ch = IPAddress.charAt(iln).toLowerCase();

      		if (( ch == '.' ) && ( DotPos_1 == 0 ) && ( DotPos_2 == 0 ) && ( DotPos_3 == 0 )) {
      			DotPos_1 = iln;
      		} else if (( ch == '.' ) && ( DotPos_1 != 0 ) && ( DotPos_2 == 0 ) && ( DotPos_3 == 0 )) {
      			DotPos_2 = iln;
      		} else if (( ch == '.' ) && ( DotPos_1 != 0 ) && ( DotPos_2 != 0 ) && ( DotPos_3 == 0 )) {
      			DotPos_3 = iln;
      			break;
      		}
	}

	if ( SegmentSeq == 1 ) {
		StartPos = 0;
		EndPos = DotPos_1;
	} else if ( SegmentSeq == 2 ) {
		StartPos = DotPos_1 + 1;
		EndPos = DotPos_2;
	} else if ( SegmentSeq == 3 ) {
		StartPos = DotPos_2 + 1;
		EndPos = DotPos_3;
	} else if ( SegmentSeq == 4 ) {
		StartPos = DotPos_3 + 1;
		EndPos = IPAddress.length;
	}

	for ( iln = StartPos ; iln < EndPos ; iln++ ) {
      		ch = IPAddress.charAt(iln).toLowerCase();
		SegmentIP = SegmentIP + ch;
	}

	return SegmentIP;
}

function ChangeInterface(Interface)
{
	if ( Interface == 0 ) {
		document.forms[0].route_ifname.selectedIndex = 0;
	} else if ( Interface == 1 ) {
		document.forms[0].route_ifname.selectedIndex = 1;
	}
}

function AddName(f)
{
	var index = f.TableSequ.value;
	if(f.route_name.value=="") {
		alert(translate_str("JavaScript",40));
		return 0;
	}
	if (!CheckQuote1(f.route_name)) return 0;
	for (var i=0;i<10;i++) 
	{
		if(index==i)
			 continue;
		if(eval("document.forms[0].sr_name"+i).value==f.route_name.value)
		{
			alert(translate_str("JavaScript",85));
			return 0;
		}
	}		
	return 1;
}
function AddEntry(passForm) {
	var index = passForm.TableSequ.value;
	var ip_address, netmask_address, gateway_address;
	var lan_addr, lan_ipaddr;
	
	ip_address = AssembleAddress(passForm, "passForm.sr_ipaddr_");
	netmask_address = AssembleAddress(passForm, "passForm.sr_netmask_");
	gateway_address = AssembleAddress(passForm, "passForm.sr_gateway_");
	var i, j, tmp;
	tmp = 0;
	lan_addr = passForm.sr_ipaddr_1.value + '.' + passForm.sr_ipaddr_2.value + '.' + passForm.sr_ipaddr_3.value;
	lan_ipaddr = '<%nvram_get("lan_ipaddr");%>';

	if(lan_ipaddr.match(lan_addr))
	{
		alert(translate_str("ADRouting",19));
		return 0;
	}
	
	for(i = 4; i >= 1; i--)
	{
		for(j = 1; j <= i; j++)
		{
			if(eval("passForm.sr_netmask_" + j).value == 0 && eval("passForm.sr_ipaddr_" + i).value != 0 && j <= i )
			{
				alert(translate_str("ADRouting",19));
				return 0;
			}
		}
	}

	for(i = 1; i <= 4; i++) {

		if(!IPCheck1(eval("passForm.sr_ipaddr_" + i)))
			return ;

		if(!IPCheck1(eval("passForm.sr_netmask_" + i)))
			return ;

		if(!IPCheck1(eval("passForm.sr_gateway_" + i)))
			return ;			

		if(isNaN(eval("passForm.sr_ipaddr_" + i).value)){
			alert(translate_str("JavaScript",73));
			eval("passForm.sr_ipaddr" + i).focus();
			return ;
		}

		if(isNaN(eval("passForm.sr_netmask_" + i).value)){
			alert(translate_str("JavaScript",73));
			eval("passForm.sr_netmask" + i).focus();
			return ;
		}

		if(isNaN(eval("passForm.sr_gateway_" + i).value)){
			alert(translate_str("JavaScript",73));
			eval("passForm.sr_gateway" + i).focus();
			return ;
		}

	}
//---Eric add PM 01:34 2008/4/1
	//if(!IsLegalRoute(ip_address, netmask_address)) {
	//	return false;
	//}
		
	return 1;
}

function RestoringEntry(f)
{
	var index = f.TableSequ.value;
	if (eval("f.sr_name"+index).value == "")
		f.route_name.value = "";	
	else
		f.route_name.value = eval("f.sr_name"+index).value;
	if (eval("f.sr_ipaddr"+index).value=="")
	{
		f.sr_ipaddr_1.value = 0;
		f.sr_ipaddr_2.value = 0;
		f.sr_ipaddr_3.value = 0;
		f.sr_ipaddr_4.value = 0;
	}
	else
	{
		f.sr_ipaddr_1.value = SpiltIP(eval("f.sr_ipaddr"+index).value,1);
		f.sr_ipaddr_2.value = SpiltIP(eval("f.sr_ipaddr"+index).value,2);
		f.sr_ipaddr_3.value = SpiltIP(eval("f.sr_ipaddr"+index).value,3);
		f.sr_ipaddr_4.value = SpiltIP(eval("f.sr_ipaddr"+index).value,4);
	}
	if (eval("f.sr_netmask"+index).value == "")
	{
		f.sr_netmask_1.value = 0;
		f.sr_netmask_2.value = 0;
		f.sr_netmask_3.value = 0;
		f.sr_netmask_4.value = 0;	
	}
	else
	{
		f.sr_netmask_1.value = SpiltIP(eval("f.sr_netmask"+index).value,1);
		f.sr_netmask_2.value = SpiltIP(eval("f.sr_netmask"+index).value,2);
		f.sr_netmask_3.value = SpiltIP(eval("f.sr_netmask"+index).value,3);
		f.sr_netmask_4.value = SpiltIP(eval("f.sr_netmask"+index).value,4);
	}
	if (eval("f.sr_gateway"+index).value == "")    
	{
		f.sr_gateway_1.value = 0;
		f.sr_gateway_2.value = 0;
		f.sr_gateway_3.value = 0;
		f.sr_gateway_4.value = 0;
	
	}
	else
	{
		f.sr_gateway_1.value = SpiltIP(eval("f.sr_gateway"+index).value,1);
		f.sr_gateway_2.value = SpiltIP(eval("f.sr_gateway"+index).value,2);
		f.sr_gateway_3.value = SpiltIP(eval("f.sr_gateway"+index).value,3);
		f.sr_gateway_4.value = SpiltIP(eval("f.sr_gateway"+index).value,4);
	}
	ChangeInterface(eval("f.sr_ifname"+index).value);
}

function DeleteEntry(f)
{
	var index = f.TableSequ.value;
	if ( window.confirm("Do you make sure to delete this entry ?") ) 
	{
		f.route_name.value="";
		f.sr_ipaddr_1.value = '0';
		f.sr_ipaddr_2.value = '0';
		f.sr_ipaddr_3.value = '0';
		f.sr_ipaddr_4.value = '0';
		f.sr_netmask_1.value = '0';
		f.sr_netmask_2.value = '0';
		f.sr_netmask_3.value = '0';
		f.sr_netmask_4.value = '0';
		f.sr_gateway_1.value = '0';
		f.sr_gateway_2.value = '0';
		f.sr_gateway_3.value = '0';
		f.sr_gateway_4.value = '0';		
		ChangeInterface("br0");
		eval("f.sr_name"+index).value = "";
		eval("f.sr_ipaddr"+index).value = "";
		eval("f.sr_netmask"+index).value = "";
		eval("f.sr_gateway"+index).value = "";
		eval("f.sr_ifname"+index).value = "";
		eval("f.sr_type"+index).value = "";


		var temp = Number(index) + 1;
		f.TableSequ.options[index] = new Option(temp + " ---", index);
		f.TableSequ.value = index;
	}	
}
