function CheckingFrag (obj)
{
	if ( obj.value == "" ) {
  		alert(translate_str("JavaScript",24));
		obj.focus();
  		return false;
	}
	
	if ( isNaN(obj.value) || obj.value < 256 || obj.value > 2346)  {
		alert(translate_str("JavaScript",25));
		obj.focus();		
		return false;
	}
	return true;
}

function CheckingRTSThreshold (obj)
{
	if ( obj.value == "" ) {
  		alert(translate_str("JavaScript",26));
  		obj.focus();
  		return false;
	}

	if ( isNaN(obj.value) || obj.value < 0 || obj.value > 2347) {
		alert(translate_str("JavaScript",28));
		obj.focus();
		return false;
	}
	
	return true;
}

function CheckingDTIM (obj)
{
	if ( obj.value == "" ) {
		alert(translate_str("JavaScript",27));
		obj.focus();
		return false;
	}

	if (isNaN(obj.value) || obj.value < 1 || obj.value > 255) {
		alert(translate_str("JavaScript",29));
		obj.focus();
		return false;
	}
	

	return true;
}

function CheckingBeaconInterval (obj)
{
	if ( obj.value == "" ) {
  		alert(translate_str("JavaScript",30));
  		obj.focus();
  		return false;
	}

	if (isNaN(obj.value) || obj.value < 1 || obj.value > 65535)  {
  		alert(translate_str("JavaScript",31));
  		obj.focus();
  		return false;
	}
	

	return true;
}