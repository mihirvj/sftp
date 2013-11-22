#include "fileop.h"

#define BYTES 1

int get_file_descriptor(char *fname, int mode)
{
	mode_t fmode;
	int fp;

	switch(mode)
	{
	case Read:
		fmode = O_RDONLY;
	break;

	case Write:
		fmode = O_WRONLY;
	break;

	case ReadWrite:
		fmode = O_RDWR;
	break;

	case Create:
		fmode = O_CREAT | O_RDWR;
	break;
	}

	fp = open(fname, fmode, S_IRUSR | S_IWUSR);

	if(fp < 0)
		error("file problem\n");

	return fp;
}

char rdt_send(int fp)
{
	char buf[BYTES];
	int readBytes;

	readBytes = read(fp, buf, BYTES);

	return (readBytes <= 0) ? 0 : (char) buf[0];
}

int output_to(int file, char *buffer, int buf_len)
{
	int bytesWritten;

	bytesWritten = write(file, buffer, buf_len);

	if(bytesWritten < 0)
		error("[error] while writing\n");

	return bytesWritten;
}
