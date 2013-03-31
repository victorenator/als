ALS (Ambient Light Sensor) Driver
===

ASUS Zenbook Ambient Light Sensor Driver.

Device /sys/bus/acpi/devices/ACPI0008:00 (ACPI path: \_SB.ALS). 

Exported attributes:
- ali (Ambient Light Illuminance) (ACPI path: \_SB.ALS._ALI)


Asus Zenbook:
==
_SB.ALS._HID() // Hardware ID. Return ACPI0008
_SB.ALS._STA() // Status. Always return 0x0B
_SB.ALS._DIS() // Disable Device. Empty method, does nothing.
_SB.ALS._ALI() // Ambient Light Illuminance. Returns the following values: 0x32, 0xC8, 0x190, 0x258, 0x320.
_SB.ALS._ALR() // Ambient Light Response. Returns [[0x30, 0x0], [0x30, 0x32], [0x64, 0xC8], [0x7D, 0x0190], [0xA0, 0x0258], [0xD0, 0x0320]].
}

For enable/disable ALS device, you must call \_SB.PCI0.LPCB.EC0.TALS method with 0x1/0x0 integer.
