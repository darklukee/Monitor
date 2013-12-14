/*
 * KeyboardTask.h
 *
 *  Created on: 13 gru 2013
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
#ifndef KEYBOARDTASK_H_
#define KEYBOARDTASK_H_

#include <cpp_task.hpp>
#include "hw_config.h"

class KeyboardTask: public scheduler_task
{
public:
	KeyboardTask();
	bool init();
	bool taskEntry();
	bool run(void *param);

	void keyAction(int);
//	static void taskResume();

private:
	void debounce(void);
	bool keyCheck(int);
	void keyClear(int);
	void keyClearAll(void);
	bool enabledLeft(void);
	int debounceIter;
};

#endif /* KEYBOARDTASK_H_ */
