#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/resultset.h>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <any>

class MySQLClient
{
	public:
		MySQLClient(const char* host, const char* userName, const char* pass, const char* dbName);
		MySQLClient(const char* fileName);
		bool connect();
		void setDb(const char* dbName);
		void closeConnection();
		void executeQuery(const char* query, std::vector<std::map<std::string, std::string>> &resVec);
		void executePreparedStatement(const char* query, std::vector<std::map<std::string, std::string>>& resVec, const std::vector<std::any> &params);
		~MySQLClient();
	private:
		bool isSelect(const char *query);
		void getColNamesFromQuery(const char *query, std::vector<std::string> &names);
		void manageResult(const char *query, std::vector<std::map<std::string, std::string>> &resVec, sql::ResultSet *resSet);
		std::string host;
		std::string userName;
		std::string password;
		std::string dbName;
		sql::Driver *driver;
		sql::Connection *con;
};
