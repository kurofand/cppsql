#include "mysqlClient.hpp"

#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <format>

MySQLClient::MySQLClient(const char* host, const char* userName, const char* pass, const char* dbName)
{
	this->host=host;
	this->userName=userName;
	this->password=pass;
	this->dbName=dbName;
}

MySQLClient::MySQLClient(const char* fileName)
{
	std::ifstream ini;
	ini.open(fileName);
	std::string params[4], line;
	if(ini.is_open())
	{

		for(uint8_t i=0;i<4;i++)
			ini>>params[i];
		ini.close();
		this->host=params[0];
		this->userName=params[1];
		this->password=params[2];
		this->dbName=params[3];
	}
	else
		errMsg="Error occured on open ini file!";

}

bool MySQLClient::connect()
{
	this->driver=get_driver_instance();
	try
	{
		std::string buf(this->host);
		buf.append("/");
		buf.append(this->dbName);
		this->con=driver->connect(buf, this->userName, this->password);
		return true;
	}
	catch(const sql::SQLException &e)
	{
		errMsg=std::format("Connection was not established! Exception: {}", e.what());
		return false;
	}
}

bool MySQLClient::executeQuery(const char* query, std::vector<std::map<std::string, std::string>> *resVec)
{
	sql::ResultSet *stmtRes;
	sql::Statement *stmt;
	stmt=con->createStatement();
	if(isSelect(query))
	{
		sql::ResultSet *stmtRes;
		try
		{
			stmtRes=stmt->executeQuery(query);
		}
		catch(const sql::SQLException &e)
		{
			errMsg=std::format("Exception occured on execute query! {}", e.what());
			delete stmt;
			return false;
		}
		manageResult(query, resVec, stmtRes);
		delete stmtRes;
	}
	else
		try
		{
			stmt->execute(query);
		}
		catch(const sql::SQLException &e)
		{
			errMsg=std::format("Exception occured on execute query! {}", e.what());
			delete stmt;
			return false;
		}

	delete stmt;
	return true;
}

bool MySQLClient::executePreparedStatement(const char* query, std::vector<std::map<std::string, std::string>> *resVec, const std::vector<std::any> &params)
{
	sql::PreparedStatement *prepStmt;
	try
	{
		prepStmt=con->prepareStatement(query);
	}
	catch(const sql::SQLException &e)
	{
		errMsg=std::format("Exception occured on prepared statement creation! {}", e.what());
		return false;
	}

	for(unsigned int i=0;i<params.size();i++)
		if(params.at(i).type()==typeid(int))
			prepStmt->setInt(i+1, std::any_cast<int>(params.at(i)));
		else if(params.at(i).type()==typeid(std::string))
			prepStmt->setString(i+1, std::any_cast<std::string>(params.at(i)));

	sql::ResultSet *res;
	try
	{
		res=prepStmt->executeQuery();
	}
	catch(const sql::SQLException &e)
	{
		errMsg=std::format("Exception occured on prepared statement execution! {}", e.what());
		delete prepStmt;
		return false;
	}

	if(isSelect(query))
		manageResult(query, resVec, res);
	delete prepStmt;
	delete res;
	return true;
}

void MySQLClient::manageResult(const char* query, std::vector<std::map<std::string, std::string>> *resVec, sql::ResultSet *resSet)
{
	std::vector<std::string> *colNames=new std::vector<std::string>();
	getColNamesFromQuery(query, *colNames);
	sql::ResultSetMetaData *metadata=resSet->getMetaData();
	int colCount=metadata->getColumnCount();
	while(resSet->next())
	{
		std::map<std::string, std::string> row;
		for(unsigned int i=1;i<=colCount;i++)
			row[colNames->at(i-1)]=resSet->getString(i);
		resVec->push_back(row);
	}
	delete colNames;
}

void MySQLClient::getColNamesFromQuery(const char* query, std::vector<std::string> &names)
{
	const std::string queryStr(query);
	std::stringstream colSection(queryStr.substr(7, queryStr.find(" FROM")-7));
	while(colSection.good())
	{
		std::string tmp;
		getline(colSection, tmp, ',');
		if(tmp[0]==' ')
			tmp=tmp.substr(1);
		if(tmp.find("AS")!=std::string::npos)
			tmp=tmp.substr(tmp.find("AS ")+3);
		names.push_back(tmp);
	}
}

bool MySQLClient::isSelect(const char *query)
{
	uint8_t i=0;
	std::string queryType;
	size_t size=strlen(query);
	while(query[i]!=' '&&i<size)
		queryType+=query[i++];
	return queryType=="SELECT";
}

void MySQLClient::closeConnection()
{
	this->con->close();
	if(this->con->isValid())
		errMsg="Close connection failed!";

}

MySQLClient::~MySQLClient()
{
	if(this->con!=nullptr)
		delete this->con;
}
