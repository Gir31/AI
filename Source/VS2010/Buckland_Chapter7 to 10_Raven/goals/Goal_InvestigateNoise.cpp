#include "Goal_InvestigateNoise.h"
#include "Goal_MoveToPosition.h" 
#include "../Raven_SensoryMemory.h" 
#include "../Raven_Bot.h"

//------------------------------- Activate ------------------------------------
// 
//-----------------------------------------------------------------------------
void Goal_InvestigateNoise::Activate()
{
    m_iStatus = active;

    RemoveAllSubgoals();

    Vector2D noisePos = m_pOwner->GetSensoryMem()->GetMostRecentlySensedPosition();

    if (noisePos == m_pOwner->Pos())
    {
        m_iStatus = completed;
        return;
    }

    AddSubgoal(new Goal_MoveToPosition(m_pOwner, noisePos));
}

//------------------------------ Process --------------------------------------
// 
//-----------------------------------------------------------------------------
int Goal_InvestigateNoise::Process()
{
    ActivateIfInactive();

    int SubgoalStatus = ProcessSubgoals();

    if (SubgoalStatus == completed)
    {
        m_pOwner->GetSensoryMem()->DismissMostRecentSound();

        m_iStatus = completed;
    }
    else if (SubgoalStatus == failed)
    {
        m_iStatus = failed;
    }

    return m_iStatus;
}