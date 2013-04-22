struct share_info
{
	int	share_type; //0:smb, 1:ftp, 2:upnp media server
	char share_partition_name[64];
	char share_partition[64];
	char share_path[128]; 
	char share_name[64];
	char share_read_user[128];
	char share_write_user[128];
}share[40];