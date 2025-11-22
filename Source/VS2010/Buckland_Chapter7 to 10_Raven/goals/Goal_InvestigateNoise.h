#ifndef GOAL_INVESTIGATE_NOISE_H
#define GOAL_INVESTIGATE_NOISE_H
#pragma warning (disable:4786)

#include "Goals/Goal_Composite.h"
#include "Raven_Goal_Types.h"
#include "../Raven_Bot.h"

class Goal_InvestigateNoise : public Goal_Composite<Raven_Bot>
{
public:

    Goal_InvestigateNoise(Raven_Bot* pBot)
        : Goal_Composite<Raven_Bot>(pBot, goal_investigate_noise)
    {
    }

    // 목표가 시작될 때 호출됨 (여기서 소리 위치를 파악하고 이동 명령을 내림)
    void Activate();

    // 매 프레임 실행됨
    int  Process();

    // 목표 종료 시 정리 (특별히 할 건 없음)
    void Terminate() {}
};

#endif