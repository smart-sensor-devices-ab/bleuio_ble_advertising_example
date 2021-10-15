/**
 ****************************************************************************************
 *
 * @file ble_peripheral_task.c
 *
 * @brief BLE peripheral task
 *
 * Copyright (C) 2015-2018 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "osal.h"
#include "sys_watchdog.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_service.h"
#include "ble_uuid.h"
#include "dis.h"
#include "sw_version.h"


/*
 * Notification bits reservation
 * bit #0 is always assigned to BLE event queue notification
 */
#define CTS_SET_TIME_NOTIF (1 << 2)
#define BCS_TIMER_NOTIF (1 << 3)

/*
 * BLE peripheral advertising data
 */
static const uint8_t adv_data[] = {
        0x0E, // Size (= Flag 1 byte, + Data 13 bytes = 14 = 0x0E in hex)
        GAP_DATA_TYPE_LOCAL_NAME, // Flag
        'B', 'l', 'u', 'I', 'O', ' ', 'E', 'x', 'a', 'm', 'p', 'l', 'e' // Data
};

static OS_TASK ble_peripheral_task_handle;

/*
 * Device Information Service data
 */

static const dis_system_id_t dis_sys_id = {
        .oui = { 0x80, 0xEA, 0xCA },    // Dialog Semiconductor Hellas SA
        .manufacturer = { 0x0A, 0x0B, 0x0C, 0x0D, 0x0E },
};

// copied from "Personal Health Devices Transcoding White Paper", available on bluetooth.org
static const uint8_t dis_reg_cert[] = {
        0x00, 0x02, 0x00, 0x12, 0x02, 0x01, 0x00, 0x08, 0x01,
        0x05, 0x00, 0x01, 0x00, 0x02, 0x80, 0x08, 0x02, 0x02,
        0x00, 0x02, 0x00, 0x00,
};

static const dis_pnp_id_t dis_pnp_id = {
        .vid_source = 0x01,     // Bluetooth SIG
        .vid = 0x00D2,          //  Dialog Semiconductor B.V.
        .pid = 0x0001,
        .version = 0x0001,
};

static const dis_device_info_t dis_info = {
        .manufacturer = "Dialog Semiconductor",
        .model_number = "Dialog BLE",
        // sample data
        .serial_number = "SN123456",
        .hw_revision = "Rev.D",
        .fw_revision = "1.0",
        .sw_revision = BLACKORCA_SW_VERSION,
        .system_id = &dis_sys_id,
        .reg_cert_length = sizeof(dis_reg_cert),
        .reg_cert = dis_reg_cert,
        .pnp_id = &dis_pnp_id,
};

/*
 * Main code
 */
static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        /**
         * Manage connection information
         */
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
}

static void handle_evt_gap_adv_completed(ble_evt_gap_adv_completed_t *evt)
{
        // restart advertising so we can connect again
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

void ble_peripheral_task(void *params)
{
        int8_t wdog_id;
        ble_service_t *svc;

        // in case services which do not use svc are all disabled, just surpress -Wunused-variable
        (void) svc;

        /* register ble_peripheral task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        ble_peripheral_task_handle = OS_GET_CURRENT_TASK();

        ble_peripheral_start();
        ble_register_app();

        ble_gap_device_name_set("BleuIO Example", ATT_PERM_READ);

        /* Add DIS */
        dis_init(NULL, &dis_info);

        ble_gap_adv_data_set(sizeof(adv_data), adv_data, 0, NULL);
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /* notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for task notification. The return value must be OS_OK */
                OS_ASSERT(ret == OS_OK);

                /* resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                /* notified from BLE manager, can get event */
                if (notif & BLE_APP_NOTIFY_MASK) {
                        ble_evt_hdr_t *hdr;

                        hdr = ble_get_event(false);
                        if (!hdr) {
                                goto no_event;
                        }

                        if (ble_service_handle_event(hdr)) {
                                goto handled;
                        }

                        switch (hdr->evt_code) {
                        case BLE_EVT_GAP_CONNECTED:
                                handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_ADV_COMPLETED:
                                handle_evt_gap_adv_completed((ble_evt_gap_adv_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_DISCONNECTED:
                                handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PAIR_REQ:
                        {
                                ble_evt_gap_pair_req_t *evt = (ble_evt_gap_pair_req_t *) hdr;
                                ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
                                break;
                        }
                        default:
                                ble_handle_event_default(hdr);
                                break;
                        }

                        handled:
                        OS_FREE(hdr);

                        no_event:
                        // notify again if there are more events to process in queue
                        if (ble_has_event()) {
                                OS_TASK_NOTIFY(OS_GET_CURRENT_TASK(), BLE_APP_NOTIFY_MASK, eSetBits);
                        }
                }
        }
}
