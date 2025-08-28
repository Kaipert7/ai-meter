#ifdef ENABLE_LORAWAN

#pragma once

#ifndef CLASSFFLOWLORAWAN_H
#define CLASSFFLOWLORAWAN_H

#include "ClassFlow.h"
#include "ClassFlowPostProcessing.h"
#include <string>

extern uint8_t dev_eui[8];
extern uint8_t app_eui[8];
extern uint8_t app_key[16];

class ClassFlowLorawan :
    public ClassFlow
{
protected:
    ClassFlowPostProcessing* flowpostprocessing;  
public:
    ClassFlowLorawan();
    ClassFlowLorawan(std::vector<ClassFlow*>* lfc);
    ClassFlowLorawan(std::vector<ClassFlow*>* lfc, ClassFlow *_prev);

    bool Start();

    bool ReadParameter(FILE* pfile, string& aktparamgraph);
    bool doFlow(string time);
    string name(){return "ClassFlowLorawan";};
};
#endif //CLASSFFLOWLORWAWAN_H
#endif //ENABLE_LORAWAN