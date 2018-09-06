/*scenario_artifact_restoacqusition*/
#include "PhasingHandler.h"
enum
{
    DATA_RESTOACQUSITON = 0,
    DATA_MAX_ENCOUNTERS = 1,
    NORMAL_PHASE = 6115,
};
struct scenario_artifact_restoacqusition : public InstanceScript
{
    scenario_artifact_restoacqusition(InstanceMap* map) : InstanceScript(map) {  }

    void Initialize() override
    {
        SetBossNumber(DATA_MAX_ENCOUNTERS);
        SetData(DATA_RESTOACQUSITON, NOT_STARTED);
        isComplete = false;
    }

    void OnPlayerEnter(Player* player) override
    {
        if (player->GetMapId() == 1599)
            PhasingHandler::AddPhase(player, NORMAL_PHASE, true);
    }
    bool isComplete;
};

void AddSC_scenario_artifact_restoacqusition()
{
    RegisterInstanceScript(scenario_artifact_restoacqusition, 1599);
}
