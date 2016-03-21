/**
 * @file
 * LightWeightM2M LWM2M client led application.
 *
 * @author Imagination Technologies
 *
 * @copyright Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group
 * companies and/or licensors.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/***************************************************************************************************
 * Includes
 **************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

#include "dev/leds.h"
#include "button-sensor.h"

#include "lwm2m_core.h"
#include "lwm2m_object_store.h"
#include "coap_abstraction.h"
#include "client/lwm2m_bootstrap.h"
#include "client/lwm2m_registration.h"
#include "client/lwm2m_device_object.h"
#include "client/lwm2m_security_object.h"
#include "client/lwm2m_server_object.h"

#include "lwm2m-client-flow-object.h"
#include "lwm2m-client-flow-access-object.h"
#include "lwm2m-client-ipso-light-control.h"

/***************************************************************************************************
 * Definitions
 **************************************************************************************************/

//! \{
#define COAP_PORT			(6000)
#define IPC_PORT			(12345)
#define BOOTSTRAP_PORT		"15683"
#define END_POINT_NAME		"LedDevice"
//! \}

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

/**
 * A structure to contain lwm2m client's options.
 */
typedef struct
{
	//! \{
	int CoapPort;
	int IpcPort;
	bool Verbose;
	char * EndPointName;
	char * BootStrap;
	//! \}
} Options;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

//! \{
static const unsigned char _LED1 = LEDS_RED;
static const unsigned char _LED2 = LEDS_GREEN;

Options options =
{
	.CoapPort = COAP_PORT,
	.IpcPort = IPC_PORT,
	.Verbose = true,
	.BootStrap = "coap://["BOOTSTRAP_IPv6_ADDR"]:"BOOTSTRAP_PORT"/",
	.EndPointName = END_POINT_NAME,
};

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

void LED_callback(void * context, bool OnOff, unsigned char Dimmer, const char * Colour)
{
	unsigned char led = *((unsigned char *)context);

	if(led == _LED1 || led == _LED2)
	{
		Lwm2m_Debug("Setting LED %d state to %s\n", led == _LED1 ? 1 : 2, OnOff ? "on" : "off");

		if(OnOff)
			leds_on(led);
		else
			leds_off(led);
	}
}

void ConstructObjectTree(Lwm2mContextType * context)
{
	Lwm2m_Debug("Construct object tree\n");

	Lwm2m_RegisterSecurityObject(context);
	if (options.BootStrap != NULL)
	{
		Lwm2m_PopulateSecurityObject(context, options.BootStrap);
	}
	Lwm2m_RegisterServerObject(context);
	Lwm2m_RegisterDeviceObject(context);

	Lwm2m_RegisterFlowObject(context);
	Lwm2m_RegisterFlowAccessObject(context);

	LightControl_RegisterLightControlObject(context);
	LightControl_AddLightControl(context, 0, LED_callback, (void*)&_LED1);
	LightControl_AddLightControl(context, 1, LED_callback, (void*)&_LED2);
}

Lwm2mContextType * Lwm2mClient_Start()
{
	Lwm2m_SetOutput(stdout);
	Lwm2m_SetLogLevel((options.Verbose) ? DebugLevel_Debug : DebugLevel_Info);
	Lwm2m_PrintBanner();
	Lwm2m_Info("LWM2M client - CoAP port %d\n", options.CoapPort);
	Lwm2m_Info("LWM2M client - IPC port %d\n", options.IpcPort);

	CoapInfo * coap = coap_Init("0.0.0.0", options.CoapPort,
		(options.Verbose) ? DebugLevel_Debug : DebugLevel_Info);
	Lwm2mContextType * context = Lwm2mCore_Init(coap, options.EndPointName);
	ConstructObjectTree(context);

	return context;
}

PROCESS(lwm2m_client, "LwM2M Client");
AUTOSTART_PROCESSES(&lwm2m_client);

PROCESS_THREAD(lwm2m_client, ev, data)
{
	PROCESS_BEGIN();

	PROCESS_PAUSE();

	Lwm2m_Info("Starting LWM2M Client for lwm2m-client-led-actuator\n");

#ifdef RF_CHANNEL
	Lwm2m_Info("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
	Lwm2m_Info("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

	Lwm2m_Info("uIP buffer: %u\n", UIP_BUFSIZE);
	Lwm2m_Info("LL header: %u\n", UIP_LLH_LEN);
	Lwm2m_Info("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
#ifdef REST_MAX_CHUNK_SIZE
	Lwm2m_Info("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);
#endif

	uip_ipaddr_t ipaddr;
	uip_ip6addr(&ipaddr, BOOTSTRAP_IPv6_ADDR1, BOOTSTRAP_IPv6_ADDR2, BOOTSTRAP_IPv6_ADDR3,
		BOOTSTRAP_IPv6_ADDR4, BOOTSTRAP_IPv6_ADDR5, BOOTSTRAP_IPv6_ADDR6, BOOTSTRAP_IPv6_ADDR7,
		BOOTSTRAP_IPv6_ADDR8);
	uip_ds6_defrt_add(&ipaddr, 0);

	static Lwm2mContextType * context;

	context = Lwm2mClient_Start();

	/* Define application-specific events here. */
	while(1)
	{
		static struct etimer et;
		static int WaitTime;
		WaitTime = Lwm2mCore_Process(context);
		etimer_set(&et, (WaitTime * CLOCK_SECOND) / 1000);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et) || (ev == sensors_event));

		//FIXME: this is crude but is a proof of concept for incrementing On Time for
		//IPSO light control
		LightControl_IncrementOnTime(context, 0, 1);
		LightControl_IncrementOnTime(context, 1, 1);
	}

	PROCESS_END();
}
//! \}
