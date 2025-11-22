#ifndef INVESTIGATE_NOISE_GOAL_EVALUATOR_H
#define INVESTIGATE_NOISE_GOAL_EVALUATOR_H
#pragma warning (disable:4786)

#include "Goal_Evaluator.h"
#include "../Raven_Bot.h"

class InvestigateNoiseGoal_Evaluator : public Goal_Evaluator
{
public:

	// 생성자: 부모 클래스(Goal_Evaluator)에 성격 편향값(bias)을 넘겨줘야 합니다.
	InvestigateNoiseGoal_Evaluator(double bias) :Goal_Evaluator(bias) {}

	// 필수 구현 함수 1: 점수 계산
	double CalculateDesirability(Raven_Bot* pBot);

	// 필수 구현 함수 2: 목표 설정
	void  SetGoal(Raven_Bot* pBot);

	// 필수 구현 함수 3: 디버그 정보 표시
	void RenderInfo(Vector2D Position, Raven_Bot* pBot);
};

#endif