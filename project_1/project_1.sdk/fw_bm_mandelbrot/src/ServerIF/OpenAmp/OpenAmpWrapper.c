/*
 * OpenAmpWrapper.c
 *
 *  Created on: 2018/03/06
 *      Author: tak
 */


#include "xil_printf.h"
#include "openamp/open_amp.h"
#include "rsc_table.h"
#include "platform_info.h"

#include "FreeRTOS.h"
#include "task.h"

/* my code */
#include "common.h"

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/* from system_helper.c */
extern void buffer_create(void);
extern int buffer_push(void *data, int len);
extern void buffer_pull(void **data, int *len);

/* Local variables */
static struct hil_proc *hproc;
static struct rpmsg_channel *app_rp_chnl;
static struct rpmsg_endpoint *rp_ept;
static struct remote_proc *proc = NULL;
static struct rsc_table_info rsc_info;
static int evt_chnl_deleted = 0;
static int evt_virtio_rst = 0;
static int evt_have_data = 0;

static void virtio_rst_cb(struct hil_proc *hproc, int id)
{
	/* hil_proc only supports single virtio device */
	(void)id;

	if (!proc || proc->proc != hproc || !proc->rdev)
		return;

	LOG("Resetting RPMsg\n");
	evt_virtio_rst = 1;
}

/*-----------------------------------------------------------------------------*
 *  RPMSG callbacks setup by remoteproc_resource_init()
 *-----------------------------------------------------------------------------*/
static void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
			  void *priv, unsigned long src)
{
	(void)priv;
	(void)src;

	if (!buffer_push(data, len)) {
		LOG_E("cannot save data\n");
	} else {
		evt_have_data =1;
	}
}

static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl)
{
	app_rp_chnl = rp_chnl;
	rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_cb, RPMSG_NULL,
				  RPMSG_ADDR_ANY);
}

static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl)
{
	(void)rp_chnl;

	rpmsg_destroy_ept(rp_ept);
	rp_ept = NULL;
	evt_chnl_deleted = 1;
}

void openAmpWrapper_send(const void *data, int len)
{
	if (rpmsg_send(app_rp_chnl, data, len) < 0) {
		LOG_E("rpmsg_send failed\n");
	}
}

void openAmpWrapper_recv(void *data, int *len)
{
	hil_poll(proc->proc, 1);	// non blocking

	*len = 0;

	if (evt_have_data) {
		evt_have_data = 0;
		void *p;
		buffer_pull(&p, len);
		memcpy(data, p, *len);

//		/* Send data back to master*/
//		if (rpmsg_send(app_rp_chnl, data, *len) < 0) {
//			LOG_E("rpmsg_send failed\n");
//		}
	}

	if (evt_virtio_rst) {
		/* vring rst callback, reset rpmsg */
		LOG_W("De-initializing RPMsg\n");
		rpmsg_deinit(proc->rdev);
		proc->rdev = NULL;

		LOG_W("Reinitializing RPMsg\n");
		int status = 0;
		status = rpmsg_init(hproc, &proc->rdev,
				    rpmsg_channel_created,
				    rpmsg_channel_deleted,
				    rpmsg_read_cb,
				    1);
		if (status != RPROC_SUCCESS) {
			LOG_E("Reinit RPMsg failed\n");
//			break;
		}
		LOG_W("Reinit RPMsg succeeded\n");
		evt_chnl_deleted=0;
		evt_virtio_rst = 0;
	}
}

void openAmpWrapper_start()
{
	unsigned long proc_id = 0;
	unsigned long rsc_id = 0;


	LOG("Starting application...\n");

	/* Create buffer to send data between RPMSG callback and processing task */
	buffer_create();

	/* Initialize HW system components */
	init_system();

	hproc = platform_create_proc(proc_id);
	if (!hproc) {
		LOG_E("Failed to create proc platform data.\n");
	} else {
		rsc_info.rsc_tab = get_resource_table((int)rsc_id, &rsc_info.size);
		if (!rsc_info.rsc_tab) {
			LOG_E("Failed to get resource table data.\n");
		}
	}


	int status = 0;

	/* Initialize RPMSG framework */
	LOG("Try to init remoteproc resource\n");
	status = remoteproc_resource_init(&rsc_info, hproc,
				     rpmsg_channel_created,
				     rpmsg_channel_deleted, rpmsg_read_cb,
				     &proc, 0);

	if (RPROC_SUCCESS != status) {
		LOG_E("Failed  to initialize remoteproc resource.\n");
		return -1;
	}
	LOG("Init remoteproc resource succeeded\n");

	hil_set_vdev_rst_cb(hproc, 0, virtio_rst_cb);

	LOG("Waiting for events...\n");

	return;
}

void openAmpWrapper_stop()
{
	/* disable interrupts and free resources */
	LOG_W("De-initializating remoteproc resource\n");
	remoteproc_resource_deinit(proc);

	LOG_W("Stopping application...\n");
	cleanup_system();
	return;
}
