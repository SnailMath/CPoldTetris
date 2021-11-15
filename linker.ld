ENTRY(_main);

SECTIONS {
	start_address = 0x8CFF0000;
	.init start_address : AT(start_address) {
		*(.init)
	}
	. = 0x8CFF0010;
	.text : {
		*(.text)
	}
}



