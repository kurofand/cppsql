#include "mysqlClient.hpp"

//#include "mysql_connection.h"
//#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

MySQLClient::MySQLClient(const char* host, const char* userName, const char* pass, const char* dbName)
{
	this->host=host;
	this->userName=userName;
	this->password=pass;
	this->dbName=dbName;
}

bool MySQLClient::connect()
{
	this->driver=get_driver_instance();
	try{
//собираем строку для коннекта к заданной базе
		std::string buf(this->host);
		buf.append("/");
		buf.append(this->dbName);
		this->con=driver->connect(buf, this->userName, this->password);
		return true;
	}
	catch(const sql::SQLException &e){
		std::cout<<"Connection was not estabilished!:"<<std::endl<<e.what()<<std::endl;
		return false;
	}
}

std::vector<std::string>* MySQLClient::executeQuery(const char* query)
{
	std::vector<std::string> *res;
//ACHTUNG!! тут меморилик засчёт создания нового и не удаления его. когда-нибудь я должен буду это исправить на умные поинтеры
	res=new std::vector<std::string>();
	sql::Statement *stmt;
	stmt=this->con->createStatement();
	//stmt->execute(query);
	std::string queryType;
	uint8_t i=0;
//компилятор страшно ругался что не можно сравнивать поинтер и целое(я в принципе его понимаю, но был уверен что элемент поинтер массива рассматривается не как поинтер, а как целое), поэтому его пришлось обмануть. оно работает, но думается мне что может отвалиться в любой момент
	while(query[i]!=*" ")
		queryType+=query[i++];
	if(queryType=="SELECT")
	{
		sql::ResultSet *stmtRes;
		stmtRes=stmt->executeQuery(query);
		while(stmtRes->next())
		{
			uint8_t j=0;
			std::string rowStr;
			while(true)
				try{
					rowStr.append(stmtRes->getString(++j));
					rowStr.append("|");
				}
				catch(...){
					res->push_back(rowStr);
					break;
				}
//		std::cout<<"before push_back"<<std::endl;
//		res->push_back(stmtRes->getString("name").c_str());

//		res->push_back(stmtRes->getString("name"));

//		std::cout<<stmtRes->getString("name").c_str()<<std::endl;
//		std::cout<<res->back()<<std::endl;
//		std::cout<<"after push_back"<<std::endl;
		}
		delete stmtRes;
	}
	else
		stmt->execute(query);

	delete stmt;
	return res;
}

/*void MySQLClient::executeQuery(const char* query, std::vector<const char*>* resVec)
{
	sql::Statement *stmt;
	sql::ResultSet *stmtRes;
	stmt=this->con->createStatement();
	stmtRes=stmt->executeQuery(query);
	while(stmtRes->next())
		resVec->push_back(stmtRes->getString("name").c_str());
	delete stmt;
	delete stmtRes;
}*/

void MySQLClient::closeConnection()
{
	this->con->close();
	if(this->con->isValid())
		std::cout<<"Close connection failed!"<<std::endl;
}

MySQLClient::~MySQLClient()
{
	delete this->con;
}
