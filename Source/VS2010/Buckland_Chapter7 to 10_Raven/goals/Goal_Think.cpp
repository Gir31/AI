#include "Goal_Think.h"
#include <list>
#include "misc/Cgdi.h"
#include "../Raven_ObjectEnumerations.h"
#include "misc/utils.h"
#include "../lua/Raven_Scriptor.h"

#include "Goal_MoveToPosition.h"
#include "Goal_Explore.h"
#include "Goal_GetItem.h"
#include "Goal_Wander.h"
#include "Raven_Goal_Types.h"
#include "Goal_AttackTarget.h"
#include "Goal_InvestigateNoise.h"


#include "GetWeaponGoal_Evaluator.h"
#include "GetHealthGoal_Evaluator.h"
#include "ExploreGoal_Evaluator.h"
#include "AttackTargetGoal_Evaluator.h"
#include "InvestigateNoiseGoal_Evaluator.h"


Goal_Think::Goal_Think(Raven_Bot* pBot):Goal_Composite<Raven_Bot>(pBot, goal_think)
{
  
  //these biases could be loaded in from a script on a per bot basis
  //but for now we'll just give them some random values
  const double LowRangeOfBias = 0.5;
  const double HighRangeOfBias = 1.5;

  double HealthBias = RandInRange(LowRangeOfBias, HighRangeOfBias);
  double ShotgunBias = RandInRange(LowRangeOfBias, HighRangeOfBias);
  double RocketLauncherBias = RandInRange(LowRangeOfBias, HighRangeOfBias);
  double RailgunBias = RandInRange(LowRangeOfBias, HighRangeOfBias);
  double ExploreBias = RandInRange(LowRangeOfBias, HighRangeOfBias);
  double AttackBias = RandInRange(LowRangeOfBias, HighRangeOfBias);
  double InvestigateBias = RandInRange(LowRangeOfBias, HighRangeOfBias); 

  //create the evaluator objects
  m_Evaluators.push_back(new GetHealthGoal_Evaluator(HealthBias));
  m_Evaluators.push_back(new ExploreGoal_Evaluator(ExploreBias));
  m_Evaluators.push_back(new AttackTargetGoal_Evaluator(AttackBias));
  m_Evaluators.push_back(new GetWeaponGoal_Evaluator(ShotgunBias,
                                                     type_shotgun));
  m_Evaluators.push_back(new GetWeaponGoal_Evaluator(RailgunBias,
                                                     type_rail_gun));
  m_Evaluators.push_back(new GetWeaponGoal_Evaluator(RocketLauncherBias,
                                                     type_rocket_launcher));
  m_Evaluators.push_back(new InvestigateNoiseGoal_Evaluator(InvestigateBias)); 
}

//----------------------------- dtor ------------------------------------------
//-----------------------------------------------------------------------------
Goal_Think::~Goal_Think()
{
  GoalEvaluators::iterator curDes = m_Evaluators.begin();
  for (curDes; curDes != m_Evaluators.end(); ++curDes)
  {
    delete *curDes;
  }
}

//------------------------------- Activate ------------------------------------
//-----------------------------------------------------------------------------
void Goal_Think::Activate()
{
  if (!m_pOwner->isPossessed())
  {
    Arbitrate();
  }

  m_iStatus = active;
}

//------------------------------ Process --------------------------------------
//
//  processes the subgoals
//-----------------------------------------------------------------------------
int Goal_Think::Process()
{
  ActivateIfInactive();
  
  int SubgoalStatus = ProcessSubgoals();

  if (SubgoalStatus == completed || SubgoalStatus == failed)
  {
    if (!m_pOwner->isPossessed())
    {
      m_iStatus = inactive;
    }
  }
  else if (!m_pOwner->isPossessed())
  {
      // 현재 수행 중인 목표가 '중단 가능한' 낮은 우선순위 목표인지 확인합니다.
      // (예: Explore, Wander 등은 언제든지 멈춰도 됨)
      bool bInterruptable = false;
      if (!m_SubGoals.empty())
      {
          unsigned int CurrentType = m_SubGoals.front()->GetType();
          if (CurrentType == goal_explore || CurrentType == goal_wander)
          {
              bInterruptable = true;
          }
      }

      // 중단 가능한 상태라면, 혹시 아주 높은 점수의 목표(예: 소리 조사, 공격)가 있는지 봅니다.
      if (bInterruptable)
      {
          // 모든 평가자를 돌면서 최고 점수를 확인
          double BestScore = 0.0;
          GoalEvaluators::iterator curDes = m_Evaluators.begin();
          for (curDes; curDes != m_Evaluators.end(); ++curDes)
          {
              double score = (*curDes)->CalculateDesirability(m_pOwner);
              if (score > BestScore) BestScore = score;
          }

          // [핵심] 만약 최고 점수가 일정 기준(예: 0.5) 이상이라면
          // Explore(약 0.05점)를 당장 멈추고 새로운 목표를 다시 선정합니다.
          if (BestScore > 0.5)
          {
              Arbitrate(); // 현재 목표를 취소하고, 최고 점수 목표(InvestigateNoise)를 새로 시작
              return active; // 상태 유지
          }
      }
  }
  return m_iStatus;
}

//----------------------------- Update ----------------------------------------
// 
//  this method iterates through each goal option to determine which one has
//  the highest desirability.
//-----------------------------------------------------------------------------
void Goal_Think::Arbitrate()
{
  double best = 0;
  Goal_Evaluator* MostDesirable = 0;

  //iterate through all the evaluators to see which produces the highest score
  GoalEvaluators::iterator curDes = m_Evaluators.begin();
  for (curDes; curDes != m_Evaluators.end(); ++curDes)
  {
    double desirabilty = (*curDes)->CalculateDesirability(m_pOwner);

    if (desirabilty >= best)
    {
      best = desirabilty;
      MostDesirable = *curDes;
    }
  }

  assert(MostDesirable && "<Goal_Think::Arbitrate>: no evaluator selected");

  MostDesirable->SetGoal(m_pOwner);
}


//---------------------------- notPresent --------------------------------------
//
//  returns true if the goal type passed as a parameter is the same as this
//  goal or any of its subgoals
//-----------------------------------------------------------------------------
bool Goal_Think::notPresent(unsigned int GoalType)const
{
  if (!m_SubGoals.empty())
  {
    return m_SubGoals.front()->GetType() != GoalType;
  }

  return true;
}

void Goal_Think::AddGoal_MoveToPosition(Vector2D pos)
{
  AddSubgoal( new Goal_MoveToPosition(m_pOwner, pos));
}

void Goal_Think::AddGoal_Explore()
{
  if (notPresent(goal_explore))
  {
    RemoveAllSubgoals();
    AddSubgoal( new Goal_Explore(m_pOwner));
  }
}

void Goal_Think::AddGoal_GetItem(unsigned int ItemType)
{
  if (notPresent(ItemTypeToGoalType(ItemType)))
  {
    RemoveAllSubgoals();
    AddSubgoal( new Goal_GetItem(m_pOwner, ItemType));
  }
}

void Goal_Think::AddGoal_AttackTarget()
{
  if (notPresent(goal_attack_target))
  {
    RemoveAllSubgoals();
    AddSubgoal( new Goal_AttackTarget(m_pOwner));
  }
}

void Goal_Think::AddGoal_InvestigateNoise()
{
    // 1. 이미 이 목표(소리 조사)를 수행 중이라면 중복해서 추가하지 않음 (notPresent 체크)
    if (notPresent(goal_investigate_noise))
    {
        // 2. 기존의 하위 목표들을 모두 지움 (하던 일 중단)
        RemoveAllSubgoals();

        // 3. 새로운 소리 조사 목표를 추가
        AddSubgoal(new Goal_InvestigateNoise(m_pOwner));
    }
}

//-------------------------- Queue Goals --------------------------------------
//-----------------------------------------------------------------------------
void Goal_Think::QueueGoal_MoveToPosition(Vector2D pos)
{
   m_SubGoals.push_back(new Goal_MoveToPosition(m_pOwner, pos));
}



//----------------------- RenderEvaluations -----------------------------------
//-----------------------------------------------------------------------------
void Goal_Think::RenderEvaluations(int left, int top)const
{
  gdi->TextColor(Cgdi::black);
  
  std::vector<Goal_Evaluator*>::const_iterator curDes = m_Evaluators.begin();
  for (curDes; curDes != m_Evaluators.end(); ++curDes)
  {
    (*curDes)->RenderInfo(Vector2D(left, top), m_pOwner);

    left += 75;

    if (left > 400)
    {
        left = 5;       // 다시 왼쪽 끝으로
        top += 12;      // 한 줄 아래로 (글자 높이에 맞춰 조정)
    }
  }
}

void Goal_Think::Render()
{
  std::list<Goal<Raven_Bot>*>::iterator curG;
  for (curG=m_SubGoals.begin(); curG != m_SubGoals.end(); ++curG)
  {
    (*curG)->Render();
  }
}


   
