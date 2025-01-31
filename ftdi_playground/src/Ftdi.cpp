#include "Ftdi.h"

Ftdi::Ftdi()
{
	// Initialize libusb
	if (libusb_init(&context) != 0)
	{
		fprintf(stderr, "Failed to initialize libusb.\n");
		throw -1;
	}
	
	// Open FTDI device
	device_handle = libusb_open_device_with_vid_pid(context, config.vendor_id, config.product_id);
	if (!device_handle)
	{
		fprintf(stderr, "Failed to open FTDI device.\n");
		libusb_exit(context);
		throw -2;
	}
	
	// Claim interface
	if (libusb_claim_interface(handle, config.interface) != 0)
	{
		fprintf(stderr, "Failed to claim interface.\n");
		libusb_close(device_handle);
		libusb_exit(context);
		throw -3;
	}
}

Ftdi::~Ftdi()
{
	libusb_release_interface(device_handle, 0);
	libusb_close(device_handle);
	libusb_exit(context);
}

Ftdi::write(uint8_t* write_buffer, size_t size)
{
	int transfered = 0;
	if (libusb_bulk_transfer(device_handle, EP_OUT, write_buffer, size, &transfered, TIMEOUT) == 0)
	{
		printf("Sent %d bytes: %s\n", transferred, write_buffer);
	}
	else
	{
		fprintf(stderr, "Failed to send data");
	}
}

Ftdi::read()
{
	
}
