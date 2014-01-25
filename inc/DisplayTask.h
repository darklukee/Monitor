/*
 * DisplayTask.h
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
#ifndef DISPLAYTASK_H_
#define DISPLAYTASK_H_

#include <cpp_task.hpp>
#include "glcd.h"

class DisplayTask: public scheduler_task
{
public:
	DisplayTask();
	bool init();
	bool taskEntry();
	bool run(void *param);

	bool isInitiated(void);
	void putChar(char);
private:
	glcd GLCD;
	bool toggle;
	bool initiated;

	void data(void);
	void logger(void);
	void timer(void);

	void Demo();
};

#endif /* DISPLAYTASK_H_ */
