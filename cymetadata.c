// cybootloaderutils lacks an implementation of the GetMetadata command
// Here's one, structured like the other cybootloaderutils commands 
// Werner Johansson <wj@unifiedengineering.se>

#include <stdint.h>
#include "cymetadata.h"
#include <cybtldr_command.h>
#include <cybtldr_api.h>

int CyBtldr_GetMetadata(uint8_t bootloadableNum, uint32_t* appCustId, uint16_t* appId, uint16_t* appVer, uint16_t* bootVer) {
    uint16_t inSize = 0;
    uint16_t outSize = 0;
    uint8_t inBuf[MAX_COMMAND_SIZE];
    uint8_t outBuf[MAX_COMMAND_SIZE];
    int err;

    err = CyBtldr_CreateGetMetadataCmd(inBuf, &inSize, &outSize, bootloadableNum);
    if (CYRET_SUCCESS == err)
        err = CyBtldr_TransferData(inBuf, inSize, outBuf, outSize);
    if (CYRET_SUCCESS == err)
        err = CyBtldr_ParseGetMetadataCmdResult(outBuf, outSize, appCustId, appId, appVer, bootVer);

    return err;
}

int CyBtldr_CreateGetMetadataCmd(uint8_t* cmdBuf, uint16_t* cmdSize, uint16_t* resSize, uint8_t bootloadableNum) {
    const uint16_t RESULT_DATA_SIZE = 56;
    uint16_t commandDataSize = 1;
    uint16_t checksum;

    *resSize = BASE_CMD_SIZE + RESULT_DATA_SIZE;
    *cmdSize = BASE_CMD_SIZE + commandDataSize;
    cmdBuf[0] = CMD_START;
    cmdBuf[1] = CMD_GET_METADATA;
    cmdBuf[2] = (uint8_t)commandDataSize;
    cmdBuf[3] = (uint8_t)(commandDataSize >> 8);
    cmdBuf[4] = bootloadableNum;
    checksum = CyBtldr_ComputeChecksum(cmdBuf, (*cmdSize) - 3);
    cmdBuf[*cmdSize - 3] = (uint8_t)checksum;
    cmdBuf[*cmdSize - 2] = (uint8_t)(checksum >> 8);
    cmdBuf[*cmdSize - 1] = CMD_STOP;

    return CYRET_SUCCESS;
}

int CyBtldr_ParseGetMetadataCmdResult(uint8_t* cmdBuf, uint16_t cmdSize, uint32_t* appCustId, uint16_t* appId, uint16_t* appVer, uint16_t* bootVer) {
    const uint16_t RESULT_DATA_SIZE = 56;
    const uint16_t RESULT_SIZE = BASE_CMD_SIZE + RESULT_DATA_SIZE;
    int err = CYRET_SUCCESS;

    if (cmdSize != RESULT_SIZE)
        err = CYRET_ERR_LENGTH;
    else if (cmdBuf[1] != CYRET_SUCCESS)
        err = CYRET_ERR_BTLDR_MASK | cmdBuf[1];
    else if (cmdBuf[0] != CMD_START || cmdBuf[2] != RESULT_DATA_SIZE || cmdBuf[3] != (RESULT_DATA_SIZE >> 8) || cmdBuf[RESULT_SIZE - 1] != CMD_STOP)
        err = CYRET_ERR_DATA;
    else {
        if(appCustId)
            *appCustId = (cmdBuf[7 + BTLDB_APP_CUST_ID_OFFSET] << 24) | (cmdBuf[6 + BTLDB_APP_CUST_ID_OFFSET] << 16) |
                (cmdBuf[5 + BTLDB_APP_CUST_ID_OFFSET] << 8) | cmdBuf[4 + BTLDB_APP_CUST_ID_OFFSET];
        if(appId)
            *appId = (cmdBuf[5 + BTLDB_APP_ID_OFFSET] << 8) | cmdBuf[4 + BTLDB_APP_ID_OFFSET];
        if(appVer)
            *appVer = (cmdBuf[5 + BTLDB_APP_VERSION_OFFSET] << 8) | cmdBuf[4 + BTLDB_APP_VERSION_OFFSET];
        if(bootVer)
            *bootVer = (cmdBuf[5 + BTLDR_APP_VERSION_OFFSET] << 8) | cmdBuf[4 + BTLDR_APP_VERSION_OFFSET];
    }

    return err;
}

