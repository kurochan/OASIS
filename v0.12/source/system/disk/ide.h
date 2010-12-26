/*!
 _file  IDEDriver.h
 _brief IDEDriver
 
 Copyright (c) 2004 HigePon
 WITHOUT ANY WARRANTY
 
 _author  HigePon
 _version $Revision: 3344 $
 _date   create:2004/07/04 update:$Date: 2006-07-21 03:03:17 +0900 (Fri, 21 Jul 2006) $
 */
#ifndef _DRIVER_IDE_H_INCLUDED_
#define _DRIVER_IDE_H_INCLUDED_

#ifndef NULL
#define NULL 0
#endif

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;

#if (!defined(__cplusplus))
typedef unsigned char  bool;
#define true  1
#define false 0
#endif

#define SECTOR_SIZE 2048

#define IDE_IRQ_PRIMARY   14
#define IDE_IRQ_SECONDARY 15

#define ISO_PRIMARY_VOLUME_DESCRIPTOR 1

enum {
    PRIMARY   = 0,
    SECONDARY = 1,
    MASTER    = 0,
    SLAVE     = 1,
    DEVICE_UNKNOWN,
    DEVICE_NONE,
    DEVICE_ATA,
    DEVICE_ATAPI
};

enum {
    SELECTION_ERROR = 1,
    DATA_READY_CHECK_ERROR,
    BUSY_TIMEOUT_ERROR,
    STATUS_ERROR
};

enum {
    ATA_DTR        = 0,
    ATA_ERR        = 1,
    ATA_FTR        = 1,
    ATA_SCR        = 2,
    ATA_IRR        = 2,
    ATA_SNR        = 3,
    ATA_CLR        = 4,
    ATA_BLR        = 4,
    ATA_CHR        = 5,
    ATA_BHR        = 5,
    ATA_DHR        = 6,
    ATA_STR        = 7,
    ATA_CMR        = 7,
    ATA_ASR        = 8,
    ATA_DCR        = 8,
    ATA_TIMEOUT    = 100000, // for Virtual PC changed from 100000 -> 10000
    BIT_BSY        = 0x80,
    BIT_DRDY       = 0x40,
    BIT_DRQ        = 8,
    BIT_ABRT       = 4,
    BIT_CHK        = 1,
    BIT_ERR        = 1,
    BIT_IO         = 2,
    BIT_CD         = 1,
    RETRY_MAX      = 2,
    DEV_HEAD_OBS   = 0xa0,
    LBA_FLG        = 0x40,
    REQUEST_SENSE_BUFFER_SIZE = 18,
	
    ATAPI_SECTOR_SIZE = 2048,
    ATA_SECTOR_SIZE   = 512
};

typedef struct {
    int  type;
    int  typeDetail;
    int  deviceNo;
    word sectorSize;
} IDEDevice;

typedef struct {
    byte irq;
    int registers[10];
    IDEDevice devices[2];
    IDEDevice* selectedDevice;
} IDEController;

typedef struct {
    byte feature;
    byte sectorCount;
    byte sectorNumber;
    byte cylinderLow;
    byte cylinderHigh;
    byte deviceNo;
    byte command;
    bool drdyCheck;
} ATACommand;

typedef struct {
    byte  feature;
    byte  deviceNo;
    byte  packet[12];
    word  limit;
    void* buffer;
} ATAPICommand;

#define ISO_POSITION(from, to) (to - from + 1)

typedef struct {
    char  type                  [ISO_POSITION (  1,   1)];
    char  id                    [ISO_POSITION (  2,   6)];
    char  version               [ISO_POSITION (  7,   7)];
    char  unused1               [ISO_POSITION (  8,   8)];
    char  system_id             [ISO_POSITION (  9,  40)];
    char  volume_id             [ISO_POSITION ( 41,  72)];
    char  unused2               [ISO_POSITION ( 73,  80)];
    char  volume_space_size     [ISO_POSITION ( 81,  88)];
    char  unused3               [ISO_POSITION ( 89, 120)];
    char  volume_set_size       [ISO_POSITION (121, 124)];
    char  volume_sequence_number[ISO_POSITION (125, 128)];
    word  logical_block_size_l;
    word  logical_block_size_b;
    dword path_table_size_l;
    dword path_table_size_b;
    dword type_l_path_table;
    dword opt_type_l_path_table;
    dword type_m_path_table;
    dword opt_type_m_path_table;
    char  root_directory_record [ISO_POSITION (157, 190)];
    char  volume_set_id         [ISO_POSITION (191, 318)];
    char  publisher_id          [ISO_POSITION (319, 446)];
    char  preparer_id           [ISO_POSITION (447, 574)];
    char  application_id        [ISO_POSITION (575, 702)];
    char  copyright_file_id     [ISO_POSITION (703, 739)];
    char  abstract_file_id      [ISO_POSITION (740, 776)];
    char  bibliographic_file_id [ISO_POSITION (777, 813)];
    char  creation_date         [ISO_POSITION (814, 830)];
    char  modification_date     [ISO_POSITION (831, 847)];
    char  expiration_date       [ISO_POSITION (848, 864)];
    char  effective_date        [ISO_POSITION (865, 881)];
    char  file_structure_version[ISO_POSITION (882, 882)];
    char  unused4               [ISO_POSITION (883, 883)];
    char  application_data      [ISO_POSITION (884, 1395)];
    char  unused5               [ISO_POSITION (1396, 2048)];
} PrimaryVolumeDescriptor;

#pragma pack(1)
typedef struct {
    byte length;
    byte ext_attr_length;
    dword extent_l;
    dword extent_b;
    dword size_l;
    dword size_b;
    char date[ISO_POSITION (19, 25)];
    unsigned existence      : 1;
    unsigned directory      : 1;
    unsigned associetedFile : 1;
    unsigned record         : 1;
    unsigned protection     : 1;
    unsigned reserved       : 2;
    unsigned lastRecord     : 1;
    byte file_unit_size;
    byte interleave;
    word volume_sequence_number_l;
    word volume_sequence_number_b;
    byte name_len;
    char name[0];
} DirectoryEntry;
#pragma pack(0)

typedef struct {
    IDEController controllers[2];
    IDEController* whichController;
    volatile void* atapiBuffer;
    volatile int atapiReadDone;
    volatile dword atapiTransferSize;
    volatile dword atapiTotalReadSize;
    byte requestSenseBuffer[REQUEST_SENSE_BUFFER_SIZE];
    int lastError;
} IDEDriver;

/* public */
void IDEDriver_IDEDriver(IDEDriver* driver, int irq_primary, int irq_secondary);

/* public */
int  IDEDriver_read(IDEDriver* driver, dword lba, void* buf, int size);

/* public */
void IDEDriver_protocolInterrupt(IDEDriver* driver);
bool IDEDriver_selectDevice(IDEDriver* driver, int controller, int deviceNo);
bool IDEDriver_findDevice(IDEDriver* driver, int type, int detail, int* controller, int* deviceNo);

/* I/O */
void IDEDriver_outp8(IDEDriver* driver, IDEController* controller, int reg, byte value);
byte IDEDriver_inp8(IDEDriver* driver, IDEController* controller, int reg);
void IDEDriver_inp16_2(IDEDriver* driver, IDEController* controller, word* data, int length);
word IDEDriver_inp16(IDEDriver* driver, IDEController* controller, int reg);
void IDEDriver_outp16(IDEDriver* driver, IDEController* controller, int reg, word value);
void IDEDriver_outp16_2(IDEDriver* driver, IDEController* controller, word* data, int length);

/* flag utilities */
bool IDEDriver_waitBusyClear(IDEDriver* driver, IDEController* controller);
bool IDEDriver_waitBusyAndDataRequestBothClear(IDEDriver* driver, IDEController* controller);
bool IDEDriver_waitDrdySet(IDEDriver* driver, IDEController* controller);

/* protocol: param IDEController. driver layer returns error code */
bool IDEDriver_protocolAtaNoneData(IDEDriver* driver, IDEController* controller, ATACommand* command);
bool IDEDriver_protocolPacket(IDEDriver* driver, IDEController* controller, ATAPICommand* command);
bool IDEDriver_protocolPioDataIn(IDEDriver* driver, IDEController* controller, ATACommand* command, word count, void* buf);

/* command : execute command using protocol function */
bool IDEDriver_commandIdleImmediate(IDEDriver* driver, IDEController* controller, int deviceNo);
bool IDEDriver_commandRead10(IDEDriver* driver, IDEController* controller, dword lba, void* buffer, int size);
bool IDEDriver_commandIdentify(IDEDriver* driver, IDEController* controller, int deviceNo, word* buffer);

/* private : functions */
void IDEDriver_sleep(IDEDriver* driver, dword ms);
void IDEDriver_initialize(IDEDriver* driver, IDEController* controller);
void IDEDriver_setDeviceTypeFirst(IDEDriver* driver, IDEController* controller, int deviceNo);
void IDEDriver_setDeviceTypeSecond(IDEDriver* driver, IDEController* controller, int deviceNo);
bool IDEDriver_selectDevice_2(IDEDriver* driver, IDEController* controller, int deviceNo);
byte IDEDriver_deviceValue(IDEDriver* driver, int deviceNo);

#endif
