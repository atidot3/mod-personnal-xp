#ifndef AZEROTHCORE_PERSONNALXPMODE_H
#define AZEROTHCORE_PERSONNALXPMODE_H

#include "ScriptMgr.h"
#include "Configuration/Config.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "Player.h"
#include "DataMap.h"

class PersonnalXpConf : public WorldScript
{
public:
    PersonnalXpConf();
    void OnBeforeConfigLoad(bool /*reload*/) override;
};

class PlayerXpRate : public DataMap::Base
{
public:
    PlayerXpRate() {}
    PlayerXpRate(uint32 XPRate) : XPRate(XPRate) {}
    uint32 XPRate = 1.0f;
};

class PersonnalXP : public PlayerScript
{
public:
    PersonnalXP();
    void OnLogin(Player* player) override;
    void OnGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override;
};

class PersonnalXpMode
{
public:
    struct PersonnalXpModule
    {
        bool Enabled, AnnounceModule, AnnounceRatesOnLogin;
        uint32 MaxRate, DefaultRate;
    };

    static PersonnalXpMode* instance();
    PersonnalXpConf& GetConf() {return _conf;}
    PersonnalXpModule& GetModule() {return _mod;}

private:
    PersonnalXpConf  _conf;
    PersonnalXpModule _mod;
};

#define sPersonnalXpMode PersonnalXpMode::instance()

#endif //AZEROTHCORE_PERSONNALXPMODE_H
