/* 
 * OpenVirtualization: 
 * For additional details and support contact developer@sierraware.com.
 * Additional documentation can be found at www.openvirtualization.org
 * 
 * Copyright (C) 2011 SierraWare
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Header file for trustzone API
 */

#ifndef __OTZ_API_H_
#define __OTZ_API_H_
#include <sw_common_types.h>
#include <sw_list.h>

#define TYPE_UINT_DEFINED 1

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

#ifndef _STDINT_H
typedef char uint8_t;
#endif

#define MAX_MEMBLOCKS_PER_OPERATION 4

/**
* @brief Error codes
*
*/
enum otz_error_codes {
/*!The operation succeeded. */
    OTZ_SUCCESS = 0x0,             
/*!The asynchronous operation is still pending. */
    OTZ_PENDING ,
/*!Access has been denied, or the item cannot be found.*/
    OTZ_ERROR_ACCESS_DENIED ,
/*!The system is busy.*/
    OTZ_ERROR_BUSY ,
/*!The execution was cancelled.*/
    OTZ_ERROR_CANCEL , 
/*!There is a system communication error.*/
    OTZ_ERROR_COMMUNICATION ,
/*!The decoder ran out of data.*/
    OTZ_ERROR_DECODE_NO_DATA ,
/*!The decoder hit a type error.*/
    OTZ_ERROR_DECODE_TYPE ,
/*!The encoded data is of a bad format.*/
    OTZ_ERROR_ENCODE_FORMAT , 
/*!The encoder ran out of memory.*/
    OTZ_ERROR_ENCODE_MEMORY ,
/*!An unspecified error has occurred.*/
    OTZ_ERROR_GENERIC ,
/*!A bad parameter has been specified.*/
    OTZ_ERROR_ILLEGAL_ARGUMENT ,
/*!A state machine has been violated.*/
    OTZ_ERROR_ILLEGAL_STATE ,
/*!There is not enough memory to perform the operation.*/
    OTZ_ERROR_MEMORY ,
/*!The functionality is not implemented.*/
    OTZ_ERROR_NOT_IMPLEMENTED ,
/*!There is a security error.*/
    OTZ_ERROR_SECURITY ,
/*!The service has returned an error in the service return code.*/
    OTZ_ERROR_SERVICE ,
/*!The input buffer is not long enough.*/
    OTZ_ERROR_SHORT_BUFFER ,
/*!The implementation has reached an UNDEFINED condition.*/
    OTZ_ERROR_UNDEFINED 
};

/**
* @brief State machine constants
*
*/
enum otz_state_machine_state_const {
/*! Structures in the UNDEFINED state may have any value for their state 
* constant; it may not exist as an explicit value.Clients should never make
* use of this constant, although an implementation
* may use it internally for debugging purposes.
*/
    OTZ_STATE_UNDEFINED = 0x0,
/*! The state is in a safe invalid state. */
    OTZ_STATE_INVALID ,
/*! The state is open.*/
    OTZ_STATE_OPEN ,
/*! The state is closing, but not yet closed.*/
    OTZ_STATE_CLOSING ,
/*! The state an operation that is not running and 
* can accept data to be encoded.
*/
    OTZ_STATE_ENCODE ,
/*! The state of an operation that is not running, but which cannot accept 
* data to be encoded. This state applies only to close operations.
*/
    OTZ_STATE_PERFORMABLE ,
/*! The state of an operation that is executing synchronously.*/
    OTZ_STATE_RUNNING ,
/*! The state of an operation that is executing asynchronously.*/
    OTZ_STATE_RUNNING_ASYNC ,
/*! The state of an operation that can have data read using the 
* decoder functions.
*/
    OTZ_STATE_DECODE 
};

/**
* @brief Login flag constants
*
*
*/
enum otz_login_flags {
/*! No login is to be used.*/
    OTZ_LOGIN_PUBLIC = 0x0,
/*! A buffer of client data is to be provided.*/
    OTZ_LOGIN_CLIENT_DATA ,
/*! The user executing the application is provided.*/
    OTZ_LOGIN_USER ,
/*! The user group executing the application is provided.*/
    OTZ_LOGIN_GROUP ,
/*! The name of the application is provided; may include path.*/
    OTZ_LOGIN_NAME ,
/*! The digest of the client application is provided.*/
    OTZ_LOGIN_DIGEST , 
/*! A utility constant indicating all available login types should be used.*/
    OTZ_LOGIN_ALL
};

/**
* @brief Shared memory flag constants
*
*
*/
enum otz_shared_mem_flags {
/*! Service can only read from the memory block.*/
    OTZ_MEM_SERVICE_RO = 0x0,
/*! Service can only write from the memory block.*/
    OTZ_MEM_SERVICE_WO ,
/*! Service can read and write from the memory block.*/
    OTZ_MEM_SERVICE_RW,
/*! Invalid flag */
    OTZ_MEM_SERVICE_UNDEFINED
};

/**
* @brief Operation type constants
*
*/
enum otz_type_of_operation {
/*! Open operation */
    OTZ_OPERATION_OPEN = 0x0,
/*! Invoke operation */
    OTZ_OPERATION_INVOKE ,
/*! Close operation */
    OTZ_OPERATION_CLOSE ,
/*! No operation will be performed */
    OTZ_OPERATION_NONE
};

/**
* @brief Param type constants
*
*/
enum otz_param_type {
/*! In parameter for read operations. */
    OTZ_PARAM_IN = 0x0,
/*! Out parameter for write operations. */
    OTZ_PARAM_OUT
};

/**
* @brief Decode type constants
*
*/
enum otz_decode_type {
/*! There is no more data in the decode stream. */
    OTZ_TYPE_NONE = 0x0,
/*! The next data type in the stream is a uint32_t. */
    OTZ_TYPE_UINT32,
/*! The next data type in the stream is an array. */
    OTZ_TYPE_ARRAY
};

typedef uint32_t otz_return_t;
typedef uint32_t otz_state_t;

/* Trust zone client API */

/**
* @brief Universally Unique IDentifier (UUID) type as defined in 
* [RFC4122].A
*
* UUID is the mechanism by which a service is identified.
*/
typedef struct otz_uuid_t
{
    uint32_t ui_time_low;
    uint32_t ui_time_mid;
    uint16_t ui_time_hi_and_version;
    uint8_t  aui_clock_seq_and_node[8];
}otz_uuid_t;

/**
* @brief Login credentials to be provided to the service
*
*/
typedef struct otz_login_t
{
/*! Bit field specifying which login credentials must 
* be provided to the service. This must be one of the following options:\n
* OTZ_LOGIN_PUBLIC: no credentials are provided.\n
* OR\n
* One or more of the following flags:\n
* OTZ_LOGIN_CLIENT_DATA: supply the client buffer specified by pBuff and
*  uiBuffLen.\n
* OTZ_LOGIN_USER: supply the identity of the “user” executing the
* client.\n
* OTZ_LOGIN_GROUP: supply the identity of the “group” executing the
* client.\n
* OTZ_LOGIN_NAME: supply the “name” of the client executable. This may
* include path information to strengthen the differentiation between
* executables with the same name.\n
* OTZ_LOGIN_DIGEST: supply the cryptographic “digest” of the currently 
* running client process to the service. This enables the service to 
* compare the digest with a known good value to ensure that the client 
* has not been tampered with. \n
* OR \n
* OTZ_LOGIN_ALL: A utility constant which indicates that all of the available
* login flags have been specified.
*/
    uint32_t ui_type; 

/*! Buffer of login information sent to the service when the
* OTZ_LOGIN_CLIENT_DATA login flag is specified. The required content of this
* buffer is defined by the client-service protocol defined by the service that
* the client is attempting to connect to.
*/
    void*   p_buff; 

/*!
The length of the buffer in bytes. This field should be zero if
* p_buff is NULL.
*/
    uint32_t ui_buff_len;
}otz_login_t;

/**
* @brief 
*/
typedef struct {
	uint32_t objectType;
	uint32_t objectSize;
	uint32_t maxObjectSize;
	uint32_t objectUsage;
	uint32_t dataSize;
	uint32_t dataPosition;
	uint32_t handleFlags;
} TEE_ObjectInfo;

/**
 * @brief 
 */
typedef struct {
	uint32_t attributeID;
	union {
	struct {
	void* buffer; size_t length;
	} ref;
	struct
	{
	uint32_t a, b;
	} value;
	} content;
} TEE_Attribute;


/**
 *
 * @brief opaque structure definition for an object handle.
 *        TODO - Fill it with something appropriate
 */
struct __TEE_ObjectHandle {
	void* dataPtr;
	uint32_t dataLen;
	uint8_t dataName[255];
	TEE_ObjectInfo *ObjectInfo;
	TEE_Attribute *Attribute;
	uint32_t attributesLen;

};
typedef struct __TEE_ObjectHandle TEE_ObjectHandle;
/**
* @brief Name value pairs
*
*/
typedef struct otz_property_t
{
/*! The numeric namespace of properties. */
    uint32_t ui_namespace;
/*! The numeric name of the property. */
    uint32_t ui_name;
/*! A binary buffer of containing the property value. The content of this
* buffer is defined by the specification of the property that is loaded.
*/
    void*    p_value;
/*! The length of the binary buffer in bytes, or zero if pValue is NULL */
    uint32_t ui_length;
}otz_property_t;

/* Lists the functions exposed to the client by the system or service */
/**
* @brief Service property 
*
*/

typedef struct otz_property_name_t
{
/*! The numeric namespace of properties. */
    uint32_t ui_namespace;
/*! The numeric name of the property. */
    uint32_t ui_name;
}otz_property_name_t;

/**
* @brief Absolute time since an arbitary origin.
*
*
*/
typedef struct otz_timelimit_t
{
    /*Implementation Defined */
}otz_timelimit_t;

/**
 * @brief 
 */
typedef struct otz_operation_t otz_operation_t;
/**
 * @brief 
 */
typedef struct otz_session_t otz_session_t;
/**
 * @brief 
 */
typedef struct otz_device_t otz_device_t;
/**
 * @brief 
 */
typedef struct otz_shared_mem_t otz_shared_mem_t;

/**
* @brief Encoder and decoder state
*
*/
struct otz_enc_dec_t
{
    int cmd_id;             /*!< Command id */
    int encode_id;          /*!< Identifier for encode or decode operations */
    int enc_error_state;    /*!< Error value of last encoded operation */
    int dec_error_state;    /*!< Error value of last decoded operation */
};

/**
* @brief Shared memory reference 
*
*/
struct otz_mem_reference
{
/*! Shared memory context */
    void            *shared_mem;
/*! Offset from the allocated Shared memory for reference */
    uint32_t        offset;
/*! Shared memory reference length */
    uint32_t        length;
/*! In or out parameter */
    int             param_type;
};

/**
* @brief The otz_operation_t structure is used to contain control information
* related to an operation that is to be invoked with the security environment.
*
*/
struct otz_operation_t
{
/*! State of the operation */
    otz_state_t ui_state;
/*! Session context*/
    otz_session_t* session;
/*! Type of operation*/
    uint32_t type;
/*! Session id for the operation*/
    int session_id;
/*! Encoder and decoder state*/    
    struct otz_enc_dec_t enc_dec;
/*! Shared memory referred during this operation*/    
    struct otz_mem_reference shared_mem[MAX_MEMBLOCKS_PER_OPERATION];
/*! Shared memory referred count*/
    int shared_mem_ref_count;
/*! Temporary shared memory referred during this operation*/
    struct otz_mem_reference temp_mem[MAX_MEMBLOCKS_PER_OPERATION];
/*! Temporary shared memory referred count*/
    int temp_mem_ref_count;
/*! Error number from the client driver*/
    int s_errno;
/*! Implementation defined structure */
    struct {
    /* Implementation Defined */
    }s_imp;

};

/**
* @brief The otz_session_t structure is used to contain control information
* related to a session between a client and a service.
*
*/
struct otz_session_t
{
/*! State of the session */
    otz_state_t ui_state;
/*! Reference count of operations*/
    int operation_count;
/*! Session id obtained for the  service*/
    int session_id;
/*! Unique service id */
    int service_id;
/*! Shared memory counter which got created for this session */
    uint32_t shared_mem_cnt;    
/*! Device context */
    otz_device_t* device;
/*! Shared memory list */
    struct list shared_mem_list;
/*! Implementation defined structure */
    struct 
    {
    /* Implementation Defined */
    }s_imp;

};

/**
* @brief The otz_device_t structure is used to contain control information
* related to the device
*
*/
struct otz_device_t
{ 
/*! State of the device */
    otz_state_t ui_state;
/*! Device identifier */
    uint32_t fd;
/*! Sessions count of the device*/
    int session_count;
/*! Error number from the client driver*/
    int s_errno;
/*! Implementation defined */
    struct {
    /* Implementation Defined*/
    }s_imp;
};

/**
* @brief The otz_shared_memory_t structure is used to contain control information
* related to a block of shared memory that is mapped between the client and the
* service.
*
*/

struct otz_shared_mem_t
{
/*! he state of this structure. For shared memory only the following
* states are used: \n
*   OTZ_STATE_INVALID: Shared memory block is not valid, but in a known state 
*   which can be freed. \n
*   OTZ_STATE_OPEN: Shared memory block is valid and references to it can be
* encoded in structured messages.\n
*   OTZ_STATE_UNDEFINED: Pseudo state covering all other behavior. A structure in
* this state must not be used unless explicitly specified, otherwise UNDEFINED
* behavior may occur 
*/
    otz_state_t ui_state;
/*! The length of the shared memory block in bytes. Should not be zero */
    uint32_t ui_length;
/*! The sharing flags of the shared memory block, indicating direction of
* data sharing. \n
* Note that these access flags cannot usually be enforced by the hardware. If a
* client or a service ignores the flags specified for a shared memory block, or
* a corresponding memory reference, UNDEFINED behavior results. \n
*  Exactly one of the following flags must be specified:\n
*  OTZ_MEM_SERVICE_RO: The service can only read from the memory block.\n
*  OTZ_MEM_SERVICE_WO: The service can only write to the memory block.\n
*  OTZ_MEM_SERVICE_RW: The service can both read from and write to the memory
*  block.
*/
    uint32_t ui_flags;
/*! The pointer to the block of shared memory. */
    void*   p_block;
/*! Session context */
    otz_session_t* session;
/*! Session identifier */
    int session_id;
/*! Operation count */
    int operation_count;
/*! Service error number */
    int s_errno;
/*! List head used by Session */
    struct list head_ref;
/*! Implementation defined structure */
    struct {
    /* Implementation defined */
    }s_imp;

};



/**
* @brief Open the device
*
* This function opens a connection with the device in the underlying operating
* environment that represents the secure environment. When the client no longer
* requires the device it must call otz_device_close to close the connection and
* free any associated resources. This function accepts a pointer to a
* otz_device_t structure assumed to be in the OTZ_STATE_UNDEFINED state. On
* success this function must set the device structure *ps_device to the state
* OTZ_STATE_OPEN with a session count of zero. On failure, the device is set to
* the state OTZ_STATE_INVALID. It is possible to create multiple concurrent
* device connections from a single client. The number of devices that can be
* supported globally within the entire system, or locally within a single
* client, is implementation-defined.
* 
* Undefined Behavior:
*  The following situations result in UNDEFINED behavior: \n
*  Calling with device set to NULL. \n
*
* @param pk_device_name: An implementation-defined binary buffer, used to
* identify the underlying device to connect to. If this is NULL, the
* implementation will use an internally defined default device name.
* 
* @param pk_init: An implementation-defined binary block used to configure the
* implementation. If this is NULL, the implementation will use predefined
* default values for the library configuration
*
* @param ps_device: A pointer to the device structure.
*
* @return otz_return_t:
* OTZ_SUCCESS: The device was successfully opened. \n
* OTZ_ERROR_*: An implementation-defined error code for any other error.
*
*
*/
otz_return_t otz_device_open(void const* pk_device_name, void const* pk_init,
                            otz_device_t* ps_device);

/**
* @brief Get local time limit
*
* Calling this function generates a device-local absolute time limit in the
* structure pointed to by ps_time_limit  from a timeout value ui_timeout. 
* The absolute time limit is equal to the  current time plus 
* the specified timeout.
* 
* Undefined Behavior:
* The following situations result in UNDEFINED behavior: \n
*      Calling with device set to NULL.\n
*      Calling with device pointing to a device in the state OTZ_STATE_INVALID.\n
*      Calling with time_limit set to NULL. \n
*      Use of the time limit outside of the device in which it was created.\n
* @param ps_device: A pointer to the device
* @param ui_timeout: The required relative timeout, in milliseconds.
* @param ps_timelimit: A pointer to the time limit structure to populate.
*
* @return 
* OTZ_SUCCESS: the time limit was created successfully.\n
* OTZ_ERROR_*: an implementation-defined error code for any other error.
*
*/
otz_return_t otz_device_get_timelimit(otz_device_t* ps_device,
                                    uint32_t ui_timeout,
                                    otz_timelimit_t* ps_timelimit);

/**
* @brief Prepare open operation
*
* This function is responsible for locally preparing an operation that can be
* used to connect with the service  defined by the UUID 
* pointed to by pks_service, using the timeout pointed to by
* pks_time_limit and the login credentials specified in pks_login.\n
*
* This function accepts a session and an operation structure assumed to be in
* the state OTZ_STATE_UNDEFINED.\n
* 
* When this function returns OTZ_SUCCESS it must increment the session count of
* the device. The count may subsequently need to be decremented by the 
* otz_operation_release function –  releasing this operation if the
* corresponding perform operation failed or was never executed.\n
*
* When this function returns OTZ_SUCCESS the operation is set to the state
* OTZ_STATE_ENCODE; this state allows the client to encode a 
* message to be exchanged with the service using the encoder 
* functions of the OTZ_API. Once the message has been encoded 
* the client must call the function otz_operation_perform, or the
* asynchronous equivalent, to issue the open session command to the security
* environment.\n
*
* When this function returns OTZ_SUCCESS the session must be in the state
* OTZ_STATE_INVALID. The state transitions to OTZ_STATE_OPEN after the perform 
* function related to the open  session operation has returned
* * OTZ_SUCCESS; only at this point does the session become usable. If the perform
* function returns any error code,
* the session is not opened and remains in the state OTZ_STATE_INVALID.\n
*
* When this function fails it can return any error code. In these conditions,
* the state of the device must be  unchanged, 
* including the session count. The state of the session is OTZ_STATE_UNDEFINED 
* and the operation  must be set to OTZ_STATE_INVALID.\n
*
* Note that if the perform function fails, the client must still call
* otz_operation_release to release resources
* associated with the operation. \n
*
* Undefined Behavior:\n
* The following situations result in UNDEFINED behavior:\n
*     Calling with device set to NULL.\n
*     Calling with device pointing to a device in the state OTZ_STATE_INVALID.\n
*     Calling with service set to NULL.\n
*     Calling with session set to NULL.\n
*     Calling with operation set to NULL.\n
*
* @param ps_device: A pointer to the device.
* @param pks_service: A pointer to the service UUID.
* @param pks_login: A pointer to the login control structure, or NULL.
* @param pks_timelimit: A pointer to the time limit, or NULL.
* @param ps_session: A pointer to the session.
* @param ps_operation: A pointer to the operation.
*
* @return 
* OTZ_SUCCESS: The operation has been prepared successfully.\n
* OTZ_ERROR_*: An implementation-defined error code for any other error.\n
*
*
*/
otz_return_t otz_operation_prepare_open(otz_device_t* ps_device,
/*                                      otz_uuid_t const* pks_service, */
                                      int pks_service,
                                      otz_login_t const* pks_login,
                                      otz_timelimit_t const* pks_timelimit,
                                      otz_session_t* ps_session,
                                      otz_operation_t* ps_operation );

/**
* @brief Prepare operation for service request
*
* This function is responsible for locally preparing an operation that can be
* used to issue a command to a service  with which the client has 
* already created a session.
*
* This function accepts an operation assumed to be in the state
* OTZ_STATE_UNDEFINED.
*
* When this function returns OTZ_SUCCESS the operation is set to the state
* OTZ_STATE_ENCODE; this state allows
* the client to encode a message to be exchanged with the service using the
* encoder functions of the OTZ_API.  Once the message has been encoded the client 
* must call the function  otz_operation_perform, or the asynchronous equivalent, 
* to issue the command to the service.
*
* When this function fails it can return any error code. In these conditions the
* state of the session must be unchanged, 
* including the operation count. The state the operation must be set
* to the state OTZ_STATE_INVALID.
*
* The pks_time_limit parameter defines the absolute time by which the operation
* must be complete, after which the implementation should attempt to cancel it. 
* This parameter may be NULL which implies no timeout it used.
*
* Note that if the perform function fails the client must still call
* otz_operation_release to release resources associated with the operation.\n
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*    Calling with session set to NULL.\n
*    Calling with session pointing to a session in a state other than 
*    OTZ_STATE_OPEN.\n
*    Calling with operation set to NULL.\n
*
* @param ps_session: A pointer to the open session.
* @param ui_command: The identifier of the command to execute, defined by the
* client-service protocol.
* @param pks_timelimit: A pointer to the time limit, or NULL.
* @param ps_operation: A pointer to the operation.
*
* @return
* OTZ_SUCCESS: The operation has been prepared successfully.\n
* OTZ_ERROR_*: An implementation-defined error code for any other error.\n
*
*/
otz_return_t otz_operation_prepare_invoke(otz_session_t* ps_session,
                                        uint32_t ui_command,
                                        otz_timelimit_t const* pks_timelimit,
                                        otz_operation_t* ps_operation);

/**
* @brief Performs the previously prepared operation
*
* This function performs a previously prepared operation – issuing it to the
* secure environment.\n
* There are three kinds of operations that can be issued: opening a client
* session, invoking a service command,
* and closing a client session. Each type of operation is prepared with its
* respective function, which returns the
* operation structure to be used:\n
*    otz_operation_prepare_open prepares an open session operation.\n
*    otz_operation_prepare_invoke prepares an invoke service command operation.\n
*    otz_operation_prepare_close prepares a close session operation.\n
* 
* When calling this function, the operation must be in the state OTZ_STATE_ENCODE
* or OTZ_STATE_PERFORMABLE.\n
*
* For operations that support a structured message it is not required that a
* message has actually been encoded  by the client. 
* Once this function has been called, the state transitions to
* OTZ_STATE_RUNNING and it is no longer possible to use to the 
* encoder functions on the operation.\n
*
* If an error is detected by the system before the operation reaches the
* service, then an error code is returned by otz_operation_perform and 
* the value OTZ_ERROR_GENERIC is assigned to *pui_service_return. 
* In this case the operation will be in the state OTZ_STATE_INVALID 
* when the function returns and the decoder functions cannot be
* used on the operation.\n
*
* The most common causes for an error occurring before the command reaches the
* service are:\n
*  The encoder ran out of space – error returned is OTZ_ERROR_ENCODE_MEMORY.\n
*  The service does not exist – error returned is OTZ_ERROR_ACCESS_DENIED.\n
*  The system rejects a new session due to bad login credentials – error
*  return is OTZ_ERROR_ACCESS_DENIED.\n
*  The operation has timed out, or been cancelled; error return is
*  OTZ_ERROR_CANCEL.\n
*  The secure environment is busy or low on resource and cannot handle 
*  the request.\n
*
*  For open and invoke operations, if the operation reaches the service, but the
*  service returns an error, then otz_operation_perform 
*  returns OTZ_ERROR_SERVICE. The error code from the service
*  is assigned to  *pui_service_return. \n
*
*  Unlike the case where a system error occurs, the service
*  can return a message: the operation transitions to the 
*  state OTZ_STATE_DECODE and the decoder functions can be used.\n
*
*  For open and invoke operations, if the operation succeeds then
*  otz_operation_perform returns OTZ_SUCCESS and
*  the value OTZ_SUCCESS is also assigned to *pui_service_return. The operation
*  transitions to the state OTZ_STATE_DECODE and the client can use the decoder 
*  functions to retrieve the message, if present, from the service.\n
*  
*  For close operations the service cannot return a structured message, and the
*  operation will always transition to OTZ_STATE_INVALID. 
*  The decoder functions cannot be used on a close operation.
*  A close operation cannot be performed while the session has other operations 
*  open, or has allocated shared memory blocks – this results in
*  UNDEFINED behavior.\n
*  
*  Regardless of the success or failure of the function the client code must
*  always call otz_operation_release to
*  release any resources used by the operation.\n
*  Undefined Behavior:\n
*  The following situations result in UNDEFINED behavior:\n
*   Calling with operation set to NULL.\n
*   Calling with operation pointing to an operation not in the state 
*       OTZ_STATE_ENCODE.\n
*   Calling with operation pointing to a close operation, where the session 
*       being closed still has other operations open, or has allocated 
*       shared memory blocks.\n
*       Calling with service_return set to NULL.
*
* @param ps_operation: A pointer to the operation.
* @param pui_service_return: A pointer to the variable that will contain the
* service return code.
*
* @return 
* OTZ_SUCCESS: The operation was executed successfully.\n
* OTZ_ERROR_ENCODE_MEMORY: The encoder is in an error condition – it ran out of
* memory space.\n
* OTZ_ERROR_ACCESS_DENIED: The service was not found or the client was not
* authorized to access it.\n
* OTZ_ERROR_SERVICE: The service itself threw an error, which can be found in
* *pui_service_return.\n
* OTZ_ERROR_CANCEL: The operation timed out, or was explicitly cancelled.\n
* OTZ_ERROR_*: An implementation-defined error code for any other error.
*
*/
otz_return_t otz_operation_perform(otz_operation_t* ps_operation,
                                 otz_return_t* pui_service_return);

/**
* @brief Release operation
*
* This function releases an operation, freeing any associated resources.
* The behavior of this function varies slightly depending on the state of the
* operation:\n
*     OTZ_STATE_ENCODE or OTZ_STATE_PERFORMABLE: The operation has not been issued
* to the system, and is destroyed without being issued. 
* In this case it may be required to unwind some of the state change made to 
* related structures, for example if the operation is a session closure 
* the session state must transition back to OTZ_STATE_OPEN.\n     
*     OTZ_STATE_DECODE: The operation has been issued to the system and a
* response has been returned.
* After destroying an operation in this state, any messages in its 
* decoder are lost, including unread entries and arrays that have been 
* decoded by reference. If the client needs to keep any data from the message
* it must copy it out of the decoder owned memory before calling this
* function.\n
*   OTZ_STATE_INVALID: This function does nothing.
* 
* After this function returns the operation must be considered to be
* in the state OTZ_STATE_UNDEFINED.\n
*
* Undefined Behavior:\n
* The following situations result in UNDEFINED behavior:\n
*   Calling with operation set to NULL.\n
*   Calling with operation pointing to an operation in a state other than 
*       OTZ_STATE_ENCODE, OTZ_STATE_PERFORMABLE, OTZ_STATE_DECODE or 
*       OTZ_STATE_INVALID.
*
* @param ps_operation: A pointer to the operation to release.
*
*
*/
void otz_operation_release(otz_operation_t* ps_operation);

/**
* @brief Prepare the operation for close session 
*
* This function is responsible for locally preparing an operation that can be
* used to close a session between the client and a service.
*
* This function accepts an operation assumed to be in the state
* OTZ_STATE_UNDEFINED.
*
* When this function returns OTZ_SUCCESS the state of the session is changed to
* OTZ_STATE_CLOSING. In this state any operation still running or shared memory 
* block still allocated can stillbe used, but it is not possible to create
* new shared memory blocks or prepare new operations within the session. If this
* operation is never issued, otz_operation_release must transition 
* the session state back to OTZ_STATE_OPEN.
*
* Note that performing a close operation while other operations exist with a
* session, or while shared memory blocks are still allocated within it, 
* results in UNDEFINED behavior.
*
* When this function returns OTZ_SUCCESS the operation is set to the state
* OTZ_STATE_PERFORMABLE; this state allows the client to perform the close 
* operation, but not to encode a message to be exchanged with the service.
* The client must call the function otz_operation_perform, or the asynchronous
* equivalent, to issue the operation to the security environment.
*
* When this function fails it can return any error code. In these conditions the
* state of the session must be unchanged, including the operation count. 
* The state the operation must be set to OTZ_STATE_INVALID.
*
* Note that the perform operation for a session closure cannot be canceled or
* timed-out by the client. When otz_operation_perform completes, 
* whether with success or failure, the session is considered closed.
* 
* On failure of the perform function the client must still call
* otz_operation_release to release resources associated
* with the operation.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*    Calling with session set to NULL.\n
*    Calling with session pointing to a session in a state other than 
*       OTZ_STATE_OPEN.\n
*    Calling with operation set to NULL.
*
* @param ps_session: A pointer to session.
* @param ps_operation: A pointer to the operation.
*
* @return 
* OTZ_SUCCESS: The operation has been prepared successfully.\n
* OTZ_ERROR_*: An implementation-defined error code for any other error.\n

*/
otz_return_t otz_operation_prepare_close(otz_session_t* ps_session,
                                       otz_operation_t* ps_operation);

/**
* @brief Close the device connection
* 
* This function closes a connection with a device, freeing any associated
* resources.
* If the passed device is in the state OTZ_STATE_INVALID this function must set
* to the state to OTZ_STATE_UNDEFINED and return OTZ_SUCCESS.
*
* If the passed device is in the state OTZ_STATE_OPEN with a session count of
* zero this function must delete the device. This operation cannot fail; 
* the function must always set the state to OTZ_STATE_UNDEFINED and return
* OTZ_SUCCESS.
*
* If the passed device is in the state OTZ_STATE_OPEN with a non-zero session
* count, this function must return OTZ_ERROR_ILLEGAL_STATE 
* and leave the device unmodified.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*    Calling with device set to NULL.\n
*
* @param ps_device: A pointer to the device to delete.
*
* @return 
* OTZ_SUCCESS: The device is successfully closed.\n
* OTZ_ERROR_ILLEGAL_STATE: The device is in the state OTZ_STATE_OPEN and has a
* non-zero session count.\n
* OTZ_ERROR_*: An implementation-defined error code for any other error.\n*
*/
otz_return_t otz_device_close(otz_device_t* ps_device);


/**
* @brief This function allocates a block of memory, defined by the structure
* pointed to by ps_shared_mem, which is shared 
* between the client and the service it is connected to.
*
* This function allocates a block of memory, defined by the structure pointed to
* by ps_shared_mem, which is shared
* between the client and the service it is connected to.
*
* Once a block is allocated, the client and the service can exchange references
* to the allocated block in messages encoded using structured messages. 
* Depending on the implementation of the secure environment, this may
* allow the service to directly access this block of memory without the need for
* a copy; this allows for high-bandwidth non-structured communications.
*
* On entry to this function the session must be in the state OTZ_STATE_OPEN.
*
* On entry to this function the fields ui_flags and ui_length of the 
* shared memory structure must have been  filled in with the required values. 
* Other fields of the shared memory structure have UNDEFINED state on entry to
* this function and are filled in by the time the function returns.
*
* If this function returns OTZ_SUCCESS, the value ps_shared_mem->p_block will
* contain the address of the shared
* memory allocation and the shared memory structure will be in the state
* OTZ_STATE_OPEN. The implementation  must guarantee that the 
* returned buffer allocation is aligned on an 8-byte address boundary.
*
* If this function returns any other value, the state of the structure is
* OTZ_STATE_INVALID and ps_shared_mem->p_block will be NULL.
*
* After successful allocation of a block the client may subsequently pass the
* shared memory structure to the
* function otz_encode_memory_reference to create a reference to a portion of the
* block.
*
* Blocks are flagged with the intended direction of data flow, as described by
* the ps_shared_mem->ui_flags parameter. If an attempt is later made to encode 
* a memory reference with incompatible sharing attributes an
* encoder error is thrown when the operation is performed.
*
* The structure must be passed to the function otz_shared_memory_release when the
* block is no longer needed
*
* Undefined Behavior :\n
* The following situations result in UNDEFINED behavior:\n
*       Calling with session set to NULL.\n
*       Calling with session in a state other than OTZ_STATE_OPEN.\n
*       Calling with shared_mem set to NULL.\n
*       Calling with shared_mem->flags set to an invalid set of flags.\n
*       Calling with shared_mem->length set to 0.\n
*
* @param ps_session: A pointer to the session.
* @param ps_shared_mem: A pointer to the shared memory block structure.
*
* @return 
* OTZ_SUCCESS: the memory was allocated successfully.\n
* OTZ_ERROR_MEMORY: there is not enough memory to meet the allocation request.\n
* OTZ_ERROR_*: an implementation-defined error code for any other error.\n
*/
otz_return_t otz_shared_memory_allocate(otz_session_t* ps_session ,
                                        otz_shared_mem_t* ps_shared_mem);

/**
* @brief This function allocates a block of memory, defined by the structure
* pointed to by ps_shared_mem, which is shared
* between the client and the service it is connected to.
* This function marks a block of shared memory associated with a session as 
* no longer shared.
*
* If the input shared memory structure is in the state OTZ_STATE_INVALID 
* this function does nothing, otherwise it frees the memory block. 
* The caller must not access the memory buffer after calling this function.
*
* The shared memory structure returned by this function will be in the state 
* OTZ_STATE_UNDEFINED.
*
* When this function is called, the shared memory block must not be referenced 
* by any operation, otherwise UNDEFINED behavior will occur.
*
* Note that shared memory blocks must always be freed by calling this function; 
* all memory blocks must be freed before a session can closed.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*   Calling with shared_mem set to NULL.\n
*   Calling with shared_mem pointing to a shared memory block that is still 
*       referenced by an operation.
*  
* @param ps_shared_mem - A pointer to the shared memory block to free.
*/
void otz_shared_memory_release(otz_shared_mem_t* ps_shared_mem);

/**
* @brief Encode unsigned 32-bit integer
*
* Calling this function appends the value of the passed uint32_t, pk_data, 
* to the end of the encoded message.
*
* This function can only be called when the operation is in the 
* state OTZ_STATE_ENCODE. This occurs after the operation has been prepared, 
* but before it has been performed.
*
* If an error occurs, for example if there is no more space in the 
* encoder buffer, this function sets the error state of the encoder.
*
* This function does nothing if the error state of the encoder is 
* already set upon entry.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*   Calling with operation set to NULL.\n
*   Calling with operation pointing to an operation in a state other 
*       than OTZ_STATE_ENCODE.
*
* @param ps_operation - A pointer to the operation for which we are encoding.
* @param data - The value to encode in the buffer.
* @param param_type - In or out 
*/
void otz_encode_uint32( otz_operation_t* ps_operation,  
                        void const* data, 
                        int param_type);

/**
* @brief Encode binary array to the encoded message
*
* Calling this function appends a binary array pointed to by array and of length
* length bytes to the end ofthe encoded message. The implementation must 
* guarantee that when decoding the array in the service the base pointer 
* is eight byte aligned to enable any basic C data structure to be 
* exchanged using this method.
*
* It is valid behavior to encode a zero length array, where array is not NULL 
* but uiLength is 0, and a NULL array, where array is NULL and length is zero, 
* using this function.
*
* This function can only be called when the operation is in the state 
* OTZ_STATE_ENCODE. This occurs after the operation has been prepared, 
* but before it has been performed.
*
* If an error occurs, for example if there is no more space in the encoder 
* buffer, this function sets the error state of the encoder.
*
* This function does nothing if the error state of the encoder is already 
* set upon entry.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*   Calling with operation set to NULL.\n
*   Calling with operation pointing to an operation in a state other than 
*       OTZ_STATE_ENCODE.\n
*   Calling with array set to NULL when length is a length other than 0.
*
* Output\n
* This function may set the state of the encoder to OTZ_ERROR_ENCODE_MEMORY 
* if there is insufficient space to encode the array.
*
* @param ps_operation - A pointer to the operation for which we are encoding.
* @param pk_array - A pointer to the binary buffer to encode.
* @param length - The length of the binary buffer in bytes.
* @param param_type - In or out data 
*/
void otz_encode_array( otz_operation_t* ps_operation,  
                        void const* pk_array, 
                        uint32_t length, 
                        int param_type);

/**
* @brief Encode empty binary array to the encoded message
*
* Calling this function appends an empty binary array of length "length" bytes 
* to the end of the encoded message and returns the pointer to this array 
* to the client. This allows an implementation with fewer copies, as
* the encoder buffer can be filled directly by the client without needing a 
* copy from an intermediate buffer into the real encoder buffer.
*
* The implementation must guarantee that the returned buffer allocation is 
* aligned on an eight byte boundary, enabling direct sharing of any C data type 
* in accordance with the ARM Application Binary Interface [ARM IHI
* 0036A].
*
* It is valid behavior to allocate space for a zero length array in the 
* encoder stream. This will return a pointer that is not NULL, 
* but this pointer must never be dereferenced by the client code or UNDEFINED 
* behavior may result.
*
* This function can only be called when the operation is in the state 
* OTZ_STATE_ENCODE. This occurs after the operation has been prepared, 
* but before it has been performed. Once the operation transitions out of the 
* state OTZ_STATE_ENCODE, which occurs if the operation is performed or 
* is released, then the client must no longer access this buffer or 
* UNDEFINED behavior may result.
*
* If an error occurs, for example if there is no more space in the 
* encoder buffer, this function sets the error state of the encoder and 
* returns NULL.
*
* This function does nothing if the error state of the encoder is already 
* set upon entry, and will return NULL.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*   Calling with operation set to NULL.\n
*   Calling with operation pointing to an operation in a state other 
*       than OTZ_STATE_ENCODE.
*
* Output\n
* This function may set the state of the encoder to OTZ_ERROR_ENCODE_MEMORY 
* if there is insufficient space to encode the array space.
*
* @param ps_operation - A pointer to the operation for which we are encoding.
* @param length - The length of the desired binary buffer in bytes.
* @param param_type - In or Out data
*
* @return  - A pointer to the buffer, or NULL upon error.
*/
void* otz_encode_array_space( otz_operation_t* ps_operation,  
                        uint32_t length,
                        int param_type);



/**
* @brief Appends a reference of previously allocated shared block to the encoded
* buffer
*
* Calling this function appends a reference to a range of a previously created 
* shared memory block.
*
* Memory references are used to provide a synchronization token protocol which 
* informs the service when it can read from or write to a portion of the shared 
* memory block. A memory reference is associated with a specific operation and 
* is valid only during the execution of that operation.
*
* When the otz_encode_memory_reference function completes successfully the 
* shared memory block is said to be “referenced”. This reference is destroyed 
* when the operation perform function completes (with or without an error). 
* If the operation is never performed for any reason, the reference is destroyed
* when the operation is released. A shared memory block cannot be released 
* while is it still referenced in an operation. Once a memory reference 
* has been created the client must not read from, or write to, 
* the referenced range until the reference is destroyed.
*
* Some implementations of the secure environment may not be able to implement 
* genuine shared memory and/or may make use of device hardware 
* outside of the core. In these cases the system may require data copies or
* cache maintenance operations to ensure visibility of the data in a 
* coherent manner. For this reason the memory reference is marked with a 
* number of flags which can be used to ensure the correct copies and cache
* maintenance operations occur. Primarily these indicate the memory operations 
* that the service is allowed to perform. 
* Exactly one of the following flags must be specified:
*
* OTZ_MEM_SERVICE_RO: The service can only read from the memory block.\n
* OTZ_MEM_SERVICE_WO: The service can only write to the memory block.\n
* OTZ_MEM_SERVICE_RW: The service can both read from and write to the 
* memory block.
*
* These flags must be a sub-set of the service permissions specified 
* when the block was created using otz_shared_memory_allocate, 
* otherwise the encoder error OTZ_ERROR_ENCODE_FORMAT will be raised.
*
* If an error occurs, for example if there no more space in the encoder buffer 
* or the range lies outside of the memory block, this function sets 
* the error state of the encoder. Additionally, there is a restriction 
* whereby the client cannot have multiple concurrent references to any address 
* in the memory block. Attempting to write a memory reference that overlaps an 
* existing one will result in the encoder entering an error state. 
* In any of these cases the shared memory block is not referenced by 
* calling this function.
*
* This function does nothing if the error state of the encoder is already 
* set upon entry.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*   Calling with psOperation set to NULL.\n
*   Calling with psOperation pointing to an operation in a state other than 
*       OTZ_STATE_ENCODE.\n
*   Calling with ps_shared_mem set to NULL.
*   Calling with ps_shared_mem pointing to a memory block in a state other than 
*       OTZ_STATE_OPEN.
*
* @param ps_operation - A pointer to the operation for which we are encoding.
* @param ps_shared_mem - A pointer to the shared memory block structure.
* @param offset - The offset, in bytes, from the start of the 
*                 shared memory block to the start of the memory range.
* @param length - The length, in bytes, of the memory range.
* @param flags - The access flags for this memory reference.
* @param param_type - In or Out data
*/
void otz_encode_memory_reference( otz_operation_t* ps_operation,  
                        otz_shared_mem_t* ps_shared_mem,
                        uint32_t offset,
                        uint32_t length,
                        uint32_t flags,
                        int param_type);


/**
* @brief Decode a unsigned 32-bit integer value from message
*
* This function decodes a single item of type uint32_t from the current offset 
* in the structured message returned by the service.
*
* If on entry the decoder is in an error state this function does nothing 
* and returns 0. The state of the decoder remains unchanged.
*
* If the decoder error state is not set on entry, the system attempts to decode 
* the data item at the current offset in the decoder and return the result. 
*
* If an error occurs this function returns 0 and sets the error state of the
* decoder. Otherwise the data value is returned by the function and the decoder 
* offset is incremented past the item that has been decoded.
*
* The decoder may set its error state in the following situations:\n
*   There are no more items in the decoder.\n
*   The item in the decoder is not of the type requested.\n
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*   Calling with "operation" set to NULL.\n
*   Calling with "operation" pointing to an operation not in the 
*       state OTZ_STATE_DECODE.
*
* Output\n
* This function will set the state of the decoder to OTZ_ERROR_DECODE_NO_DATA 
* if there is no further data to decode.
*
* This function may optionally check the type of the data returned and may set 
* the state of the decoder to OTZ_ERROR_DECODE_TYPE if there is a type mismatch. 
* The presence of type checking in the library is implementation-defined; 
* to ensure safety, clients must check the value of returned data to ensure 
* that it meets any critical criteria.
*
* @param ps_operation - The operation from which we decoding the integer.
*
* @return - The value of the data item on success, 0 on any error.
*/
uint32_t otz_decode_uint32( otz_operation_t* ps_operation);


/**
* @brief  Decode a block of binary data from the message
*
* This function decodes a block of binary data from the current offset 
* in the structured message returned by the service. 
* The length of the block is returned in *pui_length and the base pointer is 
* the function return value.
*
* The implementation must guarantee that the returned buffer allocation is 
* aligned on an eight byte boundary, enabling direct sharing of any C data type 
* in accordance with the ARM Application Binary Interface [ARM IHI
* 0036A].
*
* It is expected that this function will return a pointer to the binary data 
* in the encoder buffer. The client must make use of this memory before 
* the operation is released. After the operation is released the client 
* must not access the buffer again. If the data is needed locally after the 
* operation has been released it must first be copied into a
* client allocated memory block.
*
* If on entry the decoder is in an error state this function does nothing 
* and returns NULL, with pui_length set to 0. 
* The state of the decoder remains unchanged.
*
* If the decoder error state is not set on entry, the system attempts to 
* decode the data item at the current offset in the decoder and 
* return the result. If an error occurs this function returns NULL as the 
* base pointer, sets the length to 0, and sets the error state of the decoder. 
* Otherwise the data value is returned by the function, and the
* decoder offset is incremented past the array item in the decoder.
*
* Note that this function may decode a NULL array and a zero length array 
* as valid types. In the former case the return value is NULL 
* and the length is zero. In the second case the return value is non-NULL 
* and the length is zero – the pointer must not be dereferenced by the client.
*
* The decoder may return errors in the following situations:\n
*       • There are no more items in the decoder.\n
*       • The item in the decoder is not of the type requested.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*       Calling with psOperation set to NULL.\n
*       Calling with psOperation pointing to an operation not in the state 
*           OTZ_STATE_DECODE. \n
*       Calling with puiLength set to NULL.\n
*
* Output:\n
* This function will set the state of the decoder to OTZ_ERROR_DECODE_NO_DATA 
* if there is no further data to decode.
*
* This function may optionally check the type of the data returned, 
* and may set the state of the decoder to OTZ_ERROR_DECODE_TYPE 
* if there is a type mismatch. The presence of type checking in the library is
* implementation-defined; to ensure safety, clients must check the value of 
* returned data to ensure that it meets any critical criteria.
*
* @param ps_operation - The operation from which we are decoding the array.
* @param plength - The pointer to the variable that will contain the 
* array length on exit.
*
* @return  - The value of the data item on success, 0 on any error.
*/
void* otz_decode_array_space(otz_operation_t* ps_operation, uint32_t *plength);



/**
* @brief Returns the decoder stream data type
*
* This function returns the type of the data at the current offset 
* in the decoder stream.
*
* This function does not affect the error state of the decoder.
* 
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*       Calling with psOperation set to NULL.\n
*       Calling with psOperation pointing to an operation not in the state 
*           OTZ_STATE_DECODE.\n
*
* @param ps_operation - The operation from which we are retrieving the result.
*
* @return -  OTZ_TYPE_NONE: There is no more data.\n
*       OTZ_TYPE_UINT32: The next item in the decode stream is a uint32.\n
*       OTZ_TYPE_ARRAY: The next item in the decode string is an array.\n
*/
uint32_t otz_decode_get_type(otz_operation_t* ps_operation);

/**
* @brief Get decode error
*
* This function returns the error state of the decoder associated 
* with the given operation.
*
* This function does not affect the error state of the decoder.
*
* Undefined Behavior\n
* The following situations result in UNDEFINED behavior:\n
*       Calling with psOperation set to NULL.\n
*       Calling with psOperation pointing to an operation not in the state 
*           OTZ_STATE_DECODE.
*
* @param ps_operation - The operation from which we are retrieving the result.
*
* @return - OTZ_SUCCESS: There have been no decoding errors.\n
*           OTZ_ERROR_*: The first decoder error to occur.
*/
otz_return_t otz_decode_get_error(otz_operation_t* ps_operation);

#endif
