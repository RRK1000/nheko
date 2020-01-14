#pragma once

#include <QDateTime>
#include <QImage>
#include <QString>

#include <string>

#include <mtx/events/join_rules.hpp>

struct RoomMember
{
        QString user_id;
        QString display_name;
        QImage avatar;
};

struct SearchResult
{
        QString user_id;
        QString display_name;
};

//! Used to uniquely identify a list of read receipts.
struct ReadReceiptKey
{
        std::string event_id;
        std::string room_id;
};

void
to_json(json &j, const ReadReceiptKey &key);

void
from_json(const json &j, ReadReceiptKey &key);

struct DescInfo
{
        QString event_id;
        QString userid;
        QString body;
        QString timestamp;
        QDateTime datetime;
};

//! UI info associated with a room.
struct RoomInfo
{
        //! The calculated name of the room.
        std::string name;
        //! The topic of the room.
        std::string topic;
        //! The calculated avatar url of the room.
        std::string avatar_url;
        //! The calculated version of this room set at creation time.
        std::string version;
        //! Whether or not the room is an invite.
        bool is_invite = false;
        //! Total number of members in the room.
        int16_t member_count = 0;
        //! Who can access to the room.
        mtx::events::state::JoinRule join_rule = mtx::events::state::JoinRule::Public;
        bool guest_access                      = false;
        //! Metadata describing the last message in the timeline.
        DescInfo msgInfo;
        //! The list of tags associated with this room
        std::vector<std::string> tags;
};

void
to_json(json &j, const RoomInfo &info);
void
from_json(const json &j, RoomInfo &info);

//! Basic information per member;
struct MemberInfo
{
        std::string name;
        std::string avatar_url;
};

void
to_json(json &j, const MemberInfo &info);
void
from_json(const json &j, MemberInfo &info);

struct RoomSearchResult
{
        std::string room_id;
        RoomInfo info;
};