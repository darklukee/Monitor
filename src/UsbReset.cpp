/*
 * UsbReset.cpp
 *
 *  Created on: 17 sty 2014
 *      Author: lukee
 *
 * Copyright (C) 2014  darklukee
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see {http://www.gnu.org/licenses/}.
 */
#include <UsbReset.h>


extern xQueueHandle xQueue_UsbReset;
extern "C"
{
#include <usb_core.h>
uint32_t USB_OTG_ReadHPRT0(USB_OTG_CORE_HANDLE*);
}

UsbReset::UsbReset() :
	scheduler_task("UsbReset", 1024, PRIORITY_MEDIUM, NULL)
{
}

bool UsbReset::init()
{
	return true;
}

bool UsbReset::taskEntry()
{
	return true;
}


bool UsbReset::run(void *param)
{
	USB_OTG_CORE_HANDLE *pdev;
	if(xQueueReceive(xQueue_UsbReset, &pdev, portMAX_DELAY))
	{
	  USB_OTG_HPRT0_TypeDef  hprt0;

	  hprt0.d32 = USB_OTG_ReadHPRT0(pdev);
	  hprt0.b.prtrst = 1;
	  USB_OTG_WRITE_REG32(pdev->regs.HPRT0, hprt0.d32);
	  vTaskDelay(OS_MS(10));                              /* See Note #1 */
	  hprt0.b.prtrst = 0;
	  USB_OTG_WRITE_REG32(pdev->regs.HPRT0, hprt0.d32);
	  vTaskDelay(OS_MS(20));
	}
	return true;
}
