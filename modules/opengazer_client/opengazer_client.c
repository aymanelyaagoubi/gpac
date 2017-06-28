#include <gpac/modules/term_ext.h>
#include <gpac/internal/terminal_dev.h>
#include <gpac/internal/compositor_dev.h>
#include <gpac/options.h>
#include <gpac/nodes_mpeg4.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include <gpac/media_tools.h>


#define SERVER_PORT 20320

#define XMIN 150

#define XMAX 1200

#define YMIN 100

#define YMAX 650

#define SERVER_PORT 20320

 typedef struct

{
 int height;
 int width;
 Bool hasBlinked;
} GAZE;


typedef struct
{
	GF_ObjectManager *odm;
	GF_Terminal *term;
	int sock;
} GF_OPENGAZER;
char message[1024];

struct sockaddr_in name;
int bytes;



static Bool opengazer_process(GF_TermExt *termext, u32 action, void *param)
{

	GF_OPENGAZER *opengazer = termext->udta;
	GAZE* gaze = (GAZE*) malloc(sizeof(GAZE));


	switch (action) {
	case GF_TERM_EXT_START:
	{
		printf("Listen activating.\n");

		opengazer->term = (GF_Terminal *) param;
		opengazer->term->compositor->gazer_enabled = GF_TRUE;
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
		fcntl(opengazer->sock, F_SETFL, fcntl(opengazer->sock, F_GETFL, 0) | O_NONBLOCK);


		printf("Socket has port number #%d\n", ntohs(name.sin_port));

		/*we are not threaded*/
		termext->caps |= GF_TERM_EXTENSION_NOT_THREADED;
	}
		return 1;

	case GF_TERM_EXT_STOP:
		close(opengazer->sock);
		break;

	case GF_TERM_EXT_PROCESS:
		if((bytes = read(opengazer->sock, message, 1024)) > 0) {
			message[bytes] = '\0';

				char msg_1[20];//that will be the first line
				char msg2[20];//that will be the second
				char msg3[20];// that will be the third


				char x[4];
				char y[4];
				char blinking[1];

				int num=strstr(message,"\n") -message;
				strncpy(msg_1,message,num);

				memcpy( x, &message[2], 4 ); //x is being extracted from the first line

				int i=strstr(message,"\n") -message+1;

				int j=0;
				while( message [i] != '\n')
				{

					msg2[j]=message[i];  // here we set msg2 to the second line
					j++;
					i++;
				}
				msg2[j+1]='\0';
				j=0;
			while(message[i+1] != '\0')
			{
				msg3[j]=message[i+1];   // here we set msg3 to the third line
				j++;
				i++;
			}
			msg3[j+1]='\0';

			memcpy( y, &msg2[2], 4 );
			memcpy( blinking, &msg3[5],1);

			sscanf(x,"%d",&(gaze->width));
			sscanf(y,"%d",&(gaze->height));
			int a;
			sscanf(blinking,"%d",&a);

			gaze->hasBlinked = a ? 1 : 0;
			opengazer->term->compositor->gaze_x=gaze->width;
			opengazer->term->compositor->gaze_y=gaze->height;


			     {
					//if look's x is goes beyond XMIN, send a key_left event

			               if ((gaze->width)<XMIN) {

			                 GF_Event event;

			                 memset(&event, 0, sizeof(GF_Event) );

			                 event.type = GF_EVENT_KEYDOWN;

			                 event.key.key_code = GF_KEY_LEFT;

			                 opengazer->term->compositor->video_out->on_event(opengazer->term->compositor->video_out->evt_cbk_hdl, &event);

			         }

			         //if look's x is goes down XMIN, send a key_right event

			               else if (gaze->width>XMAX) {

			                 GF_Event event;

			                 memset(&event, 0, sizeof(GF_Event) );

			                 event.type = GF_EVENT_KEYDOWN;

			                 event.key.key_code = GF_KEY_RIGHT;

			                 opengazer->term->compositor->video_out->on_event(opengazer->term->compositor->video_out->evt_cbk_hdl, &event);

			         }

			         //if look's x is goes beyond XMAX, send a key_up event

			               if (gaze->height<YMIN) {

			                 GF_Event event;

			                 memset(&event, 0, sizeof(GF_Event) );

			                 event.type = GF_EVENT_KEYDOWN;

			                 event.key.key_code = GF_KEY_UP;

			                 opengazer->term->compositor->video_out->on_event(opengazer->term->compositor->video_out->evt_cbk_hdl, &event);

			         }

			         //if look's x is goes down YMIN, send a key_down event

			               else if (gaze->height>YMAX)

			               {

			                 GF_Event event;

			                 memset(&event, 0, sizeof(GF_Event) );

			                 event.type = GF_EVENT_KEYDOWN;

			                 event.key.key_code = GF_KEY_DOWN;

			                 opengazer->term->compositor->video_out->on_event(opengazer->term->compositor->video_out->evt_cbk_hdl, &event);

			               }

			       }

			 /*    Bool gazeTile =GF_FALSE;

			     for (srd in dash)
			     {
			    	 int width = srd[5];
			    	 int height = srd[6];
			    	 int columns = width/srd[3];
			    	 int lines = height/srd[4];
			    	 int i;
			    	 int j;
			    	 for (int l=0;l<width;l++)
			    	 {
			    		 if ((l*width/columns<=gaze->width) &&((l+1)*width/columns>=gaze->width))
			    		 {
			    			 i=l;
			    			 break;
			    		 }
			    	 }
			    	 for (int l=0;l<width;l++)
			    	 {
			    	 	if ((l*height/lines<=gaze->height) &&((l+1)*height/lines=>gaze->height))
			    	 	{
			    	 		j=l;
			    	 		break;
			    	 	}
			    	 }

			     }
			     */



			     if(gaze->hasBlinked) {

			       if (gf_term_get_option(opengazer->term, GF_OPT_PLAY_STATE)==GF_STATE_PLAYING) {

			           gf_term_set_option(opengazer->term, GF_OPT_PLAY_STATE, GF_STATE_PAUSED);

			       }



			       else {

			         gf_term_set_option(opengazer->term, GF_OPT_PLAY_STATE, GF_STATE_PLAYING);

			       }

			     }
		}
		break;
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
