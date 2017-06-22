/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Authors: Jean Le Feuvre
 *			Copyright (c) Telecom ParisTech 2011-2012
 *			All rights reserved
 *
 *  This file is part of GPAC / Sampe On-Scvreen Display sub-project
 *
 *  GPAC is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  GPAC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <gpac/modules/term_ext.h>
#include <gpac/internal/terminal_dev.h>
#include <gpac/internal/compositor_dev.h>
#include <gpac/nodes_mpeg4.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_PORT 20320


typedef struct
{
	GF_ObjectManager *odm;
	GF_Terminal *term;
	int sock;
} GF_OPENGAZER;
char message[1024];

struct sockaddr_in name;
struct hostent *hp, *gethostbyname();
int bytes;



static Bool opengazer_process(GF_TermExt *termext, u32 action, void *param)
{
	const char *opt;
	GF_OPENGAZER *opengazer = termext->udta;

	switch (action) {
	case GF_TERM_EXT_START:
				{printf("Listen activating.\n");

				/* Create socket from which to read */
				opengazer->sock = socket(AF_INET, SOCK_DGRAM, 0);
				if (opengazer->sock < 0)   {
					perror("Opening datagram socket");
					exit(1);
				}

				/* Bind our local address so that the client can send to us */
				bzero((char *) &name, sizeof(name));
				name.sin_family = AF_INET;
				name.sin_addr.s_addr = htonl(INADDR_ANY);
				name.sin_port = htons(SERVER_PORT);

				if (bind(opengazer->sock, (struct sockaddr *) &name, sizeof(name))) {
					perror("binding datagram socket");
					exit(1);
				}

				printf("Socket has port number #%d\n", ntohs(name.sin_port));

				}

	case GF_TERM_EXT_STOP:
		close(opengazer->sock);

	case GF_TERM_EXT_PROCESS:
	if((bytes = read(opengazer->sock, message, 1024)) > 0) {
		message[bytes] = '\0';
		printf("%s\n", message);
	}
	}
	return 0;
}


GF_TermExt *opengazer_new()
{
	GF_TermExt *dr;
	GF_OPENGAZER *opengazer;
	dr = (GF_TermExt*)gf_malloc(sizeof(GF_TermExt));
	memset(dr, 0, sizeof(GF_TermExt));
	GF_REGISTER_MODULE_INTERFACE(dr, GF_TERM_EXT_INTERFACE, "GPAC OnScreen Display", "gpac distribution");

	GF_SAFEALLOC(opengazer, GF_OPENGAZER);
	dr->process = opengazer_process;
	dr->udta = opengazer;
	return dr;
}


void opengazer_delete(GF_BaseInterface *ifce)
{
	GF_TermExt *dr = (GF_TermExt *) ifce;
	GF_OPENGAZER *opengazer = dr->udta;
	gf_free(opengazer);
	gf_free(dr);
}

GPAC_MODULE_EXPORT
const u32 *QueryInterfaces()
{
	static u32 si [] = {
		GF_TERM_EXT_INTERFACE,
		0
	};
	return si;
}

GPAC_MODULE_EXPORT
GF_BaseInterface *LoadInterface(u32 InterfaceType)
{
	if (InterfaceType == GF_TERM_EXT_INTERFACE) return (GF_BaseInterface *)opengazer_new();
	return NULL;
}

GPAC_MODULE_EXPORT
void ShutdownInterface(GF_BaseInterface *ifce)
{
	switch (ifce->InterfaceType) {
	case GF_TERM_EXT_INTERFACE:
		opengazer_delete(ifce);
		break;
	}
}

GPAC_MODULE_STATIC_DECLARATION( opengazer )
