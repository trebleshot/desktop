//
// Created by veli on 2/24/19.
//

#ifndef TREBLESHOT_INTERRUPTER_H
#define TREBLESHOT_INTERRUPTER_H

#include <QObject>

class Interrupter {

    bool m_interrupted = false;

public:
    bool interrupted();

    void interrupt();

    void reset();
};


#endif //TREBLESHOT_INTERRUPTER_H
