/*scenario_artifact_restoacqusition*/
#include "PhasingHandler.h"
#include "ScriptedGossip.h"
#include "InstanceScenario.h"
#include "Log.h"
enum
{
    DATA_RESTOACQUSITON = 0,
    DATA_STAGE_1 = 1,
    DATA_STAGE_2 = 2,
    DATA_STAGE_3 = 3,
    DATA_STAGE_4 = 4,
    DATA_STAGE_5 = 5,
    DATA_STAGE_6 = 6,
    DATA_STAGE_7 = 7,
    DATA_MAX_ENCOUNTERS = 1,
    NORMAL_PHASE = 6115,
    QUEST_CLEANSING_THE_MOTHER_TREE = 41689,
};

enum
{
    NPC_SKYLORD_OMNURON_104620 = 104620,
    NPC_ZEN_TABRA_104658 = 104658,
    NPC_CELESTINE_OF_THE_HARVEST_104657 = 104657,
    NPC_ARCHDRUID_HAMUUL_RUNETOTEM_104659 = 104659,
    NPC_LYESSA_BLOOMWATCHER_104628 = 104628,
};

struct scenario_artifact_restoacqusition : public InstanceScript
{
    scenario_artifact_restoacqusition(InstanceMap* map) : InstanceScript(map) {  }

    void Initialize() override
    {
        SetBossNumber(DATA_MAX_ENCOUNTERS);
        SetData(DATA_RESTOACQUSITON, NOT_STARTED);
        for (uint8 i = 1; i < 8; ++i)
            SetData(i, NOT_STARTED);

        isComplete = false;
        StepID = 1;
        stage2count = 0;
    }

    void OnPlayerEnter(Player* player) override
    {
        InstanceScript::OnPlayerEnter(player);
        ///player->GetMapId() == 1599
        if (player->GetMapId() == 1599 && player->HasQuest(QUEST_CLEANSING_THE_MOTHER_TREE))
        {
            PhasingHandler::AddPhase(player, NORMAL_PHASE, true);
            
            if (Creature* lyessa = instance->GetCreature(_lyessaGUID))
                lyessa->Say("Such devastation.", LANG_UNIVERSAL, player);

            if (Creature* omnuron = instance->GetCreature(_omnuronGUID))
                omnuron->Say("$n...thank the earth that you're here.", LANG_UNIVERSAL, player);
        }          
    }

    void OnCreatureCreate(Creature* creature) override
    {
        InstanceScript::OnCreatureCreate(creature);
        switch (creature->GetEntry())
        {
            case NPC_SKYLORD_OMNURON_104620:
            {
                _omnuronGUID = creature->GetGUID();
                creature->SetHealth(creature->GetMaxHealth() * 0.8f);
                break;
            }
            case NPC_ZEN_TABRA_104658:
            {
                _zentabraGUID = creature->GetGUID();
                break;
            }
            case NPC_CELESTINE_OF_THE_HARVEST_104657:
            {
                _cothGUID = creature->GetGUID();
                creature->SetHealth(creature->GetMaxHealth() * 0.15f);
                break;
            }
            case NPC_ARCHDRUID_HAMUUL_RUNETOTEM_104659:
            {
                _hamuulGUID = creature->GetGUID();
                creature->SetHealth(creature->GetMaxHealth() * 0.2f);
                break;
            }
            case NPC_LYESSA_BLOOMWATCHER_104628:
            {
                _lyessaGUID = creature->GetGUID();
                break;
            }
        }   
    }

    void NextStep()
    {
        TC_LOG_ERROR("server.worldserver", "====================scenario_artifact_restoacqusition  NextStep  %u====================== ", StepID);
        if (StepID < DATA_STAGE_7)
        {
            ++StepID;
            if (Scenario* scenario = instance->GetInstanceScenario())
                scenario->CompleteCurrStep();
        }
        else if (StepID == DATA_STAGE_7)
        {
            if (!isComplete)
                if (Scenario* scenario = instance->GetInstanceScenario())
                    scenario->CompleteCurrStep();

            ///COMPLETE SCENARIO
            if (Scenario* scenario = instance->GetInstanceScenario())
                scenario->CompleteScenario();
        }
        
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == DATA_STAGE_1 && data == DONE)
        {
            NextStep();
            if (Creature* lyessa = instance->GetCreature(_lyessaGUID))
                lyessa->AI()->DoAction(1);
            if (Creature* omnuron = instance->GetCreature(_omnuronGUID))
                omnuron->GetMotionMaster()->MoveAwayAndDespawn(15.0f, 3000);
        }
        else if (type == DATA_STAGE_2 && data == DONE)
        {
            ++stage2count;
            if (stage2count >= 3)
            {
                NextStep();
            }
        }
            
    }


private:
    bool isComplete;
    ObjectGuid _omnuronGUID;
    ObjectGuid _zentabraGUID;
    ObjectGuid _cothGUID;
    ObjectGuid _hamuulGUID;
    ObjectGuid _lyessaGUID;
    uint8 StepID;
    uint8 stage2count;
};

struct npc_skylord_omnuron_104620 : public ScriptedAI
{
    npc_skylord_omnuron_104620(Creature* creature) : ScriptedAI(creature) { Initialize(); }

    void DoAction(int32 param)
    {
        switch (param)
        {
        case 1:
            me->Say("The gates of hell descended upon us. They were everywhere...", LANG_UNIVERSAL);

            me->GetScheduler().Schedule(Milliseconds(2000), [this](TaskContext context)
            {
                me->Say("The other's wounds are more severe than my own. They are in dire need of aid.", LANG_UNIVERSAL);
            });
            ///delay 2s next step
            me->GetScheduler().Schedule(Milliseconds(4000), [this](TaskContext context)
            {
                DoAction(2);
            });
            break;
        case 2:
            instance->SetData(DATA_STAGE_1, DONE);
            break;
        }
    }

    void Initialize()
    {
        instance = me->GetInstanceScript();
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/)
    {
        CloseGossipMenuFor(player);
        DoAction(1);
    }
private:
    InstanceScript * instance;
    TaskScheduler _scheduler;
};

struct npc_zen_tabra_104658 : public ScriptedAI
{
    npc_zen_tabra_104658(Creature* creature) : ScriptedAI(creature) { Initialize(); }

    void DoAction(int32 param)
    {
        switch (param)
        {
        case 1:
            break;
        }
    }

    void Initialize()
    {
        instance = me->GetInstanceScript();
        ishaveaura = true;
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (ishaveaura && !me->HasAura(207117))
        {
            ishaveaura = false;
            instance->SetData(DATA_STAGE_2, DONE);
        }
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
private:
    bool ishaveaura;
    InstanceScript * instance;
    TaskScheduler _scheduler;
};

struct npc_celestine_of_the_harvest_104657 : public ScriptedAI
{
    npc_celestine_of_the_harvest_104657(Creature* creature) : ScriptedAI(creature) { Initialize(); }

    void DoAction(int32 param)
    {
        switch (param)
        {
        case 1:
            break;
        }
    }

    void Initialize()
    {
        instance = me->GetInstanceScript();
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
private:
    InstanceScript * instance;
    TaskScheduler _scheduler;
};

struct npc_archdruid_hamuul_runetotem_104659 : public ScriptedAI
{
    npc_archdruid_hamuul_runetotem_104659(Creature* creature) : ScriptedAI(creature) { Initialize(); }

    void DoAction(int32 param)
    {
        switch (param)
        {
        case 1:
            break;
        }
    }

    void Initialize()
    {
        instance = me->GetInstanceScript();
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
private:
    InstanceScript * instance;
    TaskScheduler _scheduler;
};


struct npc_lyessa_bloomwatcher_104628 : public ScriptedAI
{
    npc_lyessa_bloomwatcher_104628(Creature* creature) : ScriptedAI(creature) { Initialize(); }

    void DoAction(int32 param)
    {
        switch (param)
        {
        case 1:
            me->Say("Skylord,you must inform the Dreamgrove.We will make sure no more life is lost this day", LANG_UNIVERSAL);
            //move to first position
            me->GetMotionMaster()->MovePoint(1, Position(5475.65f, -3488.561f, 1555.676f), true);
            break;
        }
    }

    void Initialize()
    {
        instance = me->GetInstanceScript();
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
private:
    InstanceScript * instance;
    TaskScheduler _scheduler;
};

void AddSC_scenario_artifact_restoacqusition()
{
    RegisterInstanceScript(scenario_artifact_restoacqusition, 1599);

    RegisterCreatureAI(npc_skylord_omnuron_104620);
    RegisterCreatureAI(npc_zen_tabra_104658);
    RegisterCreatureAI(npc_celestine_of_the_harvest_104657);
    RegisterCreatureAI(npc_archdruid_hamuul_runetotem_104659);
    RegisterCreatureAI(npc_lyessa_bloomwatcher_104628);
}
