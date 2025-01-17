#ifndef AZEROTHCORE_PERSONNALXPMODE_H
#define AZEROTHCORE_PERSONNALXPMODE_H

#include "ScriptMgr.h"
#include "Configuration/Config.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "Player.h"
#include "Object.h"
#include "DataMap.h"

class IndividualXPConf : public WorldScript
{
public:
    IndividualXPConf();
    void OnBeforeConfigLoad(bool /*reload*/) override;
};

class PlayerXpRate : public DataMap::Base
{
public:
    PlayerXpRate() {}
    PlayerXpRate(float XPRate) : XPRate(XPRate) {}
    float XPRate = 1.0f;
};

class PersonnalXP : public PlayerScript
{
public:
    PersonnalXP();
    void OnLogin(Player* player) override;
    void OnLogout(Player* player) override;
    void OnGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override;
};

class PersonnalXpMode
{
public:
    struct PersonnalXpModule
    {
        bool Enabled, AnnounceModule, AnnounceRatesOnLogin;
        float MaxRate, DefaultRate;
    };

    static PersonnalXpMode* instance();
    IndividualXPConf& GetConf() {return _conf;}
    PersonnalXpModule& GetModule() {return _mod;}

private:
    IndividualXPConf  _conf;
    PersonnalXpModule _mod;
};

#define sPersonnalXpMode PersonnalXpMode::instance()

#endif //AZEROTHCORE_PERSONNALXPMODE_H
