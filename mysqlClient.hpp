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
		void executeQuery(const char* query, std::vector<std::string>& resVec);
	private:
		const char* host;
		const char* userName;
		const char* password;
		const char* dbName;
		sql::Driver *driver;
		sql::Connection *con;
};
