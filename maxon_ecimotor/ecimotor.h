#pragma once
#include "Definitions.h"


class ecimotor
{
public:
	void vcsStart();
	void vcsEnable();
	void vcsDisable();
	void vcsSetPosition(const long ltargetPositon);
	bool vcsIsArrivePosition();
	void vcsStop();


private:

	__int8 m_bMode;
	BOOL m_oImmediately;
	BOOL m_oInitialisation;
	BOOL m_oUpdateActive;
	DWORD m_ulErrorCode;
	DWORD m_ulProfileAcceleration;
	DWORD m_ulProfileDeceleration;
	DWORD m_ulProfileVelocity;
	HANDLE m_KeyHandle = nullptr;
	WORD m_usNodeId = 0;


	int     m_oRadio = 1; // 0 relative, 1 absolute
	long    m_lActualValue = 0;
	long    m_lStartPosition = 0;
	long    m_lTargetPosition = 0;
};

