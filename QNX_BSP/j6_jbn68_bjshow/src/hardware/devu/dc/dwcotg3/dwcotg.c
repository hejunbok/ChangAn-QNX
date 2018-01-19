/*
 * $QNXLicenseC: 
 * Copyright 2011, QNX Software Systems.  
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

/* 
 * Notes :
 * 
 * The only control endpoint allowed is on EP0, which uses
 * epidx0 and epidx1.  Further work is required to support control endpoints 
 * in a more generic fashion
 *
 * Isoch is not supported.
 *
 * simple mapping between endpoint numbers and endpoint index... 
 * . 	#define EPIDX( epnum, epdir )		( 2*(epnum) + ((epdir)?1:0) )
 * A more complicated scheme could be implemented to add flexibility, but
 * I don't think its warranted at this point
 *
 * The Control tranfer state machine is a little simplified, but seems to be working
 * The simplified version needs to be tested under all major operating systems
 *
 * Hacks: Currently uses a delayed execution handler to punt up the first setup packet
 * to avoid a race condtion with the stack extract code... talk to Henry about
 * this
 *
 */
 
 

 
#include <descriptors.h>

#include "dwcotg.h"

extern char usbdc_config_descriptor[];


static iousb_pipe_methods_t dwcotg_control_pipe_methods = {
	dwcotg_control_endpoint_enable,
	dwcotg_control_endpoint_disable,
	dwcotg_control_transfer,
	dwcotg_control_transfer_abort,
	NULL
};

static iousb_pipe_methods_t dwcotg_interrupt_pipe_methods = {
	dwcotg_endpoint_enable,
	dwcotg_endpoint_disable,
	dwcotg_transfer,
	dwcotg_transfer_abort,
	NULL
};

static iousb_pipe_methods_t dwcotg_bulk_pipe_methods = {
	dwcotg_endpoint_enable,
	dwcotg_endpoint_disable,
	dwcotg_transfer,
	dwcotg_transfer_abort,
	NULL
};

static iousb_pipe_methods_t dwcotg_isoch_pipe_methods = {
	dwcotg_isoch_endpoint_enable,
	dwcotg_isoch_endpoint_disable,
	dwcotg_isoch_transfer,
	dwcotg_isoch_transfer_abort,
	NULL
};


dc_methods_t dwcotg_controller_methods = {
		20,
		dwcotg_init,
		dwcotg_start,
		dwcotg_stop,
		dwcotg_shutdown,
		NULL,
		NULL,    
		dwcotg_set_bus_state,
		dwcotg_set_device_feature,
		dwcotg_clear_device_feature,
		dwcotg_set_device_address,
		dwcotg_get_descriptor,
		dwcotg_select_configuration,
		dwcotg_interrupt,
		dwcotg_set_endpoint_state,
		dwcotg_clear_endpoint_state,
		dwcotg_set_sel,
		&dwcotg_control_pipe_methods,
		&dwcotg_interrupt_pipe_methods,
		&dwcotg_bulk_pipe_methods,
		&dwcotg_isoch_pipe_methods,
};

usb_controller_methods dwcotg_usb_controller_methods = {
	NULL,
	&dwcotg_controller_methods,
	NULL,
	NULL
};

io_usb_dll_entry_t io_usb_dll_entry = {
	USBDC_DLL_NAME,
	0xffff,  // pci device id
	0xffff,
	0xffff,
	USB_CONTROLLER_DEVICE,
	NULL,
	NULL,
	&dwcotg_usb_controller_methods
};




int 
dwcotg_slogf( dctrl_t * dc, int level, const char *fmt, ...)
{
	int rc;
	va_list arglist;
	
	if ( dc && ( level > dc->verbosity ) ) 
		return( 0 ); 
	
	va_start( arglist, fmt );
	rc = vslogf( 12, level, fmt, arglist );
	va_end( arglist );
	return( rc );
}


int dwcotg_xfer_start( dctrl_t * dc, trb_cluster_t * trb_cluster, ep_ctx_t * ep, uint32_t epidx ) {
	cmd_t          epcmd;
	int            rc;

	/* The current implementation only supports 1 active transfer per endpoint at a time.
	 * Given this limitation, this check for no active xfers makes sense.  If we want to add multiple
	 * xfers per endpoint, then the transfer functions  need to re-implemented 
	 * to use a trb hooking model or something...... maybe dwcotg_xfer_start() to get things
	 * started, then xfer_hook(trb) or something
	 */
	 
	if ( ( ep->flags & EPFLAG_ENABLED ) && !( ep->flags & EPFLAG_XFER_ACTIVER ) ) {
		ep->flags |= EPFLAG_XFER_ACTIVER;

		DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: epidx = %d control = 0x%x bufsz_sts = 0x%x paddr_hi = 0x%x paddr_lo = 0x%x "
			, __func__, epidx, trb_cluster->trb_arr[0].control, trb_cluster->trb_arr[0].bufsz_sts,
			trb_cluster->trb_arr[0].bufptr_high, trb_cluster->trb_arr[0].bufptr_low );

		epcmd_build_DEPSTRTXFER( &epcmd, trb_cluster );
		rc = epcmd_send( dc, ep, epidx, &epcmd, 0 );
		if ( rc != EOK ) {
			dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send epcmd_build_DEPSTRTXFER command to epidx = %d  ", __func__, epidx );
		}
		ep->xfer_rsc_idx = epcmd.xfer_rsc_idx;
		
	} else {
		rc = -1;
		if ( !( ep->flags & EPFLAG_ENABLED ) ) {
			dwcotg_slogf(dc, _SLOG_ERROR, "%s: endpoint isn't enabled... cannot start new transfer ", __func__, epidx );
		} else if ( ep->flags & EPFLAG_XFER_ACTIVER ) {
			dwcotg_slogf(dc, _SLOG_ERROR, "%s: endpoint already has an active transfer... cannot start new transfer ", __func__, epidx );
		}
	}
	
	return rc;
}


int dwcotg_setup_packet_get( dctrl_t * dc ) {
	ep_ctx_t * ep = &dc->ep_arr[0];
	int        rc;
	
	trb_build_ctrl_setup( ep->trb_cluster, ep->setup_packet_paddr64 );
	rc = dwcotg_xfer_start( dc, ep->trb_cluster, ep, 0 );
	if ( rc != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to hook setup_packet TRB on epidx -  ", __func__ );		
	}
	
	return rc;
}


int dwcotg_xfer_abort( dctrl_t * dc, ep_ctx_t *ep, int epidx ) {
	cmd_t			epcmd;	
	int             rc;

	DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG2, "%s: aborting epidx = %d xfer_rsc_idx = %d", __func__, epidx, ep->xfer_rsc_idx );
	
	epcmd_build_DEPENDXFER( &epcmd, ep->xfer_rsc_idx );
	rc = epcmd_send( dc, ep, epidx, &epcmd, 0 );
	if ( rc != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPENDXFER command to epidx = %d ( rc = %d )", __func__, epidx, rc );
	}
	
	return rc;
}


uint32_t dwcotg_interrupt( usbdc_device_t *udc )
{
	dctrl_t  	        *dc = udc->dc_data;
	uint32_t            evntcount_bytes, count;
	uint32_t	        raw_event;
	evt_t               decoded_event;
	volatile uint32_t   *eventq_mem = dc->eventq_mem;
	int                 timeout;
	
	while ( ( count = evntcount_bytes = ( HW_Read32( dc->IoBase, GEVNTCOUNT ) & EVNTCOUNT_MSK  ) ) ) {	

		if ( count % DWCOTG_EVT_SZ ) {
			dwcotg_slogf(dc, _SLOG_ERROR, "%s: evntcount_bytes = %d, which is not modulo %d", __func__, count, DWCOTG_EVT_SZ );
		}

		while ( count ) {

			
			timeout = 10;
			while ( !( raw_event = eventq_mem[ dc->eventq_cur_elem ] ) && timeout ) {
				dwcotg_slogf(dc, _SLOG_ERROR, "%s: raw_event has not been updated by the hardware!!! timeout = %d", __func__, timeout );
				nanospin_ns(100);
				timeout--;				
			}
		
			DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: count = %d raw = 0x%x", __func__, count, raw_event );

			event_decode( dc, raw_event, &decoded_event ); 
			event_process( dc, &decoded_event );
			
			// zero out the event so we know when the hardware has actually updated the memory next time
			eventq_mem[ dc->eventq_cur_elem ] = 0;
			
			/*
			 * future: add handling for "Vendor Device Test LMP Received" event
			 * which is the only event which isn't 4 bytes.  For now, only
			 * support 4-byte events because the LMP Received event isn't enabled
			 */
			
			dc->eventq_cur_elem = ( dc->eventq_cur_elem + 1 ) % DWCOTG_N_EVT;
			count -= DWCOTG_EVT_SZ;
		}
		
		// Tell the hardware how many events were processed
		HW_Write32( dc->IoBase, GEVNTCOUNT, evntcount_bytes );
	}
	

	return EOK;
}



static void * delayed_execution_handler( dctrl_t * dc ) {
	struct _pulse		pulse;
	ep_ctx_t 	*ep = &dc->ep_arr[0];

	while( 1 ) {
		if( ( MsgReceivePulse( dc->chid, &pulse, sizeof( pulse ), NULL ) ) == -1 ) {
			continue;
		}

		switch( pulse.code ) {
			case PULSE_DELAY_SETUP_PKT:
				dwcotg_slogf( dc, _SLOG_INFO, "%s() : PULSE_DELAY_SETUP_PKT received", __func__ );
				dc->udc->usbdc_self->usbdc_setup_packet_process( dc->udc, ep->setup_packet );

				break;
			default:
				dwcotg_slogf( dc, _SLOG_ERROR, "%s() : Unknown pulse code %d", __func__ , pulse.code );
				break;
		}
	}	

	return NULL;
}


uint32_t
dwcotg_set_device_address(  usbdc_device_t *udc, uint32_t address )
{
	dctrl_t  	*dc = udc->dc_data;
	ep_ctx_t 	*ep = &dc->ep_arr[0];
	int         err;
		
	dwcotg_slogf(dc, _SLOG_INFO, "%s: address = 0x%x", __func__, address);
	
	/* designware datasheet table 9-4: Initialization on SetAddress Request */

	/* Step 1:
	 * DCFG: Program the DCFG register with the device address received as part 
	 * of the SetAddress request when SETUP packet is decoded.
	 */
	HW_Write32Or( dc->IoBase, DCFG,  address << DEVADDR_POS  );
	delay(1);
	
	
	/* Step 2:  
	 *  DEPCMD1 : After receiving the XferNotReady(Status) event, acknowledge 
	 * the status stage by issuing a DEPSTRTXFER command pointing to a Status TRB. 
	 * This step must be done after the DCFG register is programmed with the new 
	 * device address
	 *
	 */

	ep->control_phase = CONTROL_PHASE_STATUS;
	trb_build_ctrl_status2( ep->trb_cluster );

	err = dwcotg_xfer_start( dc, ep->trb_cluster, ep, 1 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to hook status2 trb on epidx 1 to complete status-IN phase  ", __func__ );		
	}
	
	
	
	return EOK; 
}


uint32_t
dwcotg_select_configuration( usbdc_device_t *udc, uint8_t config )
{
	dctrl_t  		*dc = udc->dc_data;
	ep_ctx_t        *ep = &dc->ep_arr[0];
	cmd_t			epcmd;	
	int             err;
	
		
	dwcotg_slogf(dc, _SLOG_INFO, "%s: config = 0x%x", __func__, config);

	/* Issue a DEPCFG command ( with ' Ignore Sequence Numer field set to 1 ) for
	 * physical endpoint 1 using current endpoint characteristics to re-initialize the TX FIFO allocation
	 */
	
	epcmd_build_DEPCFG( &epcmd, ep, 1, 1 );
	err = epcmd_send( dc, ep, 1, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPCFG command to epidx = 1 ", __func__ );
	}
	
	/* Issue a DEPSTARTCFG command  with XferRscIdx set to 2 to re-initialize the transfer resource allocation */	
	epcmd_build_DEPSTARTCFG( &epcmd,2 );
	err = epcmd_send( dc, ep, 0, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPSTARTCFG command to epidx = 0 ", __func__ );
	}
	
	
	
	return EOK;
}


uint32_t
dwcotg_set_endpoint_state( usbdc_device_t *udc, iousb_endpoint_t *iousb_ep, uint32_t ep_state )
{
	dctrl_t 		*dc 		= udc->dc_data;
	ep_ctx_t		*ep = iousb_ep->user;
	cmd_t			epcmd;	
	int             err;
	
	dwcotg_slogf(dc, _SLOG_INFO, "%s: state = %d",__func__, ep_state );
	
	
	switch ( ep_state ) {
		case IOUSB_ENDPOINT_STATE_READY :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_READY epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		
		case IOUSB_ENDPOINT_STATE_STALLED :
			dwcotg_slogf(dc, _SLOG_INFO, "          STALL epnum = 0x%x",  iousb_ep->edesc.bEndpointAddress);

			epcmd_build_DEPSETSTALL( &epcmd );
			err = epcmd_send( dc, ep, ep->idx, &epcmd, 0 );
			if ( err != EOK ) {
				dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPSETSTALL command to epidx = %d ", __func__, ep->idx );
			}

			ep->flags |= EPFLAG_STALLED;

			if ( ep->num == 0 ) {
				// Get ready for next setup packet.
				ep->control_phase = CONTROL_PHASE_SETUP;
				dwcotg_setup_packet_get( dc );
			}
			
			break;
		
		case IOUSB_ENDPOINT_STATE_RESET :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_RESET epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		case IOUSB_ENDPOINT_STATE_ENABLE :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_ENABLE epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		case IOUSB_ENDPOINT_STATE_DISABLED :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_DISABLED epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		
		case IOUSB_ENDPOINT_STATE_NAK :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_NAK epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		
		default :
			break;
	}
	
	
	return EOK;
}

uint32_t
dwcotg_clear_endpoint_state( usbdc_device_t *udc, iousb_endpoint_t *iousb_ep, uint32_t ep_state )
{
	dctrl_t 		*dc = udc->dc_data;
	ep_ctx_t		*ep = iousb_ep->user;
	cmd_t			epcmd;	
	int             err;
	
	switch ( ep_state ) {
		case IOUSB_ENDPOINT_STATE_READY :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_READY  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		
		case IOUSB_ENDPOINT_STATE_STALLED :
			dwcotg_slogf(dc, _SLOG_INFO, "          Clear STALL  epnum = 0x%x",  iousb_ep->edesc.bEndpointAddress);

			epcmd_build_DEPCSTALL( &epcmd );
			err = epcmd_send( dc, ep, ep->idx, &epcmd, 0 );
			if ( err != EOK ) {
				dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPCSTALL command to epidx = %d ", __func__, ep->idx );
			}
			ep->flags &= ~EPFLAG_STALLED;
			
			break;
		
		case IOUSB_ENDPOINT_STATE_RESET :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_RESET  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		case IOUSB_ENDPOINT_STATE_ENABLE :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_ENABLE  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		case IOUSB_ENDPOINT_STATE_DISABLED :
			dwcotg_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_DISABLED  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		case IOUSB_ENDPOINT_STATE_NAK :		
			dwcotg_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_NAK  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		default :
			break;
	}        
	
	return EOK;
}


uint32_t dwcotg_set_sel( usbdc_device_t *udc, uint8_t u1sel, uint8_t u1pel, uint16_t u2sel, uint16_t u2pel ) {
	dctrl_t 		*dc = udc->dc_data;
	cmd_t 			cmd;
	dwcotg_slogf(dc, _SLOG_INFO, "%s: U1_SEL = %d U1_PEL = %d U2_SEL = %d U2_PEL = %d", __func__, u1sel, u1pel, u2sel, u2pel );

	// section 7.3.1.6.1 of the designware databook says thate if u1 and u2 are enabled, the u2pel should be used...
	// if only u1 is enabled, then u1pel should be used.
	// given this, assume that we will either support u1 and u2 or nothing at all.
	dgcmd_build_SETPERI_PARAM( &cmd,  u2pel );

	// note: generic commands are non-blocking for now...
	// do we want to block on completion?
	dgcmd_send( dc, &cmd );


	return EOK;
}



int dwcotg_disconnect( dctrl_t  * dc ) {
	int         timeout = 10; // ms
	int         rc = EOK; 
	int         i;
	uint32_t    reg;
	ep_ctx_t    *ep;

	dwcotg_slogf(dc, _SLOG_INFO, "%s: Aborting Transfer and stopping the controller", __func__ );
	
	
	//unidrectional non-control endpoints assumed
	for( i=2; i < dc->n_ep; i++ ) {
		ep = &dc->ep_arr[i];
		if ( ep->flags & EPFLAG_XFER_ACTIVER ) {
			ep->flags &= ~EPFLAG_XFER_ACTIVER;
			dwcotg_xfer_abort( dc, ep, i );
		}
	}

	delay( 1 );
	
	// do soft disconnect
	reg = HW_Read32( dc->IoBase, DCTL );
	reg &= ~( RUNSTOP );
	HW_Write32( dc->IoBase, DCTL, reg );
	
	while( --timeout && !( HW_Read32( dc->IoBase, DSTS ) & DEVCTRLHLT ) ) {
		delay( 1 );
	}
	
	if ( timeout == 0 ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s Controller didnt't halt  ( DSTS & DEVCTRLHLT ) == true ",__func__ );
		rc = ETIME;
	}
	
	return rc;
}

/* call to enable connection to host or signal resume to host */
uint32_t
dwcotg_set_bus_state( usbdc_device_t *udc, uint32_t device_state )
{
	dctrl_t  *dc = udc->dc_data;
	uint32_t reg;
	
	switch ( device_state ) {
	case IOUSB_BUS_STATE_DISCONNECTED :
		dwcotg_disconnect( dc );
		break;
		
	case IOUSB_BUS_STATE_CONNECTED :
		// do soft connect
		// start the controller
		reg = HW_Read32( dc->IoBase, DCTL );
		reg &= ~( TRGTULST_MSK );
		reg |= TRGTULST_RX_DET | RUNSTOP;
		HW_Write32( dc->IoBase, DCTL, reg );
		
	case IOUSB_BUS_STATE_RESUME :
		break;
	}
	
	
	return EOK;
}

/*
	Set port test mode. This must be done witin 3ms of status phase of request.
*/

uint32_t
dwcotg_set_test_mode( dctrl_t *dc, uint16_t wIndex ) 
{
	uint32_t        mode;
	uint32_t        reg;

	dwcotg_slogf(dc, _SLOG_INFO , "%s : 0x%x", __func__, wIndex );

	switch( wIndex ) {	
		case USB_TEST_MODE_TEST_J :
			mode = TSTCTL_J;
			break;

		case USB_TEST_MODE_TEST_K :
			mode = TSTCTL_K; 
			break;

		case USB_TEST_MODE_TEST_SE0_NAK :
			mode = TSTCTL_SE0_NAK;
			break;

		case USB_TEST_MODE_TEST_PACKET :
			mode = TSTCTL_TESTPKT;
			break;

		case USB_TEST_MODE_TEST_FORCE_ENABLE : // only vald for downstream port(Host)
		default :	
			return( ENOTSUP );	
	}

	reg = HW_Read32( dc->IoBase, DCTL );
	reg &= ~TSTCTL_MSK;
	reg |= mode;
	HW_Write32( dc->IoBase, DCTL, reg );

	return( EOK );
}

/* enable a feature of a device */
uint32_t
dwcotg_set_device_feature( usbdc_device_t *udc, uint32_t feature, uint16_t wIndex )
{
	dctrl_t  *dc = udc->dc_data;
	uint32_t status = EOK;


	switch ( feature ) {

		case USB_FEATURE_U1_ENABLE:
			if ( dc->link_pm_mask & INITU1ENA ) {
				HW_Write32Or( dc->IoBase, DCTL, INITU1ENA );
			}
			break;

		case USB_FEATURE_U2_ENABLE:
			if (dc->link_pm_mask & INITU2ENA ) {
				HW_Write32Or( dc->IoBase, DCTL, INITU2ENA );
			}
			break;
		case USB_FEATURE_LTM_ENABLE:
			break;

		case USB_FEATURE_TEST_MODE :
			status =  dwcotg_set_test_mode( dc, wIndex );
			break;

		case USB_FEATURE_DEV_WAKEUP :
			break;

		default :
			status = ENOTSUP;
			break;
	}

	return status;
}

/* clear a feature of a device */
uint32_t
dwcotg_clear_device_feature( usbdc_device_t *udc, uint32_t feature )
{
	uint32_t status = EOK;
	dctrl_t  *dc = udc->dc_data;

	switch ( feature ) {

		case USB_FEATURE_U1_ENABLE:
			HW_Write32And( dc->IoBase, DCTL, ~INITU1ENA );
			break;

		case USB_FEATURE_U2_ENABLE:
			HW_Write32And( dc->IoBase, DCTL, ~INITU2ENA );
			break;

		case USB_FEATURE_LTM_ENABLE:
			break;

		case USB_FEATURE_DEV_WAKEUP :
			break;
		case USB_FEATURE_TEST_MODE : // don't support clearing of test modes(will never get here)
			break;
		default :
			status = ENOTSUP;
			break;
	}

	return status;
}

uint32_t
dwcotg_get_descriptor( usbdc_device_t *udc, uint8_t type, uint8_t index, uint16_t lang_id, uint8_t **desc, uint32_t speed )
{	
	dctrl_t *dc = udc->dc_data;

	switch ( type ) {
	case USB_DESC_DEVICE :
		dwcotg_slogf(dc, _SLOG_INFO, "%s : get USBDC_DEVICE_DESCRIPTOR", __func__);
		switch( speed ) {
			case IOUSB_DEVICE_HIGH_SPEED:
				*desc = (uint8_t *) USBDC_HS_DEVICE_DESCRIPTOR;
				break;
			case IOUSB_DEVICE_SUPER_SPEED:
				*desc = (uint8_t *) USBDC_SS_DEVICE_DESCRIPTOR;
				break;
			case IOUSB_DEVICE_FULL_SPEED:
			default:
				*desc = (uint8_t *) USBDC_FS_DEVICE_DESCRIPTOR;
				break;
		}
		break;
	
	case USB_DESC_CONFIGURATION :
		if ( index < USBDC_NUM_CONFIGURATIONS ) {
			dwcotg_slogf(dc, _SLOG_INFO, "%s : get USBDC_CONFIG_DESCRIPTOR speed = %d index = %d", __func__,speed,index);
			switch( speed ) {
				case IOUSB_DEVICE_HIGH_SPEED:
					*desc = (uint8_t *) USBDC_HS_CONFIG_DESCRIPTOR[index];
					break;
				case IOUSB_DEVICE_SUPER_SPEED:
					*desc = (uint8_t *) USBDC_SS_CONFIG_DESCRIPTOR[index];
					break;
				case IOUSB_DEVICE_FULL_SPEED:
				default:
					*desc = (uint8_t *) USBDC_FS_CONFIG_DESCRIPTOR[index];
					break;
			}
		} else {
			return( ENOTSUP );
		}
		break;
	
	case USB_DESC_STRING :
		if ( index <= USBDC_MAX_STRING_DESCRIPTOR ) {
			dwcotg_slogf(dc, _SLOG_INFO, "%s : get USBDC_STRING_DESCRIPTOR idx=%d", __func__,index);
			switch( speed ) {
				case IOUSB_DEVICE_HIGH_SPEED:
					*desc = (uint8_t *) USBDC_HS_STRING_DESCRIPTOR[index];
					break;
				case IOUSB_DEVICE_SUPER_SPEED:
					*desc = (uint8_t *) USBDC_SS_STRING_DESCRIPTOR[index];
					break;
				case IOUSB_DEVICE_FULL_SPEED:
				default:
					*desc = (uint8_t *) USBDC_FS_STRING_DESCRIPTOR[index];
					break;
			}

		} else {
			return ENOTSUP;
		}
		break;
	
	case USB_DESC_DEVICE_QUALIFIER :
		dwcotg_slogf(dc, _SLOG_INFO, "%s : get USB_DESC_DEVICE_QUALIFIER", __func__);
		switch( speed ) {
			case IOUSB_DEVICE_HIGH_SPEED:
				*desc = (uint8_t *) USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR;
				break;
			case IOUSB_DEVICE_SUPER_SPEED:
				*desc = (uint8_t *) USBDC_SS_DEVICE_QUALIFIER_DESCRIPTOR;
				break;
			case IOUSB_DEVICE_FULL_SPEED:
			default:
				*desc = (uint8_t *) USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR;
				break;
		}
		break;
		
	case USB_DESC_OTHER_SPEED_CONF :
		// What about SuperSpeed

		dwcotg_slogf(dc, _SLOG_INFO, "%s : get USB_DESC_OTHER_SPEED_CONF speed = %d index = %d", __func__, speed, index);
		*desc  = (speed == IOUSB_DEVICE_HIGH_SPEED) ? (uint8_t *) USBDC_HS_CONFIG_DESCRIPTOR[0] : (uint8_t *) USBDC_FS_CONFIG_DESCRIPTOR[0];

		break;
		
	case USB_DESC_BOS:
		*desc = (uint8_t *) USBDC_BOS_DESCRIPTOR;
		break;

		
	case USB_DESC_INTERFACE_POWER :
	case USB_DESC_INTERFACE :
	case USB_DESC_ENDPOINT :	
	default :
		return ENOTSUP;
		break;
	}
	return EOK;
}

/* This function is meant to configure ep0 and should only be called at
 * driver init time
 */
int ep0_cfg( dctrl_t * dc ) {
	ep_ctx_t				*ep = &dc->ep_arr[0];
	cmd_t                   epcmd;
	int                     err;
	

	ep->mps = 512;
	ep->num = 0;
	ep->dir = 0;
	ep->idx = 0;
	ep->type = USB_ATTRIB_CONTROL;
	ep->fifonum = 0;
	ep->burstsz = 0;
	ep->urb = 0;
	ep->xfer_flags = 0;
	ep->req_xfer_len = 0;
	
	ep->flags = EPFLAG_ENABLED;
	
	// allocate transfer descriptors for this endpoint
	ep->trb_cluster = trb_cluster_alloc( dc, 1 );
	if ( ep->trb_cluster == NULL ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to allocated trb cluster", __func__ );
		err = ENOMEM;
		goto error;
	}

	// allocate setup packet
	ep->setup_packet =  mmap64( 	NULL, 
									SETUP_PACKET_SIZE, 
									PROT_READ | PROT_WRITE | PROT_NOCACHE, 
									MAP_PRIVATE | MAP_ANON | MAP_PHYS,
									NOFD,
									0 								);

	if ( ep->setup_packet == MAP_FAILED ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s mmap failed to allocate setup packet buffer",__func__);
		err = ENOMEM;
		goto error2;
	}
	
	err = mem_offset64( 	ep->setup_packet,
							NOFD,
							DWCOTG_EVTQ_MEM_SIZE,
							&ep->setup_packet_paddr64,
							NULL					);
	if ( err != EOK ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s failed to calc paddr64 for setup packet buffer",__func__ );
		goto error3;
	}
	dwcotg_slogf(dc, _SLOG_INFO, "%s: Setup Packet Buffer Paddr = 0x%llx", __func__, ep->setup_packet_paddr64 );


	/* Issue a DEPSTARTCFG command with DEPCMD0.XferRscIdx set to 0 and CmdIOC set to 0 to
	 * initialize the transfer resource allocation. Poll CmdAct for completion
	 * Only do this on first call
	 */
	epcmd_build_DEPSTARTCFG( &epcmd,0 );
	err = epcmd_send( dc, ep, 0, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPSTARTCFG command to epidx = 0 ", __func__ );
	}
	
	/* 
	 * Configure EP0.... both epidx0 ( OUT ) and epidx1(IN) need to be configured
	 * Needs to be called every time to update the endpoint MPS
	 */

	epcmd_build_DEPCFG( &epcmd, ep, 0, 0 );
	err = epcmd_send( dc, ep, 0, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPCFG command to epidx = 0  ", __func__ );
	}
	
	epcmd_build_DEPCFG( &epcmd, ep, 1, 0 );
	err = epcmd_send( dc, ep, 1, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPCFG command to epidx = 1 ", __func__ );
	}

	epcmd_build_DEPXFERCFG( &epcmd, 1 );
	err = epcmd_send( dc, ep, 0, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPXFERCFG command to epidx = 0  ", __func__ );
	}
	
	epcmd_build_DEPXFERCFG( &epcmd, 1 );
	err = epcmd_send( dc, ep, 1, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPXFERCFG command to epidx = 1 ", __func__ );
	}
		
	// enable endpoint 0 ( epidx0 and epidx1 )
	HW_Write32Or( dc->IoBase, DALEPENA, 3 );
	
	
	// Get Ready for Next Setup Packet... possibly after re-insertion
	dwcotg_slogf(dc, _SLOG_INFO, "%s: Get Ready for First Setup Packet", __func__);
	ep->control_phase = CONTROL_PHASE_SETUP;
	dwcotg_setup_packet_get( dc );

	return EOK;

error3:
	munmap( ep->setup_packet, SETUP_PACKET_SIZE );
error2:
	trb_cluster_free( dc, ep->trb_cluster );
error:
	return err;	
}

void ep0_decfg( dctrl_t * dc ) {
	ep_ctx_t				*ep = &dc->ep_arr[0];

	// disable endpoint 0... i.e. both epidx0 and epidx1 
	HW_Write32And( dc->IoBase, DALEPENA, ~3 );
	
	munmap( ep->setup_packet, SETUP_PACKET_SIZE );
	trb_cluster_free( dc, ep->trb_cluster );

	ep->flags &= ~EPFLAG_ENABLED;
}


_uint32 
dwcotg_control_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	
	/* ep0 has already been configured in ep0_cfg()... all we have to do is link it to the
	 * generic parent struct. This code will have to change to support non ep0 control enpdpoints
	 */
	iousb_ep->user = &dc->ep_arr[0];
	
	return EOK;	
}


// used by both bulk and interrupt endpoints
_uint32 
dwcotg_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t		*ep = iousb_ep->user; 
	uint32_t		epnum = iousb_ep->edesc.bEndpointAddress & 0x7f;
	uint32_t		epdir = iousb_ep->edesc.bEndpointAddress & USB_ENDPOINT_IN;
	cmd_t           epcmd;	
	int				err;
	int				first_call = 0;
	int             addr,sz;
	
	dwcotg_slogf(dc, _SLOG_INFO, "%s: ep = 0x%x mps = %d", __func__, iousb_ep->edesc.bEndpointAddress, iousb_ep->edesc.wMaxPacketSize);

	if ( !iousb_ep->user ) {
		first_call = 1;
		ep = iousb_ep->user = &dc->ep_arr[EPIDX( epnum, epdir )];
		ep->iousb_ep = iousb_ep;	// backref
		ep->mps = iousb_ep->edesc.wMaxPacketSize;
		ep->num = epnum;
		ep->dir = epdir;
		ep->idx = EPIDX( epnum, epdir );
		ep->type = iousb_ep->edesc.bmAttributes & 3;
		ep->flags = EPFLAG_ENABLED;

		/* IN Endpoints use separate TX fifos corresponding to the endpoint number
		 OUT endpoints use FIFO 0 */
		ep->fifonum = ( epdir ) ? epnum : 0;

		/* hardcode super speed bulk endpoints burst size to 15... 
		 * companion descriptor must match
		 */
		 
		if ( ep->type == USB_ATTRIB_BULK ) {
			ep->burstsz = 0xf;
		} else {
			ep->burstsz = 0x0;
		}
		
		ep->trb_cluster = trb_cluster_alloc( dc, 1 );
		if ( ep->trb_cluster == NULL ) {
			dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to allocated trb cluster... epnum = 0x%x epdir = 0x%x", __func__, epnum, epdir  );
			err = ENOMEM;
			goto error;
		}
		
		if ( ep->dir ) {
			// alloc txfifo
			addr = fifomem_alloc( dc->fifomem_hdl, 2, ep->mps, &sz );
			if ( addr < 0 ) {
				dwcotg_slogf(dc, _SLOG_ERROR, "%s: fifomem_alloc() failed for epnum = %d epdir = 0x%x", __func__, epnum, epdir  );
				err = ENOMEM;
				goto error2;
			}
			HW_Write32( dc->IoBase, GTXFIFOSIZ( ep->fifonum ), ( addr << TXFSTADDR_POS ) | sz  );
		} // else { // rxfifos are shared}

		epcmd_build_DEPCFG( &epcmd, ep, epdir, 0 );
		err = epcmd_send( dc, ep, ep->idx, &epcmd, 0 );
		if ( err != EOK ) {
			dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPCFG command to epidx = %d  ", __func__, ep->idx );
		}

		epcmd_build_DEPXFERCFG( &epcmd, 1 );
		err = epcmd_send( dc, ep, ep->idx, &epcmd, 0 );
		if ( err != EOK ) {
			dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPXFERCFG command to epidx = %d  ", __func__, ep->idx );
		}
			// enable the endpoint
		HW_Write32Or( dc->IoBase, DALEPENA, EPMSK(ep->idx) );
	}

	
	return EOK;
error2: 	
	if ( first_call ) {
		trb_cluster_free( dc, ep->trb_cluster );
	}
error:
	return err;

}

_uint32 
dwcotg_isoch_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;

	dwcotg_slogf(dc, _SLOG_ERROR, "%s: Not Supported", __func__ );
	
	return EOK;
}


_uint32 
dwcotg_control_endpoint_disable( void *chdl, iousb_endpoint_t *iousb_ep )
{
	return ( EOK );
}

_uint32 
dwcotg_endpoint_disable( void *chdl, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t				*ep = iousb_ep->user;

	dwcotg_slogf(dc, _SLOG_INFO, "%s: epidx = %d epnum = %d epdir = %s "
		, __func__, ep->idx, ep->num, EPDIR(ep->idx)  );
	
	HW_Write32And( dc->IoBase, DALEPENA, ~EPMSK(ep->idx) );
	trb_cluster_free( dc, ep->trb_cluster );

	if ( ep->dir ) {
		fifomem_free( dc->fifomem_hdl, HW_Read32( dc->IoBase, GTXFIFOSIZ( ep->fifonum )) >> TXFSTADDR_POS );
	}
	
	
	ep->flags &= ~EPFLAG_ENABLED;
	iousb_ep->user = 0;
	
	return ( EOK );
}

_uint32 
dwcotg_isoch_endpoint_disable( void *chdl, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;

	dwcotg_slogf(dc, _SLOG_ERROR, "%s: Not Supported", __func__ );
	
	return EOK;
}

// assumes control endpoint is the default control pipe only, and uses epidx0 and epidx1
_uint32 
dwcotg_control_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	
	if ( ep == NULL ) {
		return EOK;
	}
	

	DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x  ep->num = %d ", __func__, iousb_ep->edesc.bEndpointAddress, ep->num);
	
	// abort any active transfers
	if ( ( ep->flags & EPFLAG_XFER_ACTIVER ) ) {
		//dwcotg_xfer_abort( dc, ep, 0 );
		//dwcotg_xfer_abort( dc, ep, 1 );
	}
	ep->flags &= ~EPFLAG_XFER_ACTIVER;
	ep->urb = 0;
	
	// Get Ready for Next Setup Packet... possibly after re-insertion
	dwcotg_slogf(dc, _SLOG_INFO, "%s: Get Ready for First Setup Packet", __func__);
	ep->control_phase = CONTROL_PHASE_SETUP;
	dwcotg_setup_packet_get( dc );
	
	
	return EOK;
}

_uint32 
dwcotg_control_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep, uint8_t *buffer, _uint32 length, _uint32 flags )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	uint32_t 		idx = ( flags & PIPE_FLAGS_TOKEN_IN ) ? 1 : 0;	
	int             err;


	DWCOTG_SLOGF_DBG(dc, _SLOG_INFO, "%s: ep = 0x%x  ep->num = %d buffer = 0x%x len = %d flags = 0x%x "
		, __func__, iousb_ep->edesc.bEndpointAddress, ep->num, buffer,length,flags );
	
	ep->urb = urb;
	ep->xfer_flags = flags;
	
	if ( ( flags & PIPE_FLAGS_TOKEN_OUT ) && !( flags & PIPE_FLAGS_TOKEN_STATUS ) && ( length % ep->mps ) ) {
		// massage the length to be module mps
		length = ep->mps * ( ( length / ep->mps ) + 1 ); 
		DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: changed length from to %d which is module mps  ", __func__, length );		
	}
	ep->req_xfer_len = length;			
	ep->bytes_xfered = 0;
	
	
	pthread_sleepon_lock();
	while ( ( ep->flags & EPFLAG_XFER_NOT_READY ) == 0 ) {
		// only wait 1 second ( 1000000000 ns ) , then report an error and keep trying
		if( pthread_sleepon_timedwait(&ep->flags, 1000000000) == ETIMEDOUT ) {
			dwcotg_slogf(dc, _SLOG_ERROR, "%s: timeout waiting for controller to report EPFLAG_XFER_NOT_READY", __func__, length );
			break;
		}
	}
	pthread_sleepon_unlock();
	
	if ( flags & PIPE_FLAGS_TOKEN_STATUS ) {
		// Status Phase

		// controller distinguishes between 2-phase and 3-phase transfers...
		if ( ep->control_phase == CONTROL_PHASE_DATA ) {
			trb_build_ctrl_status3( ep->trb_cluster );
			
		} else {
			trb_build_ctrl_status2( ep->trb_cluster );
		}			
		ep->control_phase = CONTROL_PHASE_STATUS;
	
	} else {
		trb_build_ctrl_data( ep->trb_cluster, (uint32_t) buffer, length );
		ep->control_phase = CONTROL_PHASE_DATA;		
	}

	err = dwcotg_xfer_start( dc, ep->trb_cluster, ep, idx );
	if ( err ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to hook trb on epidx %d to complete data/status phase  ", __func__, idx );		
	}
	
	
	return EOK;
}

	
_uint32 
dwcotg_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	
	if ( ep ) {
		DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s:  ep->num = %d  ep->idx", __func__, ep->num, ep->idx );

		if ( ep->flags & EPFLAG_XFER_ACTIVER ) {
			ep->flags &= ~EPFLAG_XFER_ACTIVER;
			dwcotg_xfer_abort( dc, ep, ep->idx );
		}
		ep->urb = 0;

	}
		
	return EOK;
}

_uint32 
dwcotg_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep, uint8_t *buffer, _uint32 length, _uint32 flags )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	int             err;
	
	DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x  ep->num = %d buffer = 0x%x len = %d flags = 0x%x "
		, __func__, iousb_ep->edesc.bEndpointAddress, ep->num, buffer,length,flags );


	ep->urb = urb;
	ep->xfer_flags = flags;

	if ( ( flags & PIPE_FLAGS_TOKEN_OUT ) && ( length % ep->mps ) ) {
		// massage the length to be module mps
		//dwcotg_slogf(dc, _SLOG_ERROR, "%s: length %d is not modulo mps", __func__, length );
		length = ep->mps * ( ( length / ep->mps ) + 1 ); 
		//dwcotg_slogf(dc, _SLOG_ERROR, "%s: changing length  to %d which is module mps  ", __func__, length );
	}
	ep->req_xfer_len = length;
	ep->bytes_xfered = 0;
	
	trb_build_normal( ep->trb_cluster, (uint32_t) buffer, length );
	err = dwcotg_xfer_start( dc, ep->trb_cluster, ep, ep->idx );
	if ( err ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to hook trb on epidx %d  ", __func__, ep->idx );		
	}

	
	return EOK;
}

_uint32 
dwcotg_isoch_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;

	dwcotg_slogf(dc, _SLOG_ERROR, "%s: Not Supported", __func__ );
	
	return ENOTSUP;	
}


_uint32 
dwcotg_isoch_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep, uint8_t *buffer, _uint32 length, _uint32 flags )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;

	dwcotg_slogf(dc, _SLOG_ERROR, "%s: Not Supported", __func__ );
	
	return ENOTSUP;
}


int
dwcotg_process_args( dctrl_t *dc, char *args )
{

	int opt;
	char *value;
	char *c;
	int len;
	
	static char *driver_opts[] = {
		"verbose", 
		"ser",
		"linkup",
		"linkdown",
		"maxspeed",
		"linkpm",
		"PARAMETER_OVERRIDE_A",
		"PARAMETER_OVERRIDE_B",
		"PARAMETER_OVERRIDE_C",
		"PARAMETER_OVERRIDE_D",
		"linkpm",
		NULL
	};
	
	dc->serial_string = NULL;
	
	if( !args ) 
		return EOK;    
	
	// convert args
	len = strlen( args );
	while ( ( c = strchr( args, ':' ) ) ) {
		if ( c - args > len )
			break;
			*c = ',';
	}
		
	while( *args != '\0' ) {
		if( ( opt = getsubopt( &args, driver_opts, &value ) ) != -1 ) {
			switch ( opt ) {
				case 0 :
					if ( value )
						dc->verbosity = strtoul( value, 0, 0 );
					else 
						dc->verbosity = 5;
					continue;
				case 1 :     // this arg should move up for know we build a proper string desc
					if ( value ) {
						uint8_t  slen;
						uint32_t x;
						slen = min(strlen( value ), 127 ); // max 8bit length each char requires 2 bytes for encoding plus 2 byte header.
						dc->udc->serial_string = calloc( 1, 3 + 2 * slen );
						dc->udc->serial_string[0] = 2 + slen *2; // string header is 2 bytes
						dc->udc->serial_string[1] = USB_DESC_STRING;
						for ( x = 1 ; x < slen + 1 ; x ++ ) {
							dc->udc->serial_string[x * 2] = value[x - 1];
						}
					}
					continue;
				case 2:
					// "linkup"
					dc->flags |= DC_FLAG_SOFT_CONNECT;
				
					continue;
				
				case 3:
					// "linkdown"
					dc->flags &= ~DC_FLAG_SOFT_CONNECT;
					continue;

				case 4:
					// default max speed is SUPERSPEED
					if ( value ) {
						if ( ( stricmp( value, "high" ) ) == 0 ) {
							dc->maxspeed = 	DEVSPD_HIGH;
						} else if ( ( stricmp( value, "full" ) ) == 0 ) {
							dc->maxspeed = 	DEVSPD_FULL;
						}
					}
					continue;

				case 5:
					dc->phy_tuning[USBPHY_PARAMETER_OVERRIDE_A] = strtoul( value, NULL, 16 );
					continue;
				case 6:
					dc->phy_tuning[USBPHY_PARAMETER_OVERRIDE_B] = strtoul( value, NULL, 16 );
					continue;
				case 7:
					dc->phy_tuning[USBPHY_PARAMETER_OVERRIDE_C] = strtoul( value, NULL, 16 );
					continue;
				case 8:
					dc->phy_tuning[USBPHY_PARAMETER_OVERRIDE_D] = strtoul( value, NULL, 16 );
					continue;
				case 9:
					// "linkpm"
					if ( value ) {
						dc->link_pm_mask = strtoul( value, 0, 0 ) << LINK_PM_POS;
					}
					continue;
				default :
					break;
			}
		}
	}
	
	return EOK;
}

static void 
default_config_set( usbdc_device_t *udc )
{
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;
	
	dc->verbosity = _SLOG_ERROR;
	dc->flags     = 0;
	dc->maxspeed = DEVSPD_SUPER;
	// default behaviour is to only allow the host to initiate u1/u2  transitions
	// many link problems have been observed when allowing the device to initiate u1/u2 which
	// needs to be investigated
	dc->link_pm_mask = ACCEPTU1ENA | ACCEPTU2ENA | INITU1ENA | INITU2ENA;
	
	
	udc->hw_ctrl.cname = "dwcotg";
	udc->hw_ctrl.max_transfer_size = 0x10000; 
	udc->hw_ctrl.max_unaligned_xfer = 0x10000; 
	udc->hw_ctrl.buff_alignment_mask = 0x1;
	udc->hw_ctrl.capabilities 	= DC_CAP_SUPER_SPEED | DC_CAP_FULL_SPEED | DC_CAP_HIGH_SPEED | DC_CAP_TEST_MODES_SUPPORTED;
	
}


static int eventq_init(  dctrl_t * dc ) {
	int    		err;
	off64_t  	paddr64;
	
	
	/* Init the Event Queue 
	 * Depending on the number of interrupts allocated, program the Event Buffer 
	 * Address and Size registers to point to the Event Buffer locations in 
	 * system memory, the sizes of the buffers, andunmask the interrupt. Note: USB 
	 * operation will stop if Event Buffer memory is insufficient, because the 
	 * core will stop receiving/transmitting packets
	 */
	 
	 
	/* map physical memory for the event queue, and get the physical address */
	dc->eventq_mem = mmap64( 	NULL, 
								DWCOTG_EVTQ_MEM_SIZE, 
								PROT_READ | PROT_WRITE | PROT_NOCACHE, 
								MAP_PRIVATE | MAP_ANON | MAP_PHYS,
								NOFD,
								0 										);
	
	if ( dc->eventq_mem == MAP_FAILED ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s mmap failed when allocating event q",__func__);
		err = ENOMEM;
		goto error;
	}
	
	err = mem_offset64( 	dc->eventq_mem,
							NOFD,
							DWCOTG_EVTQ_MEM_SIZE,
							&paddr64,
							NULL					);
	if ( err != EOK ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s failed to calc paddr64 for event q",__func__ );
		goto error2;
	}
							
	
	/* setup the hardware to use newly allocated event memory */
	HW_Write32( dc->IoBase, GEVNTADRLO, paddr64 & 0xffffffff );
	HW_Write32( dc->IoBase, GEVNTADRHI, paddr64 >> 32 );
	HW_Write32( dc->IoBase, GEVNTSIZ, DWCOTG_EVTQ_MEM_SIZE );
	HW_Write32( dc->IoBase, GEVNTCOUNT, 0 );
		
	
	return EOK;
	
error2:
	munmap( dc->eventq_mem, DWCOTG_EVTQ_MEM_SIZE );	
error:
	return err;	
	
}

static void eventq_destroy(  dctrl_t * dc ) {
	munmap( dc->eventq_mem, DWCOTG_EVTQ_MEM_SIZE );	
}

static int reset_controller( dctrl_t * dc ) 
{
    int         rc = EOK;
    int         timeout = 1000;	// in ms
    
    HW_Write32Or( dc->IoBase, DCTL, CSFTRST  );
    
	while( --timeout && HW_Read32( dc->IoBase, DCTL ) & CSFTRST ) {
		delay( 1 );
	}
    
    if ( timeout == 0 ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s Core Soft Reset Failed ",__func__ );
		rc = ETIME;
    }
    
    
    return rc;
}



/*
Note: The PHYs can be reset only if you are using the pipe3_reset_n and usb2phy_reset signals
from the core.

The GUSB2PHYCFG, GUSB3PIPECTL, and GCTL registers control the USB2.0 PHY reset, USB3.0 PHY
reset, and core's internal reset. Software resets must be generated in the following sequence:
    - Set GUSB2PHYCFG[31], GUSB3PIPECTL[31, and GCTL[11]. This resets the PHYs and keeps the
core in reset state.
    - Reset GUSB2PHYCFG[31] and GUSB3PIPECTL[31] after they meet PHY reset duration. This
removes the reset to the PHYs.
    -Wait for the PHY clock to stabilize and reset GCTL[11] bit. This ensures that the reset to all the
internal blocks are asserted when all the clocks are stable.
*/


static int phy_reset( dctrl_t * dc ) {
	
	HW_Write32Or( dc->IoBase, GUSB2PHYCFG, PHYSOFTRST  );
	HW_Write32Or( dc->IoBase, GUSB3PIPECTL, PHYSOFTRST  );
	HW_Write32Or( dc->IoBase, GCTL, CORESOFTRESET  );
	
	delay( 10 );
	
	dwcotg_phy_in_reset( dc );
	
	
	HW_Write32And( dc->IoBase, GUSB2PHYCFG, ~PHYSOFTRST  );
	HW_Write32And( dc->IoBase, GUSB3PIPECTL, ~PHYSOFTRST  );
	
	delay( 10 );
	
	HW_Write32And( dc->IoBase, GCTL, ~CORESOFTRESET  );
	
	return EOK;
}

static int 
chip_config ( dctrl_t * dc ) 
{
	int        rc;
	uint32_t   reg;
	int        addr,sz;

	
	rc = phy_reset( dc );
	if ( rc != EOK ) {
		goto error;
	}
	
	// force controller in device mode
	reg = HW_Read32( dc->IoBase, GCTL );
	reg &= ~PRTCAPDIR_MSK;
	reg |= PRTCAPDIR_DEVICE;

	/*
	 * WORKAROUND: DWC3 revisions <1.90a have a bug
	 * where the device can fail to connect at SuperSpeed
	 * and falls back to high-speed mode which causes
	 * the device to enter a Connect/Disconnect loop
	 */
	if ( HW_Read32( dc->IoBase, GSNPSID ) < DWC3_REVISION_190A ) {
		reg |= U2RSTECN;
	}
#if  defined(VARIANT_exynos5410smdk) || defined(VARIANT_musket)
	// Fail to establish a superspeed link once every 8 insertions without ths bit set.
	// exynos-drc.c ( linux ) blindly sets this as well...
	reg |= U2RSTECN;
#endif
	HW_Write32( dc->IoBase, GCTL, reg ); 

	/* All of these initializations are from the SuperSpeed USB 3.0 
	 * DesignWare Controller Databook, Chapter9 Table 9-1
	 */
	
	/* Step 1:
	 * DCTL: Set the CSftRst field to ‘1’ and wait for a read to return ‘0’. 
	 * This will reset the device core
	 */
	rc = reset_controller( dc );
	if ( rc != EOK ) {
		goto error;
	}
	
	// set ep0 tx fifo size
	addr = fifomem_alloc_ep0( dc->fifomem_hdl, &sz );
	HW_Write32( dc->IoBase, GTXFIFOSIZ( 0 ), ( addr << TXFSTADDR_POS ) | sz  );
	
	/* Step 2: Nothing to do yet
	 * GSBUSCFG0/1: Leave the default values if the correct power-on 
	 * values were selected during coreConsultant configuration.
	 */
	 
	/* Step 3: Nothing to do yet
	 * GTXtHRCFG/ GRXTHRCFG:  This is required only if you are planning to enable thresholding. Leave 
	 * the default values if the correct power-on values were selected during 
	 * coreConsultant configuration.
	 */
	  
	  
	/* Step 4: Nothing to do yet
	 * GSNPSID: The software must read the Synopsys ID register to find the core 
	 * version and configure the driver for any version-specific features.
	 */

	/* Step 5: Nothing to do yet
	 * GUID: Optionally, the software can program the User ID GUID register, if this 
	 * register is selected for implementation in coreConsultant
	 */
	 
	/* Step6: Nothing to do yet 
	 * GUSB2PHYCFG: Program the following PHY configuration fields: 
	 * USBTrdTim, FSIntf, PHYIf, TOUTCal, or leave the default values if the 
	 * correct power-on values were selected during coreConsultant configuration
	 */
	 
	/* Step 7: Nothing to do yet 
	 * GUSB3PIPECTL : Program the following PHY configuration fields: 
	 * DatWidth, PrtOpDir, or leave the default values if the correct power-on 
	 * values were selected during coreConsultant configuration.
	 */
	 
	/* Step 8: 
	 * GTXFIFOSIZn : Write these registers to allocate prefetch buffers for each 
	 * Tx endpoint. Unless the packet sizes of the endpoints are 
	 * application-specific, it is recommended to use the default value. For details	,
	 * see “Device TxFIFO Data Allocation” on page 331.
	 */
	/* Step 9 : 
	 * GRXFIFOSIZ0 : Write this register to allocate the receive buffer for all 
	 * endpoints. Unless the packet sizes of the endpoints are application-specific, 
	 * it is recommended to use the default value. For details, see
	 * “Device RxFIFO Data Allocation” on page 331.
	 */
	 
	/* Step 10: */ 
	rc = eventq_init( dc );	
	if ( rc != EOK ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s eventq_init() failed",__func__ );
		goto error;
	}
	
	/* Step 11: 
	 * GCTL: Program this register to override scaledown, loopback, RAM clock select, 
	 * and clock gating parameters
	 */
	 
	/* Step 12: 
	 * DCFG: Program device speed and periodic frame interval
	 */
	 
	reg = HW_Read32( dc->IoBase, DCFG );
	reg &= ~DEVSPD_MSK;
	reg |= dc->maxspeed;
	//
	// need to determine why isn't the controller reset putting this in a known good state.
	// reset the 'NumP' field to default value in case it is clobbered by the bootloader
	reg &= ~NUMP_MSK;
	reg |= NUMP_DEFAULT;
	//
	HW_Write32( dc->IoBase, DCFG, reg );

	/* Step 13:
	 * DEVTEN: At a minimum, enable USB Reset, Connection Done, and USB/Link 
	 * State Change events
	 */
	
	HW_Write32( dc->IoBase, DEVTEN, 
		DISCONNEVTEN | USBRSTEN | CONNECTDONEEN | ULSTCNGEN | CMDCMPLTEN );
	 
	/* Defer The the control endpoint initialization until the stack 
	 * enables EP0
	 */

	 rc = ep0_cfg(dc);
	 if ( rc != EOK ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s ep0_cfg() failed",__func__ );
		goto error2;
	 }
	 
	
	return EOK;	

error2:
	eventq_destroy( dc );	
error:	
	return rc;
}

static int create_delayed_execution_handler( dctrl_t * dc ) {
	pthread_attr_t     		attr;
	struct sched_param  	param;
	int						error;
	
	dc->chid = ChannelCreate( _NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK );
	if ( dc->chid < 0 ) {
		error = errno;
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to create channel (error=%d)",__func__,error);	
		goto fail;
	}
	
	dc->coid = ConnectAttach( 0, 0, dc->chid, _NTO_SIDE_CHANNEL, 0 );
	if ( dc->coid < 0 ) {
		error = errno;
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to attach to channel (error=%d)",__func__);	
		goto fail2;
	}
	
	pthread_attr_init( &attr );
	pthread_attr_setschedpolicy( &attr, SCHED_RR );
	param.sched_priority = 10;
	pthread_attr_setschedparam( &attr, &param );
	pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	
	error = pthread_create( &dc->tid, &attr, (void*)delayed_execution_handler, dc );
	if( error != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to create interrupt handler thread (error = %d) ",__func__,error);
		goto fail3;
	}

	return EOK;

fail3:
	ConnectDetach( dc->coid );	
fail2:
	ChannelDestroy( dc->chid );
fail:
	return error;	
} 

void destroy_delayed_execution_handler( dctrl_t * dc ) {
	pthread_cancel( dc->tid );
	ConnectDetach( dc->coid );	
	ChannelDestroy( dc->chid );
}


uint32_t
dwcotg_init( usbdc_device_t *udc, io_usbdc_self_t *udc_self, char *args )
{
	dctrl_t * dc;
	pthread_mutexattr_t mattr;    
	int rc;
	char *args_copy;
    
    /* allocate device ctx */
	dc = udc->dc_data = calloc( 1, sizeof( dctrl_t ) );
	if ( !dc ) {
		dwcotg_slogf( NULL, _SLOG_ERROR, "%s calloc failed",__func__);
		rc = ENOMEM;
		goto error;
	}
	dc->udc = udc;

    /* set default driver configurations */
	default_config_set( udc );
	
    /* parse command line arguments to override default configs */
    
    args_copy = strdup( args );
    if ( args_copy == NULL ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s strdup failed",__func__);
		rc = ENOMEM;
		goto error2;
    }
    
	rc = dwcotg_process_args(dc, args);
	if ( rc ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s couldn't parse command line args",__func__);
		goto error3;        
	}
    
	rc = dwcotg_custom_init1( dc, args_copy );
	if ( rc ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s dwcotg_custom_init1() failed err = %d",__func__, rc);
		goto error3;        
	}
	
	// no longer need args copy, because it has alreay been processed by custom_int1
	free( args_copy );
	args_copy = NULL;
	
	// map io
	dc->IoBase =  mmap_device_memory( NULL, 
						DWCOTG_SIZE,  
						PROT_READ | PROT_WRITE | PROT_NOCACHE, 
						MAP_SHARED | MAP_PHYS,
						PCI_MEM_ADDR( udc->hw_ctrl.pci_inf->CpuBaseAddress[0] ) );
	if ( dc->IoBase == MAP_FAILED ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s mmap failed",__func__);
		rc = ENOMEM;
		goto error4;
	}

	// cache how many endpoints are supported by the controller
	dc->n_ep =  
		( HW_Read32( dc->IoBase, GHWPARAMS3 ) &  DWC_USB3_NUM_EPS_MSK ) >> DWC_USB3_NUM_EPS_POS;
	dwcotg_slogf(dc, _SLOG_INFO, "%s: controller has %d endpoints", __func__, dc->n_ep );
	
	// create the driver mutex    
	pthread_mutexattr_init( &mattr );
	pthread_mutexattr_setrecursive( &mattr, PTHREAD_RECURSIVE_ENABLE );
	if( pthread_mutex_init( &dc->mutex, &mattr ) == -1 ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s could not create mutex",__func__);
		rc = ENOMEM;
		goto error5;
	}

	dc->fifomem_hdl = fifomem_init( dc );
	if ( dc->fifomem_hdl == NULL ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s failed to create fifomem allocator",__func__);
		goto error6;
	}
	
	// setup usb controller
	rc = chip_config( dc );
	if ( rc ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s could not init the controller",__func__);
		goto error7;
	}
	
	rc = dwcotg_custom_init2( dc );
	if ( rc ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s dwcotg_custom_init2() failed err = %d",__func__, rc);
		goto error7;        
	}
	
	rc = create_delayed_execution_handler( dc );
	if ( rc != EOK ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s failed to created delayed execution handler err = %d",__func__, rc);
		goto error8;        
	}
	
	
	udc->usbdc_self->usbdc_set_device_speed( udc, IOUSB_DEVICE_FULL_SPEED );  
	
	return EOK;

error8:
	dwcotg_custom_fini2( dc );
error7:
	fifomem_fini( dc->fifomem_hdl );	
error6:
	pthread_mutex_destroy( &dc->mutex );
error5:
	munmap( dc->IoBase, DWCOTG_SIZE );
error4:
	dwcotg_custom_fini1( dc );
error3:
	if ( args_copy ) 
		free( args_copy );
error2:
	free(dc);  
error:    
	return rc;
}

uint32_t
dwcotg_start( usbdc_device_t *udc )
{
	return EOK;
}

uint32_t
dwcotg_stop( usbdc_device_t *udc )
{
	return EOK;
}

uint32_t
dwcotg_shutdown( usbdc_device_t *udc )
{
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;
	
	// force disconnect
	HW_Write32And( dc->IoBase, DCTL, ~RUNSTOP );

	// free resources
	ep0_decfg( dc );
	destroy_delayed_execution_handler(dc);
	pthread_mutex_destroy( &dc->mutex );
	fifomem_fini( dc->fifomem_hdl );	
	munmap( dc->IoBase, DWCOTG_SIZE);
	dwcotg_custom_fini2( dc );
	dwcotg_custom_fini1( dc );
	eventq_destroy( dc );
	free(dc);  
	
	return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/dwcotg3/dwcotg.c $ $Rev: 735878 $")
#endif
