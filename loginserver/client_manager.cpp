/**
 * EQEmulator: Everquest Server Emulator
 * Copyright (C) 2001-2019 EQEmulator Development Team (https://github.com/EQEmu/Server)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY except by those people which sell it, which
 * are required to give you total support for your newly bought product;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "client_manager.h"
#include "login_server.h"

extern LoginServer server;
extern bool        run_server;

#include "../common/eqemu_logsys.h"
#include "../common/eqemu_logsys_fmt.h"

ClientManager::ClientManager()
{
	int                             titanium_port = server.config.GetVariableInt("Titanium", "port", 5998);
	EQStreamManagerInterfaceOptions titanium_opts(titanium_port, false, false);
	titanium_stream = new EQ::Net::EQStreamManager(titanium_opts);
	titanium_ops    = new RegularOpcodeManager;
	if (!titanium_ops->LoadOpcodes(
		server.config.GetVariableString(
			"Titanium",
			"opcodes",
			"login_opcodes.conf"
		).c_str())) {
		Log(Logs::General, Logs::Error, "ClientManager fatal error: couldn't load opcodes for Titanium file %s.",
			server.config.GetVariableString("Titanium", "opcodes", "login_opcodes.conf").c_str());
		run_server = false;
	}

	titanium_stream->OnNewConnection(
		[this](std::shared_ptr<EQ::Net::EQStream> stream) {
			LogF(Logs::General,
				 Logs::Login_Server,
				 "New Titanium client connection from {0}:{1}",
				 stream->GetRemoteIP(),
				 stream->GetRemotePort());
			stream->SetOpcodeManager(&titanium_ops);
			Client *c = new Client(stream, cv_titanium);
			clients.push_back(c);
		}
	);

	int                             sod_port = server.config.GetVariableInt("SoD", "port", 5999);
	EQStreamManagerInterfaceOptions sod_opts(sod_port, false, false);
	sod_stream = new EQ::Net::EQStreamManager(sod_opts);
	sod_ops    = new RegularOpcodeManager;
	if (!sod_ops->LoadOpcodes(server.config.GetVariableString("SoD", "opcodes", "login_opcodes.conf").c_str())) {
		Log(Logs::General, Logs::Error, "ClientManager fatal error: couldn't load opcodes for SoD file %s.",
			server.config.GetVariableString("SoD", "opcodes", "login_opcodes.conf").c_str());
		run_server = false;
	}

	sod_stream->OnNewConnection(
		[this](std::shared_ptr<EQ::Net::EQStream> stream) {
			LogF(Logs::General,
				 Logs::Login_Server,
				 "New SoD client connection from {0}:{1}",
				 stream->GetRemoteIP(),
				 stream->GetRemotePort());
			stream->SetOpcodeManager(&sod_ops);
			Client *c = new Client(stream, cv_sod);
			clients.push_back(c);
		}
	);
}

ClientManager::~ClientManager()
{
	if (titanium_stream) {
		delete titanium_stream;
	}

	if (titanium_ops) {
		delete titanium_ops;
	}

	if (sod_stream) {
		delete sod_stream;
	}

	if (sod_ops) {
		delete sod_ops;
	}
}

void ClientManager::Process()
{
	ProcessDisconnect();

	auto iter = clients.begin();
	while (iter != clients.end()) {
		if ((*iter)->Process() == false) {
			Log(Logs::General, Logs::Debug, "Client had a fatal error and had to be removed from the login.");
			delete (*iter);
			iter = clients.erase(iter);
		}
		else {
			++iter;
		}
	}
}

void ClientManager::ProcessDisconnect()
{
	auto iter = clients.begin();
	while (iter != clients.end()) {
		std::shared_ptr<EQStreamInterface> c = (*iter)->GetConnection();
		if (c->CheckState(CLOSED)) {
			Log(Logs::General, Logs::Login_Server, "Client disconnected from the server, removing client.");
			delete (*iter);
			iter = clients.erase(iter);
		}
		else {
			++iter;
		}
	}
}

void ClientManager::RemoveExistingClient(unsigned int account_id, const std::string &loginserver)
{
	auto iter = clients.begin();
	while (iter != clients.end()) {
		if ((*iter)->GetAccountID() == account_id && (*iter)->GetLoginServerName().compare(loginserver) == 0) {
			Log(Logs::General,
				Logs::Login_Server,
				"Client attempting to log in and existing client already logged in, removing existing client.");
			delete (*iter);
			iter = clients.erase(iter);
		}
		else {
			++iter;
		}
	}
}

Client *ClientManager::GetClient(unsigned int account_id, const std::string &loginserver)
{
	auto iter = clients.begin();
	while (iter != clients.end()) {
		if ((*iter)->GetAccountID() == account_id && (*iter)->GetLoginServerName().compare(loginserver) == 0) {
			return (*iter);
		}
		++iter;
	}

	return nullptr;
}
