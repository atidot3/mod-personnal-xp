/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "PersonnalXp.h"

#include "ScriptMgr.h"
#include "GameObjectAI.h"
#include "ScriptedGossip.h"

PersonnalXpMode* PersonnalXpMode::instance()
{
    static PersonnalXpMode instance;
    return &instance;
}

IndividualXPConf::IndividualXPConf() : WorldScript("PersonnalXpConf")
{

}
void IndividualXPConf::OnBeforeConfigLoad(bool /*reload*/) /*override*/
{
    sPersonnalXpMode->GetModule().Enabled = sConfigMgr->GetOption<bool>("PersonnalXp.Enabled", true);
    sPersonnalXpMode->GetModule().AnnounceModule = sConfigMgr->GetOption<bool>("PersonnalXp.Announce", true);
    sPersonnalXpMode->GetModule().AnnounceRatesOnLogin = sConfigMgr->GetOption<bool>("PersonnalXp.AnnounceRatesOnLogin", true);
    sPersonnalXpMode->GetModule().MaxRate = sConfigMgr->GetOption<float>("PersonnalXp.MaxXPRate", 5.0f);
    sPersonnalXpMode->GetModule().DefaultRate = sConfigMgr->GetOption<float>("PersonnalXp.DefaultXPRate", 1.0f);
}

PersonnalXP::PersonnalXP()
    : PlayerScript("PersonnalXP")
{

}

void PersonnalXP::OnLogin(Player* player) /*override*/
{
    QueryResult result = CharacterDatabase.Query("SELECT `XPRate` FROM `personnalxp` WHERE `CharacterGUID`='{}'", player->GetGUID().GetCounter());

    if (!result)
    {
        player->CustomData.GetDefault<PlayerXpRate>("PersonnalXP")->XPRate = sPersonnalXpMode->GetModule().DefaultRate;
    }
    else
    {
        Field* fields = result->Fetch();
        player->CustomData.Set("PersonnalXP", new PlayerXpRate(fields[0].Get<float>()));
    }

    if (sPersonnalXpMode->GetModule().Enabled)
    {
        // Announce Module
        if (sPersonnalXpMode->GetModule().AnnounceModule)
        {
            ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00PersonnalXPRate |rmodule.");
        }

        // Announce Rates
        if (sPersonnalXpMode->GetModule().AnnounceRatesOnLogin)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffffff[XP] Your current XP rate is {}.|r", player->CustomData.GetDefault<PlayerXpRate>("PersonnalXP")->XPRate);
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffffff[XP] The maximum rate limit is 5.|r", sPersonnalXpMode->GetModule().MaxRate);
        }
    }
}
void PersonnalXP::OnLogout(Player* player) /*override*/
{
    if (PlayerXpRate* data = player->CustomData.Get<PlayerXpRate>("PersonnalXP"))
    {
        CharacterDatabase.DirectExecute("REPLACE INTO `personnalxp` (`CharacterGUID`, `XPRate`) VALUES ('{}', '{}');", player->GetGUID().GetCounter(), data->XPRate);
    }
}
void PersonnalXP::OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/, uint8 /*xpSource*/) /*override*/
{
    if (sPersonnalXpMode->GetModule().Enabled)
    {
        if (PlayerXpRate* data = player->CustomData.Get<PlayerXpRate>("PersonnalXP"))
        {
            amount = static_cast<uint32>(std::round(static_cast<float>(amount) * data->XPRate));
        }
    }
}

class gobject_personnalxp_modes : public GameObjectScript
{
public:
    gobject_personnalxp_modes() : GameObjectScript("gobject_personnalxp_modes") { }

    struct gobject_personnalxp_modeAI: GameObjectAI
    {
        explicit gobject_personnalxp_modeAI(GameObject* object) : GameObjectAI(object) { };
    };


    //New menu System
    bool OnGossipHello(Player* player, GameObject* go) override
    {   
        // Function to add challenge items to the menu
        auto addChallengeMenuItem = [player](const float& rates, const std::string& name)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, name, 0, rates);
        };

        addChallengeMenuItem(1, "Reset XPRates to 1");
        addChallengeMenuItem(3, "Set XPRates to 3");
        addChallengeMenuItem(5, "Set XPRates to 5");

        // Display the menu
        SendGossipMenuFor(player, 12669, go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 action) override
    {
        std::vector<uint32> rates = {
            1, 3, 5
        };
        if (action > rates.size())
        {
            return false;
        }

        // apply xp rates <action>
        player->CustomData.GetDefault<PlayerXpRate>("PersonnalXP")->XPRate = action;
        ChatHandler(player->GetSession()).PSendSysMessage("[XP] You have updated your XP rate to {}.", action);

        return true;
    }

    GameObjectAI* GetAI(GameObject* object) const override
    {
        return new gobject_personnalxp_modeAI(object);
    }
};

void AddPersonnalXPScripts()
{
    new gobject_personnalxp_modes();
}