#include <iostream>
#include <Windows.h>

#include "ecimotor.h"

int main()
{

    ecimotor eci30motor;
    eci30motor.vcsStart();

    long    m_lTargetPosition = 0;
    long    usrinput = 0;


    while (1)
    {
        

        // receive cmd
        std::cout << "Please input m_lTargetPosition ";
        std::cin >> m_lTargetPosition;

        if (m_lTargetPosition == 999)
            break;

        eci30motor.vcsSetPosition(m_lTargetPosition);

        


        while (!eci30motor.vcsIsArrivePosition())
        {

        }

        break;
    }


    // 
    eci30motor.vcsDisable();
    eci30motor.vcsStop();

    std::cout << "motor stoped\n";

}