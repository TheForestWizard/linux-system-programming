#ifndef _FTDI_H_
#define _FTDI_H_

class Ftdi
{
	public:

	Ftdi(FtdiConfig config);

	~Ftdi();

	size_t write();

	size_t read();

	private:

	libusb_context* context;
	libsub_device_handle* device_handle;

	FtdiConfig config;
};

#endif // _FTDI_H_
