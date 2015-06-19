#include "cyerror.h"
#include <stdint.h>
#include <cybtldr_utils.h>

typedef struct {
    uint16_t num;
    char* text;
} cyerror_t;

cyerror_t msg[] = {
		{ CYRET_SUCCESS, "Completed successfully" },
		{ CYRET_ERR_FILE, "File is not accessible" },
		{ CYRET_ERR_EOF, "Reached the end of the file" },
		{ CYRET_ERR_LENGTH, "The amount of data available is outside the expected range" },
		{ CYRET_ERR_DATA, "The data is not of the proper form" },
		{ CYRET_ERR_CMD, "The command is not recognized" },
		{ CYRET_ERR_DEVICE, "The expected device does not match the detected device" },
		{ CYRET_ERR_VERSION, "The bootloader version detected is not supported" },
		{ CYRET_ERR_CHECKSUM, "The checksum does not match the expected value" },
		{ CYRET_ERR_ARRAY, "The flash array is not valid" },
		{ CYRET_ERR_ROW, "The flash row is not valid" },
		{ CYRET_ERR_BTLDR, "The bootloader is not ready to process data" },
		{ CYRET_ERR_ACTIVE, "The application is currently marked as active" },
		{ CYRET_ERR_UNK, "An unknown error occured" },
		{ CYRET_ABORT, "The operation was aborted" },
		{ 0xffff, "Error message not found" }
		};

char* cyerror_get(uint16_t errnum) {
    char* retval;
    uint8_t idx = 0;
    cyerror_t* match;
    do {
        match = &msg[idx];
        retval = match->text;
        idx++;
    } while( errnum != match->num && 0xffff != match->num );
    return retval;
}

