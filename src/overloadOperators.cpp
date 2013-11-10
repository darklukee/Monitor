/*
 * overloadOperators.cpp
 *
 *  Created on: 10 lis 2013
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

//#include <cstddef>
#include <stdlib.h>
#include <FreeRTOS.h>

void *operator new(size_t size)
{
	return pvPortMalloc(size);
}

void *operator new[](size_t size)
{
	return pvPortMalloc(size);
}

void operator delete(void *p)
{
	vPortFree(p);
}

void operator delete[](void *p)
{
	vPortFree(p);
}
