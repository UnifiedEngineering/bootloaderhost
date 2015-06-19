// cybootloaderutils lacks an implementation of the GetMetadata command
// Here's one, mostly structured like the other cybootloaderutils commands 
// Werner Johansson <wj@unifiedengineering.se>

#include <stdint.h>
#include "cymetadata.h"
#include <cybtldr_command.h>
#include <cybtldr_api.h>
#include <cybtldr_parse.h>

int CyBtldr_GetMetadata(CyBtldr_CommunicationsData* cyComms, const char* filename, uint8_t bootloadableNum, cymeta_t* meta) {
    uint16_t inSize = 0;
    uint16_t outSize = 0;
    uint8_t inBuf[MAX_COMMAND_SIZE];
    uint8_t outBuf[MAX_COMMAND_SIZE];
    int err;

    err = CyBtldr_OpenDataFile(filename);
    if (CYRET_SUCCESS == err) {
        unsigned long blVer = 0;
        unsigned long siliconId = 0;
        unsigned char siliconRev = 0;
        CyBtldr_ChecksumType chksumtype = SUM_CHECKSUM;
        uint8_t line[MAX_BUFFER_SIZE];
        unsigned int lineLen;
        uint8_t* securityKey = NULL;
        meta->appId = meta->appVer = meta->bootVer = meta->fileAppId = meta->fileAppVer = 0;
        meta->appCustId = meta->fileAppCustId = 0;

        err = CyBtldr_ReadLine(&lineLen, line);
        if (CYRET_SUCCESS == err)
            err = CyBtldr_ParseHeader(lineLen, line, &siliconId, &siliconRev, (uint8_t*)&chksumtype);

        if (CYRET_SUCCESS == err) {
            CyBtldr_SetCheckSumType(chksumtype);
            while( CYRET_SUCCESS == (err = CyBtldr_ReadLine(&lineLen, line)) ) {
                uint8_t dummyArrayId, dummyChecksum;
                uint16_t dummyRowNum, buffersize = sizeof(line);
                err = CyBtldr_ParseRowData(lineLen, line, &dummyArrayId, &dummyRowNum, line, &buffersize, &dummyChecksum);
                if(CYRET_SUCCESS == err) {
                    meta->fileAppCustId = (line[195 + BTLDB_APP_CUST_ID_OFFSET] << 24) | (line[194 + BTLDB_APP_CUST_ID_OFFSET] << 16) |
                                    (line[193 + BTLDB_APP_CUST_ID_OFFSET] << 8) | line[192 + BTLDB_APP_CUST_ID_OFFSET];
                    meta->fileAppId = (line[193 + BTLDB_APP_ID_OFFSET] << 8) | line[192 + BTLDB_APP_ID_OFFSET];
                    meta->fileAppVer = (line[193 + BTLDB_APP_VERSION_OFFSET] << 8) | line[192 + BTLDB_APP_VERSION_OFFSET];
                }
            }
            err = CyBtldr_StartBootloadOperation(cyComms, siliconId, siliconRev, &blVer, securityKey);
        }
    }
    CyBtldr_CloseDataFile();

    if (CYRET_SUCCESS == err)
        err = CyBtldr_CreateGetMetadataCmd(inBuf, &inSize, &outSize, bootloadableNum);
    if (CYRET_SUCCESS == err)
        err = CyBtldr_TransferData(inBuf, inSize, outBuf, outSize);
    if (CYRET_SUCCESS == err)
        err = CyBtldr_ParseGetMetadataCmdResult(outBuf, outSize, meta);

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

int CyBtldr_ParseGetMetadataCmdResult(uint8_t* cmdBuf, uint16_t cmdSize, cymeta_t* meta) {
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
        meta->appCustId = (cmdBuf[7 + BTLDB_APP_CUST_ID_OFFSET] << 24) | (cmdBuf[6 + BTLDB_APP_CUST_ID_OFFSET] << 16) |
                (cmdBuf[5 + BTLDB_APP_CUST_ID_OFFSET] << 8) | cmdBuf[4 + BTLDB_APP_CUST_ID_OFFSET];
        meta->appId = (cmdBuf[5 + BTLDB_APP_ID_OFFSET] << 8) | cmdBuf[4 + BTLDB_APP_ID_OFFSET];
        meta->appVer = (cmdBuf[5 + BTLDB_APP_VERSION_OFFSET] << 8) | cmdBuf[4 + BTLDB_APP_VERSION_OFFSET];
        meta->bootVer = (cmdBuf[5 + BTLDR_APP_VERSION_OFFSET] << 8) | cmdBuf[4 + BTLDR_APP_VERSION_OFFSET];
    }

    return err;
}

