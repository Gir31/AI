#ifndef INVESTIGATE_NOISE_GOAL_EVALUATOR_H
#define INVESTIGATE_NOISE_GOAL_EVALUATOR_H
#pragma warning (disable:4786)

#include "Goal_Evaluator.h"
#include "../Raven_Bot.h"

class InvestigateNoiseGoal_Evaluator : public Goal_Evaluator
{
public:
	InvestigateNoiseGoal_Evaluator(double bias) :Goal_Evaluator(bias) {}
	double CalculateDesirability(Raven_Bot* pBot);
	void  SetGoal(Raven_Bot* pBot);
	void RenderInfo(Vector2D Position, Raven_Bot* pBot);
};

#endif