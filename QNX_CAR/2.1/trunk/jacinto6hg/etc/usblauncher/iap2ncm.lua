-- Sample USB descriptors for an iAP2 NCM Accessory Device

-- complete templates for the .device, .fs_config, and .hs_config table values

USB_CONFIG_SELFPOWERED         = 0x40
USB_CONFIG_REMOTEWAKEUP        = 0x20
USB_MAX_CURRENT                = 0
NCM_SEG_LEN                    = 8192

ncm_functionals = {
	--[[
	* Note: fill in all the raw descriptor bytes, with the exception of any strings,
	  which will be assigned an index number and the string added to the global
	  strings table.
	--]]
	-- USB Communication Class Header Functional Descriptor
	0x05,              -- bLength
	0x24,              -- bDescriptorType
	0x00,              -- bDescriptorSubType
	0x10,              -- bcdCDC (LSB)
	0x01,              -- bcdCDC (MSB)

-- USB Communication Class Union Functional Descriptor
	0x05,              -- bLength
	0x24,              -- bDescriptorType
	0x06,              -- bDescriptorSubType
	0x01,              -- bControlInterface
	0x02,              -- bSubordinateInterface0

-- USB Communication Class Ethernet Networking Functional Descriptor
	0x0D,              -- bLength
	0x24,              -- bDescriptorType
	0x0F,              -- bDescriptorSubType
	'000022446688',    -- iMACAddress
	0x00,              -- bmEthernetStatistics
	0x00,
	0x00,
	0x00,
	lsb(NCM_SEG_LEN),  -- wMaxSegmentSize (LSB)
	msb(NCM_SEG_LEN),  -- wMaxSegmentSize (MSB)
	0x00,              -- wNumberMCFilters (LSB)
	0x00,              -- wNumberMCFilters (MSB)
	0x00,              -- bNumberPowerFilters

-- USB Communication Class NCM Functional Descriptor
	0x06,              -- bLength
	0x24,              -- bDescriptorType
	0x1A,              -- bDescriptorSubType
	0x00,              -- bcdNcmVersion (LSB)
	0x01,              -- bcdNcmVersion (MSB)
	0x00,              -- bmNetworkCapabilities
}

iap2ncm = {}

iap2ncm.device = Device{
	bDeviceClass = USB_CLASS_DEVICE,
	bDeviceSubClass = 0x0,
	bDeviceProtocol = 0x0,
	bMaxPacketSize = 64,
	idVendor = 0x1234,
	idProduct = 0xfff1,
	bcdDevice = 0x0200,
	manufacturer = 'QNX Software Systems',
	product = 'iAP2 NCM Accessory',
	serial = 'ABC-0123456789',
	bNumConfigurations = 1,
}

iap2ncm.fs_config = Config{  -- full speed
	bConfigurationValue = 1,
	description = 'Default Configuration',
	bmAttributes = USB_CONFIG_SELFPOWERED,
	bMaxPower = USB_MAX_CURRENT,
	interfaces = {
		Iface{
			bInterfaceClass = USB_CLASS_VENDOR_SPECIFIC,
			bInterfaceSubClass = 0xF0,
			bInterfaceProtocol = 0x0,
			bAlternateSetting = 0,
			description = 'iAP Interface',
			endpoints = {
				InterruptIn{bInterval = 4, wMaxPacketSize = 64},
				BulkIn{wMaxPacketSize = 64},
				BulkOut{wMaxPacketSize = 64},
			}
		},
		Iface{
			bInterfaceClass = USB_CLASS_COMMS,
			bInterfaceSubClass = USB_COMMS_SUBCLASS_NCM,
			bInterfaceProtocol = 0x0,
			bAlternateSetting = 0,
			description = 'CDC Network Control Module (NCM)',
			class_specific = ncm_functionals,
			endpoints = {
				InterruptIn{bInterval = 1, wMaxPacketSize = 64}
			},
		},
		Iface{
			bInterfaceClass = USB_CLASS_CDC_DATA,
			bInterfaceSubClass = 0x0,
			bInterfaceProtocol = 0x1,
			bAlternateSetting = 0,
			description = 'CDC Network Data',
			endpoints = {},
		},
		Iface{
			bInterfaceClass = USB_CLASS_CDC_DATA,
			bInterfaceSubClass = 0x0,
			bInterfaceProtocol = 0x1,
			bAlternateSetting = 1,
			description = 'CDC Network Data',
			endpoints = {
				BulkIn{wMaxPacketSize = 64},
				BulkOut{wMaxPacketSize = 64},
			},
		},
	},
}

iap2ncm.hs_config = Config{  -- high speed
	bConfigurationValue = 1,
	description = 'Default Configuration',
	bmAttributes = USB_CONFIG_SELFPOWERED,
	bMaxPower = USB_MAX_CURRENT,
	interfaces = {
		Iface{
			bInterfaceClass = USB_CLASS_VENDOR_SPECIFIC,
			bInterfaceSubClass = 0xF0,
			bInterfaceProtocol = 0x0,
			bAlternateSetting = 0,
			description = 'iAP Interface',
			endpoints = {
				InterruptIn{bInterval = 6, wMaxPacketSize = 64},
				BulkIn{wMaxPacketSize = 512},
				BulkOut{wMaxPacketSize = 512},
			}
		},
		Iface{
			bInterfaceClass = USB_CLASS_COMMS,
			bInterfaceSubClass = USB_COMMS_SUBCLASS_NCM,
			bInterfaceProtocol = 0x0,
			bAlternateSetting = 0,
			description = 'CDC Network Control Module (NCM)',
			class_specific = ncm_functionals,
			endpoints = {
				InterruptIn{bInterval = 1, wMaxPacketSize = 64}
			},
		},
		Iface{
			bInterfaceClass = USB_CLASS_CDC_DATA,
			bInterfaceSubClass = 0x0,
			bInterfaceProtocol = 0x1,
			bAlternateSetting = 0,
			description = 'CDC Network Data',
			endpoints = {},
		},
		Iface{
			bInterfaceClass = USB_CLASS_CDC_DATA,
			bInterfaceSubClass = 0x0,
			bInterfaceProtocol = 0x1,
			bAlternateSetting = 1,
			description = 'CDC Network Data',
			endpoints = {
				BulkIn{wMaxPacketSize = 512},
				BulkOut{wMaxPacketSize = 512},
			},
		},
	},
}

