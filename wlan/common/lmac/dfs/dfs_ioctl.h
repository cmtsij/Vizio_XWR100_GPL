/*
 * Copyright (c) 2009, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */

/*
 * ioctl defines
 */

#define	DFS_MUTE_TIME		1
#define	DFS_SET_THRESH		2
#define	DFS_GET_THRESH		3
#define	DFS_GET_USENOL		4
#define DFS_SET_USENOL		5
#define DFS_RADARDETECTS	6
#define	DFS_BANGRADAR		7 
#define	DFS_SHOW_NOL		8 
#define	DFS_DISABLE_DETECT	9 
#define	DFS_ENABLE_DETECT	10 
#define	DFS_DISABLE_FFT	        11 
#define	DFS_ENABLE_FFT  	12 
#define	DFS_SET_DEBUG_LEVEL  	13 

/* 
 * ioctl parameter types
 */

#define	DFS_PARAM_FIRPWR	1
#define	DFS_PARAM_RRSSI		2
#define	DFS_PARAM_HEIGHT	3
#define	DFS_PARAM_PRSSI		4
#define	DFS_PARAM_INBAND	5
//5413 specific parameters
#define DFS_PARAM_RELPWR        7
#define DFS_PARAM_RELSTEP       8
#define DFS_PARAM_MAXLEN        9

struct dfs_ioctl_params {
	int32_t		dfs_firpwr;	/* FIR pwr out threshold */
	int32_t		dfs_rrssi;	/* Radar rssi thresh */
	int32_t		dfs_height;	/* Pulse height thresh */
	int32_t		dfs_prssi;	/* Pulse rssi thresh */
	int32_t		dfs_inband;	/* Inband thresh */
	int32_t		dfs_relpwr;	/* pulse relative pwr thresh */
	int32_t		dfs_relstep;	/* pulse relative step thresh */
	int32_t		dfs_maxlen;	/* pulse max duration */
};

#define	DFS_IOCTL_PARAM_NOVAL	-65535
