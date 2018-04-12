#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <string>
#include <vector>

class MySQLClient
{
	public:
		MySQLClient(const char* host, const char* userName, const char* pass, const char* dbName);
		~MySQLClient();
		bool connect();
		void setDb(const char* dbName);
		void closeConnection();
		std::vector<std::string>* executeQuery(const char* query);
	private:
		const char* host;
		const char* userName;
		const char* password;
		const char* dbName;
		sql::Driver *driver;
		sql::Connection *con;
};
