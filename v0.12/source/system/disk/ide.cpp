#include "kernel.h"
#include <string.h>

void IDEDriver_IDEDriver(IDEDriver* driver, int irq_primary, int irq_secondary) {
    driver->controllers[PRIMARY].registers[ATA_DTR] = 0x1f0;
    driver->controllers[PRIMARY].registers[ATA_ERR] = 0x1f1;
    driver->controllers[PRIMARY].registers[ATA_SCR] = 0x1f2;
    driver->controllers[PRIMARY].registers[ATA_SNR] = 0x1f3;
    driver->controllers[PRIMARY].registers[ATA_CLR] = 0x1f4;
    driver->controllers[PRIMARY].registers[ATA_CHR] = 0x1f5;
    driver->controllers[PRIMARY].registers[ATA_DHR] = 0x1f6;
    driver->controllers[PRIMARY].registers[ATA_STR] = 0x1f7;
    driver->controllers[PRIMARY].registers[ATA_ASR] = 0x3f6;
    driver->controllers[PRIMARY].irq = irq_primary;
    driver->controllers[PRIMARY].selectedDevice = NULL;
	
    driver->controllers[SECONDARY].registers[ATA_DTR] = 0x170;
    driver->controllers[SECONDARY].registers[ATA_ERR] = 0x171;
    driver->controllers[SECONDARY].registers[ATA_SCR] = 0x172;
    driver->controllers[SECONDARY].registers[ATA_SNR] = 0x173;
    driver->controllers[SECONDARY].registers[ATA_CLR] = 0x174;
    driver->controllers[SECONDARY].registers[ATA_CHR] = 0x175;
    driver->controllers[SECONDARY].registers[ATA_DHR] = 0x176;
    driver->controllers[SECONDARY].registers[ATA_STR] = 0x177;
    driver->controllers[SECONDARY].registers[ATA_ASR] = 0x376;
    driver->controllers[SECONDARY].irq = irq_secondary;
    driver->controllers[SECONDARY].selectedDevice = NULL;
	
    IDEDriver_initialize(driver, &driver->controllers[PRIMARY]);
    IDEDriver_initialize(driver, &driver->controllers[SECONDARY]);
	
    driver->whichController = NULL;
    driver->atapiBuffer     = NULL;
    driver->atapiReadDone   = true;
}

bool IDEDriver_selectDevice(IDEDriver* driver, int controller, int deviceNo) {
    if (controller != PRIMARY && controller != SECONDARY) return false;
    if (deviceNo != MASTER && deviceNo != SLAVE) return false;
	
    IDEDevice* device = &driver->controllers[controller].devices[deviceNo];
	
    if (device->type == DEVICE_NONE || device->type == DEVICE_UNKNOWN) return false;
	
    driver->controllers[controller].selectedDevice = device;
    driver->whichController = &driver->controllers[controller];
    return true;
}

bool IDEDriver_findDevice(IDEDriver* driver, int type, int detail, int* controller, int* deviceNo) {
    int i, j;
	
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            IDEDevice* device = &driver->controllers[i].devices[j];
			
            if (type != device->type || detail != device->typeDetail) continue;
			
            *controller = i;
            *deviceNo = j;
            return true;
        }
    }
	
    return false;
}

int IDEDriver_read(IDEDriver* driver, dword lba, void* buffer, int size) {
    int i, j;
	
    if (driver->whichController == NULL) return 1;
	
    if (driver->whichController->selectedDevice->type == DEVICE_ATAPI) {
        int count = (size + 0xf800 - 1) / 0xf800;
		
        for (i = 0; i < count; i++) {
            int readSize;
            bool readResult;
            if (i == count - 1) {
                readSize = size - 0xf800 * i;
            } else {
                readSize = 0xf800;
            }
			
            for (j = 0; j < 20; j++) {
                readResult = IDEDriver_commandRead10(driver, driver->whichController, lba + 0xf800 * i / 2048, buffer, readSize);
                if (readResult) break;
            }
			
            buffer = (void*)((byte*)buffer + readSize);
			
            if (!readResult) {
                return driver->lastError;
            }
        }
        return 0;
    } else if (driver->whichController->selectedDevice->type == DEVICE_ATA) {
        return 4;
    } else {
        return 5;
    }
}

void IDEDriver_outp8(IDEDriver* driver, IDEController* controller, int reg, byte value) {
    io_out8(controller->registers[reg], value);
}

byte IDEDriver_inp8(IDEDriver* driver, IDEController* controller, int reg) {
    return io_in8(controller->registers[reg]);
}

word IDEDriver_inp16(IDEDriver* driver, IDEController* controller, int reg) {
    return io_in16(controller->registers[reg]);
}

void IDEDriver_outp16(IDEDriver* driver, IDEController* controller, int reg, word value) {
    io_out16(controller->registers[reg], value);
}

void IDEDriver_outp16_2(IDEDriver* driver, IDEController* controller, word* data, int length) {
    int i;
	
    for (i = 0; i < length; i++) {
        IDEDriver_outp16(driver, controller, ATA_DTR, *data);
        data++;
    }
}

void IDEDriver_inp16_2(IDEDriver* driver, IDEController* controller, word* data, int size) {
    int i;
    int length = size / 2;
	
    if (data == NULL) {
        for (i = 0; i < length; i++) {
            IDEDriver_inp16(driver, controller, ATA_DTR);
        }
    } else {
        for (i = 0; i < length; i++) {
            *data = IDEDriver_inp16(driver, controller, ATA_DTR);
            data++;
        }
    }
}

bool IDEDriver_waitBusyAndDataRequestBothClear(IDEDriver* driver, IDEController* controller) {
    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++) {
        byte status = IDEDriver_inp8(driver, controller, ATA_ASR);
        if ((status & BIT_BSY) == 0 && (status & BIT_DRQ) == 0) break;
    }
    return (i != ATA_TIMEOUT);
}

bool IDEDriver_waitBusyClear(IDEDriver* driver, IDEController* controller) {
    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++) {
        byte status = IDEDriver_inp8(driver, controller, ATA_ASR);
        if ((status & BIT_BSY) == 0) break;
    }
    return (i != ATA_TIMEOUT);
}

bool IDEDriver_waitDrdySet(IDEDriver* driver, IDEController* controller) {
    dword i;
	
    for (i = 0; i < ATA_TIMEOUT; i++) {
        byte status = IDEDriver_inp8(driver, controller, ATA_ASR);
        if (status & BIT_DRDY) break;
    }
	
    return (i != ATA_TIMEOUT);
}

bool IDEDriver_protocolPacket(IDEDriver* driver, IDEController* controller, ATAPICommand* command) {
    IDEDriver_outp8(driver, controller, ATA_DCR, 0x8);
	
    driver->atapiBuffer = command->buffer;
    driver->atapiReadDone = false;
    driver->atapiTotalReadSize = command->limit;
	
    if (!IDEDriver_selectDevice_2(driver, controller, command->deviceNo)) {
        driver->lastError = SELECTION_ERROR;
        return false;
    }
	
    IDEDriver_outp8(driver, controller, ATA_FTR, command->feature);
    IDEDriver_outp8(driver, controller, ATA_SCR, 0);
    IDEDriver_outp8(driver, controller, ATA_BLR, (byte)(command->limit & 0xff));
    IDEDriver_outp8(driver, controller, ATA_BHR, (byte)(command->limit >> 8));
    IDEDriver_outp8(driver, controller, ATA_CMR, 0xa0);
    IDEDriver_sleep(driver, 1);
	
    dword i;
    for (i = 0; i < ATA_TIMEOUT; i++) {
        byte status = IDEDriver_inp8(driver, controller, ATA_ASR);
		
        if ((status & BIT_BSY) != 0) continue;
        if ((status & BIT_CHK) != 0) {
            driver->atapiBuffer = NULL;
            IDEDriver_inp8(driver, controller, ATA_ERR);
            driver->lastError = STATUS_ERROR;
            return false;
        }
		
        byte reason = IDEDriver_inp8(driver, controller, ATA_IRR);
        if (((status & BIT_DRQ) != 0) && ((reason & BIT_IO) == 0) && ((reason & BIT_CD) != 0)) break;
    }
	
    if (i == ATA_TIMEOUT) {
        driver->atapiBuffer = NULL;
        driver->lastError = BUSY_TIMEOUT_ERROR;
        return false;
    }
	
    IDEDriver_outp16_2(driver, controller, (word*)command->packet, 6);
    for (i = 0; i < ATA_TIMEOUT; i++) {
        IDEDriver_protocolInterrupt(driver);
        byte status = IDEDriver_inp8(driver, controller, ATA_ASR);
        if ((status & BIT_BSY) != 0) continue;
        if ((status & BIT_CHK) != 0) {
            driver->atapiBuffer = NULL;
            driver->lastError = STATUS_ERROR;
            return false;
        }
        if (driver->atapiReadDone) break;
    }
	
    IDEDriver_inp8(driver, controller, ATA_STR);
	
    if (i == ATA_TIMEOUT) {
        driver->atapiBuffer = NULL;
        driver->lastError = BUSY_TIMEOUT_ERROR;
        return false;
    }
	
    return true;
}

bool IDEDriver_protocolPioDataIn(IDEDriver* driver, IDEController* controller, ATACommand* command, word count, void* buf) {
    int i;
    word* p = (word*)buf;
	
    if (!IDEDriver_selectDevice_2(driver, controller, command->deviceNo)) {
		
        driver->lastError = SELECTION_ERROR;
        return false;
    }
	
    IDEDriver_outp8(driver, controller, ATA_DCR, 0x02);
    IDEDriver_outp8(driver, controller, ATA_FTR, command->feature);
    IDEDriver_outp8(driver, controller, ATA_SCR, command->sectorCount);
    IDEDriver_outp8(driver, controller, ATA_SNR, command->sectorNumber);
    IDEDriver_outp8(driver, controller, ATA_CLR, command->cylinderLow);
    IDEDriver_outp8(driver, controller, ATA_CHR, command->cylinderHigh);
	
    if (command->drdyCheck && !IDEDriver_waitDrdySet(driver, controller)) {
        driver->lastError = DATA_READY_CHECK_ERROR;
        return false;
    }
	
    IDEDriver_outp8(driver, controller, ATA_CMR, command->command);
    IDEDriver_sleep(driver, 1);
	
    IDEDriver_inp8(driver, controller, ATA_ASR);
	
    for (i = 0; i < count; i++, p+=256) {
        if (!IDEDriver_waitBusyClear(driver, controller)) {
            driver->lastError = BUSY_TIMEOUT_ERROR;
            return false;
        }
		
        byte status = IDEDriver_inp8(driver, controller, ATA_STR);
		
        if ((status & BIT_ERR) != 0) {
            driver->lastError = STATUS_ERROR;
            return false;
        }
		
        if ((status & BIT_DRQ) == 0) {
            driver->lastError = DATA_READY_CHECK_ERROR;
            return false;
        }
		
        IDEDriver_inp16_2(driver, controller, p, 512);
    }
	
    IDEDriver_inp8(driver, controller, ATA_ASR);
    byte status = IDEDriver_inp8(driver, controller, ATA_STR);
	
    if (status & BIT_ERR) {
        IDEDriver_inp8(driver, controller, ATA_ERR);
        driver->lastError = STATUS_ERROR;
        return false;
    }
    return true;
}

void IDEDriver_protocolInterrupt(IDEDriver* driver) {
    for (;;) {
        //if (!MONAPI_WAIT_INTERRUPT(1000, driver->whichController->irq)) {
        //    return;
        //}
		
        byte status = IDEDriver_inp8(driver, driver->whichController, ATA_STR);
        byte reason = IDEDriver_inp8(driver, driver->whichController, ATA_IRR);
		
        if (((reason & BIT_IO) != 0) && ((reason & BIT_CD) == 0) && ((status & BIT_DRQ) != 0)) {
            word transferSize = (IDEDriver_inp8(driver, driver->whichController, ATA_BHR) << 8) | IDEDriver_inp8(driver, driver->whichController, ATA_BLR);
            driver->atapiTransferSize += transferSize;
			
            if (driver->atapiTransferSize > driver->atapiTotalReadSize) {
                IDEDriver_inp16_2(driver, driver->whichController, NULL, transferSize);
            } else {
                IDEDriver_inp16_2(driver, driver->whichController, (word*)driver->atapiBuffer, transferSize);
                driver->atapiBuffer = (void*)((byte*)driver->atapiBuffer + transferSize);
            }
        }
		
        if (((reason & BIT_IO)!=0) && ((reason & BIT_CD) != 0) && ((status & BIT_DRQ) == 0)) {
            driver->atapiReadDone = true;
            return;
        }
    }
}

bool IDEDriver_commandRead10(IDEDriver* driver, IDEController* controller, dword lba, void* buffer, int size) {
    ATAPICommand command;
    memset(&command, 0, sizeof(command));
	
    int count = (size + ATAPI_SECTOR_SIZE - 1) / ATAPI_SECTOR_SIZE;
	
    command.feature = 0;
    command.deviceNo = controller->selectedDevice->deviceNo;
    command.packet[0] = 0x28;
    command.packet[2] = (lba >> 24) & 0xff;
    command.packet[3] = (lba >> 16) & 0xff;
    command.packet[4] = (lba >> 8) & 0xff;
    command.packet[5] = (lba ) & 0xff;
    command.packet[7] = (count >> 8) & 0xff;
    command.packet[8] = (count ) & 0xff;
    command.limit = ATAPI_SECTOR_SIZE * count;
    command.buffer = buffer;
    driver->atapiTransferSize = 0;
	
    return IDEDriver_protocolPacket(driver, controller, &command);
}

bool IDEDriver_commandIdentify(IDEDriver* driver, IDEController* controller, int deviceNo, word* buffer) {
    int i;
    ATACommand command;
    memset(&command, 0, sizeof(command));
	
    IDEDevice* device = &controller->devices[deviceNo];
	
    command.deviceNo = deviceNo;
    if (device->type == DEVICE_ATA) {
        command.drdyCheck = true;
        command.command = 0xec;
    } else {
        command.drdyCheck = false;
        command.command = 0xa1;
    }
	
    bool commandOK = IDEDriver_protocolPioDataIn(driver, controller, &command, 1, buffer);
	
    if (!commandOK) return false;
	
    word* p = buffer;
    for (i = 0; i < 256; i++) {
        word value = *p;
        *p = ((value >> 8) & 0xff) | ((value << 8) & 0xff00);
        p++;
    }
	
    return true;
}

void IDEDriver_sleep(IDEDriver* driver, dword ms) {
    while (0 < ms--) {
        __asm__ __volatile__ ("outb %%al, $0x80" ::: "%eax");
    }
}

void IDEDriver_initialize(IDEDriver* driver, IDEController* controller) {
    IDEDriver_outp8(driver, controller, ATA_DCR, 0x06);
    IDEDriver_sleep(driver, 5);
	
    IDEDriver_outp8(driver, controller, ATA_DCR, 0x02);
    IDEDriver_sleep(driver, 5);
	
    IDEDriver_setDeviceTypeFirst(driver, controller, MASTER);
    IDEDriver_setDeviceTypeSecond(driver, controller, MASTER);
	
    IDEDriver_setDeviceTypeFirst(driver, controller, SLAVE);
    IDEDriver_setDeviceTypeSecond(driver, controller, SLAVE);
}

void IDEDriver_setDeviceTypeFirst(IDEDriver* driver, IDEController* controller, int deviceNo) {
    dword l;
    byte c;
    byte c1 = 0xff;
    byte c2 = 0xff;
    bool timeout;
	
    IDEDevice* device = &controller->devices[deviceNo];
    device->deviceNo = deviceNo;
	
    for (l = 0; l < RETRY_MAX; l++) {
		
        IDEDriver_outp8(driver, controller, ATA_DHR, IDEDriver_deviceValue(driver, deviceNo));
        IDEDriver_sleep(driver, 10);
		
        c = IDEDriver_inp8(driver, controller, ATA_STR);
        if (c == 0xff) break;
		
        timeout = !IDEDriver_waitBusyClear(driver, controller);
        if (timeout) break;
		
		
        byte error = IDEDriver_inp8(driver, controller, ATA_ERR);
        if (deviceNo == MASTER && (error & 0x7f) != 1) {
            device->type = DEVICE_UNKNOWN;
            return;
        } else if (deviceNo == SLAVE && error != 1) {
            device->type = DEVICE_UNKNOWN;
            return;
        }
		
        c = IDEDriver_inp8(driver, controller, ATA_DHR);
        if ((c & (deviceNo << 4)) == (deviceNo << 4)) {
            c1 = IDEDriver_inp8(driver, controller, ATA_CLR);
            c2 = IDEDriver_inp8(driver, controller, ATA_CHR);
            break;
        }
    }
	
    switch (c1 | (c2 << 8)) {
		case 0xEB14:
			device->type = DEVICE_ATAPI;
			break;
		case 0:
			device->type = DEVICE_ATA;
			break;
		default:
			device->type = DEVICE_NONE;
			break;
    }
}

void IDEDriver_setDeviceTypeSecond(IDEDriver* driver, IDEController* controller, int deviceNo) {
    word buffer[256];
    IDEDevice* device = &(controller->devices[deviceNo]);
	
    if (!IDEDriver_waitBusyClear(driver, controller)) {
        device->type = DEVICE_NONE;
        return;
    }
	
    byte l;
    for (l = 0; l < RETRY_MAX; l++) {
        bool firstResult = IDEDriver_commandIdentify(driver, controller, deviceNo, buffer);
        int firstError = driver->lastError;
        IDEDriver_sleep(driver, 5);
        bool secondResult = IDEDriver_commandIdentify(driver, controller, deviceNo, buffer);
        int secondError = driver->lastError;
		
        if (firstResult && secondResult) {
            break;
        } else if (!firstResult && !secondResult) {
            if (firstError != secondError) continue;
            if (firstError == SELECTION_ERROR || firstError == BUSY_TIMEOUT_ERROR || firstError == DATA_READY_CHECK_ERROR) {
                device->type = DEVICE_NONE;
                break;
            }
        }
    }
	
    if (l == RETRY_MAX) {
        device->type = DEVICE_UNKNOWN;
    }
	
    switch (device->type) {
		case DEVICE_ATA:
			device->typeDetail = -1;
			device->sectorSize = ATA_SECTOR_SIZE;
			break;
			
		case DEVICE_ATAPI:
			device->typeDetail = buffer[0] & 0x1f;
			device->sectorSize = ATAPI_SECTOR_SIZE;
			break;
			
		case DEVICE_NONE:
			device->typeDetail = -1;
			break;
			
		case DEVICE_UNKNOWN:
			device->typeDetail = -1;
			break;
    }
}

bool IDEDriver_selectDevice_2(IDEDriver* driver, IDEController* controller, int deviceNo) {
    if (driver->whichController != NULL) {
        IDEDevice* device = driver->whichController->selectedDevice;
        if (driver->whichController == controller && device->deviceNo == deviceNo) {
            IDEDriver_outp8(driver, controller, ATA_DHR, IDEDriver_deviceValue(driver, deviceNo));
            IDEDriver_sleep(driver, 10);
            return true;
        }
    }
	
    if (!IDEDriver_waitBusyAndDataRequestBothClear(driver, controller)) return false;
	
    IDEDriver_outp8(driver, controller, ATA_DHR, IDEDriver_deviceValue(driver, deviceNo));
    IDEDriver_sleep(driver, 10);
	
    if (!IDEDriver_waitBusyAndDataRequestBothClear(driver, controller)) return false;
	
    driver->whichController = controller;
    driver->whichController->selectedDevice = &controller->devices[deviceNo];
    driver->whichController->selectedDevice->deviceNo = deviceNo;
	
    return true;
}

byte IDEDriver_deviceValue(IDEDriver* driver, int deviceNo) {
    return (byte)(DEV_HEAD_OBS | (deviceNo << 4));
}
