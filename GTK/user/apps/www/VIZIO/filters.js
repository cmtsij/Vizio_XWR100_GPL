VPNClientTableURL = new Array("VPNClientTable1.htm");

function CombineScheduleList(f,index)
{
	var schedule="";
	index -= 1;
	var k = index = 1 ;
	if ((eval("f.arnform_"+k).value !="") && 
		(eval("f.days_"+k+"_8").checked==false)&&(eval("f.days_"+k+"_1").checked==false)&&(eval("f.days_"+k+"_2").checked==false)&&(eval("f.days_"+k+"_3").checked==false) &&
		(eval("f.days_"+k+"_4").checked==false)&&(eval("f.days_"+k+"_5")==false)&&(eval("f.days_"+k+"_6").checked==false)&&(eval("f.days_"+k+"_7").checked==false)
		)
	{
		alert(translate_str("JavaScript",5));
		return 0;
	}		
	//if (f.time[1].checked && (!CheckTimeRange(f))) return;
	//if ((!CheckTimeRange(f))) return;
	if (eval("f.days_"+k+"_8").checked)
		schedule = "8:";
	else 
	{
		for (var i=1; i<8 ;i++)
			if (eval("f.days_"+k+"_"+i).checked)
			 schedule += (i + "-");

		if (schedule.charAt(schedule.length-1) == "-")
			schedule = schedule.substring(0,schedule.length-1);
		schedule += ":";
	}

//	if (f.time[0].checked)
//		schedule += ("24");
//	else
//	{
		schedule += ("0:" + eval("f.start_hour_"+k).value 	+ ":");
		schedule += (eval("f.start_min_"+k).value + ":0:");
		schedule += (eval("f.end_hour_"+k).value 	+ ":");
		schedule += (eval("f.end_min_"+k).value + ":");
//	}
	
	var schedulelist = f.ScheduleList.value.split("|");
	var schedules="";
	schedulelist[index] = schedule;	

	for (var i=0;i<10;i++)
	{
		if (schedulelist[i] == "" || schedulelist[i] == null)
			schedulelist[i]=0;
       if (i<9)			
		schedules += (schedulelist[i] + "|");
	   else 
	    schedules +=schedulelist[i]
	}
	document.forms[0].ScheduleList.value = schedules;
	
	return 1;
}

function CombineWeb(f, index, str)
{
	var webobj;
	var webui;
	var webfilter="";
	index -= 1;
		if (str == "url")
		{
			webobj = eval("document.forms[0].URL"+index);
			webui  = f.urladdr;
			for (var i=0; i < webui.length; i++)
				CheckURL(webui[i]);
		}
		else 
		{
			webobj = eval("document.forms[0].Text"+index);
			webui  = f.urlkeyword; 
		}
		for (var i=0; i < webui.length; i++)
		{
			if (webui[i].value=="")
				continue;
			if (!(CheckLegalChar(webui[i])))
				return 0;
			if(i<6)
				webfilter += (webui[i].value + "#");
			else 
				webfilter += (webui[i].value);		
		}
		webobj.value = webfilter;
	return 1;
}

function CheckURL(obj)
{
	var data = obj.value;
	data = data.replace(/http:\/\//g, "");
	data = data.replace(/\//g, "");
	obj.value = data;
}

function CheckLegalChar(obj)
{
	var data = obj.value;

	for (var i=0;i<data.length;i++)
	{
	 	//if (((data.charAt(i)=='.')||(data.charAt(i)=='-')||(data.charAt(i)==':')) && (obj.name == "urladdr"))
	 	if ((data.charAt(i)=='.')||(data.charAt(i)=='-')||(data.charAt(i)==':'))
	 		continue;	
		if (!((data.charAt(i)>='0') && (data.charAt(i)<='9')) &&
			!((data.charAt(i)>='a') && (data.charAt(i)<='z')) &&
			!((data.charAt(i)>='A') && (data.charAt(i)<='Z'))
			)
		{
			alert(translate_str("JavaScript",6));
			obj.focus();
			return 0;
		}
	}
	return 1;
}

function CheckTimeRange(f,k)
{
/*	
	if(f.start_time.value=="AM" && f.end_time.value=="PM") return 1;
	else 
	if(f.start_time.value=="PM" && f.end_time.value=="AM")
	{
		alert(translate_str("JavaScript",7));
		return 0;
	}
*/	
	var starthour = parseInt(eval("f.start_hour_"+k).value, 10);
	var endhour = parseInt(eval("f.end_hour_"+k).value, 10);	
	var startminute = parseInt(eval("f.start_min_"+k).value, 10);
	var endminute = parseInt(eval("f.end_min_"+k).value, 10);	
	if (starthour > endhour)
	{
		alert(translate_str("JavaScript",7));
		eval("f.start_hour_"+k).focus();
		return 0;
	}
	else if ((starthour == endhour) && (startminute >= endminute))
	{
		alert(translate_str("JavaScript",7));
		eval("f.start_hour_"+k).focus();
		return 0;
	}		
	return 1;
}

function CombineInternetFilter(f, index)
{
	
	var filterStatus = f.FilterStatuss.value.split("|"); 
	var filterAllow  = f.FilterAllows.value.split("|"); 
	
	var FS = eval("f.filterStatus_"+index).checked ;
	var FA = eval("f.filterAllow_"+index).value ;
	
	var filterStatusList="";
	var filterAllowList="";
	index-=1;

	if (FS == true)
	{
		filterStatus[index] = 1;
		filterStatus[index] = 1;	
	}
	else
	{
		filterStatus[index] = 0;
		filterStatus[index] = 0;
	}
	
	//if (eval("f.filterAllow_"+index).value == '0')
	if (FA == '0')
	{
		filterAllow[index] = 0;
		filterAllow[index] = 0;
	}
	else
	{
		filterAllow[index] = 1;
		filterAllow[index] = 1;	
	}
	
	for(var i=0;i<10;i++)
	{
		if (filterStatus[i] == null || filterStatus[i] =="")
			filterStatus[i] = 0;
		if (filterAllow[i] == null || filterAllow[i] == "")
			filterAllow[i] = 0;				
		if (i!=9)
		{
			filterStatusList += (filterStatus[i]+"|");
			filterAllowList  += (filterAllow[i]+"|");

		}
		else
		{
			filterStatusList += (filterStatus[i]+"|");
			filterAllowList  += (filterAllow[i]+"|");
		}
	}
	f.FilterStatuss.value = filterStatusList;
	f.FilterAllows.value = filterAllowList;	

}

function ShowWeb(f, index, str)
{

	var webobj;
	var webui;
	index -= 1;
	if (str == "url")
	{
		webobj = eval("f.URL"+index);
		webui  = f.urladdr;
	}
	else
	{
		webobj = eval("f.Text"+index);
		webui  = f.urlkeyword;
	}
	
	webfilter = webobj.value.split("#");	
	for	(var i=0;i<webui.length; i++)
	{
		webui[i].value = "";
	}
	
	for (var i=0;i<webfilter.length-1; i++)
	{
		if (webfilter[i]==null)
			break;
			
		webui[i].value = webfilter[i];
	}		
}

function url_check(f)
{
	var urlreg=/^[A-Za-z0-9]+\.[A-Za-z0-9]+[\/=\?%\-&_~`@[\]\':+!]*([^<>\"\"])*$/
	var url=f.value
		
	if (url == "")
		return true;
	
	if (!urlreg.test(url)){
		alert(translate_str("JavaScript",74));
		f.focus();
		return false;
	}
	else
		return true;

}