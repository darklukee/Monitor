/*
 * PGA.cpp
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

#include "PGA.h"
#include "hw_config.h"
#include "stm32f4xx_spi.h"

bool PGA::GPIO_Configured = false;

PGA::PGA()
{
	// TODO Auto-generated constructor stub
}

PGA::~PGA()
{
	// TODO Auto-generated destructor stub
}

void PGA::GPIO_Config(void)
{
	//TODO: implement me!
	GPIO_Configured = true;
}
