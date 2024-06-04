#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <string>
#include <vector>
#include <fstream>

class MySQLClient
{
	public:
		MySQLClient(const char* host, const char* userName, const char* pass, const char* dbName);
		MySQLClient(const char* fileName);
		bool connect();
		void setDb(const char* dbName);
		void closeConnection();
		void executeQuery(const char* query, std::vector<std::string>& resVec);
		~MySQLClient();
	private:
		std::string host;
		std::string userName;
		std::string password;
		std::string dbName;
		sql::Driver *driver;
		sql::Connection *con;
};
