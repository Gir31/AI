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

    // 1. 최근 감지된 소리가 얼마나 가까운지 가져옴 (0.0 ~ 1.0)
    double NoiseCloseness = Raven_Feature::ClosenessToLastNoise(pBot);

    // 소리가 너무 멀거나(0.0) 없으면 관심 없음
    if (NoiseCloseness <= 0.0)
    {
        return 0.0;
    }

    // 2. 눈앞에 적이 있으면(TargetPresent) 소리 추적보다 전투가 우선이므로 0점
    if (pBot->GetTargetSys()->isTargetPresent() 
        && pBot->GetTargetSys()->isTargetWithinFOV())
    {
        return 0.0;
    }

    // 3. 점수 계산
    const double Tweaker = 1.0; // 기본 중요도
    Desirability = Tweaker * NoiseCloseness;

    // 4. 봇의 성격(Bias) 반영
    Desirability *= m_dCharacterBias;

    // 0~1 범위 클램핑
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