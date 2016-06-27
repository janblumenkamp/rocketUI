/*
 * Definitions that are specific to our communication with the rocket: what registers do we have?
 *
 * [START][ID]<TYPE><REG><LENGTH>[DATA][CHECKSUM]

[START]: ? Start sequence for new package
    <0X00><0XFF><0X00><0XFF>
[ID]: ? ID of the message. Incrementing after each message. There are different ID counter for GCS and Rocket. The ack package always answers with the same ID!
    <ID_MSB><ID><ID><ID_LSB>
<TYPE>: ? Type of package
<REG>: ? The reg represents the information what command, register, etc. is transmitted. If it is an data type, the lenght is important (represents following registers in the data area to reduce the overhead)
<LENGTH>:  ? The length of the data depends on the type byte. Maximum 255 Bytes!
    TYPE=   ACK:    0
            DATA:   AMOUNT OF REGISTERS INCLUDING REG, MAX 255
            CMD:    AMOUNT OF REGISTERS INCLUDING REG, MAX 255
            STATUS: 1
            ERR:    1
[DATA]:     ([LENGTH]*<DATA>)
[CHECKSUM]: <CHECK_MSB><CHECK_LSB>

TYPE:   ACK         0
        DATA        1
        CMD         2
        STATUS      3
        ERR         4
        HEARTBEAT   5
REG:
    ACK:            0
    DATA:
        ALTITUDE    0
        SPEED_VERT  1
        ACC_X       2
        ACC_Y       3
        ACC_Z       4
        GYR_X       5
        GYR_Y       6
        GYR_Z       7
        IMU_X       8
        IMU_Y       9
        IMU_Z       10
        TEMPERATURE 11
    CMD:
        SET STREAM  0       ? We want to be able to tell the rocket what data it is supposed to stream continously without us always asking. The data bytes represent the enable/disable flag for the corresponding registers.
        CALIBRATE   1
        START       2
        DEPLOY PARACHUTE 3

    STATUS:
        IDLE        0       ? Rocket is waiting on the ground for the start signal
        ACCEL_UP    1       ? Rocket is accelerating (motor burning)
        DECEL_UP    2       ? Rocket is decelerating and approaching epogeum
        EPO         3       ? Rocket reached highest point
        PARACHUTE   4       ? Parachute deployed
        ACCEL_DOWN  5       ? Rocket is accelerating to the ground
        LANDED      6       ? Rocket has landed
    ERROR:
        START       0       ? Start not possible
        CALIBRATE   1       ? Calibration not possible
        DEPLOY      2       ? Parachute deploy not possible
        MEMORY      3       ? Volatile memory on the rocket is full!

Example:
1) Start rocket:
    GCS -> Rocket:
    00 01 00 01 (Start seq) 00 00 ff 01 (id) 02 (type: cmd) 02 (reg: start) 01 (length: 1 byte) 01 (1 data byte) xx xx (checksum)
    Rocket -> GCS:
    00 01 00 01 (Start seq) 00 00 ff 01 (id) 00 (type: ack) 00 (reg: ack)   00 (length: 0 byte) (no data) xx xx (checksum)


The ack packages always answer with the ID of the package. There are different ID Counter for incoming
and outgoing packages.
The timeout for an acknowledge is 1s.
The rocket streams data in a period of 500ms.

The messages that the rocket wants to send are generally stored in a volatile (external) memory. If
the message could be transmitted and an ack was received, the message is cleared from the memory. If
the connection is lost (rocket out of reach; transmission timeout), it stays in the memory and is
being sent continously until an ack is received (the connection to the GCS is reestablished). If the
memory on the rocket is about to get full, a memory error is stored as last message. It's the same on
the GCS, even though the memory is unlikely to fill, we want to check this corner case. Incoming
messages on the rocket are processed as soon as the message memory is empty and are stored for
processing in a different section!
 */
#ifndef ROCKETCOMM_DEFS_H
#define ROCKETCOMM_DEFS_H

enum PACKAGE_TYPE_DATA_REG {
    PACKAGE_DATA_ALTITUDE,
    PACKAGE_DATA_SPEED_VERT,
    PACKAGE_DATA_ACC_X, PACKAGE_DATA_ACC_Y, PACKAGE_DATA_ACC_Z,
    PACKAGE_DATA_GYR_X, PACKAGE_DATA_GYR_Y, PACKAGE_DATA_GYR_Z,
    PACKAGE_DATA_MAG_X, PACKAGE_DATA_MAG_Y, PACKAGE_DATA_MAG_Z,
    PACKAGE_DATA_IMU_X, PACKAGE_DATA_IMU_Y, PACKAGE_DATA_IMU_Z,
    PACKAGE_DATA_TEMPERATURE,
    PACKAGE_DATA_GPS_LON, PACKAGE_DATA_GPS_LAT,
    //////
    PACKAGE_DATA_NUM
};

enum PACKAGE_TYPE_CMD_REG {
    PACKAGE_CMD_SET_STREAM,
    PACKAGE_CMD_CALIBRATE,
    PACKAGE_CMD_START,
    PACKAGE_CMD_DEPLOY_PARACHUTE,
    ////////
    PACKAGE_CMD_NUM
};

enum PACKAGE_TYPE_STATUS_REG {
    PACKAGE_STATUS_IDLE,
    PACKAGE_STATUS_ACCEL_UP,
    PACKAGE_STATUS_DECEL_UP,
    PACKAGE_STATUS_EPO,
    PACKAGE_STATUS_PARACHUTE,
    PACKAGE_STATUS_ACCEL_DOWN,
    PACKAGE_STATUS_LANDED,
    ///////
    PACKAGE_TYPE_NUM
};

enum PACKAGE_TYPE_ERROR_REG {
    PACKAGE_ERROR_START,
    PACKAGE_ERROR_CALIBRATE,
    PACKAGE_ERROR_DEPLOY,
    PACKAGE_ERROR_MEMORY,
    //////
    PACKAGE_ERROR_NUM
};

typedef enum PACKAGE_TYPE_DATA_REG package_type_data_t;
    typedef enum PACKAGE_TYPE_CMD_REG package_type_cmd_t;
    typedef enum PACKAGE_TYPE_STATUS_REG package_type_status_t;
    typedef enum PACKAGE_TYPE_ERROR_REG package_type_error_t;

#endif // ROCKETCOMM_DEFS_H
