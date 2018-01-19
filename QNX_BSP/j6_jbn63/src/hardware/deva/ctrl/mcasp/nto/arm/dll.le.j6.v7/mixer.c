/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */


struct mcasp_card;

#define MIXER_CONTEXT_T struct mcasp_card
#include <mcasp.h>


/* Required for compatibility with Audioman
 * This switch is called by audio manager to ask deva to send the current HW status, i.e., whether headset is connected
 */
static int32_t
mixer_audioman_refresh_set(MIXER_CONTEXT_T * hw_ctx, ado_dswitch_t * dswitch, snd_switch_t * cswitch, void *instance_data)
{
	return (EOK);
}

static int32_t
mixer_audioman_refresh_get(MIXER_CONTEXT_T * hw_ctx, ado_dswitch_t * dswitch, snd_switch_t * cswitch, void *instance_data)
{
	/* Always return disabled as this switch does not maintain state */
	cswitch->type = SND_SW_TYPE_BOOLEAN;
	cswitch->value.enable = 0;
	return 0;
}

static ado_mixer_reset_t reset_mixer;
static  int32_t
reset_mixer (MIXER_CONTEXT_T * mixer)
{
	return (0);
}

int
codec_mixer (ado_card_t * card, HW_CONTEXT_T * hwc)
{
	int32_t status;

	ado_debug (DB_LVL_MIXER, "Initializing Mixer");

	if ((status = ado_mixer_create (card, "MCASP", &hwc->mixer, hwc)) != EOK)
		return (status);

	/* Add Audioman Switch */
	if (ado_mixer_switch_new(hwc->mixer, "Audioman Refresh", SND_SW_TYPE_BOOLEAN, 0, mixer_audioman_refresh_get,
								mixer_audioman_refresh_set, NULL, NULL) == NULL)
	{
		ado_error("%s: Faild to create Audioman Refresh switch - %s", __FUNCTION__,strerror(errno));
		return -1;
	}

	ado_mixer_set_reset_func (hwc->mixer, reset_mixer);

	return (0);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/deva/ctrl/mcasp/nto/arm/dll.le.j6.v7/mixer.c $ $Rev: 742567 $")
#endif
