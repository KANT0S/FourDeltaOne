#include "aci3.h"

static const char Base64[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';

#define Assert()

int
	b64_ntop(uint8_t const *src, size_t srclength, char *target, size_t targsize) {
		size_t datalength = 0;
		uint8_t input[3];
		uint8_t output[4];
		size_t i;

		while (2 < srclength) {
			input[0] = *src++;
			input[1] = *src++;
			input[2] = *src++;
			srclength -= 3;

			output[0] = input[0] >> 2;
			output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
			output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
			output[3] = input[2] & 0x3f;
			Assert(output[0] < 64);
			Assert(output[1] < 64);
			Assert(output[2] < 64);
			Assert(output[3] < 64);

			if (datalength + 4 > targsize)
				return (-1);
			target[datalength++] = Base64[output[0]];
			target[datalength++] = Base64[output[1]];
			target[datalength++] = Base64[output[2]];
			target[datalength++] = Base64[output[3]];
		}

		/* Now we worry about padding. */
		if (0 != srclength) {
			/* Get what's left. */
			input[0] = input[1] = input[2] = '\0';
			for (i = 0; i < srclength; i++)
				input[i] = *src++;

			output[0] = input[0] >> 2;
			output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
			output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
			Assert(output[0] < 64);
			Assert(output[1] < 64);
			Assert(output[2] < 64);

			if (datalength + 4 > targsize)
				return (-1);
			target[datalength++] = Base64[output[0]];
			target[datalength++] = Base64[output[1]];
			if (srclength == 1)
				target[datalength++] = Pad64;
			else
				target[datalength++] = Base64[output[2]];
			target[datalength++] = Pad64;
		}
		if (datalength >= targsize)
			return (-1);
		target[datalength] = '\0';	/* Returned value doesn't count \0. */
		return (datalength);
}
