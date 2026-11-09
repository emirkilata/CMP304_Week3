#include "AIConstructor_BT.h"
#include "AIActor_Guard.h"

bool AIConstructor_BT::Init() {


	return true;
}


/*
DefineActions creates all the possible actions that the AI can perform
*/
void AIConstructor_BT::DefineActions()
{



	// ** Patrol Action **

	//Define the function for Patrolling
	auto patrolFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus {
		AIActor_Guard* actor = static_cast<AIActor_Guard*>(bb.GetActorContext());

		// Call behaviour functions here
		// return the result of the action
		return actor->Patrol();


		};


	// Link the function to an identifier
	AddActionByName("ActionPatrol", patrolFunction);


	// ** Get Patrol Path Action **

	//Define the function for  Get Patrol Path 
	auto getPatrolPathFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus {
		AIActor_Guard* actor = static_cast<AIActor_Guard*>(bb.GetActorContext());

		// Call behaviour functions here
		// return the result of the action
		return actor->GetPatrolPath();



		};


	// Link the function to an identifier
	AddActionByName("ActionGetPatrolPath", getPatrolPathFunction);




	auto restFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus
		{
			AIActor_Guard* actor =
				static_cast<AIActor_Guard*>(bb.GetActorContext());

			return actor->Rest();
		};

	AddActionByName("ActionRest", restFunction);



	auto chaseFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus
		{
			AIActor_Guard* actor = static_cast<AIActor_Guard*>(bb.GetActorContext());
			return actor->Chase();
		};

	AddActionByName("ActionChase", chaseFunction);



	auto getInvestigatePathFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus
		{
			AIActor_Guard* actor = static_cast<AIActor_Guard*>(bb.GetActorContext());
			return actor->GetInvestigatePath();
		};

	AddActionByName("ActionGetInvestigatePath", getInvestigatePathFunction);

	auto investigateFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus
		{
			AIActor_Guard* actor = static_cast<AIActor_Guard*>(bb.GetActorContext());
			return actor->Investigate();
		};

	AddActionByName("ActionInvestigate", investigateFunction);

}

void AIConstructor_BT::DefineConsiderations()
{
	// Example Consideration - Can see an enemy?
	auto cCanSeeEnemy = [](AIBrainBlackboardBase& bb) -> bool {

		bool enemySeen = (bb.GetValue<int>("CanSeePlayer") == 1);
		return(enemySeen);

		};


	AddConsiderationByName("ConsiderationSeePlayer", cCanSeeEnemy);


	auto cNeedsRest = [](AIBrainBlackboardBase& bb) -> bool
		{
			return bb.GetValue<float>("Energy") <= 0.0f;
		};

	AddConsiderationByName("ConsiderationNeedsRest", cNeedsRest);

	auto cHasEnergy = [](AIBrainBlackboardBase& bb) -> bool
		{
			return bb.GetValue<float>("Energy") > 0.0f;
		};

	AddConsiderationByName("ConsiderationHasEnergy", cHasEnergy);


	auto cCannotSeePlayer = [](AIBrainBlackboardBase& bb) -> bool
		{
			return bb.GetValue<int>("CanSeePlayer") == 0;
		};

	AddConsiderationByName("ConsiderationCannotSeePlayer", cCannotSeePlayer);

	auto cCanHearPlayer = [](AIBrainBlackboardBase& bb) -> bool
		{
			return bb.GetValue<int>("CanHearPlayer") == 1;
		};

	AddConsiderationByName("ConsiderationHearPlayer", cCanHearPlayer);

	auto cCannotHearPlayer = [](AIBrainBlackboardBase& bb) -> bool
		{
			return bb.GetValue<int>("CanHearPlayer") == 0;
		};

	AddConsiderationByName("ConsiderationCannotHearPlayer", cCannotHearPlayer);
	
}


/*
* DefineOptions links the Options with Considerations and Actions
*/
void AIConstructor_BT::DefineOptions()
{


	// Set the Node Type for the Root Node
	rootType = AIReasonerBase::NodeType::Fallback;

	// - LEAF NODES - ACTIONS - 
	// Add any leaf nodes (actions) using AddOptionByName()
	

	AddOptionByName("OptionGetPatrolPath", "ActionGetPatrolPath");
	AddOptionByName("OptionPatrol", "ActionPatrol");
	AddOptionByName("OptionRest", "ActionRest");
	AddOptionByName("OptionChase", "ActionChase");
	AddOptionByName("OptionGetInvestigatePath", "ActionGetInvestigatePath");
	AddOptionByName("OptionInvestigate", "ActionInvestigate");




	


	// - CONTROL NODES - SUB REASONERS - 
	// Add any Control Nodes using AddControlNodeByName()
	// You do not need to create the Root, that node is already created (called "Root")

	AddControlNodeByName("OptionPatrolSeq", AIReasonerBase::Sequence);
	AddControlNodeByName("OptionRestSeq", AIReasonerBase::Sequence);
	AddControlNodeByName("OptionRestDecorator", AIReasonerBase::Decorator);
	AddControlNodeByName("OptionChaseSeq", AIReasonerBase::Sequence);
	AddControlNodeByName("OptionChaseDecorator", AIReasonerBase::Decorator);
	AddControlNodeByName("OptionInvestigateSeq", AIReasonerBase::Sequence);
	AddControlNodeByName("OptionInvestigateDecorator", AIReasonerBase::Decorator);



	AddDecoratorConsideration("OptionRestDecorator", "ConsiderationNeedsRest");
	AddOptionConsideration("OptionPatrol", "ConsiderationHasEnergy");
	AddDecoratorConsideration("OptionChaseDecorator", "ConsiderationSeePlayer");
	AddOptionConsideration("OptionPatrol", "ConsiderationCannotSeePlayer");
	AddOptionConsideration("OptionChase", "ConsiderationSeePlayer");
	AddOptionConsideration("OptionChase", "ConsiderationHasEnergy");
	AddDecoratorConsideration("OptionInvestigateDecorator", "ConsiderationHearPlayer");
	AddOptionConsideration("OptionInvestigate", "ConsiderationHasEnergy");
	AddOptionConsideration("OptionInvestigate", "ConsiderationCannotSeePlayer");
	AddOptionConsideration("OptionPatrol", "ConsiderationCannotHearPlayer");





	// Root: priority order.
	AddOptionToTreeNode("Root", "OptionRestSeq");
	AddOptionToTreeNode("Root", "OptionChaseSeq");
	AddOptionToTreeNode("Root", "OptionInvestigateSeq");
	AddOptionToTreeNode("Root", "OptionPatrolSeq");

	// Rest sequence.
	AddOptionToTreeNode("OptionRestSeq", "OptionRestDecorator");
	AddOptionToTreeNode("OptionRestSeq", "OptionRest");

	// Chase sequence.
	AddOptionToTreeNode("OptionChaseSeq", "OptionChaseDecorator");
	AddOptionToTreeNode("OptionChaseSeq", "OptionChase");

	// Investigate sequence.
	AddOptionToTreeNode("OptionInvestigateSeq", "OptionInvestigateDecorator");
	AddOptionToTreeNode("OptionInvestigateSeq", "OptionGetInvestigatePath");
	AddOptionToTreeNode("OptionInvestigateSeq", "OptionInvestigate");

	// Patrol sequence.
	AddOptionToTreeNode("OptionPatrolSeq", "OptionGetPatrolPath");
	AddOptionToTreeNode("OptionPatrolSeq", "OptionPatrol");








}




















// ** Helper Functions **
// these link to the AI Framework and should not be edited 



void AIConstructor_BT::AddControlNodeByName(std::string _subReasonerName, AIReasonerBase::NodeType _nodeType, int _priority)
{



	// create the Action - a SubReasoner Actions
	std::shared_ptr<AIActionSubReasoner> _subReasoner = std::make_shared<AIActionSubReasoner>();

	_subReasoner->Init(_subReasonerName, nullptr);
	_subReasoner->SetReasonerType(_nodeType);
	actions.insert({ _subReasonerName, _subReasoner });

	

	// create the option, and link to the Action
	AIOptionDefinition _option = AIOptionDefinition();
	_option.SetOptionName(_subReasonerName);
	_option.SetActionName(_subReasonerName);
	_option.SetPriority(_priority);
	options.insert({ _subReasonerName, _option });


}



void AIConstructor_BT::AddDecoratorConsideration(std::string _decorator, std::string  _consideration) {

	decoratorConditions.insert({ _decorator, _consideration,  });


}

std::vector< std::shared_ptr<AIConsiderationBase>> AIConstructor_BT::GetConsiderationsForKey(std::string _decorator)
{

	std::vector< std::shared_ptr<AIConsiderationBase>> _matches;
	for (std::multimap<std::string, std::string>::iterator it = decoratorConditions.begin(); it != decoratorConditions.end(); ++it) {

		if (it->first == _decorator)
		{
			_matches.push_back(considerations[it->second]);
		}
	}

	return _matches;
}