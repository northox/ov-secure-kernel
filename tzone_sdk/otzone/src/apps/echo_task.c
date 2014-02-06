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
 * Echo task implementation
 */


#include <sw_types.h>
#include <monitor.h>
#include <sw_debug.h>
#include <cpu_data.h>
#include <global.h>
#include <task.h>
#include <dispatcher_task.h>
#include <page_table.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>
#include <sw_wait.h>
#include <sw_semaphores.h>
#include <elf_loader.h>
#include <otz_common.h>
#include <otz_id.h>
#include <cpu.h>
#include <task_control.h>
#include <echo_task.h>

#include <otz_app_eg.h>

#include <sw_io.h>

#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
/**
 * @brief Echo the data for the user supplied buffer with async support
 *
 * This function copies the request buffer to response buffer to show the 
 * non-zero copy functionality and to show the async support by wait for the 
 * flag and it got set in interrupt handler
 *
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param res_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: API processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_echo_async_send_cmd(void *req_buf, u32 req_buf_len, 
		void *res_buf, u32 res_buf_len, 
		struct otzc_encode_meta *meta_data,
		u32 *ret_res_buf_len)
{
	echo_data_t echo_data;
	char *out_buf;
	int offset = 0, pos = 0, mapped = 0, type, out_len;
	int task_id; 
	sw_tls *tls;
	struct echo_global *echo_global;


	task_id = get_current_task_id();
	tls = get_task_tls(task_id);
	echo_global = (struct echo_global *)tls->private_data;

	if(!echo_global->data_available)
	{
		struct timer_event* tevent;
		timeval_t time;

		tevent = timer_event_create(&echo_task_handler,(void*)task_id);
		if(!tevent){
			sw_printf("SW: Out of Memory : Cannot register Handler\n");
			return SMC_ENOMEM;
		}

		/* Time duration = 100ms */
		time.tval.nsec = 100000000;
		time.tval.sec = 0;

		struct sw_task* task = get_task(task_id);

		task->wq_head.elements_count = 0;
		INIT_LIST_HEAD(&task->wq_head.elements_list);
		task->wq_head.spin_lock.lock = 0;
		timer_event_start(tevent,&time);

#ifdef ASYNC_DBG
		sw_printf("SW: Before calling wait event \n"); 
#endif
		sw_wait_event_async(&task->wq_head, 
				echo_global->data_available, SMC_PENDING);
#ifdef ASYNC_DBG
		sw_printf("SW: Coming out from wait event \n");
#endif


	}

	if(req_buf_len > 0) {
		while (offset <= req_buf_len) {
			if(decode_data(req_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_ENC_UINT32)
					return SMC_EINVAL_ARG;

				echo_data.len = *((u32*)out_buf);
			}

			if(decode_data(req_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_ENC_ARRAY)
					return SMC_EINVAL_ARG;

				sw_memcpy(echo_data.data, out_buf, echo_data.len);
			}

			break;
		}
	}

	offset = 0, pos = OTZ_MAX_REQ_PARAMS;
	if(res_buf_len > 0) {
		while (offset <= res_buf_len) {
			if(decode_data(res_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_ENC_ARRAY)
					return SMC_EINVAL_ARG;
			}
			sw_memcpy(out_buf, echo_data.data, echo_data.len);
			if(update_response_len(meta_data, pos, echo_data.len))
				return SMC_EINVAL_ARG;

			break;
		}
		*ret_res_buf_len = echo_data.len;
	}
	/*
	   sw_printf("SW: echo task data: %s\n", echo_data.data);
	 */

	/*
	   sw_printf("SW: echo send cmd %s and len 0x%x strlen 0x%x\n", echo_data.data,
	   echo_data.len, sw_strlen(echo_data.data));
	 */

	return 0;
}
#endif

/**
 * @brief Echo the data for the user supplied buffer
 *
 * This function copies the request buffer to response buffer to show the 
 * non-zero copy functionality
 *
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param res_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: API processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_echo_send_cmd(void *req_buf, u32 req_buf_len, 
		void *res_buf, u32 res_buf_len, 
		struct otzc_encode_meta *meta_data,
		u32 *ret_res_buf_len)
{
	echo_data_t echo_data;
	char *out_buf;
	int offset = 0, pos = 0, mapped = 0, type, out_len; 

	if(req_buf_len > 0) {
		while (offset <= req_buf_len) {
			if(decode_data(req_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_ENC_UINT32)
					return SMC_EINVAL_ARG;

				echo_data.len = *((u32*)out_buf);
			}

			if(decode_data(req_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_ENC_ARRAY)
					return SMC_EINVAL_ARG;

				sw_memcpy(echo_data.data, out_buf, echo_data.len);
			}

			break;
		}
	}

	offset = 0, pos = OTZ_MAX_REQ_PARAMS;
	if(res_buf_len > 0) {
		while (offset <= res_buf_len) {
			if(decode_data(res_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_ENC_ARRAY)
					return SMC_EINVAL_ARG;
			}
			sw_memcpy(out_buf, echo_data.data, echo_data.len);
			if(update_response_len(meta_data, pos, echo_data.len))
				return SMC_EINVAL_ARG;

			break;
		}
		*ret_res_buf_len = echo_data.len;
	}
	/*
	   sw_printf("SW: echo task data: %s\n", echo_data.data);
	 */
	/*
	   sw_printf("SW: echo send cmd %s and len 0x%x strlen 0x%x\n", echo_data.data,
	   echo_data.len, sw_strlen(echo_data.data));
	 */
	return 0;
}

/**
 * @brief Echo the data for the shared buffer
 *
 * This function copies the request buffer to response buffer to show the 
 * zero copy functionality
 *
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param res_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: API processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_echo_send_cmd_shared_buf(void *req_buf, u32 req_buf_len, 
		void *res_buf, u32 res_buf_len, 
		struct otzc_encode_meta *meta_data,
		u32 *ret_res_buf_len)
{
	echo_data_t echo_data;
	char *out_buf;
	int offset = 0, pos = 0, mapped = 0, type, out_len; 

	if(req_buf_len > 0) {
		while (offset <= req_buf_len) {
			if(decode_data(req_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_ENC_UINT32)
					return SMC_EINVAL_ARG;

				echo_data.len = *((u32*)out_buf);
			}

			if(decode_data(req_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_MEM_REF)
					return SMC_EINVAL_ARG;

				sw_memcpy(echo_data.data, out_buf, echo_data.len);

				if(mapped)
					unmap_from_ns((va_t)out_buf);
			}

			break;
		}
	}

	offset = 0, pos = OTZ_MAX_REQ_PARAMS;
	if (res_buf_len > 0) {
		while (offset <= res_buf_len) {
			if(decode_data(res_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_MEM_REF)
					return SMC_EINVAL_ARG;
			}

			sw_memcpy(out_buf, echo_data.data, echo_data.len);
			if(update_response_len(meta_data, pos, echo_data.len))
				return SMC_EINVAL_ARG;

			if(mapped)
				unmap_from_ns((va_t)out_buf);

			break;
		}
		*ret_res_buf_len = echo_data.len;
	}

	/*
	   sw_printf("SW: echo task data: %s\n", echo_data.data);
	 */
	/*
	   sw_printf("SW: echo send cmd %s and len 0x%x strlen 0x%x\n",
	   echo_data.data, echo_data.len, sw_strlen(echo_data.data));

	 */
	return 0;
}


/**
 * @brief Echo the data for IPI testing
 *
 * This function copies the request buffer to response buffer to show the 
 * IPI functionality
 *
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param res_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: API processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_echo_ipi_send_cmd(void *req_buf, u32 req_buf_len, 
		void *res_buf, u32 res_buf_len, 
		struct otzc_encode_meta *meta_data,
		u32 *ret_res_buf_len)
{
	echo_data_t echo_data;
	char *out_buf;
	int offset = 0, pos = 0, mapped = 0, type, out_len; 

	if(req_buf_len > 0) {
		while (offset <= req_buf_len) {
			if(decode_data(req_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_MEM_REF)
					return SMC_EINVAL_ARG;

				sw_memcpy(echo_data.data, out_buf, out_len);
				echo_data.len = out_len;
			}
			break;
		}
	}

	offset = 0, pos = OTZ_MAX_REQ_PARAMS;
	if (res_buf_len > 0) {
		while (offset <= res_buf_len) {
			if(decode_data(res_buf, meta_data, 
						&type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			else {
				if(type != OTZ_MEM_REF)
					return SMC_EINVAL_ARG;
			}
			sw_memcpy(out_buf, echo_data.data, echo_data.len);
			if(update_response_len(meta_data, pos, echo_data.len))
				return SMC_EINVAL_ARG;

			break;
		}
		*ret_res_buf_len = echo_data.len;
	}

	/*
	   sw_printf("SW: echo send cmd %s and len 0x%x strlen 0x%x\n",
	   echo_data.data, echo_data.len, sw_strlen(echo_data.data));

	 */
	return 0;
}


/**
 * @brief Process echo service 
 *
 * This function process the echo service commands
 *
 * @param svc_cmd_id: Command identifer to process the echo service command
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param resp_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: Echo service command processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_echo_svc(u32 svc_cmd_id, 
		void *req_buf, u32 req_buf_len, 
		void *resp_buf, u32 res_buf_len,
		struct otzc_encode_meta *meta_data, u32 *ret_res_buf_len) 
{
	int ret_val = SMC_ERROR;

	switch (svc_cmd_id) {
		case OTZ_ECHO_CMD_ID_SEND_CMD:
			ret_val = process_otz_echo_send_cmd(
					req_buf,
					req_buf_len,
					resp_buf,
					res_buf_len,
					meta_data,
					ret_res_buf_len);
			break;
		case OTZ_ECHO_CMD_ID_SEND_CMD_SHARED_BUF:
		case OTZ_ECHO_CMD_ID_SEND_CMD_ARRAY_SPACE:
			ret_val = process_otz_echo_send_cmd_shared_buf(
					req_buf,
					req_buf_len,
					resp_buf,
					res_buf_len,
					meta_data,
					ret_res_buf_len);
			break;
		case OTZ_ECHO_CMD_ID_IPI_SEND_CMD:
			ret_val = process_otz_echo_ipi_send_cmd(
					req_buf,
					req_buf_len,
					resp_buf,
					res_buf_len,
					meta_data,
					ret_res_buf_len);
			break;
#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
		case OTZ_ECHO_CMD_ID_TEST_ASYNC_SEND_CMD:
			ret_val = process_otz_echo_async_send_cmd(
					req_buf,
					req_buf_len,
					resp_buf,
					res_buf_len,
					meta_data,
					ret_res_buf_len);
			break;
#endif
		default:
			ret_val = SMC_EOPNOTSUPP;
			break;
	}

	return ret_val;
}

#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
/**
 * @brief Interrupt handler for echo task
 *
 * This function set the variable to implement the functionality of notification
 * meachanism.
 *
 * @param interrupt: Interrupt ID
 * @param data: TLS data
 */
void echo_task_handler(struct timer_event* tevent)
{
	void* data = tevent->data;
	sw_tls *tls;
	struct echo_global *echo_data;

	u32 task_id = (u32) data;

	sw_printf("SW: echo task handler 0x%x\n", task_id);
	tls = get_task_tls(task_id);
	if(tls) {
		echo_data = (struct echo_global *)tls->private_data;
		echo_data->data_available = 1;
	}

	if(tls) {    
		notify_ns(task_id);
	}
	else {
		sw_printf("SW: where is the task???\n");
	}

	tevent->state &= ~TIMER_STATE_EXECUTING;
	timer_event_destroy(tevent);
}
#endif

#ifdef WQ_TEST_FUNC
/* Functions to test the Wait Queue Implementation */
int wq_test_var;

/**
 * @brief 
 *      Dummy wake up function. It is called from wakeup/wakeup function
 * @param wq
 * @param wakeup_flag
 */
void test_wq_dummy_wakeup_function(struct wait_queue* wq, u32 wakeup_flag)
{
	sw_printf("SW: Inside Test Dummy wakeup function\n");
	sw_default_wake_function(wq, wakeup_flag);
}

/**
 * @brief 
 *      This function wakes up the task but does not set the condition
 *      So the task will go back to sleep again.
 *      This function is called from Timer Interrupt handler
 * @param tevent
 */
void test_wq_noset_condition(struct timer_event* tevent)
{
	void* data = tevent->data;
	u32 task_id = (u32)data;
	sw_printf("SW: No Set condition Int Handler\n");

	struct sw_task* task = get_task(task_id);
	sw_printf("SW: Calling wakeup task from Int handler \n"); 
	sw_wakeup(&task->wq_head, WAKE_UP);
	sw_printf("SW: Called wakeup task from Int handler \n"); 

	tevent->state &= ~TIMER_STATE_EXECUTING;
	timer_event_destroy(tevent);
}

/**
 * @brief 
 *       This function sets the condition and wakes up the task
 *       This function is called from Timer Interrupt handler
 * @param tevent
 * @param tevent
 */
void test_wq_set_condition(struct timer_event* tevent)
{
	void* data = tevent->data;
	u32 task_id = (u32)data;

	sw_printf("SW: Set condition Int Handler\n");
	wq_test_var = 1;

	struct sw_task* task = get_task(task_id);
	sw_printf("SW: Calling wakeup task from Int handler \n"); 
	sw_wakeup(&task->wq_head, WAKE_UP);
	sw_printf("SW: Called wakeup task from Int handler \n"); 

	tevent->state &= ~TIMER_STATE_EXECUTING;
	timer_event_destroy(tevent);
}

/**
 * @brief 
 *      This function initializes the waitqueue and put itself to a waitqueue.
 *      It also registers timer handlers whill will set the condition and wakes
 *      up the task again
 * @param task_id
 */
void test_wait_queue(int task_id)
{
	wq_test_var = 0;
	struct sw_task* task = get_task(task_id);

	struct timer_event *tevent,*tevent1;
	timeval_t time;

	tevent = timer_event_create(&test_wq_set_condition,(void*)task_id);
	time.tval.sec = 0;
	time.tval.nsec = 10000000;/*10ms*/
	timer_event_start(tevent, &time);

	tevent1 = timer_event_create(&test_wq_noset_condition,(void*)task_id);
	time.tval.sec = 0;
	time.tval.nsec = 1000000;/*1ms*/
	timer_event_start(tevent1, &time);

	task->wq_head.elements_count = 0;
	INIT_LIST_HEAD(&task->wq_head.elements_list);

#ifdef ECHO_DBG
	sw_printf("SW: Before calling wait event \n"); 
	sw_wait_event(&task->wq_head, (wq_test_var==1));
	sw_printf("SW: Coming out from wait event \n");
#endif

	struct wait_queue wq;
	/*sw_init_waitqueue_entry(&wq);*/
	sw_init_waitqueue_entry_func(&wq, &test_wq_dummy_wakeup_function);
	sw_add_to_waitqueue(&task->wq_head,&wq);

	sw_printf("SW: Before putting the task to sleep in waitqueue  \n"); 
	while(!(wq_test_var==1)){
		sw_set_task_state(TASK_STATE_WAIT);
		schedule();
	}
	sw_printf("SW: coming out of sleep from waitqueue \n"); 
	sw_remove_from_waitqueue(&task->wq_head,&wq);
}
#endif /* WQ_TEST_FUNC */

/**
 * @brief Echo task entry point
 *
 * This function implements the commands to echo the input buffer using copying 
 * the buffer and using shared memory
 *
 * @param task_id: task identifier
 * @param tls: Pointer to task local storage
 */
void echo_task(int task_id, sw_tls* tls)
{
	tls->ret_val = 0;

#ifdef ECHO_TIMER_TEST
	struct timer_event* tevent1;
#endif

#ifdef WQ_TEST_FUNC
	test_wait_queue(task_id);
#endif

#ifdef ECHO_TIMER_TEST
	tevent1 = timer_event_create(&echo_task_handler,(void*)task_id);
	if(!tevent1){
		sw_printf("SW: Out of Memory : Cannot register Handler\n");
		return;
	}

	time.tval.nsec = 1000;
	time.tval.sec = 0;
	timer_event_start(tevent1, time);
#endif

	tls->ret_val = process_otzapi(task_id, tls);


#ifdef ECHO_IPC_TEST
	ipc_test_crypto(OTZ_SVC_ECHO, task_id);
#endif

	handle_task_return(task_id, tls);

	DIE_NOW(0, "echo task - hangs\n");
}

/**
 * @brief: Echo task init
 *
 * This function initializes echo task parameters
 *
 * @param psa_config: Configuration parameter for the task and its get called 
 * before the task creation
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */

int echo_task_init(sa_config_t *psa_config)
{
	sw_memset(psa_config, 0x0, sizeof(sa_config_t));
	psa_config->service_uuid = OTZ_SVC_ECHO;
	sw_strcpy(psa_config->service_name, "echo");
	psa_config->stack_size = TASK_STACK_SIZE;
	psa_config->data = (void*)sw_malloc(sizeof(struct echo_global));
	if(!psa_config->data) {
		sw_printf("SW: echo task init: allocation of local storage data failed\n");
		return OTZ_ENOMEM;
	}

	psa_config->entry_point = &echo_task;
	psa_config->process = (u32)&process_otz_echo_svc;	

	sw_memset(psa_config->data, 0, sizeof(struct echo_global));
	return OTZ_OK;
}


/*
 * @brief Echo task exit
 *
 * This function gets called before the task deletion
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n 
 */
int echo_task_exit(void *data)
{
	if(data)
		sw_free(data);
	return OTZ_OK;
}
