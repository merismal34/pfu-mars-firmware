/* scsi_defs.h: the SCSI protocol's constants (command codes, status, sense, mode pages, window
   descriptor codes, vendor diagnostic requests), shared by the application (through common.h) and the
   boot programs (boot.h). */
#ifndef SCSI_DEFS_H
#define SCSI_DEFS_H

/* SCSI command operation codes (the command tables of the application and of program 2; SANE fujitsu-scsi.h) */
#define SCSI_OP_TEST_UNIT_READY        0x00
#define SCSI_OP_REQUEST_SENSE          0x03
#define SCSI_OP_INQUIRY                0x12
#define SCSI_OP_MODE_SELECT            0x15
#define SCSI_OP_RESERVE_UNIT           0x16
#define SCSI_OP_RELEASE_UNIT           0x17
#define SCSI_OP_MODE_SENSE             0x1A
#define SCSI_OP_SCAN                   0x1B
#define SCSI_OP_READ_DIAGNOSTIC        0x1C
#define SCSI_OP_SEND_DIAGNOSTIC        0x1D
#define SCSI_OP_SET_WINDOW             0x24
#define SCSI_OP_GET_WINDOW             0x25
#define SCSI_OP_READ                   0x28
#define SCSI_OP_SEND                   0x2A
#define SCSI_OP_OBJECT_POSITION        0x31
#define SCSI_OP_GET_DATA_BUFFER_STATUS 0x34
#define SCSI_OP_SET_SUBWINDOW          0xC0
#define SCSI_OP_ENDORSER               0xC1
#define SCSI_OP_GET_HW_STATUS          0xC2
#define SCSI_OP_SCANNER_CONTROL        0xF1

/* SCSI status bytes (scsi_status_byte, the status phase, byte 9 of the USB status block) */
#define SCSI_STATUS_GOOD                 0x00
#define SCSI_STATUS_CHECK_CONDITION      0x02
#define SCSI_STATUS_BUSY                 0x08
#define SCSI_STATUS_RESERVATION_CONFLICT 0x18

/* sense keys (sense byte 2 bits 3-0; scsi_set_sense's first argument) */
#define SCSI_SENSE_NO_SENSE        0x0
#define SCSI_SENSE_NOT_READY       0x2
#define SCSI_SENSE_MEDIUM_ERROR    0x3
#define SCSI_SENSE_HARDWARE_ERROR  0x4
#define SCSI_SENSE_ILLEGAL_REQUEST 0x5
#define SCSI_SENSE_UNIT_ATTENTION  0x6
#define SCSI_SENSE_ABORTED_COMMAND 0xB

/* additional sense code and qualifier, asc << 8 | ascq (scsi_set_sense's second argument, the error-to-sense tables); vendor ASC 0x80 means one thing per sense key, the key is part of the name */
#define SCSI_ASC_NONE                        0x0000
#define SCSI_ASC_PARAMETER_LIST_LENGTH       0x1A00
#define SCSI_ASC_INVALID_OPCODE              0x2000
#define SCSI_ASC_INVALID_FIELD_IN_CDB        0x2400
#define SCSI_ASC_INVALID_FIELD_IN_PARAMETERS 0x2600
#define SCSI_ASC_POWER_ON_RESET              0x2900
#define SCSI_ASC_MODE_PARAMETERS_CHANGED     0x2A01
#define SCSI_ASC_COMMAND_SEQUENCE            0x2C00
#define SCSI_ASC_INVALID_WINDOW_COMBINATION  0x2C02
#define SCSI_ASC_MESSAGE_ERROR               0x4300
#define SCSI_ASC_INTERNAL_TARGET_FAILURE     0x4400
#define SCSI_ASC_SELECT_RESELECT_FAILURE     0x4500
#define SCSI_ASC_SCSI_PARITY_ERROR           0x4700
#define SCSI_ASC_INITIATOR_DETECTED_ERROR    0x4800
#define SCSI_ASC_OVERLAPPED_COMMANDS         0x4E00
#define SCSI_ASC_MEDIUM_PAPER_JAM            0x8001
#define SCSI_ASC_MEDIUM_COVER_OPEN           0x8002
#define SCSI_ASC_MEDIUM_HOPPER_EMPTY         0x8003
#define SCSI_ASC_MEDIUM_UNUSUAL_PAPER        0x8004
#define SCSI_ASC_MEDIUM_MULTIFEED            0x8007
#define SCSI_ASC_MEDIUM_NO_CARTRIDGE         0x8010
#define SCSI_ASC_MEDIUM_TEMPORARY_NO_DATA    0x8013
#define SCSI_ASC_MEDIUM_IMPRINTER            0x8014
#define SCSI_ASC_MEDIUM_STOP_BUTTON          0x8020
#define SCSI_ASC_MEDIUM_PAUSED_BY_USER       0x8031
#define SCSI_ASC_MEDIUM_SCAN_PAUSED          0x8032
#define SCSI_ASC_HARDWARE_MOTOR_FUSE         0x8001
#define SCSI_ASC_HARDWARE_LAMP_FUSE          0x8003
#define SCSI_ASC_HARDWARE_MOTOR_CIRCUIT      0x8004
#define SCSI_ASC_HARDWARE_MECHANICAL         0x8005
#define SCSI_ASC_HARDWARE_OPTICAL            0x8006
#define SCSI_ASC_HARDWARE_FAN                0x8007
#define SCSI_ASC_HARDWARE_OPTION             0x8008
#define SCSI_ASC_HARDWARE_IMPRINTER          0x8010
#define SCSI_ASC_HARDWARE_IMPRINTER_FUSE     0x8011
#define SCSI_ASC_ABORTED_IMAGE_TRANSFER      0x8001

/* program 2 passes key and ASC/ASCQ as one int (fw_update_set_sense) */
#define SCSI_SENSE(key, asc_ascq) ((key) << 16 | (asc_ascq))

/* fixed-format sense data bytes */
#define SCSI_SENSE_CURRENT_ERROR 0xF0
#define SCSI_SENSE_EOM           0x40
#define SCSI_SENSE_SKSV          0x80

/* MODE SELECT / MODE SENSE page codes (page code & 0x3F) */
#define SCSI_PAGE_DISCONNECT_RECONNECT    0x02
#define SCSI_PAGE_MEASUREMENT_UNITS       0x03
#define SCSI_PAGE_POWER_CONTROL           0x2C
#define SCSI_PAGE_DRIVER_FUNCTION_CONFIRM 0x2D
#define SCSI_PAGE_AUTO_COLOR              0x32
#define SCSI_PAGE_PREPICK                 0x33
#define SCSI_PAGE_SLEEP                   0x34
#define SCSI_PAGE_INTERLACE_READ          0x35
#define SCSI_PAGE_DEVICE_CONTROLS         0x36
#define SCSI_PAGE_BACKING                 0x37
#define SCSI_PAGE_DOUBLE_FEED             0x38
#define SCSI_PAGE_DROPOUT                 0x39
#define SCSI_PAGE_BUFFER                  0x3A
#define SCSI_PAGE_AUTO_SIZE               0x3C
#define SCSI_PAGE_LAMP_TIMER              0x3D
#define SCSI_PAGE_JOB_SEPARATION          0x3E
#define SCSI_PAGE_ALL                     0x3F

/* MODE SENSE page control (CDB byte 2 & 0xC0) */
#define SCSI_PAGE_CONTROL_CURRENT    0x00
#define SCSI_PAGE_CONTROL_CHANGEABLE 0x40
#define SCSI_PAGE_CONTROL_DEFAULT    0x80

/* SCANNER CONTROL function (CDB byte 1 & 0x0F) */
#define SCSI_SCANNER_CONTROL_ADF            0
#define SCSI_SCANNER_CONTROL_FLATBED        1
#define SCSI_SCANNER_CONTROL_FLATBED_HS     2
#define SCSI_SCANNER_CONTROL_LAMP_OFF       3
#define SCSI_SCANNER_CONTROL_CANCEL         4
#define SCSI_SCANNER_CONTROL_LAMP_ON        5
#define SCSI_SCANNER_CONTROL_LAMP_NORMAL    6
#define SCSI_SCANNER_CONTROL_LAMP_SAVING    7
#define SCSI_SCANNER_CONTROL_SCAN_COMPLETE  9
#define SCSI_SCANNER_CONTROL_EJECT_COMPLETE 10
#define SCSI_SCANNER_CONTROL_MANUAL_FEED    12
#define SCSI_SCANNER_CONTROL_PAUSE          13

/* READ data type code (CDB byte 2) */
#define SCSI_READ_IMAGE      0x00
#define SCSI_READ_PIXEL_SIZE 0x80
#define SCSI_READ_PAPER_SIZE 0x81

/* SEND data type code (CDB byte 2) */
#define SCSI_SEND_HALFTONE      0x02
#define SCSI_SEND_GAMMA         0x03
#define SCSI_SEND_LUT           0x83
#define SCSI_SEND_JPEG_QTABLE   0x88
#define SCSI_SEND_ENDORSER_DATA 0x90

/* SEND DIAGNOSTIC: the target of a vendor request (parameter byte 0) */
#define SCSI_DIAG_TARGET_SOC            0xFA
#define SCSI_DIAG_TARGET_SDC            0xFB
#define SCSI_DIAG_TARGET_IMPRINTER_POST 0xFC
#define SCSI_DIAG_TARGET_PUC            0xFD
#define SCSI_DIAG_TARGET_MDC            0xFE
#define SCSI_DIAG_TARGET_IMPRINTER_PRE  0xFF
#define SCSI_DIAG_TARGET_DEFECT_LISTS   0xF0

/* SEND DIAGNOSTIC: the firmware-update request after a target byte (parameter byte 1) */
#define SCSI_DIAG_FLASH_WRITE        0x00
#define SCSI_DIAG_FLASH_INFO_SEND    0x01
#define SCSI_DIAG_FLASH_WRITE_START  0x10
#define SCSI_DIAG_FLASH_WRITE_END    0x11
#define SCSI_DIAG_FLASH_READ_BLOCK   0x80
#define SCSI_DIAG_FLASH_READ_INFO    0x81
#define SCSI_DIAG_FLASH_READ_VERSION 0x82

/* SEND DIAGNOSTIC after SCSI_DIAG_TARGET_DEFECT_LISTS: write (with the lists) or arm a read-back
   (parameter byte 1), and the lists' length */
#define SCSI_DIAG_DEFECT_LISTS_WRITE 0x00
#define SCSI_DIAG_DEFECT_LISTS_READ  0x80
#define SCSI_DIAG_DEFECT_LISTS_LEN   0x900

/* SET WINDOW descriptor byte 25: image composition */
#define SCSI_WIN_COMPOSITION_LINE_ART 0
#define SCSI_WIN_COMPOSITION_HALFTONE 1
#define SCSI_WIN_COMPOSITION_GRAY     2
#define SCSI_WIN_COMPOSITION_COLOR    5

/* SET WINDOW descriptor bytes 27-28: halftone type and pattern */
#define SCSI_WIN_HALFTONE_DEFAULT         0
#define SCSI_WIN_HALFTONE_DITHER          1
#define SCSI_WIN_HALFTONE_ERROR_DIFFUSION 2
#define SCSI_WIN_HALFTONE_DOWNLOADED      0x80

/* SET WINDOW descriptor byte 32: compression type (also win_side.compression) */
#define SCSI_WIN_COMPRESSION_NONE  0x00
#define SCSI_WIN_COMPRESSION_MH    0x01
#define SCSI_WIN_COMPRESSION_MR    0x02
#define SCSI_WIN_COMPRESSION_MMR   0x03
#define SCSI_WIN_COMPRESSION_JPEG1 0x81
#define SCSI_WIN_COMPRESSION_JPEG2 0x82

/* SET WINDOW descriptor byte 62: IPC mode */
#define SCSI_WIN_IPC_DTC  0x40
#define SCSI_WIN_IPC_SDTC 0x80

/* SET WINDOW descriptor byte 50: white level follow */
#define SCSI_WIN_WHITE_LEVEL_FOLLOW_ON  0x80
#define SCSI_WIN_WHITE_LEVEL_FOLLOW_OFF 0xC0

/* SET WINDOW descriptor byte 0: window identifier (side) */
#define SCSI_WIN_ID_BACK  0x80
#define SCSI_WIN_ID_FRONT 0x00

/* SET WINDOW descriptor: vendor unique ID */
#define SCSI_WIN_VENDOR_MONO  0x00
#define SCSI_WIN_VENDOR_COLOR 0xC1

/* INQUIRY with EVPD: the only vital product data page (CDB byte 2) */
#define SCSI_INQUIRY_VPD_PAGE 0xF0

#endif
