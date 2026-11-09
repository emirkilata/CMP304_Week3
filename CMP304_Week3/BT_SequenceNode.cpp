#include "BT_SequenceNode.h"
#include "AIConstructor_BT.h"


#include "BT_DecoratorNode.h"
#include "BT_FallbackNode.h"
#include <iostream>



/*
* Think should check of the conditions (if any) for this node are met, and set the status accordingly
*/
void BT_SequenceNode::Think()
{
	ActionStatus _currentStatus = GetStatus();
	if (_currentStatus == ActionStatus::ACTION_IDLE)
	{
		Start();
	}
	else if (_currentStatus == ActionStatus::ACTION_RUNNING)
	{

		// select the current running index, but check if any of the conditions are met
		selectedOption = options[runningOptionIndex].get();

		std::vector<std::shared_ptr<AIConsiderationBase>> conditionList = selectedOption->GetOptionConsiderations();



		bool conditionsMet = true;
		for (auto c : conditionList)
		{
			bool ruleResult = c->Calculate(*actorBlackboard);

			if (!ruleResult)
			{
				std::cout << "[" << actorBlackboard->GetActorContext()->GetActorID() << "] BT Log: Sequence Node - " << selectedOption->GetOptionID() << " Consideration " << c->GetID() <<" was not TRUE " << std::endl;
				conditionsMet = false;
			}
		}

		if (!conditionsMet)
		{
			
			SetStatus(ActionStatus::ACTION_FAILURE);
		}
	}


}


/*
* Act should maintin the status tracking of the child nodes and handle their reporting of sucess and failure
*/
ActionStatus BT_SequenceNode::Act()
{


	if (selectedOption != nullptr)
	{


		// IF GetStatus() is FAILURE, the node conditions have failed, and so the Node should stop and report failure to it's parent

		if (GetStatus() == ActionStatus::ACTION_FAILURE)
		{
			std::cout << "[" << actorBlackboard->GetActorContext()->GetActorID() << "] BT Log: SequenceNode " << reasonerID << " Action has Failed due to Option " << std::endl;
			return GetStatus();
		}
		else
		{


			ActionStatus _actionResult =
				selectedOption->GetOptionAction()->PerformAction(*actorBlackboard);

			if (_actionResult == ActionStatus::ACTION_SUCCESS)
			{
				if (runningOptionIndex == options.size() - 1)
				{
					std::cout
						<< "[" << actorBlackboard->GetActorContext()->GetActorID()
						<< "] BT Log: Sequence Node "
						<< reasonerID
						<< " Complete - Success"
						<< std::endl;

					SetStatus(ActionStatus::ACTION_SUCCESS);
				}
				else
				{
					runningOptionIndex++;

					std::cout
						<< "[" << actorBlackboard->GetActorContext()->GetActorID()
						<< "] BT Log: Sequence Node "
						<< reasonerID
						<< " moving to child index "
						<< runningOptionIndex
						<< std::endl;

					SetStatus(ActionStatus::ACTION_RUNNING);
				}
			}
			else if (_actionResult == ActionStatus::ACTION_FAILURE)
			{
				std::cout
					<< "[" << actorBlackboard->GetActorContext()->GetActorID()
					<< "] BT Log: Sequence Node "
					<< reasonerID
					<< " failed because of "
					<< selectedOption->GetOptionID()
					<< std::endl;

				SetStatus(ActionStatus::ACTION_FAILURE);
			}
			else if (_actionResult == ActionStatus::ACTION_RUNNING)
			{
				SetStatus(ActionStatus::ACTION_RUNNING);
			}

	


		}
	

	}


	// return GetStatus
	return GetStatus();
}

void BT_SequenceNode::Reset()
{

	BT_ReasonerNode::Reset();
	runningOptionIndex = 0;
	selectedOption = nullptr;
	SetStatus(ActionStatus::ACTION_IDLE);
}



