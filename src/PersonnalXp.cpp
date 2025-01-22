/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "PersonnalXp.h"

#include "ScriptMgr.h"
#include "CreatureAI.h"
#include "ScriptedGossip.h"

PersonnalXpMode* PersonnalXpMode::instance()
{
    static PersonnalXpMode instance;
    return &instance;
}

PersonnalXpConf::PersonnalXpConf() : WorldScript("PersonnalXpConf")
{

}
void PersonnalXpConf::OnBeforeConfigLoad(bool /*reload*/) /*override*/
{
    sPersonnalXpMode->GetModule().Enabled = sConfigMgr->GetOption<bool>("PersonnalXp.Enabled", true);
    sPersonnalXpMode->GetModule().AnnounceModule = sConfigMgr->GetOption<bool>("PersonnalXp.Announce", true);
    sPersonnalXpMode->GetModule().AnnounceRatesOnLogin = sConfigMgr->GetOption<bool>("PersonnalXp.AnnounceRatesOnLogin", true);
    sPersonnalXpMode->GetModule().MaxRate = sConfigMgr->GetOption<float>("PersonnalXp.MaxXPRate", 5);
    sPersonnalXpMode->GetModule().DefaultRate = sConfigMgr->GetOption<float>("PersonnalXp.DefaultXPRate", 1);
}

PersonnalXP::PersonnalXP()
    : PlayerScript("PersonnalXP")
{

}

void PersonnalXP::OnLogin(Player* player) /*override*/
{
    auto personnalRate = player->GetPlayerSetting("PersonnalXP", 0).value;
    if (personnalRate == 0)
    {
        player->UpdatePlayerSetting("PersonnalXP", 0, sPersonnalXpMode->GetModule().DefaultRate);
        personnalRate = sPersonnalXpMode->GetModule().DefaultRate;
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
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffffff[XP] Your current XP rate is {}.|r", personnalRate);
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffffff[XP] The maximum rate limit is {}.|r", sPersonnalXpMode->GetModule().MaxRate);
        }
    }
}

void PersonnalXP::OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/, uint8 /*xpSource*/) /*override*/
{
    if (sPersonnalXpMode->GetModule().Enabled)
    {
        if (uint32 multiplier = player->GetPlayerSetting("PersonnalXP", 0).value)
        {
            amount = static_cast<uint32>(std::round(static_cast<float>(amount) * multiplier));
        }
    }
}

class creature_personnalxp_mode : public CreatureScript
{
public:
    creature_personnalxp_mode() : CreatureScript("creature_personnalxp_mode") { }

    //New menu System
    bool OnGossipHello(Player* player, Creature* entity) override
    {
        // Function to add challenge items to the menu
        auto addChallengeMenuItem = [player](const float& rates, const std::string& name)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, name, 0, rates);
        };

        addChallengeMenuItem(0, "Disable XP, set rate to 0");
        for (auto index = 1; index < sPersonnalXpMode->GetModule().MaxRate; ++index)
        {
            addChallengeMenuItem(index, "Set my xp rates to " + std::to_string(index));
        }
        // Display the menu
        SendGossipMenuFor(player, entity->GetEntry(), entity->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*entity*/, uint32 /*sender*/, uint32 action) override
    {
        // safe protection
        if (action != 0 && (action > sPersonnalXpMode->GetModule().MaxRate || action < sPersonnalXpMode->GetModule().DefaultRate))
        {
            return false;
        }

        // apply xp rates <action>
        player->PlayDirectSound(12867);
        player->UpdatePlayerSetting("PersonnalXP", 0, action);
        ChatHandler(player->GetSession()).PSendSysMessage("[XP] You have updated your XP rate to {}.", action);

        CloseGossipMenuFor(player);

        return true;
    }

};

void AddPersonnalXPScripts()
{
    new creature_personnalxp_mode();
    new PersonnalXpConf();
    new PersonnalXP();
}
