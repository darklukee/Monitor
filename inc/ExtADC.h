/*
 * LTC2991.h
 *
 *  Created on: 28 sie 2013
 *      Author: lukee
 *
 * Copyright (C) 2013  darklukee
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

#ifndef LTC2991_H_
#define LTC2991_H_

class ExtADC
{
public:
	ExtADC();
	~ExtADC();
	static void GPIO_Config(void);
	void Init(void);

	void Write(uint8_t reg, uint8_t value);
	uint8_t Read(uint8_t reg);

private:
	bool GPIO_Configured;
};

#endif /* LTC2991_H_ */
