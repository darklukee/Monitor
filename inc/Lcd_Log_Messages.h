/*
 * Lcd_Log_Messages.h
 *
 *  Created on: 22 sty 2014
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
#ifndef LCD_LOG_MESSAGES_H_
#define LCD_LOG_MESSAGES_H_

static const char LcdLogTab[][22] =
	{ "USB Connected",
		"USB Disconnected",
		"USB Enabled",
		"USB Disabled"
	};
enum LcdLogEnum
{
	LOG_usbCon = 0,
	LOG_usbDisCon,
	LOG_usbEn,
	LOG_usbDis
};
#endif /* LCD_LOG_MESSAGES_H_ */
