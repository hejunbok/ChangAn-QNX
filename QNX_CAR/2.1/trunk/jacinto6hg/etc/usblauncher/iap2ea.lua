-- Sample USB descriptors for an iAP2 Accessory Device with EA

-- complete templates for the .device, .fs_config, and .hs_config table values

USB_CONFIG_SELFPOWERED         = 0x40
USB_CONFIG_REMOTEWAKEUP        = 0x20
USB_MAX_CURRENT                = 0

iap2ea = {}

iap2ea.device = Device{
	bDeviceClass = 0x0,
	bDeviceSubClass = 0x0,
	bDeviceProtocol = 0x0,
	bMaxPacketSize = 64,
	idVendor = 0x1234,
	idProduct = 0xfff1,
	bcdDevice = 0x0200,
	manufacturer = 'QNX Software Systems',
	product = 'iAP2 Accessory',
	serial = 'ABC-0123456789',
	bNumConfigurations = 1,
}

iap2ea.fs_config = Config{  -- full speed
	bConfigurationValue = 1,
	description = 'Default Configuration',
	bmAttributes = USB_CONFIG_SELFPOWERED,
	bMaxPower = USB_MAX_CURRENT,
	interfaces = {
		Iface{
			bInterfaceClass = 0xFF,
			bInterfaceSubClass = 0xF0,
			bInterfaceProtocol = 0x0,
			bAlternateSetting = 0,
			description = 'iAP Interface',
			endpoints = {
				InterruptIn{bInterval = 4, wMaxPacketSize = 64},
				BulkIn{wMaxPacketSize = 64},
				BulkOut{wMaxPacketSize = 64},
			},
		},
		Iface{ -- Alternate 0 - Zero Bandwidth
			bInterfaceClass = 0xFF,
			bInterfaceSubClass = 0xF0,
			bInterfaceProtocol = 0x1,
			bAlternateSetting = 0,
			description = 'com.qnx.eatest',
			endpoints = {},
		},
		Iface{ -- Alternate 1 - Active Session
			bInterfaceClass = 0xFF,
			bInterfaceSubClass = 0xF0,
			bInterfaceProtocol = 0x1,
			bAlternateSetting = 1,
			description = 'com.qnx.eatest',
			endpoints = {
				BulkIn{wMaxPacketSize = 64},
				BulkOut{wMaxPacketSize = 64},
			},
		},
	},
}

iap2ea.hs_config = Config{  -- high speed
	bConfigurationValue = 1,
	description = 'Default Configuration',
	bmAttributes = USB_CONFIG_SELFPOWERED,
	bMaxPower = USB_MAX_CURRENT,
	interfaces = {
		Iface{
			bInterfaceClass = 0xFF,
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
		Iface{ -- Alternate 0 - Zero Bandwidth
			bInterfaceClass = 0xFF,
			bInterfaceSubClass = 0xF0,
			bInterfaceProtocol = 0x1,
			bAlternateSetting = 0,
			description = 'com.qnx.eatest',
			endpoints = {},
		},
		Iface{ -- Alternate 1 - Active Session
			bInterfaceClass = 0xFF,
			bInterfaceSubClass = 0xF0,
			bInterfaceProtocol = 0x1,
			bAlternateSetting = 1,
			description = 'com.qnx.eatest',
			endpoints = {
				BulkIn{wMaxPacketSize = 512},
				BulkOut{wMaxPacketSize = 512},
			},
		},
	},
}
