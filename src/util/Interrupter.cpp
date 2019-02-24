//
// Created by veli on 2/24/19.
//

#include "Interrupter.h"

bool Interrupter::interrupted()
{
    return m_interrupted;
}

void Interrupter::interrupt()
{
    m_interrupted = true;
}
