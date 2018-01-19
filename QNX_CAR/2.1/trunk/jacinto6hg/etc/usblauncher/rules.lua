CONFIG_DIR = "/base/etc/usblauncher/";
function file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end


--- Return true only if MIRRORLINK=False exists in services_enabled.
services_enabled = "/var/etc/services-enabled"
function is_mirrorlink_explicitly_disabled()
    if file_exists(services_enabled) then
        for line in io.lines(services_enabled) do
            if string.find(string.lower(line), "mirrorlink") then
                if string.find(string.lower(line), "false") then
                    return true
                end
            end
        end
    end
    return false
end

if not is_mirrorlink_explicitly_disabled() then
    MirrorLink = {
    	version = 0x0101;
    	vendor_id = 0xABAB;
    	blacklist = {
    		{ vid = 0x0781; did = 0x74e0 }; -- SanDisk Sansa Fuze+
    		{ vid = 0x0951; did = 0x1624 }; -- Kingston DataTraveler G2
    		{ vid = 0x05ac;              }; -- any Apple device
    		{ vid = 0x0424;              }; -- any Standard Microsystems Corp. device
    		{ vid = 0x0eef;              }; -- any D-WAV Scientific Co., Ltd
    	};
    	timeout = 2000;
    	ignore  = false;
    	stall_reset = true;
    }
end

------------------------------------------------------------------------------

	USB_CLASS_DEVICE = 0x00
	USB_CLASS_AUDIO = 0x01
		USB_AUDIO_PROTOCOL_CONTROL = 0x01
		USB_AUDIO_PROTOCOL_STREAMING = 0x02
		USB_AUDIO_PROTOCOL_MIDI_STREAMING = 0x03
	USB_CLASS_COMMS = 0x02
		USB_COMMS_SUBCLASS_DIRECT_LINE = 0x01
		USB_COMMS_SUBCLASS_MODEM = 0x02
		USB_COMMS_SUBCLASS_TELEPHONE = 0x03
		USB_COMMS_SUBCLASS_MULTICHANNEL = 0x04
		USB_COMMS_SUBCLASS_CAPI_CONTROL = 0x05
		USB_COMMS_SUBCLASS_ETHERNET = 0x06
		USB_COMMS_SUBCLASS_ATM = 0x07
		USB_COMMS_SUBCLASS_WIRELESS_HANDSET = 0x08
		USB_COMMS_SUBCLASS_DEVICE_MGMT = 0x09
		USB_COMMS_SUBCLASS_MOBILE_DIRECT_LINE = 0x0a
		USB_COMMS_SUBCLASS_OBEX = 0x0b
		USB_COMMS_SUBCLASS_NCM  = 0x0d
		USB_COMMS_SUBCLASS_ETHERNET_EMULATION = 0x0c
			USB_COMMS_PROTOCOL_ETHERNET_EMULATION = 0x07
	USB_CLASS_HID = 0x03
		USB_HID_SUBCLASS_NONE = 0x00
			USB_HID_PROTOCOL_NONE = 0x00
			USB_HID_PROTOCOL_KEYBOARD = 0x01
			USB_HID_PROTOCOL_MOUSE = 0x02
		USB_HID_SUBCLASS_BOOTINTERFACE = 0x01
	USB_CLASS_PHYSICAL = 0x05
	USB_CLASS_IMAGING = 0x06
		USB_IMAGING_SUBCLASS_STILL = 0x01
			USB_IMAGING_STILL_PROTOCOL_PTP = 0x01
	USB_CLASS_PRINTER = 0x07
	USB_CLASS_MASS_STORAGE = 0x08
	USB_CLASS_HUB = 0x09
		USB_HUB_PROTOCOL_FULL = 0x00
		USB_HUB_PROTOCOL_HI_SINGLE_TT = 0x01
		USB_HUB_PROTOCOL_HI_MULTIPLE_TT = 0x02
	USB_CLASS_CDC_DATA = 0x0a
	USB_CLASS_SMART_CARD = 0x0b
	USB_CLASS_CONTENT_SECURITY = 0x0d
	USB_CLASS_VIDEO = 0x0e
	USB_CLASS_DIAGNOSTIC = 0xdc
	USB_CLASS_WIRELESS = 0xe0
		USB_WIRELESS_SUBCLASS_RADIO = 0x01
			USB_WIRELESS_PROTOCOL_BLUETOOTH_PI = 0x01
			USB_WIRELESS_PROTOCOL_UWB_RCI = 0x02
			USB_WIRELESS_PROTOCOL_RNDIS = 0x03
		USB_WIRELESS_SUBCLASS_ADAPTER = 0x02
			USB_WIRELESS_PROTOCOL_HOST_WIRE = 0x01
			USB_WIRELESS_PROTOCOL_DEVICE_WIRE = 0x02
			USB_WIRELESS_PROTOCOL_DEVICE_WIRE_ISOCHRONOUS = 0x03
	USB_CLASS_MISC = 0xef
	USB_CLASS_APPSPEC = 0xef
		USB_APPSPEC_SUBCLASS_FIRMWARE_UPDATE = 0x01
			USB_APPSPEC_PROTOCOL_FIRMWARE_UPDATE = 0x01
		USB_APPSPEC_SUBCLASS_IRDA_BRIDGE = 0x02
			USB_APPSPEC_PROTOCOL_IRDA_BRIDGE = 0x00
		USB_APPSPEC_SUBCLASS_TEST_MEASURE = 0x03
			USB_APPSPEC_PROTOCOL_TMC = 0x00
			USB_APPSPEC_PROTOCOL_USB488 = 0x01
	USB_CLASS_VENDOR_SPECIFIC = 0xff

------------------------------------------------------------------------------

peripheral_descriptors = {
	busno = 0,
	devno = 1,
	vendor_id = 0xabcd,
	product_id = 0x4321,
	configuration_name = "Configuration description",
	serial_number = "1234567890ABC",
	device_class = 0x00, -- Independent per interface
	device_subclass = 0,
	device_protocol = 0,
	max_packet_size0 = 64,
	manufacturer = "Manufacturer name",
	product = "Product name",
	interfaces = {
		{
			interface_class = USB_CLASS_MASS_STORAGE,
			interface_subclass = 0,
			interface_protocol = 0,
			interface_name = "Mass storage interface",
		},
		{
			interface_class = USB_CLASS_COMMS,
			interface_subclass = USB_COMMS_SUBCLASS_NCM,
			interface_protocol = 0,
			interface_name = "NCM interface",
		},
		{
			interface_class = USB_CLASS_HID,
			interface_subclass = 0x0,
			interface_protocol = 0,
			interface_name = "HID interface",
		}
	},
};

-- peripheral
device(peripheral_descriptors.vendor_id, peripheral_descriptors.product_id) {
	class(USB_CLASS_MASS_STORAGE) {
		driver"devu-umass_client-block -s /dev/otg/io-usb-dcd  -l lun=0,devno=1,iface=0,fname=/dev/hd1";
	};
	class(USB_CLASS_COMMS, USB_COMMS_SUBCLASS_NCM) {
		start"echo Communications interface $(iface)";
	};
	class(USB_CLASS_HID) {
		start"echo HID interface $(iface)";
	}
}

skip_devices = {
    device(0x0424, 0xec00);
    device(0x0eef, 0x0001);  -- eGalax Inc. USB TouchController
    device(0x0e0f, 0x0003)
}

foreach (skip_devices) {
    Ignore;  -- don't even attach to these devices
}

char_devices = {
    device(0x0557, 0x2008);  -- ATEN_232A/GUC_232A
    device(0x2478, 0x2008);  -- TRIPP LITE U2009-000-R
    device(0x9710, 0x7720);  -- MOSCHIP 7720
    device(0x0403, 0x6001);  -- FTDI 8U232AM
    device(0x1199, 0x0120);  -- Sierra AirCard U595
    device(0x0681, 0x0040);  -- Siemens HC25
    device(0x1bc7, 0x1003);  -- Telit UC864E
    device(0x067b, 0x2303);  -- Prolific
}

foreach (char_devices) {
    driver"devc-serusb -d vid=$(vendor_id),did=$(product_id),busno=$(busno),devno=$(devno)";
}

-- basic options for devb-umass
DISK_OPTS  = "cam cache,quiet blk cache=1m,vnode=384,auto=none,delwri=2:2,rmvto=none,noatime disk name=umass cdrom name=umasscd";
UMASS_OPTS = "umass priority=21";
--UMASS_OPTS = "umass path=/dev/otg/io-usb,priority=21";

UMASS_OVERRIDES_FILE = CONFIG_DIR .. "umass.lua";
if file_exists(UMASS_OVERRIDES_FILE) then
	print("usblauncher: Including umass overrides in " .. UMASS_OVERRIDES_FILE);
	dofile (UMASS_OVERRIDES_FILE);
end

-- table of specific MSC devices that can't handle Microsoft descriptor queries
-- By specifying the vid/did, the rule is tried before any generic rules
known_msc = {
	device(0x0781, 0x5567);	-- SanDisk Cruzer Blade
	device(0x0930, 0x6545); -- Kingston DataTraveler G3
}

foreach (known_msc) {
	driver"devb-umass $(DISK_OPTS) $(UMASS_OPTS),vid=$(vendor_id),did=$(product_id),busno=$(busno),devno=$(devno),iface=$(iface),ign_remove";
}

-- QNX Apple Interface
device(0x05fc, 0xff01) {
    driver"i2c-qui --u99";
};

-- LinkM Dongle
device(0x20a0, 0x4110) {
    driver"i2c-linkm --u99";
};

-- iPod

product(0x05AC, 0x1200, 0x12FF) {
--    iAP2;
    class(USB_CLASS_AUDIO, USB_AUDIO_PROTOCOL_CONTROL) {
--        inc_usr_spec_id = unique"/fs/ipod";  -- does rsrcdbmgr_devno_attach
        driver"io-audio -dipod busno=$(busno),devno=$(devno),cap_name=ipod-$(busno)-$(devno)";
    };
    class(USB_CLASS_HID) {
        driver"io-fs-media -dipod,transport=usb:busno=$(busno):devno=$(devno):audio=/dev/snd/ipod-$(busno)-$(devno),darates=+8000:11025:12000:16000:22050:24000,playback,acp=i2c:addr=0x11:path=/dev/i2c99,fnames=short,config=/etc/mm/ipod.cfg,stalk=500";
    };
    class(USB_CLASS_AUDIO, USB_AUDIO_PROTOCOL_STREAMING) {
	    Ignore;
    };
};


--
-- generic MTP rule should proceed generic MSC (mass storage) rule so that it is tried first.
--
-- generic MTP rule

device() {
    ms_desc("MTP") {
        driver"io-fs-media -dpfs,device=$(busno):$(devno):$(iface)";
    };
};

-- generic mass storage rule

device() {
    class(USB_CLASS_MASS_STORAGE) {
        driver"devb-umass $(DISK_OPTS) $(UMASS_OPTS),vid=$(vendor_id),did=$(product_id),busno=$(busno),devno=$(devno),iface=$(iface),ign_remove";
    };
};

-- MirrorLink NCM device rule

device() {
    class(USB_CLASS_COMMS, USB_COMMS_SUBCLASS_NCM) {
        driver"/scripts/vncdiscovery/usb-device-attached.sh $(vendor_id) $(product_id) $(busno) $(devno)";
	removal"/opt/runml.sh stop";
    };
};
AOA = {
	-- list of devices that should not be probed for Android Open Accessory (AOA) protocol version
	-- the list is just an example; you may wish to add more devices
	blacklist = {
		{ vid = 0x05ac;              }; -- Any Apple device
		{ vid = 0x0781; did = 0x5530 }; -- SanDisk Cruzer
		{ vid = 0x04e8; did = 0x5091 }; -- Samsung YP-S3 media player
	};
}

------------------------------------------------------------------------------

if verbose >= 3 then
    table.dump(new_conf)
    table.dump(new_conf.flags)
    show_config()
end

-- QWF rules
QWF_RULES = CONFIG_DIR .. "qwf.lua";
if file_exists(QWF_RULES) then
    print("usblauncher: Including QWF rules in " .. QWF_RULES);
    dofile(QWF_RULES);
end

