#ifdef __COUNTRY_DE
	#define LOG_LOG_MSG_1 fprintf(fp ," Erhalten Sie IP ADDRESS %s Vom DHCP-Bediener => %s\n", ipaddr, ctime(&t));
	#define LOG_LOG_MSG_3 fprintf(fp ," Erhalten Sie IP ADDRESS %s Vom PPPoE-Bediener => %s\n", ipaddr, ctime(&t));
	#define LOG_LOG_MSG_4 fprintf(fp ," Erhalten Sie IP ADDRESS %s Vom PPTP-Bediener => %s\n", ipaddr, ctime(&t));
  #define LOG_LOG_MSG_2 fprintf(fp ," Falscher Benutzer-LOGON : Username ist %s, Kennwort ist %s Von %s=> %s\n",authinfo, authpass, inet_ntoa(client_ip), ctime(&t));
	#define LOG_LOG_MSG_5 fprintf(fp ,"Benutzer Anmeldung von %s => %s\n",inet_ntoa(client_ip), ctime(&t));
	#define LOG_LOG_MSG_6 fprintf(fp ,"Benutzer Anmeldungsfehler von %s => %s\n",inet_ntoa(client_ip), ctime(&t));
#elif __COUNTRY_EU
	#define LOG_LOG_MSG_1 fprintf(fp ,"Get IP Address %s From DHCP Server => %s\n", ipaddr, ctime(&t));
	#define LOG_LOG_MSG_3 fprintf(fp ,"Get IP Address %s From PPPoE Server => %s\n", ipaddr, ctime(&t));
	#define LOG_LOG_MSG_4 fprintf(fp ,"Get IP Address %s From PPTP Server => %s\n", ipaddr, ctime(&t));
  #define LOG_LOG_MSG_2 fprintf(fp ,"Incorrect User login : Username is %s, Password is %s From %s=> %s\n",authinfo, authpass, inet_ntoa(client_ip), ctime(&t));
	#define LOG_LOG_MSG_5 fprintf(fp ,"User Login From %s => %s\n",inet_ntoa(client_ip), ctime(&t));
	#define LOG_LOG_MSG_6 fprintf(fp ,"User Login Error From %s => %s\n",inet_ntoa(client_ip), ctime(&t));
#elif __COUNTRY_FR
	#define LOG_LOG_MSG_1 fprintf(fp ," Obtenez le IP ADDRESS %s Du Serveur de DHCP => %s\n", ipaddr, ctime(&t));
	#define LOG_LOG_MSG_3 fprintf(fp ," Obtenez le IP ADDRESS %s Du Serveur de PPPoE => %s\n", ipaddr, ctime(&t));
	#define LOG_LOG_MSG_4 fprintf(fp ," Obtenez le IP ADDRESS %s Du Serveur de PPTP => %s\n", ipaddr, ctime(&t));
  #define LOG_LOG_MSG_2 fprintf(fp ," Ouverture incorrecte d'utilisateur : Le username est %s, Le mot de passe est %s De %s=> %s\n",authinfo, authpass, inet_ntoa(client_ip), ctime(&t));
	#define LOG_LOG_MSG_5 fprintf(fp ,"User Login From %s => %s\n",inet_ntoa(client_ip), ctime(&t));
	#define LOG_LOG_MSG_6 fprintf(fp ,"User Login Error From %s => %s\n",inet_ntoa(client_ip), ctime(&t));
#else
	#define LOG_LOG_MSG_1 fprintf(fp ,"Get IP Address %s From DHCP Server => %s\n", ipaddr, ctime(&t));
	#define LOG_LOG_MSG_3 fprintf(fp ,"Get IP Address %s From PPPoE Server => %s\n", ipaddr, ctime(&t));
	#define LOG_LOG_MSG_4 fprintf(fp ,"Get IP Address %s From PPTP Server => %s\n", ipaddr, ctime(&t));
  #define LOG_LOG_MSG_2 fprintf(fp ,"Incorrect User login : Username is %s, Password is %s From %s=> %s\n",authinfo, authpass, inet_ntoa(client_ip), ctime(&t));
  #define LOG_LOG_MSG_5 fprintf(fp ,"User Login From %s => %s\n",inet_ntoa(client_ip), ctime(&t));
	#define LOG_LOG_MSG_6 fprintf(fp ,"User Login Error From %s => %s\n",inet_ntoa(client_ip), ctime(&t));
#endif
