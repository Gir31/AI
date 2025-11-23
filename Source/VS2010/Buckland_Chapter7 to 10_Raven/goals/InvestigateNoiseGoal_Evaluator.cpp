#include "InvestigateNoiseGoal_Evaluator.h"
#include "Goal_Think.h"
#include "Raven_Goal_Types.h"
#include "Raven_Feature.h"
#include "misc/cgdi.h"
#include "misc/Stream_Utility_Functions.h"
#include "../Raven_SensoryMemory.h" 
#include "../Raven_Game.h"

//------------------ CalculateDesirability ------------------------------------
double InvestigateNoiseGoal_Evaluator::CalculateDesirability(Raven_Bot* pBot)
{
    double Desirability = 0.0;
    
    double NoiseCloseness = Raven_Feature::ClosenessToLastNoise(pBot);

    if (NoiseCloseness <= 0.0) return 0.0;

    if (pBot->GetTargetSys()->isTargetPresent())
    {
        Raven_Bot* pTarget = pBot->GetTargetBot();
        if (pTarget && pBot->GetSensoryMem()->isOpponentWithinFOV(pTarget))
        {
            return 0.0;
        }
    }

    Vector2D NoisePos = pBot->GetSensoryMem()->GetMostRecentlySensedPosition();
    double DistToNoiseSq = pBot->Pos().DistanceSq(NoisePos);
    double StopRadius = pBot->BRadius() * 2.0;

    if (DistToNoiseSq < StopRadius * StopRadius) return 0.0;

    const double Tweaker = 1.0;
    Desirability = Tweaker * NoiseCloseness;

    if (pBot->GetBrain()->isGoalActive(goal_investigate_noise))
    {
        Desirability *= 2.0;
    }

    Desirability *= m_dCharacterBias;

    if (Desirability > 1.0) Desirability = 1.0;
    if (Desirability < 0.0) Desirability = 0.0;

    return Desirability;
}

//----------------------------- SetGoal ---------------------------------------
void InvestigateNoiseGoal_Evaluator::SetGoal(Raven_Bot* pBot)
{
    pBot->GetBrain()->AddGoal_InvestigateNoise();
}

//-------------------------- RenderInfo ---------------------------------------
void InvestigateNoiseGoal_Evaluator::RenderInfo(Vector2D Position, Raven_Bot* pBot)
{
    gdi->TextAtPos(Position, "IN: " + ttos(CalculateDesirability(pBot), 2));
}