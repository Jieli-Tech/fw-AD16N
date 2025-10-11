        btctler_code_start = .;
        BTCTLER_CONTROLLER_CODE_START = .;
		*(.bt_rf_text_const)
		*(.bt_rf_text)
        BTCTLER_CONTROLLER_CODE_SIZE = ABSOLUTE(. - BTCTLER_CONTROLLER_CODE_START);

        BTCTLER_CONTROLLER_HCI_CODE_START = .;
		*(.vendor_manager_text_const)
		*(.vendor_manager_text)
		*(.hci_controller_text_const)
		*(.hci_controller_text)
        BTCTLER_CONTROLLER_HCI_CODE_SIZE = ABSOLUTE(. - BTCTLER_CONTROLLER_HCI_CODE_START);

        BTCTLER_LE_CONTROLLER_BLE_RF_CODE_START = .;
		*(.ble_rf_text_const)
		*(.ble_rf_text)
        BTCTLER_LE_CONTROLLER_BLE_RF_CODE_SIZE = ABSOLUTE(. - BTCTLER_LE_CONTROLLER_BLE_RF_CODE_START);

        BTCTLER_LE_CONTROLLER_BLE_LL_CODE_START = .;
	    *(.ble_ll_text_const)
	    *(.ble_ll_text)
        BTCTLER_LE_CONTROLLER_BLE_LL_CODE_SIZE = ABSOLUTE(. - BTCTLER_LE_CONTROLLER_BLE_LL_CODE_START);

        BTCTLER_LE_CONTROLLER_BLE_HCI_CODE_START = .;
		*(.ble_hci_text_const)
		*(.ble_hci_text)

        btctler_code_end = .;
        BTCTLER_LE_CONTROLLER_BLE_HCI_CODE_SIZE = ABSOLUTE(. - BTCTLER_LE_CONTROLLER_BLE_HCI_CODE_START);
        BTCTLER_LE_CONTROLLER_CODE_SIZE = BTCTLER_LE_CONTROLLER_BLE_RF_CODE_SIZE + BTCTLER_LE_CONTROLLER_BLE_LL_CODE_SIZE + BTCTLER_LE_CONTROLLER_BLE_HCI_CODE_SIZE;

        BTCTLER_CL_CODE_START = .;
        BTCTLER_CL_CODE_SIZE = ABSOLUTE(. - BTCTLER_CL_CODE_START);
        BTCTLER_CODE_TOTAL  = (btctler_code_end - btctler_code_start);
