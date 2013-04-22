var MultiLanguage = 0;
var ie4 = ((navigator.appName == "Microsoft Internet Explorer") && (parseInt(navigator.appVersion) >= 4 ))
var ns4 = ((navigator.appName == "Netscape") && (parseInt(navigator.appVersion) < 6 ))
var ns6 = ((navigator.appName == "Netscape") && (parseInt(navigator.appVersion) >= 6 ))

function char_hex_to_oct(ch){

	if(ch == "A" ||ch == "a")
		return 10 ;
	else if(ch == "B" ||ch == "b")
		return 11 ;
	else if(ch == "C" ||ch == "c")
		return 12 ;
	else if(ch == "D" ||ch == "d")
		return 13 ;
	else if(ch == "E" ||ch == "e")
		return 14 ;
	else if(ch == "F" ||ch == "f")
		return 15 ;
	else
		return ch ;

}

function dw(obj)
{
	document.write(obj);
}
function isdigit(I,M,dfValue)
{
	for(i=0 ; i<I.value.length; i++){
		ch = I.value.charAt(i);
		if(ch < '0' || ch > '9'){
			I.value = dfValue;
			alert(M + errmsg.err28);
			return false;
		}
	}
	return true;
}
function valid_mac(I,T,dfValue)
{
	var m1,m2=0;

	if(I.value.length == 1)
		I.value = "0" + I.value;
	m1 =parseInt(I.value.charAt(0), 16);
	m2 =parseInt(I.value.charAt(1), 16);
	if( isNaN(m1) || isNaN(m2) )
	{
		I.value = dfValue;
		alert(errmsg.err15);
	}
	I.value = I.value.toUpperCase();
	if(T == 0)
        {
		if((m2 & 1) == 1){
			I.value = dfValue;
			alert(errmsg.err16);
		}
        }
        return true
}
function valid_macs_17(I,dfValue)
{
	oldmac = I.value;
	if(I.value.length < 17){
		I.value = dfValue;
		alert(errmsg.err18);
		return false;
	}
	var mac = ignoreSpaces(oldmac);
	if (mac == "")
	{
		return true;
	}
	var m = mac.split(":");
	if (m.length != 6)
	{
		I.value = dfValue;
		alert(errmsg.err21);
		return false;
	}
	var idx = oldmac.indexOf(':');
	if (idx != -1) {
		var pairs = oldmac.substring(0, oldmac.length).split(':');
		for (var i=0; i<pairs.length; i++) {
			nameVal = pairs[i];
			len = nameVal.length;
			if (len < 1 || len > 2) {
				I.value = dfValue;
				alert (errmsg.err22);
				return false;
			}
			for(iln = 0; iln < len; iln++) {
				ch = nameVal.charAt(iln).toLowerCase();
				if (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f') {
				}
				else {
					I.value = dfValue;
					alert (errmsg.err23 + nameVal + errmsg.err24 + oldmac);
					return false;
				}
			}
		}
	}
	I.value = I.value.toUpperCase();
	if(I.value == "FF:FF:FF:FF:FF:FF"){
		I.value = dfValue;
		alert(errmsg.err19);
	}
	m3 = I.value.charAt(1);
	
	if(m3 == "B")
		m4 = 11;
	else if(m3 == "D")
		m4 = 13;
	else if(m3 == "F")
		m4 = 15;
	else
		m4 = m3;
	
	if((m4 & 1) == 1){
		I.value = dfValue;
		alert(errmsg.err16);
	}
	return true;
}
function valid_macs_all(I)
{
	if(I.value == "")
		return true;
	else if(I.value.length == 12) {
		valid_macs_12(I,I.defaultValue);
		
		if (I.value.length == 12) {
		  var refmt = "00:00:00:00:00:00";
		  refmt = I.value.substr(0,2) + ":" +
				 I.value.substr(2,2) + ":" +
				 I.value.substr(4,2) + ":" +
				 I.value.substr(6,2) + ":" +
				 I.value.substr(8,2) + ":" +
				 I.value.substr(10,2);
		  I.value = refmt;
		}
		
	}
	else if(I.value.length == 17)
		valid_macs_17(I,I.defaultValue);
	else{
		I.value = I.defaultValue;
		alert(translate_str("JavaScript",17));
        }
}
function closeWin(var_win){
	if ( ((var_win != null) && (var_win.close)) || ((var_win != null) && (var_win.closed==false)) )
		var_win.close();
}
function IPFirstCheck ( I )
{
	if ( isNaN( I.value ))
	{
		alert(translate_str("JavaScript",8));
		I.focus();
		return false;
	}	

	d = I.value;
	data = d.match(/[^0-9]/g);
	if (data || !d) {
		alert(translate_str("JavaScript",9));
		I.focus();
		return false;
	}

	if ( !(d<224 && d>=0)) 
	{
		//alert(translate_str("JavaScript",10) + " [0-223] !");
		alert(translate_str("JavaScript",68));
		I.focus();	
		return false;
	}
	return true;
}

function IPCheck_radius_1 ( I )	//1-223
{
	if ( isNaN( I.value ))
	{
		alert(translate_str("JavaScript",8));
		I.focus();
		return false;
	}	
	d = I.value;
	data = d.match(/[^0-9]/g);
	if (data || !d) {
		alert(translate_str("JavaScript",9));
		I.focus();
		return false;
	}

	if ( !(d<224 && d>0)) 
	{
		//alert(translate_str("JavaScript",10) + " [1-223] !");
		alert(translate_str("JavaScript",68));
		I.focus();	
		return false;
	}
	return true;
}

function IPCheck ( I )	//1-254
{
	if ( isNaN( I.value ))
	{
		alert(translate_str("JavaScript",8));
		I.focus();
		return false;
	}	
	d = I.value;
	data = d.match(/[^0-9]/g);
	if (data || !d) {
		alert(translate_str("JavaScript",9));
		I.focus();
		return false;
	}

	if ( !(d<255 && d>0)) 
	{
		//alert(translate_str("JavaScript",10) + " [1-254] !");
		alert(translate_str("JavaScript",68));
		I.focus();	
		return false;
	}
	return true;
}

function IPCheck1 ( I )	//0-255
{
	d = I.value;
	if(I.value=="")
		return true;	
	else {
		data = d.match(/[^0-9]/g);
		if (data || !d) {
			alert(translate_str("JavaScript",9));
			I.focus();
			return false;
		}
	
		if ( !(d<256 && d>=0)) 
		{
			//alert(translate_str("JavaScript",10) + " [0-255] !");
			alert(translate_str("JavaScript",68));
			I.focus();	
			return false;
		}
		return true;
	}
}

function IPCheck2 ( I )
{
	d = I.value;
	if(I.value=="")
		return true;	
	else {
		data = d.match(/[^0-9]/g);
		if (data || !d) {
			alert(translate_str("JavaScript",9));
			I.focus();
			return false;
		}
	
		if ( !(d<255 && d>=0)) 
		{
			//alert(translate_str("JavaScript",10) + " [0-254] !");
			alert(translate_str("JavaScript",68));
			I.focus();	
			return false;
		}
		return true;
	}
}

function IPCheck3 ( I )
{
	d = I.value;
	if(I.value=="")
		return true;	
	else {
		data = d.match(/[^0-9]/g);
		if (data || !d) {
			alert(translate_str("JavaScript",9));
			I.focus();
			return false;
		}
	
		if ( !(d<224 && d>0)) 
		{
			//alert(translate_str("JavaScript",10) + " [1-223] !");
			alert(translate_str("JavaScript",68));
			I.focus();	
			return false;
		}
		return true;
	}
}

function IPCheck4 ( I )	//for DMZ
{
	if ( isNaN( I.value ))
	{
		alert(translate_str("JavaScript",8));
		I.focus();
		return false;
	}	
	d = I.value;
	data = d.match(/[^0-9]/g);
	if (data || !d) {
		alert(translate_str("JavaScript",9));
		I.focus();
		return false;
	}

	if ( !(d<255 && d>0)) 
	{
		alert(translate_str("JavaScript",68));
		I.focus();	
		return false;
	}
	return true;
}

function Multicast_IPCheck ( I )	//Can not accept multicast IP Address.  224.0.0.0¡Ð239.255.255.255 
{
	//d = parseInt ( I.value , 10 );
	d = I.value;
//	if(d >= "224" && d <= "239"){   //224-239 is for Multycast IP.    240-254 is Reserved IP.
	if(d >= "224" && d <= "255"){
		alert(translate_str("JavaScript",93));
		f.radius_ipaddr_1.focus();
		return false;
	}
	else
		return true;

}

function ToUpperCase ( string )
{
	var	key = string;

	string = "";
	for ( iln = 0 ; iln < key.length ; iln++ ) {
      		ch = key.charAt(iln).toUpperCase();
		string = string + ch;
	}

	return string;
}
function SpaceCheck ( I )
{
	for ( iln = 0 ; iln < I.value.length ; iln++ ) 
	{
    ch = I.value.charAt(iln).toLowerCase();
	  if (ch == ' ') 
		{
	    alert (translate_str("JavaScript",11));
	    I.focus();
	    return false;
	  }
	}	
	return true;
}

function MACheck ( I )
{
	if ( I.length == 1 ) {
		I.value = '0' + I.value;
	}

	I.value = ToUpperCase ( I.value );
	
	var i;
	var tmp = 0;
	var ch = "";
	for(i=0; i<2; i++)
	{
		ch = I.value.charAt(i);
		if(ch >= '0' && ch <= '9' || ch >= 'A' && ch <= 'F')
			tmp++;
	}
	if(tmp == 2)
		return true;
	else
	{
		alert(translate_str("JavaScript",14) + ' [00 - FF]');
		I.focus();		
		return false;		
	}	
}
function PortCheck_Ping(I)
{
	d =parseInt(I.value, 10);
	if ( !( d<65536 && (d+1)>0)|| isNaN(d) )		
	{
		alert(translate_str("JavaScript",15) + ' [0 - 65535]');
		I.focus();
		I.value = I.defaultValue;
		return 0;
	}
	return 1;
}

function PortCheck(I)
{
//+++Eric add PM 05:42 2008/2/4	
	var txt = I.value ;
	var num ;

	for(num=0;num<txt.length;num++)
	{
		c=txt.charAt(num);
		if("0123456789".indexOf(c,0)<0)
		{
			alert(translate_str("JavaScript",22));
			I.focus();
			return 0;
		}
	}

//---Eric add PM 05:42 2008/2/4	
	d =parseInt(I.value, 10);
	if(isdigit(I,errmsg.err76,I.defaultValue)!=true)
		return 0;
	if ( !( d<65536 && d>0)|| isNaN(d) )		
	{
		alert(translate_str("JavaScript",15) + ' [1 - 65535]');
		I.focus();
		I.value = I.defaultValue;
		return 0;
	}
	return 1;
}
function PortCheck1(I)
{	
	if(I.value=="")
		return 1;	
	else {
		if(isdigit(I,errmsg.err76,I.defaultValue)!=true)
			return 0;
			
		d =parseInt(I.value, 10);
		if ( !( d<65536 && d>0)|| isNaN(d) )		
		{
			alert(translate_str("JavaScript",15) + '[1 - 65535]');
			I.focus();
			I.value = I.defaultValue;
			return 0;
		}
		return 1;
	}
}
function PortCheck2(StartPort,EndPort,ShowText)
{
		if (parseInt(StartPort.value,10) > parseInt(EndPort.value,10))
		{
			//alert("The "+ ShowText + " Port Start must less than " +ShowText+ " Port End.");
			alert(translate_str("JavaScript",16));
			StartPort.focus();
			return false;
		}
		return true;
}
function GetIPSegment ( IPString , SegmentNum )
{
	var	DMZ = IPString;
	var	Counter = 0;

	IPString = "";
	if (DMZ=="")
		return "0";
	for ( iln = 0 ; iln < DMZ.length ; iln++ ) {
      		ch = DMZ.charAt ( iln );
		if ( ch != '.' ) { IPString = IPString + ch; }
		else {
			Counter = Counter + 1;

			if ( Counter == SegmentNum ) { break; }
			else { IPString = ""; }
		}
	}

	return IPString;
}

function GetMACSegment ( MACString , SegmentNum )
{
	var	MAClone = MACString;
	var	Counter = 0;

	MACString = "";

	for ( iln = 0 ; iln < MAClone.length ; iln++ ) {
      		ch = MAClone.charAt ( iln );
		if ( ch != ':' ) { MACString = MACString + ch; }
		else {
			Counter = Counter + 1;

			if ( Counter == SegmentNum ) { break; }
			else { MACString = ""; }
		}
	}

	return MACString;
}

function check_overlap(i_startport,i_endport,o_startport,o_endport)
{
  var num_array = new Array();
  if(i_startport > i_endport)
  {
    num_array = swap_num(i_startport,i_endport);
    i_startport = num_array[0];
    i_endport = num_array[1];
  }

  if(o_startport > o_endport)
  {
    num_array = swap_num(o_startport,o_endport);
    o_startport = num_array[0];
    o_endport = num_array[1];
  }
  if((i_startport <= o_startport) && (i_endport >= o_endport))
    return false;
  else if((i_startport >= o_startport) && (i_startport <= o_endport) && (i_endport >= o_endport))
    return false ; 
  else if((i_endport >= o_startport) && (i_endport <= o_endport))
    return false;
  else if((i_startport >= o_startport) && (i_endport <= o_endport))
    return false;
  else if((i_startport == o_endport) || (i_endport == o_startport) || (i_endport == o_endport))
    return false;

  return true;
}

function checkPort(start, end)
{
	var port;
	if (start.value==0&&end.value==0)
		return true;
	port = end.value - start.value;

	if(port < 0 || isNaN(port))
	{
		alert(translate_str("JavaScript",16));
		start.focus();
		return false;
	}

	return true;
}
function ignoreSpaces(string) {
  var temp = "";

  string = '' + string;
  splitstring = string.split(" ");
  for(i = 0; i < splitstring.length; i++)
    temp += splitstring[i];
  return temp;
}

function macsCheck(oldmac)
{
	var mac = ignoreSpaces(oldmac.value);
  	oldmac.value = mac;
  	
  	if (mac == null || mac == "" || mac == "ff:ff:ff:ff:ff:ff") {
    	alert(translate_str("JavaScript",17));
  		return false;
  	}
  	if (mac == "00:00:00:00:00:00")
  		return true;
	var m = new Array();
	if 	(mac.indexOf(":")==-1)
	{
		if (mac.length != 12)
  		{
    		alert(translate_str("JavaScript",17));
    		return false;
    	}
    	for (var i=0; i<6; i++)
    		m[i] = mac.substring(i*2, (i+1)*2);
	}
	else
	{
  		m = mac.split(":");
  		if (mac.length != 17 || m.length != 6) 
  		{
    		alert(translate_str("JavaScript",17));
    		return false;
    	}
  	}

    if (
                (parseInt(m[0],16) & 1) ||
                (
                        (parseInt(m[0],16) & parseInt(m[1],16) &
                         parseInt(m[2],16) & parseInt(m[3],16) &
                         parseInt(m[4],16) & parseInt(m[5],16)) == 0xff) ||
         ((parseInt(m[0],16) | parseInt(m[1],16) | parseInt(m[2],16) | parseInt(m[3],16) | parseInt(m[4],16) | parseInt(m[5],16)) == 0x00))
         {
                alert(translate_str("JavaScript",18));
                return false;
    }
    
  for (var i=0; i<m.length; i++) {
    nameVal = m[i];
	len = nameVal.length;
	if (len < 1 || len > 2) {
	  alert (translate_str("JavaScript",19));
	  return false;
    }
	for(iln = 0; iln < len; iln++) {
      ch = nameVal.charAt(iln).toLowerCase();
	  if (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f') {
	  }
	  else {
				alert (translate_str("JavaScript",12) + nameVal + translate_str("JavaScript",13) + oldmac.value);
	  return false;
	  }
	}	
   }
	if 	(mac.indexOf(":")==-1)
	{
		oldmac.value = "";
	    for (var i=0; i<6; i++)
    	{
    		if (i!=5)
    			oldmac.value += (mac.substring(i*2, (i+1)*2) + ":");
    		else
    			oldmac.value += mac.substring(i*2, (i+1)*2);
    	}
	}
  return true;
}
function macsCheck1(oldmac)
{
	var mac = ignoreSpaces(oldmac.value);
  	oldmac.value = mac;
  	
  	if (mac == null || mac == "" || mac == "00:00:00:00:00:00" || mac == "ff:ff:ff:ff:ff:ff") {
    	alert(translate_str("JavaScript",17));
  		return false;
  	}

	var m = new Array();
	if 	(mac.indexOf(":")==-1)
	{
		if (mac.length != 12)
  		{
    		alert(translate_str("JavaScript",17));
    		return false;
    	}
    	for (var i=0; i<6; i++)
    		m[i] = mac.substring(i*2, (i+1)*2);
	}
	else
	{
  		m = mac.split(":");
  		if (mac.length != 17 || m.length != 6) 
  		{
    		alert(translate_str("JavaScript",17));
    		return false;
    	}
  	}

    if (
                (parseInt(m[0],16) & 1) ||
                (
                        (parseInt(m[0],16) & parseInt(m[1],16) &
                         parseInt(m[2],16) & parseInt(m[3],16) &
                         parseInt(m[4],16) & parseInt(m[5],16)) == 0xff) ||
         ((parseInt(m[0],16) | parseInt(m[1],16) | parseInt(m[2],16) | parseInt(m[3],16) | parseInt(m[4],16) | parseInt(m[5],16)) == 0x00))
         {
                alert(translate_str("JavaScript",18));
                return false;
    }
    
  for (var i=0; i<m.length; i++) {
    nameVal = m[i];
	len = nameVal.length;
	if (len < 1 || len > 2) {
	  alert (translate_str("JavaScript",19));
	  return false;
    }
	/*for(iln = 0; iln < len; iln++) {
      ch = nameVal.charAt(iln).toLowerCase();
	  if (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f') {
	  }
	  else {
				alert (translate_str("JavaScript",12) + nameVal + translate_str("JavaScript",13) + oldmac.value);
	  return false;
	  }
	}	*/
   }
	if 	(mac.indexOf(":")==-1)
	{
		oldmac.value = "";
	    for (var i=0; i<6; i++)
    	{
    		if (i!=5)
    			oldmac.value += (mac.substring(i*2, (i+1)*2) + ":");
    		else
    			oldmac.value += mac.substring(i*2, (i+1)*2);
    	}
	}
  return true;
}
function CheckEmpty(obj, str)
{
	if (obj.value == "")
	{
		if (str != "")
		{		
			alert(str);
			obj.focus();
		}
		return 0;	
	}
	return 1;
}

function CheckQuote(obj)
{

	if(obj.value.match(/\\/g)) {
		alert(translate_str("JavaScript",20) + " \\");
		obj.focus();
		return 0;
	}
	var src = new String(obj);
	for(i=0;i<src.length;i++)
	{
		var c = src.charAt(i);
		if(c.charCodeAt(0) < 32 || c.charCodeAt(0) > 127)	{
			alert(translate_str("JavaScript",20));
			obj.focus();
			return 0;
		}
	}
	return 1;
}
function CheckQuote1(obj)
{
	if (obj.value.match(/\[|\]|\\|\||\:|\"|\;|\<|\>|\?|\&|\,|\/|\+|\=|\*|\'|\`|\%/g))
	{
		alert(translate_str("JavaScript",20) + " [ ] \\ \& | : \" ; < > ? , / + = * \' \` %");
		obj.focus();
		return 0;
	}
	var src = new String(obj);
	for(i=0;i<src.length;i++)
	{
		var c = src.charAt(i);
		if(c.charCodeAt(0) < 32 || c.charCodeAt(0) > 127)	{
			alert(translate_str("JavaScript",20));
			obj.focus();
			return 0;
		}
	}
	return 1;
}
function CheckQuote2(obj)
{
	if (obj.value.match(/\[|\]|\\|\||\:|\"|\;|\<|\>|\?|\,|\/|\+|\=|\#|\*|\'|\`/g))
	{
		alert(translate_str("JavaScript",20) + " [ ] \\ | : \" ; < > # ? , / + = * \' \`");
		obj.focus();
		return 0;
	}
	var src = new String(obj);
	for(i=0;i<src.length;i++)
	{
		var c = src.charAt(i);
		if(c.charCodeAt(0) < 32 || c.charCodeAt(0) > 127)	{
			alert(translate_str("JavaScript",20));
			obj.focus();
			return 0;
		}
	}
	return 1;
}


function CheckSemicolon (I)
{
	if(I.value.length == 64){
		if(!isxdigit1(I, I.value)) return false;
	}
	else if(I.value.length >=8 && I.value.length <= 63 ){
		if(!isascii(I,I.value)) return false;
	}
	else{
                I.value = I.defaultValue;
                //alert(errmsg.err56);
                alert(translate_str("JavaScript",129));
		return false;
	}
	return true;
}
function isxdigit1(I,M)
{

	for(i=0 ; i<I.value.length; i++){
		ch = I.value.charAt(i).toLowerCase();
		if(ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f'){}
		else{
			I.value = I.defaultValue;
                      alert(M + errmsg.err81);
			return false;
		}
	}
	return true;
}
function isascii(I,M,dfValue)
{
	for(i=0 ; i<I.value.length; i++){
		ch = I.value.charAt(i);
		if(ch < ' ' || ch > '~'){
			I.value = dfValue;
			alert(M + errmsg.err29);
			return false;
		}
	}
	return true;
}

function AssembleSrcStartIP ()
{

	if ( !(IPCheck(document.forms[0].dmz_src_1)) ) {
    		document.forms[0].dmz_src_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].dmz_src_2)) ) {
    		document.forms[0].dmz_src_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].dmz_src_3)) ) {
    		document.forms[0].dmz_src_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck(document.forms[0].dmz_src_4)) ) {
    		document.forms[0].dmz_src_4.value = 0;
    		return false;
  	}
	if ((document.forms[0].dmz_src_1.value==255) &&
	(document.forms[0].dmz_src_2.value==255) &&
	(document.forms[0].dmz_src_3.value==255) &&
	(document.forms[0].dmz_src_4.value==255)
	)
	{
		alert(translate_str("JavaScript",3));
		document.forms[0].dmz_src_1.focus();
		return 0;
	}  	
	if (document.forms[0].dmz_src_4.value==255)
	{
		alert(translate_str("JavaScript",21));
		document.forms[0].dmz_src_4.focus();
		return 0;
	}
	d = (document.forms[0].dmz_src_1.value * 1 ) + '.';
  	d = d + (document.forms[0].dmz_src_2.value * 1 ) + '.';
  	d = d + (document.forms[0].dmz_src_3.value * 1 ) + '.';
  	d = d + (document.forms[0].dmz_src_4.value * 1 );

	document.forms[0].dmz_src_ipaddr_start.value = d;
	return true;
}
function AssembleSrcEndIP ()
{
	if ( !(IPCheck(document.forms[0].dmz_src_4_1)) ) {
    		document.forms[0].dmz_src_4_1.value = 0;
    		return false;
  	}
	if (document.forms[0].dmz_src_4_1.value==255)
	{
		alert(translate_str("JavaScript",21));
		document.forms[0].dmz_src_4_1.focus();
		return 0;
	}
	d = (document.forms[0].dmz_src_1.value * 1 ) + '.';
  	d = d + (document.forms[0].dmz_src_2.value * 1 ) + '.';
  	d = d + (document.forms[0].dmz_src_3.value * 1 ) + '.';
  	d = d + (document.forms[0].dmz_src_4_1.value * 1 );

	document.forms[0].dmz_src_ipaddr_end.value = d;
	return true;
}

function VerifyDmzIP (f)
{
	var dmz_destination_ip=f.cur_ipaddr.value;
	if ( !IPCheck4( f.dmz_ipaddr_4 ) )
    	return 0;		
	d = GetIPSegment ( dmz_destination_ip , 1 )+"."
	   +GetIPSegment ( dmz_destination_ip , 2 )+"."
	   +GetIPSegment ( dmz_destination_ip , 3 )+"."  
	   +f.dmz_ipaddr_4.value;
    f.dmz_dest_ip.value = d;
	return true;
}
function mavis_write(string_name)
{
	document.write(eval("string_name[" + MultiLanguage + "]"));
}
function mavis_str(string_name)
{
	return eval("string_name[" + MultiLanguage + "]");
}
function AssembleRemoteIPAddressStart ()
{
	if ((document.forms[0].rm_ipaddr_1.value == "") && (document.forms[0].rm_ipaddr_2.value == "") &&(document.forms[0].rm_ipaddr_3.value == "") &&(document.forms[0].rm_ipaddr_4.value == ""))
	{
   alert(translate_str("JavaScript",2));
		return 0;		
	}	
	
	if ((document.forms[0].rm_ipaddr_1.value == 0) &&
	    (document.forms[0].rm_ipaddr_2.value == 0) &&
		(document.forms[0].rm_ipaddr_3.value == 0) &&
	    (document.forms[0].rm_ipaddr_4.value == 0)	    
	    )
	{
		 alert(translate_str("JavaScript",2));		
		return 0;		
	}		    
	
	if ( !(IPCheck3(document.forms[0].rm_ipaddr_1)) ) 
	{
    		document.forms[0].rm_ipaddr_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].rm_ipaddr_2)) ) 
	{
    		document.forms[0].rm_ipaddr_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].rm_ipaddr_3)) ) 
	{
    		document.forms[0].rm_ipaddr_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck(document.forms[0].rm_ipaddr_4)) ) 
	{
    		document.forms[0].rm_ipaddr_4.value = 0;
    		return false;
  	}

	d = (document.forms[0].rm_ipaddr_1.value * 1 ) + '.';
  	d = d + (document.forms[0].rm_ipaddr_2.value * 1 ) + '.';
  	d = d + (document.forms[0].rm_ipaddr_3.value * 1 ) + '.';
  	d = d + (document.forms[0].rm_ipaddr_4.value * 1 );

	document.forms[0].remote_mgmt_ip_start.value = d;
	return true;
}
function AssembleRemoteIPAddressEnd ()
{
	if ((document.forms[0].rm_ipaddr_1.value == "") && (document.forms[0].rm_ipaddr_2.value == "") &&(document.forms[0].rm_ipaddr_3.value == "") &&(document.forms[0].rm_ipaddr_4_1.value == ""))
	{
   alert(translate_str("JavaScript",2));
		return 0;		
	}	
	
	if ((document.forms[0].rm_ipaddr_1.value == 0) &&
	    (document.forms[0].rm_ipaddr_2.value == 0) &&
		(document.forms[0].rm_ipaddr_3.value == 0) &&
	    (document.forms[0].rm_ipaddr_4_1.value == 0)	    
	    )
	{
		 alert(translate_str("JavaScript",2));		
		return 0;		
	}		    
	
	if ( !(IPCheck3(document.forms[0].rm_ipaddr_1)) ) 
	{
    		document.forms[0].rm_ipaddr_1.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].rm_ipaddr_2)) ) 
	{
    		document.forms[0].rm_ipaddr_2.value = 0;
    		return false;
  	}

	if ( !(IPCheck1(document.forms[0].rm_ipaddr_3)) ) 
	{
    		document.forms[0].rm_ipaddr_3.value = 0;
    		return false;
  	}

	if ( !(IPCheck(document.forms[0].rm_ipaddr_4_1)) ) 
	{
    		document.forms[0].rm_ipaddr_4_1.value = 0;
    		return false;
  	}
  if ( parseInt(document.forms[0].rm_ipaddr_4_1.value,10) < parseInt(document.forms[0].rm_ipaddr_4.value,10) ) 
	{
    		alert(translate_str("JavaScript",66));
    		return false;
  	}
  

	d = (document.forms[0].rm_ipaddr_1.value * 1 ) + '.';
  	d = d + (document.forms[0].rm_ipaddr_2.value * 1 ) + '.';
  	d = d + (document.forms[0].rm_ipaddr_3.value * 1 ) + '.';
  	d = d + (document.forms[0].rm_ipaddr_4_1.value * 1 );

	document.forms[0].remote_mgmt_ip_end.value = d;
	return true;
}

function SplitRMIPAddress ( IPString )
{
	document.forms[0].rm_ipaddr_1.value = GetIPSegment ( IPString , 1 );
	document.forms[0].rm_ipaddr_2.value = GetIPSegment ( IPString , 2 );
	document.forms[0].rm_ipaddr_3.value = GetIPSegment ( IPString , 3 );
	document.forms[0].rm_ipaddr_4.value = GetIPSegment ( IPString , 4 );
}

function checkNum(I)
{
	txt = I.value;
	for(i=0;i<txt.length;i++)
	{
		c=txt.charAt(i);
		if("0123456789".indexOf(c,0)<0)
		{
			alert(translate_str("JavaScript",22));
			I.focus();
			return 0;
		}
	}
	return 1;
}

function SpecialCharCheck( charField, fldName )
{
	var checkChar = "\\";
	txt = charField.value;
	for (i=0; i<txt.length; i++) {
		c = txt.charAt(i);
		if(checkChar.indexOf(c,0) >= 0) {
			alert(fldName + " " + translate_str("JavaScript",23) + c + " !");
			return false;
		}
	}
	return true;
}

function BlankCheck(charField,fldName)
{
	if (charField.value == "") {
		alert(translate_str("JavaScript",126) + fldName +" .");
		charField.focus();
		return false;
	}
	return true;
}

function translate_strWrite( category, s_number )
{
	document.write(eval(category+"["+s_number+"]"));
}

function translate_str( category, s_number )
{
	return eval(category+"["+s_number+"]");
}

var showit = (document.layers) ? "show" : "visible";
var hideit = (document.layers) ? "hide" : "hidden";
var relateit = "relative";
var absoluteit = "absolute";
var NS4 = (document.layers);
function setVisible(divID, setto)
{
	if (NS4)
		document.layers[divID].visibility = setto;
	else if (document.all)
		document.all(divID).style.visibility = setto;
	else if (document.getElementById)
		document.getElementById(divID).style.visibility = setto;
}

function setPosition(divID, setto)
{
	if (NS4)
		document.layers[divID].position = setto;
	else if (document.all)
		document.all(divID).style.position = setto;
	else if (document.getElementById)
		document.getElementById(divID).style.position = setto;
}
function Capture(obj)
{
	document.write(obj);
}
function valid_range(I,start,end,M,dfValue)
{
	M1 = unescape(M);
	isdigit(I,M1,dfValue);

	d = parseInt(I.value, 10);
	if ( !(d<=end && d>=start) )
	{
		I.value = dfValue;
		alert(M1 + errmsg.err14 + '[' + start + ' - ' + end +']');
		return false;
	}
	else
		I.value = d;	// strip 0
	return true;
}
function getLayer(layerID) 
{
	if (self.parent.document.getElementById)
		return self.parent.document.getElementById(layerID);
	else if (self.parent.document.all)
		return self.parent.document.all[layerID];
	else 
		return null;
}

function MAC_FormatCheck(I)
{
	if(I.value.match(/\:|\-/g))	{
		if(txt = I.value.split(":"))	{
			alert(txt);
		}
		else if (txt = I.value.split("-"))	{
			alert(txt);
		}
		else
			return false;
	}
	return true;
}

// Added by Joseph 2007/7/26
// the parameter ip is a string object
// If ip address is legal, then return the true. Or return false

function IsLegalIPAddress(ip) {  
	var re = new RegExp(/^\d{1,3}(\.\d{1,3}){3}$/);
	var legal = re.test(ip);
	var IPSegment, i;

	if(legal) {
		IPSegment = ip.split(".");
		for(i = 0; i < IPSegment.length; i ++) {
			if(String(Number(IPSegment[i])) != IPSegment[i] || Number(IPSegment[i]) > 255) {
				legal = false;
			}
		}
	}
	return legal;
}

// Reference: RFC 1700
// the parameter ip is a string object
// If ip is lookback address, then return true. Or return false

function IsLoopbackAddress(ip) {
	var addr = IsLegalIpAddress(ip);
	
	if(addr == false) {
		alert(translate_str("JavaScript",68)); 
		return false;
	}
	else if(addr[1] != "127") {
		return false;
	}

	return true;
}

// Reference: RFC 3171
// the parameter ip is a string object
// If ip is multicast address, then return the true. Or return false

function IsMulticastAddress(ip) {
	var addr = IsLegalIpAddress(ip);

	if(addr == false) { 
		alert(translate_str("JavaScript",68)); 
		return false;
	}
	else if(Number(addr[1]) < 224 || Number(addr[1]) > 239) {
		return false;
	}
	return true;
}

function alertContents(httpRequest, passForm) {

        if (httpRequest.readyState == 4) {
            if (httpRequest.status == 200) {
				if(httpRequest.responseText == "tru") {
				//	alert("OK");
					passForm.storage_ftp_port.value = passForm.ftp_port.value;
					passForm.submit();
				}
				else if(httpRequest.responseText == "fals") {
					alert(translate_str("JavaScript",90) + passForm.ftp_port.value + translate_str("JavaScript",91));
				}
            } else {
                alert(translate_str("JavaScript",92));
            }
        }
}

function ToBinary(item) {
	var num = Number(item);
	var result = "";
	while(item != 0) {
		result += Math.floor(item % 2);
		item = Math.floor(item / 2);
	}

	for(i = result.length; i < 8; i ++) {
		result += "0";
	}
	return result.split("").reverse().join("");  // Reverse the string
}

// Added by Joseph 2007/7/24
// To verify whether the subnet address is legal or not.
// ex: IsLegalSubnet("255.255.255.0");  -> return true
//     IsLegalSubnet("255.0.255.0");  -> return false

function IsLegalSubnet(subnet) {
	var binary_subnet = "";
	var i, arr, legal = true;
	for(i = 0, arr = subnet.split("."); i < arr.length; i ++) {
		binary_subnet += ToBinary(arr[i]);
	}
	for(i = 0; i < binary_subnet.length; i ++) {
		if(binary_subnet.charAt(i) != '1') break;
	}
	for(i ++; i < binary_subnet.length; i ++) {
		if(binary_subnet.charAt(i) == '1') {
			legal = false;
			break;
		}
	}
	return legal;
}
// Added by Joseph 2007/7/25
// Verify if the MAC address is legal or not
// Ex:  IsLegalMACAddress("00:DD:EE:DF:0F:FE")   -->  return true
// Ex:  IsLegalMACAddress("00:DD:GE:DF:0F:FE")   -->  return false

function IsLegalMACAddress(str) {
	var re = new RegExp(/^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){5}$/);
	var legal = re.test(str);
	if(legal && (str == "FF:FF:FF:FF:FF:FF" || str == "00:00:00:00:00:00")) {
		legal = false;
	}
	return legal;
}

// Added By Joseph 2007/7/27
// To verify whether the rule is a legal static route or not
// related functions:  IsLegalIPAddress(), HostIDBits(), ToBinary(), _IsLegalSubnet(), 
//			   AllZeroesOrOnes(), IsNetworkID_or_BroadcastIP(), IsLegalRoute()
//
// Main used function: IsLegalRoute()
// Ex:  IsLegalRoute("192.168.1.1", "255.255.255.0")   return 1
// Ex:  IsLegalRoute("192.168.1.0", "255.255.255.0")   return 0
// Ex:  IsLegalRoute("192.168.1.255", "255.255.255.0")   return 0
// Ex:  IsLegalRoute("192.168.1.255", "255.255.255.128")   return 0
// Ex:  IsLegalRoute("192.168.1.127", "255.255.255.128")   return 0
// Ex:  IsLegalRoute("192.168.1.128", "255.255.255.128")   return 0
// Ex:  IsLegalRoute("192.168.1.18", "255.255.255.128")   return 1

// return -1 for loopback address, return -2 for multicast address, return -3 for future use
// otherwise return class' Host ID bits

function HostIDBits(ip) {
	var IPSegment;
	var bits = 0;

	IPSegment = ip.split(".");
	
	if(IPSegment[0] == 127) {
		return -1; // for loopback address
	} else if(IPSegment[0] >= 224 && IPSegment[0] < 240) {
		return -2; // for multicast address
	} else if(IPSegment[0] >= 240) {
		return -3; // for future use
	}

	if(IPSegment[0] < 127) {   //  Class A, by the way, 127 is for loopback address
		bits = 24;
	} else if(IPSegment[0] < 192) {  // Class B
		bits = 16;
	} else if(IPSegment[0] < 224) {  // Class C
		bits = 8;
	} 

	return bits;
}

// To verify whether the subnet address is legal or not.
// ex: _IsLegalSubnet("255.255.255.0", 24);  -> return subnet mask hostID bits
//     _IsLegalSubnet("255.0.255.0", 24);  -> return -1

function _IsLegalSubnet(subnet, bits) {
	var binary_subnet = "";
	var i, arr;
	var subnet_hostID_bits = 0;
	var networkID_bits = 32 - bits;


	//alert("Bit : " + bits);
	for(i = 0, arr = subnet.split("."); i < arr.length; i ++) {
		binary_subnet += ToBinary(arr[i]);
	}
	//alert(binary_subnet);
	for(i = 0; i < binary_subnet.length; i ++) {
		if(binary_subnet.charAt(i) != '1') break;
	}
	subnet_hostID_bits = i;
	//alert("netmask length : " + subnet_hostID_bits);
	for(i ++; i < binary_subnet.length; i ++) {
		if(binary_subnet.charAt(i) == '1') {
			subnet_hostID_bits = -1;
			break;
		}
	}
	
	if(subnet_hostID_bits != -1) {
		if(networkID_bits > subnet_hostID_bits) {
			subnet_hostID_bits = -1;
		} else {
			subnet_hostID_bits -= networkID_bits;
			subnet_hostID_bits = bits - subnet_hostID_bits;
		}
	}

	return subnet_hostID_bits;
}

// All Zeroes return 0,  all Ones return 1, otherwise return 2, 

function AllZeroesOrOnes(binary, subnet_hostID_bits) {  
	var i;
	var result = 2;

	//alert("subnet_hostID_bits : " + subnet_hostID_bits);

	for(i = 0; i < subnet_hostID_bits && binary.charAt(i) == '0'; i ++);
	if(i == subnet_hostID_bits) {
		result = 0;
	}

	//alert("After reversing, binary : " + binary);

	for(i = 0; i < subnet_hostID_bits && binary.charAt(i) == '1'; i ++);
	if(i == subnet_hostID_bits) {
		result = 1;
	}
	return result;
}


//  address_type 0 for Network ID, address_type 1 for Broadcase IP, 2 for normal address

function IsNetworkID_or_BroadcastIP(ip, subnet_hostID_bits) {
	var binary_ip = "";
	var i, arr;
	var address_type = 2;  

	

	for(i = 0, arr = ip.split("."); i < arr.length; i ++) {
		binary_ip += ToBinary(arr[i]);  // binary_ip is a string object
	}
//	alert("Before : " + binary_ip);
	binary_ip = binary_ip.split("").reverse().join("");  // After reversing, it is still a string object
	
	address_type = AllZeroesOrOnes(binary_ip, subnet_hostID_bits);

	return address_type;
}

// 0 for illegal, 1 for legal


function IsLegalRoute(ip, subnetmask) {
	var original_hostID_bits, subnet_hostID_bits;
	var address_type;

	if(!IsLegalIPAddress(ip)) {
		alert(translate_str("JavaScript",138));
		//alert("test");
		return 0;
	}
	alert(ip);
	alert(subnetmask);
	original_hostID_bits = HostIDBits(ip);
	switch(original_hostID_bits) {
		case -1:
//			alert(translate_str("JavaScript",139));
//			return 0;
		case -2:
//			alert(translate_str("JavaScript",140));
//			return 0;
		case -3:
//			alert(translate_str("JavaScript",141));
//			return 0;
			alert(translate_str("ADRouting",19));
			return 0;			
	}
	alert((subnet_hostID_bits = _IsLegalSubnet(subnetmask, original_hostID_bits)));
	if((subnet_hostID_bits = _IsLegalSubnet(subnetmask, original_hostID_bits)) == -1) {
		alert(translate_str("ADRouting",19));
//		alert(translate_str("JavaScript",142));
		return 0;
	}

	if(subnet_hostID_bits == 0) {
		return 1;
	}

	address_type = IsNetworkID_or_BroadcastIP(ip, subnet_hostID_bits);
//	}

	if(address_type == 0) {
		//alert("It's a Network ID.");
		return 1;
	} else {
		alert(translate_str("ADRouting",19));
//		alert(translate_str("JavaScript",98));
		return 0;
	}
}

function AssembleAddress(passForm, str) {
	var address = "";
	
	for(index = 1; index < 4; index ++) {
		address += eval(str + index).value + ".";
	}
	address += eval(str + index).value;

	return address;
}

function chage_color_no_underline(e)
{
	e.style.color="#4F81BD";
	//e.style.textDecoration="underline"; 
}

function back_color_no_underline(e)
{
	e.style.color="#B8B8B8";
	//e.style.textDecoration="none"; 
}