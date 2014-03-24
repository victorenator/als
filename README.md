ALS (Ambient Light Sensor) Driver
===

ASUS Zenbook Ambient Light Sensor Driver.

Device /sys/bus/acpi/devices/ACPI0008:00 (ACPI path: \_SB.ALS). 

Exported attributes:
- ali (Ambient Light Illuminance) (ACPI path: \_SB.ALS._ALI)


Asus Zenbook:
==
- \_SB.ALS._HID() // Hardware ID. Return ACPI0008
- \_SB.ALS._STA() // Status. Always return 0x0B
- \_SB.ALS._DIS() // Disable Device. Empty method, does nothing.
- \_SB.ALS._ALI() // Ambient Light Illuminance. Returns one of the following values: 0x32 (min illuminance), 0xC8, 0x190, 0x258, 0x320 (max illuminance).
- \_SB.ALS._ALR() // Ambient Light Response. Returns [[0x30, 0x0], [0x30, 0x32], [0x64, 0xC8], [0x7D, 0x0190], [0xA0, 0x0258], [0xD0, 0x0320]].

Note:
For enable/disable ALS device, you must call \_SB.PCI0.LPCB.EC0.TALS method with 0x1/0x0 integer.
For notifications set acpi_osi="Windows 2012".

What methods notify ALS device about changes:
- \_SB.ATKD.ALSC (flag) // status changed
- \_SB.PCI0.LPCB.EC0.EC0W 
- \_SB.PCI0.LPCB.EC0._QCD () // illuminance changed
- \_SB.PCI0.LPCB.EC0._QDD () // illuminance changed

What methods control ALS device:
- \_SB.ATKD.ALSC (flag) // ALS Control
- \_SB.PCI0.LPCB.EC0.TALS (flag)

ALS device status:
- \_SB.ATKD.GALS () & 0x10 // Get ALs Status

ALS related paths:
- \_SB.ALAE // ALS Device Enabled; 0-1
- \_SB.LSTP // Light Sensor ? Present; 1
- \_SB.ALS.LUXL // LUX Level; 0
- \_SB.PCI0.LPCB.EC0 // Embedded Controller ITE IT8572G
- \_SB.PCI0.LPCB.EC0.RRAM (0x04C9) // Read RAM; returns 0x0-0xF
- \_SB.PCI0.LPCB.EC0.OLUX // Old LUX

Stack:
\_SB.ALS._ALI ()
    ^^PCI0.LPCB.EC0.RALS ()
        if (ALAE) {
            v = RRAM (0x04C9)
            if (v <= 1) return 0x32;
            else if (v <= 0x06) return 0xC8;
            else if (v <= 0x09) return 0x0190;
            else if (v <= 0x0E) return 0x0258;
            else return 0x0320;

        } else return 0xC8
