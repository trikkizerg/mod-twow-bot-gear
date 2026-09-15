#include "botpch.h"

#include "GearRefresh.h"
#include "BotManager.h"

#include "ScriptObjects.h"
#include "Chat/Chat.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Group/Group.h"
#include "Log.h"

#include <cctype>
#include <string>

namespace ModBotGear
{
    namespace
    {
        std::string ToLower(std::string s)
        {
            for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            return s;
        }

        void ReplyTo(ChatHandler* handler, const char* msg)
        {
            if (handler) handler->SendSysMessage(msg);
        }

        void RefreshOne(Player* bot, ChatHandler* handler, const char* who)
        {
            if (!bot)
            {
                ReplyTo(handler, "No such bot.");
                return;
            }
            if (!TortoiseBots::BotManager::Instance().IsBot(bot->GetObjectGuid()))
            {
                ReplyTo(handler, "That character is not a managed bot.");
                return;
            }
            if (GearRefresh::TryRefresh(bot, "command", true))
            {
                std::string out = std::string("Re-geared ") + who + ".";
                ReplyTo(handler, out.c_str());
            }
            else
            {
                std::string out = std::string(who) + " already geared for this band (or ineligible).";
                ReplyTo(handler, out.c_str());
            }
        }
    }

    // Handles `.bgi [name|all]`
    class GearCommandScript final : public AllCommandScript
    {
    public:
        GearCommandScript() : AllCommandScript("mod_twow_bot_gear_cmd") {}

        bool CanExecuteCommand(ChatHandler* handler, char const* command, char const* args) override
        {
            if (!command)
                return true;

            std::string name = ToLower(command);
            if (name != "bgi")
                return true;

            Handle(handler, args ? args : "");
            return false;   // consumed
        }

    private:
        void Handle(ChatHandler* handler, const char* args)
        {
            if (!handler)
                return;

            Player* requester = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
            if (!requester)
            {
                ReplyTo(handler, ".bgi must be used in-game.");
                return;
            }

            std::string arg(args);
            // trim
            while (!arg.empty() && (arg[0] == ' ' || arg[0] == '\t')) arg.erase(0, 1);
            while (!arg.empty() && (arg.back() == ' ' || arg.back() == '\t')) arg.pop_back();

            if (arg.empty())
            {
                // Scope: target if it's a bot, else self if self is a bot
                Player* target = requester->GetMap() ? nullptr : nullptr;   // placeholder
                // Try the requester's current selection
                if (requester->GetSelectionGuid())
                    target = sObjectAccessor.FindPlayer(requester->GetSelectionGuid());
                if (!target || !TortoiseBots::BotManager::Instance().IsBot(target->GetObjectGuid()))
                    target = requester;   // fallback to self if self is a bot

                RefreshOne(target, handler, target ? target->GetName() : "target");
                return;
            }

            if (ToLower(arg) == "all")
            {
                // Iterate the requester's party and gear each bot
                Group* g = requester->GetGroup();
                if (!g)
                {
                    // No group: try self + target
                    if (TortoiseBots::BotManager::Instance().IsBot(requester->GetObjectGuid()))
                        RefreshOne(requester, handler, requester->GetName());
                    else
                        ReplyTo(handler, "Not in a group.");
                    return;
                }

                uint32 count = 0;
                for (GroupReference* itr = g->GetFirstMember(); itr; itr = itr->next())
                {
                    Player* m = itr->getSource();
                    if (!m) continue;
                    if (!TortoiseBots::BotManager::Instance().IsBot(m->GetObjectGuid())) continue;
                    if (GearRefresh::TryRefresh(m, "command", true)) count++;
                }
                std::string out = "Re-geared " + std::to_string(count) + " bot(s).";
                ReplyTo(handler, out.c_str());
                return;
            }

            // Named bot
            Player* target = sObjectAccessor.FindPlayerByName(arg.c_str());
            RefreshOne(target, handler, arg.c_str());
        }
    };
}

void AddModBotGearCommandScript()
{
    new ModBotGear::GearCommandScript();
}
