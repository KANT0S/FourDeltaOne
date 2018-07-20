-- Prosody IM
-- Copyright (C) 2008-2010 Matthew Wild
-- Copyright (C) 2008-2010 Waqas Hussain
-- 
-- This project is MIT/X11 licensed. Please see the
-- COPYING file in the source package for more information.
--




local log = require "util.logger".init("rostermanager");

local setmetatable = setmetatable;
local format = string.format;
local loadfile, setfenv, pcall = loadfile, setfenv, pcall;
local pairs, ipairs = pairs, ipairs;
local tostring = tostring;

local hosts = hosts;
local bare_sessions = bare_sessions;

local datamanager = require "util.datamanager"
local st = require "util.stanza";

local DBI = require "DBI"

local connection;
local resolve_relative_path = require "core.configmanager".resolve_relative_path;

local params = { driver = "MySQL", database = "xnp", username = "xnp", password = "ShKtrV8ykuAL3QUeGrFGT7Ln", host = "fourdeltaone.net" };

module "rostermanager"

local function test_connection()
	if not connection then return nil; end
	if connection:ping() then
		return true;
	else
		log("debug", "Database connection closed");
		connection = nil;
	end
end
local function connect()
	if not test_connection() then
		local dbh, err = DBI.Connect(
			params.driver, params.database,
			params.username, params.password,
			params.host, params.port
		);
		if not dbh then
			log("debug", "Database connection failed: %s", tostring(err));
			return nil, err;
		end
		log("debug", "Successfully connected to database");
		dbh:autocommit(true); -- don't run in transaction
		connection = dbh;
		return connection;
	end
end

do -- process options to get a db connection
	params = params or { driver = "SQLite3" };
	
	if params.driver == "SQLite3" then
		params.database = resolve_relative_path(prosody.paths.data or ".", params.database or "prosody.sqlite");
	end
	
	--assert(params.driver and params.database, "Both the SQL driver and the database need to be specified");
	
	--assert(connect());
	
	connect();
end

local function getsql(sql, ...)
	if params.driver == "PostgreSQL" then
		sql = sql:gsub("`", "\"");
	end
	if not test_connection() then connect(); end
	-- do prepared statement stuff
	local stmt, err = connection:prepare(sql);
	if not stmt and not test_connection() then error("connection failed"); end
	if not stmt then log("error", "QUERY FAILED: %s %s", err, debug.traceback()); return nil, err; end
	-- run query
	local ok, err = stmt:execute(...);
	if not ok and not test_connection() then error("connection failed"); end
	if not ok then return nil, err; end
	
	return stmt;
end
local function setsql(sql, ...)
	local stmt, err = getsql(sql, ...);
	if not stmt then return stmt, err; end
	return stmt:affected();
end

function add_to_roster(session, jid, item)
	return nil, "cancel", "not-authorized", "This feature is not supported.";
--[[
	if session.roster then
		local old_item = session.roster[jid];
		session.roster[jid] = item;
		if save_roster(session.username, session.host) then
			return true;
		else
			session.roster[jid] = old_item;
			return nil, "wait", "internal-server-error", "Unable to save roster";
		end
	else
		return nil, "auth", "not-authorized", "Session's roster not loaded";
	end]]
end

function remove_from_roster(session, jid)
	return nil, "cancel", "not-authorized", "This feature is not supported.";

--[[	if session.roster then
		local old_item = session.roster[jid];
		session.roster[jid] = nil;
		if save_roster(session.username, session.host) then
			return true;
		else
			session.roster[jid] = old_item;
			return nil, "wait", "internal-server-error", "Unable to save roster";
		end
	else
		return nil, "auth", "not-authorized", "Session's roster not loaded";
	end]]
end

function roster_push(username, host, jid)
	local roster = jid and jid ~= "pending" and hosts[host] and hosts[host].sessions[username] and hosts[host].sessions[username].roster;
	if roster then
		local item = hosts[host].sessions[username].roster[jid];
		local stanza = st.iq({type="set"});
		stanza:tag("query", {xmlns = "jabber:iq:roster", ver = tostring(roster[false].version or "1")  });
		if item then
			stanza:tag("item", {jid = jid, subscription = item.subscription, name = item.name, ask = item.ask});
			for group in pairs(item.groups) do
				stanza:tag("group"):text(group):up();
			end
		else
			stanza:tag("item", {jid = jid, subscription = "remove"});
		end
		stanza:up(); -- move out from item
		stanza:up(); -- move out from stanza
		-- stanza ready
		for _, session in pairs(hosts[host].sessions[username].sessions) do
			if session.interested then
				-- FIXME do we need to set stanza.attr.to?
				session.send(stanza);
			end
		end
	end
end

function load_roster(username, host)
	--[[local jid = username.."@"..host;
	log("debug", "load_roster: asked for: "..jid);
	local user = bare_sessions[jid];
	local roster;
	if user then
		roster = user.roster;
		if roster then return roster; end
		log("debug", "load_roster: loading for new user: "..username.."@"..host);
	else -- Attempt to load roster for non-loaded user
		log("debug", "load_roster: loading for offline user: "..username.."@"..host);
	end
	local data, err = datamanager.load(username, host, "roster");
	roster = data or {};
	if user then user.roster = roster; end
	if not roster[false] then roster[false] = { broken = err or nil }; end
	if roster[jid] then
		roster[jid] = nil;
		log("warn", "roster for "..jid.." has a self-contact");
	end
	if not err then
		hosts[host].events.fire_event("roster-load", username, host, roster);
	end
	return roster, err;]]
	
	local jid = username.."@"..host;
	log("debug", "load_roster: asked for: "..jid);
	
	--local stmt, err = getsql("SELECT f.friend_id, u.username FROM `xnp`.`friends` f, `aiw2`.`phpbb_users` u WHERE f.user_id = ? AND f.friend_id = u.user_id", username);
	--local stmt, err = getsql("SELECT f.zebra_id, u.username FROM `aiw2`.`phpbb_zebra` f, `aiw2`.`phpbb_users` u WHERE f.user_id = ? AND f.zebra_id = u.user_id AND f.friend = 1 AND f.approval = 0", username);
	local stmt, err = getsql("SELECT f.friend_id, u.username FROM `xnp`.`friends` f, `phpbb3_4d1`.`phpbb_users` u WHERE f.user_id = ? AND f.friend_id = u.user_id", username);
	
	if not stmt then
		log("warn", err);
		return {}, err;
	end
	
	local roster;
	
	local user = bare_sessions[jid];
	if user then
		roster = user.roster;
		if roster then return roster; end
		log("debug", "load_roster: loading for new user: "..username.."@"..host);
	else -- Attempt to load roster for non-loaded user
		log("debug", "load_roster: loading for offline user: "..username.."@"..host);
	end
	
	roster = {};
	
	for row in stmt:rows(true) do
		--log("debug", "friend " .. row.zebra_id .. " named " .. row.username);
		
		roster[row.friend_id .. "@fourdeltaone.net"] = {
			["subscription"] = "both";
			["name"] = row.username;
			["groups"] = {
				["Friends"] = true;
			};
		};
	end
	
	if user then user.roster = roster; end
	if not roster[false] then roster[false] = { broken = err or nil }; end
	
	return roster, err
end

function save_roster(username, host, roster)
	log("debug", "save_roster: saving roster for "..username.."@"..host);
	if not roster then
		roster = hosts[host] and hosts[host].sessions[username] and hosts[host].sessions[username].roster;
		--if not roster then
		--	--roster = load_roster(username, host);
		--	return true; -- roster unchanged, no reason to save
		--end
	end
	if roster then
		local metadata = roster[false];
		if not metadata then
			metadata = {};
			roster[false] = metadata;
		end
		if metadata.version ~= true then
			metadata.version = (metadata.version or 0) + 1;
		end
		if roster[false].broken then return nil, "Not saving broken roster" end
		return datamanager.store(username, host, "roster", roster);
	end
	log("warn", "save_roster: user had no roster to save");
	return nil;
end

function process_inbound_subscription_approval(username, host, jid)
	--[[local roster = load_roster(username, host);
	local item = roster[jid];
	if item and item.ask then
		if item.subscription == "none" then
			item.subscription = "to";
		else -- subscription == from
			item.subscription = "both";
		end
		item.ask = nil;
		return save_roster(username, host, roster);
	end]]
end

function process_inbound_subscription_cancellation(username, host, jid)
	--[[local roster = load_roster(username, host);
	local item = roster[jid];
	local changed = nil;
	if is_contact_pending_out(username, host, jid) then
		item.ask = nil;
		changed = true;
	end
	if item then
		if item.subscription == "to" then
			item.subscription = "none";
			changed = true;
		elseif item.subscription == "both" then
			item.subscription = "from";
			changed = true;
		end
	end
	if changed then
		return save_roster(username, host, roster);
	end]]
end

function process_inbound_unsubscribe(username, host, jid)
	--[[local roster = load_roster(username, host);
	local item = roster[jid];
	local changed = nil;
	if is_contact_pending_in(username, host, jid) then
		roster.pending[jid] = nil; -- TODO maybe delete roster.pending if empty?
		changed = true;
	end
	if item then
		if item.subscription == "from" then
			item.subscription = "none";
			changed = true;
		elseif item.subscription == "both" then
			item.subscription = "to";
			changed = true;
		end
	end
	if changed then
		return save_roster(username, host, roster);
	end]]
end

local function _get_online_roster_subscription(jidA, jidB)
	local user = bare_sessions[jidA];
	local item = user and (user.roster[jidB] or { subscription = "none" });
	return item and item.subscription;
end
function is_contact_subscribed(username, host, jid)
	do
		local selfjid = username.."@"..host;
		local subscription = _get_online_roster_subscription(selfjid, jid);
		if subscription then return (subscription == "both" or subscription == "from"); end
		local subscription = _get_online_roster_subscription(jid, selfjid);
		if subscription then return (subscription == "both" or subscription == "to"); end
	end
	local roster, err = load_roster(username, host);
	local item = roster[jid];
	return item and (item.subscription == "from" or item.subscription == "both"), err;
end

function is_contact_pending_in(username, host, jid)
	local roster = load_roster(username, host);
	return roster.pending and roster.pending[jid];
end
function set_contact_pending_in(username, host, jid, pending)
	local roster = load_roster(username, host);
	local item = roster[jid];
	if item and (item.subscription == "from" or item.subscription == "both") then
		return; -- false
	end
	if not roster.pending then roster.pending = {}; end
	roster.pending[jid] = true;
	return save_roster(username, host, roster);
end
function is_contact_pending_out(username, host, jid)
	local roster = load_roster(username, host);
	local item = roster[jid];
	return item and item.ask;
end
function set_contact_pending_out(username, host, jid) -- subscribe
	local roster = load_roster(username, host);
	local item = roster[jid];
	if item and (item.ask or item.subscription == "to" or item.subscription == "both") then
		return true;
	end
	if not item then
		item = {subscription = "none", groups = {}};
		roster[jid] = item;
	end
	item.ask = "subscribe";
	log("debug", "set_contact_pending_out: saving roster; set "..username.."@"..host..".roster["..jid.."].ask=subscribe");
	return save_roster(username, host, roster);
end
function unsubscribe(username, host, jid)
	local roster = load_roster(username, host);
	local item = roster[jid];
	if not item then return false; end
	if (item.subscription == "from" or item.subscription == "none") and not item.ask then
		return true;
	end
	item.ask = nil;
	if item.subscription == "both" then
		item.subscription = "from";
	elseif item.subscription == "to" then
		item.subscription = "none";
	end
	return save_roster(username, host, roster);
end
function subscribed(username, host, jid)
	if is_contact_pending_in(username, host, jid) then
		local roster = load_roster(username, host);
		local item = roster[jid];
		if not item then -- FIXME should roster item be auto-created?
			item = {subscription = "none", groups = {}};
			roster[jid] = item;
		end
		if item.subscription == "none" then
			item.subscription = "from";
		else -- subscription == to
			item.subscription = "both";
		end
		roster.pending[jid] = nil;
		-- TODO maybe remove roster.pending if empty
		return save_roster(username, host, roster);
	end -- TODO else implement optional feature pre-approval (ask = subscribed)
end
function unsubscribed(username, host, jid)
	local roster = load_roster(username, host);
	local item = roster[jid];
	local pending = is_contact_pending_in(username, host, jid);
	local changed = nil;
	if is_contact_pending_in(username, host, jid) then
		roster.pending[jid] = nil; -- TODO maybe delete roster.pending if empty?
		changed = true;
	end
	if item then
		if item.subscription == "from" then
			item.subscription = "none";
			changed = true;
		elseif item.subscription == "both" then
			item.subscription = "to";
			changed = true;
		end
	end
	if changed then
		return save_roster(username, host, roster);
	end
end

function process_outbound_subscription_request(username, host, jid)
	local roster = load_roster(username, host);
	local item = roster[jid];
	if item and (item.subscription == "none" or item.subscription == "from") then
		item.ask = "subscribe";
		return save_roster(username, host, roster);
	end
end

--[[function process_outbound_subscription_approval(username, host, jid)
	local roster = load_roster(username, host);
	local item = roster[jid];
	if item and (item.subscription == "none" or item.subscription == "from" then
		item.ask = "subscribe";
		return save_roster(username, host, roster);
	end
end]]



return _M;
