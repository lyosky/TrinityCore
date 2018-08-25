/*
scenario_7.0_artifact_brokenshore
lyosky
*/
#include "Log.h"
#include "Scenario.h"
#include "GameObject.h"
#include "InstanceScenario.h"
#include "PhasingHandler.h"
#include "SceneMgr.h"
#include "ScriptMgr.h"

enum DataTypes
{
    DATA_CLAWS_OF_URSOC = 0,
    DATA_MAX_ENCOUNTERS = 1,

    SCENARIO_ID_WARRIOR = 961,
    PHASE_WARRIOR = 6815,
    QUEST__RETURN_TO_THE_BROKEN_SHORE = 38904,
    DATA_STEP_1 = 1939,
    DATA_STEP_2 = 2327,
    DATA_STEP_3 = 2030,
    DATA_STEP_4 = 2033,
    DATA_STEP_5 = 2034,
    DATA_STEP_6 = 2035,
    DATA_STEP_7 = 2040,
    DATA_STEP_8 = 2041,

    EVENT_STEP_1 = 1,
    EVENT_STEP_2 = 2,
    EVENT_STEP_3 = 3,
    EVENT_STEP_4 = 4,
    EVENT_STEP_5 = 5,
    EVENT_STEP_6 = 6,
    EVENT_STEP_7 = 7,
    EVENT_STEP_8 = 8,
    SCENE_PLAYER_ENTER = 1177,
    SPELL_SCENE_MALGALOR_ARRIVES = 197593, ///SceneID: 1144     PlaybackFlags : 25    SceneInstanceID : 1    SceneScriptPackageID : 1515
};

struct scenario_artifact_brokenshore : public InstanceScript
{
    scenario_artifact_brokenshore(InstanceMap* map) : InstanceScript(map) { }

    void Initialize() override
    {
        SetBossNumber(DATA_MAX_ENCOUNTERS);
        //events.ScheduleEvent(DATA_STEP_1, 3s);
        SetData(DATA_CLAWS_OF_URSOC, NOT_STARTED);
        isIntr = false;
        needKillCount = 0;
        KillCount = 0;
        StepID = EVENT_STEP_1;
        isComplete = false;
    }

    void OnPlayerAreaUpdate(Player* player, uint32 newAreaId, uint32 /*oldAreaId*/)
    {
        if (newAreaId == 7796 && player->HasQuest(QUEST__RETURN_TO_THE_BROKEN_SHORE) && player->getClass() == CLASS_WARRIOR)
        {
            PhasingHandler::AddPhase(player, PHASE_WARRIOR);
            SendScenarioState(ScenarioData(SCENARIO_ID_WARRIOR, DATA_STEP_1));
            ///SMSG_SET_DUNGEON_DIFFICULTY 12
        }
    }

private:
    EventMap events;
    bool isIntr;
    ObjectGuid GUID_SKYLORD_SHADEGROVE;
    ObjectGuid GUID_CYLINDER_COLLISION;
    std::set<ObjectGuid> m_guardMobs;
    std::set<ObjectGuid> m_targetMobs;
    uint32 needKillCount;
    uint32 KillCount;
    uint8 StepID;
    bool isComplete;
};

///SceneID: 1144     PlaybackFlags : 25    SceneInstanceID : 1    SceneScriptPackageID : 1515
class scene_malgalor_arrives : public SceneScript
{
public:
    scene_malgalor_arrives() : SceneScript("scene_malgalor_arrives") { }

    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        ///SUMMON MALGALOR 92608 ON Position: X: -2211.66 Y: -1215.43 Z: 62.9837
    }
};

void AddSC_scenario_artifact_brokenshore()
{
    RegisterInstanceScript(scenario_artifact_brokenshore, 1500);
    new scene_malgalor_arrives();
}
