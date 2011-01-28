/*
 *  resource.h
 *
 *  OASIS Project
 *
 *  Created by Liva on 10/12/25.
 *
 */


class AvoidDeadLock {
	bool	_usingFlag;
	short	_usingPID;
public:
	AvoidDeadLock(void);
	void Lock(void);
	void Free(void);
};

