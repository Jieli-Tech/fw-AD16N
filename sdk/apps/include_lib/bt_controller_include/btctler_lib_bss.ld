        btctler_bss_start = .;

        BTCTLER_CONTROLLER_BSS_START = .;


        *(.bt_rf_data_bss)
		*(.hci_controller_data_bss)
		*(.vendor_manager_data_bss)
        BTCTLER_CONTROLLER_BSS_SIZE = ABSOLUTE(. - BTCTLER_CONTROLLER_BSS_START);

        BTCTLER_LE_CONTROLLER_BSS_START = .;
        *(.ble_hci_data_bss)
        *(.ble_ll_data_bss)
        *(.ble_rf_data_bss)
        BTCTLER_LE_CONTROLLER_BSS_SIZE = ABSOLUTE(. - BTCTLER_LE_CONTROLLER_BSS_START);

        BTCTLER_CL_BSS_START = .;
        BTCTLER_CL_BSS_SIZE = ABSOLUTE(. - BTCTLER_CL_BSS_START);

        btctler_bss_end = .;

