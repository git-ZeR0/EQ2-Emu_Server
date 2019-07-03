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

#ifndef EQEMU_DATABASE_H
#define EQEMU_DATABASE_H

#include <string>

#define EQEMU_MYSQL_ENABLED

/**
* Base database class, intended to be extended.
*/
class Database {
public:
	Database() : user(""), pass(""), host(""), port(""), name("") {}
	virtual ~Database() {}

	virtual bool IsConnected() { return false; }

	/**
	 * Retrieves the login data (password hash and account id) from the account name provided needed for client login procedure
	 *
	 * @param name
	 * @param loginserver
	 * @param password
	 * @param id
	 * @return
	 */
	virtual bool GetLoginDataFromAccountInfo(
		const std::string &name,
		const std::string &loginserver,
		std::string &password,
		unsigned int &id
	) { return false; }

	virtual bool GetLoginTokenDataFromToken(
		const std::string &token,
		const std::string &ip,
		unsigned int &db_account_id,
		std::string &db_loginserver,
		std::string &user
	) { return false; }

	virtual bool CreateLoginData(
		const std::string &name,
		const std::string &password,
		const std::string &loginserver,
		unsigned int &id
	) { return false; }

	virtual bool CreateLoginDataWithID(
		const std::string &name,
		const std::string &password,
		const std::string &loginserver,
		unsigned int id
	) { return false; }

	virtual void UpdateLoginHash(const std::string &name, const std::string &loginserver, const std::string &hash) {}

	/**
	 * Retrieves the world registration from the long and short names provided
	 * Needed for world login procedure
	 * Returns true if the record was found, false otherwise
	 *
	 * @param long_name
	 * @param short_name
	 * @param id
	 * @param desc
	 * @param list_id
	 * @param trusted
	 * @param list_desc
	 * @param account
	 * @param password
	 * @return
	 */
	virtual bool GetWorldRegistration(
		std::string long_name,
		std::string short_name,
		unsigned int &id,
		std::string &desc,
		unsigned int &list_id,
		unsigned int &trusted,
		std::string &list_desc,
		std::string &account,
		std::string &password
	) { return false; }

	virtual void UpdateLSAccountData(unsigned int id, std::string ip_address) {}

	/**
	 * Updates or creates the login server account with info from world server
	 *
	 * @param id
	 * @param name
	 * @param password
	 * @param email
	 */
	virtual void UpdateLSAccountInfo(unsigned int id, std::string name, std::string password, std::string email) {}

	/**
	 * Updates the ip address of the world with account id = id
	 *
	 * @param id
	 * @param long_name
	 * @param ip_address
	 */
	virtual void UpdateWorldRegistration(unsigned int id, std::string long_name, std::string ip_address) {}

	/**
	 * Creates new world registration for unregistered servers and returns new id
	 *
	 * @param long_name
	 * @param short_name
	 * @param id
	 * @return
	 */
	virtual bool CreateWorldRegistration(std::string long_name, std::string short_name, unsigned int &id) { return false; }
protected:
	std::string user, pass, host, port, name;
};

#endif

