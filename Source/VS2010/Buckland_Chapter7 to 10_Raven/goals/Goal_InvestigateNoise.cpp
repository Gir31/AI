#include "Goal_InvestigateNoise.h"
#include "Goal_MoveToPosition.h"      // 이동을 담당할 하위 목표
#include "../Raven_SensoryMemory.h"   // 소리 위치 기억 조회용
#include "../Raven_Bot.h"

//------------------------------- Activate ------------------------------------
// 목표가 처음 시작되거나 재시작될 때 호출됩니다.
//-----------------------------------------------------------------------------
void Goal_InvestigateNoise::Activate()
{
    m_iStatus = active;

    // 기존에 남아있던 하위 목표들을 깨끗이 비웁니다.
    RemoveAllSubgoals();

    // 1. 감각 기억 시스템(SensoryMemory)에서 가장 최근 소리가 났던 위치를 가져옵니다.
    Vector2D noisePos = m_pOwner->GetSensoryMem()->GetMostRecentlySensedPosition();

    // (방어 코드) 만약 위치가 유효하지 않다면(예: 봇의 현재 위치와 동일), 목표를 즉시 완료 처리합니다.
    if (noisePos == m_pOwner->Pos())
    {
        m_iStatus = completed;
        return;
    }

    // 2. 해당 위치로 이동하는 하위 목표(Goal_MoveToPosition)를 추가합니다.
    //    Goal_MoveToPosition은 길 찾기(Pathfinding)까지 알아서 처리해줍니다.
    AddSubgoal(new Goal_MoveToPosition(m_pOwner, noisePos));
}

//------------------------------ Process --------------------------------------
// 매 업데이트마다 호출되어 하위 목표(이동)가 끝났는지 확인합니다.
//-----------------------------------------------------------------------------
int Goal_InvestigateNoise::Process()
{
    // 만약 비활성 상태라면 Activate()를 호출해 시작합니다.
    ActivateIfInactive();

    // 하위 목표(Goal_MoveToPosition)를 실행하고 상태를 받아옵니다.
    m_iStatus = ProcessSubgoals();

    // 만약 이동이 끝났다면(completed) 혹은 실패했다면(failed),
    // 이 InvestigateNoise 목표도 그에 맞춰 종료됩니다.
    return m_iStatus;
}