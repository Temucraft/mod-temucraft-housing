/*
 * mod-temucraft-housing: guild houses. Every location in mod_temucraft_housing_location (world) can belong to one
 * guild. The guild master buys a free one at the Housing Steward (creature 608802) for Temucraft.Housing.Price; guild
 * members then travel there from the steward or with .temucraft guildhouse (every Temucraft.Housing.Cooldown seconds, never
 * in combat, battlegrounds, arenas or instances). The guild master can sell the house back for
 * Temucraft.Housing.RefundPercent of the price. A guild that disbands loses its house.
 *
 * Text comes from module_string (module "mod-temucraft-housing"), with English built in as a fallback.
 */
#include "Chat.h"
#include "ChatCommand.h"
#include "Config.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "StringFormat.h"
#include "TemucraftPlayerSettings.h"
#include "WorldSession.h"
#include <map>
#include <unordered_map>

using namespace Acore::ChatCommands;

namespace
{
    constexpr char const* MODULE = "mod-temucraft-housing";
    constexpr uint32 PAGE = 20;

    enum Str : uint32
    {
        S_GO = 1, S_BUY_LIST, S_SELL, S_NO_GUILD, S_NOT_MASTER, S_HAS_HOUSE, S_TAKEN, S_BOUGHT, S_SOLD, S_NO_MONEY, S_NO_HOUSE,
        S_COOLDOWN, S_NOT_HERE, S_NEXT, S_BUY_CONFIRM, S_SELL_CONFIRM, S_DISABLED, S_INFO
    };
    std::unordered_map<uint32, char const*> const FALLBACK =
    {
        { S_GO,           "Take me to our guild house ({})." },
        { S_BUY_LIST,     "Show me the houses for sale." },
        { S_SELL,         "Sell our guild house." },
        { S_NO_GUILD,     "You need to be in a guild." },
        { S_NOT_MASTER,   "Only the guild master can buy or sell the guild house." },
        { S_HAS_HOUSE,    "Your guild already owns a house." },
        { S_TAKEN,        "Another guild bought that house first." },
        { S_BOUGHT,       "Your guild now owns: {}." },
        { S_SOLD,         "The guild house is sold; {} returned." },
        { S_NO_MONEY,     "A guild house costs {}." },
        { S_NO_HOUSE,     "Your guild has no house." },
        { S_COOLDOWN,     "You can travel to the guild house again in {} minutes." },
        { S_NOT_HERE,     "You can't travel to the guild house from here or right now." },
        { S_NEXT,         "More houses..." },
        { S_BUY_CONFIRM,  "Buy {} for your guild?" },
        { S_SELL_CONFIRM, "Sell your guild house for {}?" },
        { S_DISABLED,     "Guild houses are turned off." },
        { S_INFO,         "Guild house: {}" },
    };
    std::string Text(Player* player, uint32 id)
    {
        if (sObjectMgr->GetModuleString(MODULE, id))
            if (std::string const* s = player->GetSession()->GetModuleString(MODULE, id))
                return *s;
        auto it = FALLBACK.find(id);
        return it != FALLBACK.end() ? it->second : "";
    }
    template<typename... Args>
    std::string Text(Player* player, uint32 id, Args&&... args) { return Acore::StringFormat(Text(player, id), std::forward<Args>(args)...); }

    struct Settings { bool enable = true; uint32 price = 10000000; uint32 refund = 50; uint32 cooldown = 1800; uint32 steward = 608802, text = 608802; } cfg;

    struct Location { uint32 id; uint32 map; float x, y, z, o; std::string name; };
    std::map<uint32, Location> locations;                       // id -> location
    std::unordered_map<uint32, uint32> houseOf;                 // guild id -> location id
    std::unordered_map<ObjectGuid, time_t> lastTravel;

    std::string Money(uint32 copper) { return std::to_string(copper / 10000) + "g"; }

    void Load()
    {
        locations.clear(); houseOf.clear();
        if (QueryResult r = WorldDatabase.Query("SELECT id, map, x, y, z, o, name FROM mod_temucraft_housing_location ORDER BY id"))
            do
            {
                Field* f = r->Fetch();
                locations[f[0].Get<uint32>()] = { f[0].Get<uint32>(), f[1].Get<uint32>(), f[2].Get<float>(), f[3].Get<float>(), f[4].Get<float>(),
                                                  f[5].Get<float>(), f[6].Get<std::string>() };
            } while (r->NextRow());
        if (QueryResult r = CharacterDatabase.Query("SELECT guild_id, location_id FROM mod_temucraft_housing"))
            do houseOf[r->Fetch()[0].Get<uint32>()] = r->Fetch()[1].Get<uint32>(); while (r->NextRow());
    }

    Location const* HouseOf(Player* player)
    {
        auto it = houseOf.find(player->GetGuildId());
        if (it == houseOf.end()) return nullptr;
        auto loc = locations.find(it->second);
        return loc != locations.end() ? &loc->second : nullptr;
    }

    bool IsTaken(uint32 locationId)
    {
        for (auto const& [guild, loc] : houseOf) if (loc == locationId) return true;
        return false;
    }

    bool IsMaster(Player* player)
    {
        Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId());
        return guild && guild->GetLeaderGUID() == player->GetGUID();
    }

    bool Travel(Player* player)
    {
        Location const* loc = HouseOf(player);
        if (!loc) { ChatHandler(player->GetSession()).SendSysMessage(Text(player, S_NO_HOUSE)); return false; }
        if (player->IsInCombat() || player->InBattleground() || player->InArena() || player->GetMap()->Instanceable() || player->IsInFlight())
        {
            ChatHandler(player->GetSession()).SendSysMessage(Text(player, S_NOT_HERE)); return false;
        }
        time_t now = GameTime::GetGameTime().count();
        auto& last = lastTravel[player->GetGUID()];
        if (!player->IsGameMaster() && last && now - last < time_t(cfg.cooldown))
        {
            ChatHandler(player->GetSession()).SendSysMessage(Text(player, S_COOLDOWN, (cfg.cooldown - (now - last) + 59) / 60)); return false;
        }
        last = now;
        player->TeleportTo(loc->map, loc->x, loc->y, loc->z, loc->o);
        return true;
    }

    enum : uint32 { ACT_GO = 1, ACT_LIST = 2, ACT_SELL = 3, ACT_PAGE = 1000, ACT_BUY = 100000 };

    void ShowMain(Player* player, Creature* creature)
    {
        ClearGossipMenuFor(player);
        if (!player->GetGuildId()) { ChatHandler(player->GetSession()).SendSysMessage(Text(player, S_NO_GUILD)); CloseGossipMenuFor(player); return; }
        if (Location const* loc = HouseOf(player))
        {
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, Text(player, S_GO, loc->name), GOSSIP_SENDER_MAIN, ACT_GO,
                             TemucraftTeleportBox(player, loc->name), 0, false);   // "Teleport to: ..." unless turned off
            if (IsMaster(player))
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, Text(player, S_SELL), GOSSIP_SENDER_MAIN, ACT_SELL,
                                 Text(player, S_SELL_CONFIRM, Money(cfg.price * cfg.refund / 100)), 0, false);
        }
        else if (IsMaster(player))
            AddGossipItemFor(player, GOSSIP_ICON_TABARD, Text(player, S_BUY_LIST), GOSSIP_SENDER_MAIN, ACT_PAGE);
        SendGossipMenuFor(player, cfg.text, creature->GetGUID());
    }

    void ShowList(Player* player, Creature* creature, uint32 page)
    {
        ClearGossipMenuFor(player);
        uint32 index = 0, shown = 0;
        for (auto const& [id, loc] : locations)
        {
            if (IsTaken(id)) continue;
            if (index++ < page * PAGE) continue;
            if (shown++ == PAGE)
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, Text(player, S_NEXT), GOSSIP_SENDER_MAIN, ACT_PAGE + page + 1);
                break;
            }
            AddGossipItemFor(player, GOSSIP_ICON_TABARD, loc.name, GOSSIP_SENDER_MAIN, ACT_BUY + id, Text(player, S_BUY_CONFIRM, loc.name), cfg.price, false);
        }
        SendGossipMenuFor(player, cfg.text, creature->GetGUID());
    }

    void Buy(Player* player, uint32 locationId)
    {
        ChatHandler h(player->GetSession());
        if (!IsMaster(player)) { h.SendSysMessage(Text(player, S_NOT_MASTER)); return; }
        if (HouseOf(player)) { h.SendSysMessage(Text(player, S_HAS_HOUSE)); return; }
        auto loc = locations.find(locationId);
        if (loc == locations.end() || IsTaken(locationId)) { h.SendSysMessage(Text(player, S_TAKEN)); return; }
        if (!player->HasEnoughMoney(cfg.price)) { h.SendSysMessage(Text(player, S_NO_MONEY, Money(cfg.price))); return; }
        player->ModifyMoney(-int32(cfg.price));
        houseOf[player->GetGuildId()] = locationId;
        CharacterDatabase.Execute("REPLACE INTO mod_temucraft_housing (guild_id, location_id) VALUES ({}, {})", player->GetGuildId(), locationId);
        h.SendSysMessage(Text(player, S_BOUGHT, loc->second.name));
    }

    void Sell(Player* player)
    {
        ChatHandler h(player->GetSession());
        if (!IsMaster(player)) { h.SendSysMessage(Text(player, S_NOT_MASTER)); return; }
        if (!HouseOf(player)) { h.SendSysMessage(Text(player, S_NO_HOUSE)); return; }
        houseOf.erase(player->GetGuildId());
        CharacterDatabase.Execute("DELETE FROM mod_temucraft_housing WHERE guild_id = {}", player->GetGuildId());
        uint32 refund = cfg.price * cfg.refund / 100;
        player->ModifyMoney(int32(refund));
        h.SendSysMessage(Text(player, S_SOLD, Money(refund)));
    }
}

class TemucraftHousing_Creature : public AllCreatureScript
{
public:
    TemucraftHousing_Creature() : AllCreatureScript("TemucraftHousing_Creature") { }

    bool CanCreatureGossipHello(Player* player, Creature* creature) override
    {
        if (creature->GetEntry() != cfg.steward) return false;
        if (!cfg.enable) { ChatHandler(player->GetSession()).SendSysMessage(Text(player, S_DISABLED)); return true; }
        ShowMain(player, creature);
        return true;
    }

    bool CanCreatureGossipSelect(Player* player, Creature* creature, uint32, uint32 action) override
    {
        if (creature->GetEntry() != cfg.steward) return false;
        if (action == ACT_GO) { CloseGossipMenuFor(player); Travel(player); }
        else if (action == ACT_SELL) { CloseGossipMenuFor(player); Sell(player); }
        else if (action >= ACT_BUY) { CloseGossipMenuFor(player); Buy(player, action - ACT_BUY); }
        else if (action >= ACT_PAGE) ShowList(player, creature, action - ACT_PAGE);
        else ShowMain(player, creature);
        return true;
    }
};

class TemucraftHousing_Guild : public GuildScript
{
public:
    TemucraftHousing_Guild() : GuildScript("TemucraftHousing_Guild", { GUILDHOOK_ON_DISBAND }) { }

    void OnDisband(Guild* guild) override
    {
        houseOf.erase(guild->GetId());
        CharacterDatabase.Execute("DELETE FROM mod_temucraft_housing WHERE guild_id = {}", guild->GetId());
    }
};

class TemucraftHousing_Command : public CommandScript
{
public:
    TemucraftHousing_Command() : CommandScript("TemucraftHousing_Command") { }
    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable sub = {
            { "",       HandleGo,     100850,        Console::No },
            { "reload", HandleReload, 100801 /* SEC_ADMINISTRATOR -> RBAC */, Console::Yes },
        };
        static ChatCommandTable table = { { "temucraft guildhouse", sub } };
        return table;
    }
    // .temucraft guildhouse: travel to your guild's house
    static bool HandleGo(ChatHandler* handler)
    {
        if (!cfg.enable) { handler->SendSysMessage(Text(handler->GetPlayer(), S_DISABLED)); return true; }
        Travel(handler->GetPlayer());
        return true;
    }
    // .temucraft guildhouse reload: reload locations and owners
    static bool HandleReload(ChatHandler* handler) { Load(); handler->PSendSysMessage("{} guild house locations, {} owned.", locations.size(), houseOf.size()); return true; }
};

class TemucraftHousing_World : public WorldScript
{
public:
    TemucraftHousing_World() : WorldScript("TemucraftHousing_World", { WORLDHOOK_ON_AFTER_CONFIG_LOAD, WORLDHOOK_ON_STARTUP }) { }
    void OnAfterConfigLoad(bool) override
    {
        cfg.enable = sConfigMgr->GetOption<bool>("Temucraft.Housing.Enable", true);
        cfg.price = sConfigMgr->GetOption<uint32>("Temucraft.Housing.Price", 10000000);
        cfg.refund = std::min<uint32>(sConfigMgr->GetOption<uint32>("Temucraft.Housing.RefundPercent", 50), 100);
        cfg.cooldown = sConfigMgr->GetOption<uint32>("Temucraft.Housing.Cooldown", 1800);
        cfg.steward = sConfigMgr->GetOption<uint32>("Temucraft.Housing.Steward", 608802);
        cfg.text = sConfigMgr->GetOption<uint32>("Temucraft.Housing.Text", 608802);
    }
    void OnStartup() override { Load(); }
};

void AddTemucraftHousingScripts()
{
    new TemucraftHousing_Creature();
    new TemucraftHousing_Guild();
    new TemucraftHousing_Command();
    new TemucraftHousing_World();
}
