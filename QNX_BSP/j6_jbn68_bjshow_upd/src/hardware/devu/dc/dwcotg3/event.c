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

#include "dwcotg.h"

////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE                                       //
////////////////////////////////////////////////////////////////////////////////

static void complete_urb( dctrl_t * dc, ep_ctx_t *ep, uint32_t urb_status ) {
	iousb_transfer_t *urb = ep->urb;

	if ( urb ) {
		// we have sent/received everyting ... complete the urb
		urb->actual_len = ep->bytes_xfered;
		ep->urb = 0;
		DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "URB COMPLETE epnum = 0x%x epdir = 0x%x epidx = %d urb->actual_len = %d ",ep->num, ep->dir, ep->idx , urb->actual_len  );
		urb->urb_complete_cbf( ep->iousb_ep, urb, urb_status, 0);
	} 
}	

static void event_process_USBRst( dctrl_t * dc ) {
	int          i;
	ep_ctx_t   * ep;
	
	/* designware datasheet table 9-2  
	 * "Initialization on USB Reset" section
	 */
	
	/* Step1:
	 * DEPCMD0: If a control transfer is still in progress, complete it and get the 
	 * core into the “Setup a Control-Setup TRB / Start Transfer” state 
	 * (see “Control Transfer Programming Model” on page 512)
	 */
	
	/* Step2: 
	 * DEPCMDn: Issue a DEPENDXFER command for any active transfers 
	 * (except for the default control endpoint 0)
	 *
	 * Note: transfer_abort() will be called by the stack to do this... don't do anything here
	 */
	 
	/* Step3: 
	 * DEPCMDn: Issue a DEPCSTALL (ClearStall) command for any endpoint that was 
	 * put into STALL mode prior to the USB Reset
	 */
	 
	// start with i = 2 because indexes 0 and 1 are reserved for the default
	// control pipe EP0. 
	for(i = 2; i < MAX_N_ENDPOINTS; i++ ) {
		ep = &dc->ep_arr[i];

		if ( ep->flags & EPFLAG_STALLED ) {
			cmd_t			epcmd;
			int             err; 

			dwcotg_slogf(dc, _SLOG_INFO, "%s: clear stall on ep->num = %d epidx = %d ", __func__, ep->num, ep->idx );
			
			ep->flags &= ~EPFLAG_STALLED;
			epcmd_build_DEPCSTALL( &epcmd );
			err = epcmd_send( dc, ep, ep->idx, &epcmd, 0 );
			if ( err != EOK ) {
				dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPCSTALL command to epidx = %d ", __func__, ep->idx );
			}
		}
	}	 
	 
	/* Step4:
	 * DCFG: Set DevAddr to ‘0’
	 */
	HW_Write32And( dc->IoBase, DCFG, ~DEVADDR_MSK );			

	// allow bus to enter u1/u2 ... set it here because these bits are cleared by usb reset
	if ( dc->link_pm_mask & ACCEPTU1ENA ) {
		HW_Write32Or( dc->IoBase, DCTL, ACCEPTU1ENA );
	}
	if ( dc->link_pm_mask & ACCEPTU2ENA ) {
		HW_Write32Or( dc->IoBase, DCTL, ACCEPTU2ENA );
	}
}

static void event_process_ConnectDone( dctrl_t * dc ) {
	uint32_t            speed;
	usbdc_device_t    * udc = dc->udc;
	ep_ctx_t          * ep = &dc->ep_arr[0];
	cmd_t               epcmd;
	int                 err;

	
	dc->flags |= DC_FLAG_CONNECTED;
	udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_INSERTED);
	dwcotg_slogf(dc, _SLOG_INFO, "%s: IOUSB_DEVICE_STATE_INSERTED", __func__ );		

	/* Step1:
	 * DSTS: Read this register to obtain the connection speed
	 */
	 speed = ( HW_Read32( dc->IoBase, DSTS ) & CONNECTSPD_MSK ) >> CONNECTSPD_POS;
	 
	 switch ( speed ) {
		case CONNECTSPD_SUPER:
			dwcotg_slogf(dc, _SLOG_INFO, "%s: SUPER SPEED DETECTED", __func__ );	
			ep->mps = 512;		
			HW_Write32Or( dc->IoBase, GUSB2PHYCFG, SUSPHY );
			
			udc->usbdc_self->usbdc_set_device_speed( udc, IOUSB_DEVICE_SUPER_SPEED);
			break;
			 
		case CONNECTSPD_HIGH:
			udc->usbdc_self->usbdc_set_device_speed( udc, IOUSB_DEVICE_HIGH_SPEED);
			dwcotg_slogf(dc, _SLOG_INFO, "%s: HIGH SPEED DETECTED", __func__ );	
			ep->mps = 64;	
			break;
		 
		case CONNECTSPD_LOW:
			udc->usbdc_self->usbdc_set_device_speed( udc, IOUSB_DEVICE_LOW_SPEED);
			dwcotg_slogf(dc, _SLOG_INFO, "%s: LOW SPEED DETECTED", __func__ );	
			break;
		
		case CONNECTSPD_FULL1:
		case CONNECTSPD_FULL2:
		default:	 
			udc->usbdc_self->usbdc_set_device_speed( udc, IOUSB_DEVICE_FULL_SPEED);
			dwcotg_slogf(dc, _SLOG_INFO, "%s: FULL SPEED DETECTED", __func__ );	
			ep->mps = 64;	
			break;
	 }
	 
	// report the reset *after* the speed has been reported
	udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_RESET );
	 
	/* Step2:
	 * GCTL: Program the RAMClkSel field to select the correct clock for the RAM 
	 * clock domain. This field is reset to 0 after USB reset, so it must be 
	 * reprogrammed each time on Connect Done.
	 */

	 // Do Nothing as written in the OMAP 5 refmanual
	 
	/* Step3: 
	 * DEPCMD0/DEPCMD1: Issue a DEPCFG command (with ‘Ignore Sequence Number’
	 * field set to 1) for physical endpoints 0 & 1 using the same endpoint 
	 * characteristics from Power-On Reset, but set MaxPacketSize to
	 * 512 (SuperSpeed), 64 (High-Speed), 8/16/32/64 (Full-Speed), or 8 (Low-Speed).
	 */
	 // re-adjust the mps based on speed
	epcmd_build_DEPCFG( &epcmd, ep, 0, 1 );
	err = epcmd_send( dc, ep, 0, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPCFG command to epidx = 0  ", __func__ );
	}
	
	epcmd_build_DEPCFG( &epcmd, ep, 1, 1 );
	err = epcmd_send( dc, ep, 1, &epcmd, 0 );
	if ( err != EOK ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: failed to send DEPCFG command to epidx = 1 ", __func__ );
	}
	// wait for DEPCFG commands to complete... need to make sure we are actually done commands
	delay(1);

	 
	/* Step4: 
	 * GUSB2CFG/GUSB3PIPECTL: Depending on the connected speed, write to the 
	 * other PHY’s control register to suspend it
	 */
	
	/* Step5:   This is optional, so we don't do it for now... 
	 * GTXFIFOSIZn: (optional) Based on the new MaxPacketSize of IN endpoint 0,
	 * software may choose to re-allocate the TX FIFO sizes by writing to these registers.
	 */
	 
	/*
	 * delay first setup packet detected after insertion to give the extract 
	 * code a chance to run and cleanup
	 */
	ep->setup_packet_delay = 1;
}

static void event_process_ULStChng( dctrl_t * dc,  uint32_t raw_event ) {
	usbdc_device_t * udc = dc->udc;

	switch ( ( raw_event & EVT_DEVINFO_LINKSTATE_MSK) >> EVT_DEVINFO_LINKSTATE_POS ) {
		case LTDBLINKSTATE_SUSPEND:
			udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_SUSPENDED );
			break;

		case LTDBLINKSTATE_RESUME:
			udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_RESUMED );
			break;

		default:
			break;
	}
}

static void event_process_DisconnEvt( dctrl_t * dc ) {
	usbdc_device_t * udc = dc->udc;
	cmd_t			 gcmd;	
	dwcotg_slogf(dc, _SLOG_INFO, "%s: IOUSB_DEVICE_STATE_REMOVED", __func__ );
	
	// flush all fifos on disconnect
	dgcmd_build_FIFO_ALL_FLUSH(&gcmd);
	dgcmd_send( dc, &gcmd );
	
	dc->flags &= ~DC_FLAG_CONNECTED;
	udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_REMOVED);
}

static void event_process_XferComplete( dctrl_t * dc, evt_t * decoded_event ) {
	ep_ctx_t 	*ep = &dc->ep_arr[decoded_event->epidx];
	uint32_t    count;

	if ( ep->flags & EPFLAG_XFER_ACTIVER ) {
		ep->flags &= ~EPFLAG_XFER_ACTIVER;
	
		count = ep->req_xfer_len -  ( ep->trb_cluster->trb_arr[0].bufsz_sts & TRB_BUFSZ_MSK );
		ep->bytes_xfered = count;
		complete_urb( dc, ep, 0 );
	} else {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: XferComplete Event.. but not active transfer on ep->num = %d, ep->dir = %d idx = %d... do nothing because it may have been aborted"
			, __func__, ep->num, ep->dir, decoded_event->epidx );				
	}
}

static void event_process_XferComplete_ep0( dctrl_t * dc, evt_t * decoded_event ) {
	ep_ctx_t 	*ep = &dc->ep_arr[0];
	uint32_t    count;
	
	if ( ep->flags & EPFLAG_XFER_ACTIVER ) {
		ep->flags &= ~(EPFLAG_XFER_ACTIVER | EPFLAG_XFER_NOT_READY);

		
		switch ( ep->control_phase ) {
			case CONTROL_PHASE_SETUP:
				
				count = SETUP_PACKET_SIZE - ( ep->trb_cluster->trb_arr[0].bufsz_sts & TRB_BUFSZ_MSK );
				
				if ( count == SETUP_PACKET_SIZE ) {
					
					if ( ep->setup_packet_delay ) {
						DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: setup packet received, but delay processing",__func__ );
						ep->setup_packet_delay = 0;
						MsgSendPulse ( dc->coid, 10, PULSE_DELAY_SETUP_PKT, 0 );
					} else {
						DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: rxpkt = 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x"
							, __func__ , ep->setup_packet[0], ep->setup_packet[1], ep->setup_packet[2], ep->setup_packet[3]
							, ep->setup_packet[4], ep->setup_packet[5], ep->setup_packet[6], ep->setup_packet[7] );

						dc->udc->usbdc_self->usbdc_setup_packet_process( dc->udc, ep->setup_packet );
					}
						
				} else {
					dwcotg_slogf( dc, _SLOG_ERROR, "%s() expected an 8-byte setup packet, but received %d bytes... discard",__func__, count );
					dwcotg_setup_packet_get( dc );
				}
					
				break;
	
			case CONTROL_PHASE_DATA:
				DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: Completed Data Phase: CONTROL_PHASE_DATA... ", __func__ );
				count = ep->req_xfer_len -  ( ep->trb_cluster->trb_arr[0].bufsz_sts & TRB_BUFSZ_MSK );
				ep->bytes_xfered = count;
				complete_urb( dc, ep, 0 );
				break;
				
			case CONTROL_PHASE_STATUS:
				DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: Completed Status Phase: CONTROL_PHASE_STATUS... waiting for next Setup Packet ", __func__ );
				complete_urb( dc, ep, 0 );
				
				ep->control_phase = CONTROL_PHASE_SETUP;
				dwcotg_setup_packet_get( dc );
				
				break;
			}
	} else {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: XferComplete Event.. but not active transfer on ep->num = %d, ep->dir=%d idx = %d... do nothing because it may have been aborted", __func__, ep->num, ep->dir, decoded_event->epidx );				
	}
} 


			
static void event_process_EPCmdCmplt( dctrl_t * dc, evt_t * decoded_event ) {
	ep_ctx_t 	*ep;  
	
	if ( ( decoded_event->epidx == 0 ) || ( decoded_event->epidx == 1 ) ) {
		ep = &dc->ep_arr[0];
	} else {
		ep = &dc->ep_arr[decoded_event->epidx];
	}

	// pass the event status back to the command thread
	pthread_sleepon_lock();
	ep->cmd_complete_event = decoded_event->raw_event;
	DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG2, "%s: pass raw event 0x%x back to command thread", __func__ , ep->cmd_complete_event );				
	pthread_sleepon_signal( &ep->cmd_complete_event );
	pthread_sleepon_unlock();
	
}

static void event_process_XferNotReady_ep0( dctrl_t * dc, evt_t * decoded_event ) {
	ep_ctx_t 	*ep = &dc->ep_arr[0];

	pthread_sleepon_lock();
	ep->flags |= EPFLAG_XFER_NOT_READY;
	pthread_sleepon_signal(&ep->flags);
	pthread_sleepon_unlock();
}


#if 0
static void event_process_XferNotReady( dctrl_t * dc, evt_t * decoded_event ) {
}
#endif


////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC                                       //
////////////////////////////////////////////////////////////////////////////////



void event_process( dctrl_t * dc, evt_t * decoded_event ) {

	if ( decoded_event->evt_class == EVT_CLASS_EP ) {
		dwcotg_slogf(dc, _SLOG_INFO, "%s() %s raw = 0x%x epidx = %d epnum = %d epdir = %s"
			, __func__, event_str_arr[decoded_event->evt_type], decoded_event->raw_event, 
			decoded_event->epidx, EPNUM(decoded_event->epidx), EPDIR(decoded_event->epidx) );
	} else {
		dwcotg_slogf(dc, _SLOG_INFO, "%s() %s raw = 0x%x"
			, __func__, event_str_arr[decoded_event->evt_type], decoded_event->raw_event );
	}
	
	switch( decoded_event->evt_type ) {
		
		case USBRst:
			event_process_USBRst( dc );
			break;

		case ConnectDone:
			event_process_ConnectDone( dc );
			break;

		case DisconnEvt:
			event_process_DisconnEvt( dc );
			break;

		case ULStChng:
			event_process_ULStChng( dc, decoded_event->raw_event );
			break;
			
		case XferComplete:
			if ( ( decoded_event->epidx == 0 ) || ( decoded_event->epidx == 1 ) ) {
				event_process_XferComplete_ep0( dc, decoded_event );
			} else {
				event_process_XferComplete( dc, decoded_event );
			}
			break;

		case EPCmdCmplt:
			event_process_EPCmdCmplt( dc, decoded_event );
			break;
			
		// we only use XferNotReady for EP0 so we can implement the control transfer
		// state machine... It doesn't appear as though we have to wait for this event
		// for non-control endpoints
		case XferNotReady:
			if ( ( decoded_event->epidx == 0 ) || ( decoded_event->epidx == 1 ) ) {
				event_process_XferNotReady_ep0( dc, decoded_event );
			}
			break;
			
		default:
			break;
	}
}

// 'event_decode' doesn't do a full decode of the event, but just enough to get
// the event type.  Further decode can be achieved via 
// defined marcros in the dwcotg header.
void event_decode( dctrl_t * dc, uint32_t raw_event, evt_t * decoded_event ) {
	
	decoded_event->raw_event = raw_event;
	if ( ( raw_event & EVT_TYPE_MSK ) == EVT_TYPE_EP ) {
		// Endpoint Event

		decoded_event->evt_class = EVT_CLASS_EP;
		decoded_event->epidx = ( raw_event & EVT_EP_NUM_MSK ) >> EVT_EP_NUM_POS;
		
		
		switch( raw_event & EVT_EP_TYPE_MSK ) {
			case EVT_EP_XferComplete:
				decoded_event->evt_type = XferComplete;
				break;
			case EVT_EP_XferInProgress:
				decoded_event->evt_type = XferInProgress;
				break;
			case EVT_EP_XferNotReady:
				decoded_event->evt_type = XferNotReady;
				break;
			case EVT_EP_RxTxfifoEvt:
				decoded_event->evt_type = RxTxfifoEvt;
				break;
			case EVT_EP_StreamEvt:
				decoded_event->evt_type = StreamEvt;
				break;
			case EVT_EP_EPCmdCmplt:
				decoded_event->evt_type = EPCmdCmplt;
				break;
			default:	
				decoded_event->evt_type = UnknownEvent;
				break;
		}

		dwcotg_slogf(dc, _SLOG_DEBUG1, "%s() %s epidx = %d",__func__,event_str_arr[decoded_event->evt_type], decoded_event->epidx );

	} else {
		
		switch ( raw_event & EVT_TYPE2_MASK ) {
			case EVT_TYPE2_DEVICE:
				decoded_event->evt_class = EVT_CLASS_DEVICE;

				switch ( raw_event & EVT_DEV_MSK ) {
					case EVT_DEV_DisconnEvt:
						decoded_event->evt_type = DisconnEvt;
						break;
					case EVT_DEV_USBRst:
						decoded_event->evt_type = USBRst;
						break;
					case EVT_DEV_ConnectDone:
						decoded_event->evt_type = ConnectDone;
						break;
					case EVT_DEV_ULStChng:
						decoded_event->evt_type = ULStChng;	
						break;
					case EVT_DEV_WkUpEvt:
						decoded_event->evt_type = WkUpEvt;			
						break;
					case EVT_DEV_EOPF:
						decoded_event->evt_type = EOPF;
						break;
					case EVT_DEV_Sof:
						decoded_event->evt_type = Sof;
						break;
					case EVT_DEV_ErrticErr:
						decoded_event->evt_type = ErrticErr;
						break;
					case EVT_DEV_CmdCmplt:
						decoded_event->evt_type = CmdCmplt;
						break;
					case EVT_DEV_EvntOverflow:
						decoded_event->evt_type = EvntOverflow;
						break;
					case EVT_DEV_VndrDevTstRcved:
						decoded_event->evt_type = VndrDevTstRcved;
						break;
					default:
						decoded_event->evt_type = UnknownEvent;
						break;						
				}
				
				dwcotg_slogf(dc, _SLOG_DEBUG1, "%s() %s",__func__,event_str_arr[decoded_event->evt_type] );
				break;
			
			case EVT_TYPE2_ULPI_CARKIT:
				// No supported Events yet
				decoded_event->evt_class = EVT_ULPI_CARKIT;
				dwcotg_slogf(dc, _SLOG_ERROR, "%s() Unsupported EVT_TYPE2_ULPI_CARKIT Event... raw = 0x%x",__func__,raw_event );
				break;

			case EVT_TYPE2_I2C:
				// No supported Events yet
				decoded_event->evt_class = EVT_I2C;
				dwcotg_slogf(dc, _SLOG_ERROR, "%s() Unsupported EVT_TYPE2_I2C Event... raw = 0x%x",__func__,raw_event );
				break;
				
			default:	
				decoded_event->evt_class = EVT_UNKNOWN;
				dwcotg_slogf(dc, _SLOG_ERROR, "%s() Unrecognized Event raw = 0x%x",__func__,raw_event );
				break;
		}
		
	}
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/dwcotg3/event.c $ $Rev: 743948 $")
#endif
