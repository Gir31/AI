#include "InvestigateNoiseGoal_Evaluator.h"
#include "Goal_Think.h"
#include "Raven_Goal_Types.h"
#include "Raven_Feature.h"
#include "misc/cgdi.h"
#include "misc/Stream_Utility_Functions.h"
#include "../Raven_SensoryMemory.h" // 필수!

//------------------ CalculateDesirability ------------------------------------
double InvestigateNoiseGoal_Evaluator::CalculateDesirability(Raven_Bot* pBot)
{
    double Desirability = 0.0;

    // 1. 최근 감지된 소리 정보 가져오기
    double NoiseCloseness = Raven_Feature::ClosenessToLastNoise(pBot);

    if (NoiseCloseness <= 0.0)
    {
        return 0.0;
    }

    // 2. (기존 코드) 눈앞에 적이 보이면 전투 우선
    if (pBot->GetTargetSys()->isTargetPresent())
    {
        Raven_Bot* pTarget = pBot->GetTargetBot();
        if (pTarget && pBot->GetSensoryMem()->isOpponentWithinFOV(pTarget))
        {
            return 0.0;
        }
    }

    // ------------------------------------------------------------------------
    // [수정된 부분] 도착 판정 범위를 봇 크기의 1.5~2배 정도로 넉넉하게 잡습니다.
    // ------------------------------------------------------------------------
    Vector2D NoisePos = pBot->GetSensoryMem()->GetMostRecentlySensedPosition();

    // BRadius()는 봇의 반지름입니다. 
    // "내 몸집만큼 근처에 왔으면 도착한 걸로 치자"라는 의미입니다.
    double StopRadius = pBot->BRadius() * 1.5;
    double StopDistanceSq = StopRadius * StopRadius;

    // 거리가 이 범위 안이면 0점 반환 (도착했으므로 조사 종료)
    if (pBot->Pos().DistanceSq(NoisePos) < StopDistanceSq)
    {
        return 0.0;
    }
    // ------------------------------------------------------------------------

    // 3. 점수 계산 (기존 로직)
    const double Tweaker = 1.0;
    Desirability = Tweaker * NoiseCloseness;
    Desirability *= m_dCharacterBias;

    // 클램핑
    if (Desirability > 1.0) Desirability = 1.0;
    if (Desirability < 0.0) Desirability = 0.0;

    return Desirability;
}

//----------------------------- SetGoal ---------------------------------------
void InvestigateNoiseGoal_Evaluator::SetGoal(Raven_Bot* pBot)
{
    // *주의: Goal_Think 클래스에 AddGoal_InvestigateNoise() 함수를 추가해야 함
    pBot->GetBrain()->AddGoal_InvestigateNoise();
}

//-------------------------- RenderInfo ---------------------------------------
void InvestigateNoiseGoal_Evaluator::RenderInfo(Vector2D Position, Raven_Bot* pBot)
{
    gdi->TextAtPos(Position, "IN: " + ttos(CalculateDesirability(pBot), 2));
}