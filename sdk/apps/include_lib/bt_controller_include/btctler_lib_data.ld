        btctler_data_start = .;

        BTCTLER_CONTROLLER_DATA_START = .;
        *(.bt_rf_data)
		*(.vendor_manager_data)
		*(.hci_controller_data)
        BTCTLER_CONTROLLER_DATA_SIZE = ABSOLUTE(. - BTCTLER_CONTROLLER_DATA_START);

        BTCTLER_LE_CONTROLLER_DATA_START = .;
        *(.ble_ll_data)
        *(.ble_rf_data)
        *(.ble_hci_data)

        BTCTLER_LE_CONTROLLER_DATA_SIZE = ABSOLUTE(. - BTCTLER_LE_CONTROLLER_DATA_START);

        BTCTLER_CL_DATA_START = .;
        BTCTLER_CL_DATA_SIZE = ABSOLUTE(. - BTCTLER_CL_DATA_START);

        btctler_data_end = .;


