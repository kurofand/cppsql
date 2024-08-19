#include "mysqlClient.hpp"

#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <sstream>
#include <fstream>
#include <cstring>

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
		std::cout<<"Error with loading ini file!"<<std::endl;
}

bool MySQLClient::connect()
{
	this->driver=get_driver_instance();
	try{
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

void MySQLClient::executeQuery(const char* query, std::vector<std::map<std::string, std::string>> &resVec)
{
	sql::Statement *stmt;
	sql::ResultSet *stmtRes;
	stmt=this->con->createStatement();
	std::string queryType;
	uint8_t i=0;
	while(query[i]!=' ')
		queryType+=query[i++];
	if(isSelect(query))
	{
		sql::ResultSet *stmtRes;
		stmtRes=stmt->executeQuery(query);
		manageResult(query, resVec, stmtRes);
		delete stmtRes;
	}
	else
		stmt->execute(query);

	delete stmt;
}

void MySQLClient::executePreparedStatement(const char* query, std::vector<std::map<std::string, std::string>> &resVec, const std::vector<std::any> &params)
{
	sql::PreparedStatement *prepStmt;
	prepStmt=con->prepareStatement(query);
	for(unsigned int i=0;i<params.size();i++)
		if(params.at(i).type()==typeid(int))
			prepStmt->setInt(i+1, std::any_cast<int>(params.at(i)));
		else if(params.at(i).type()==typeid(std::string))
			prepStmt->setString(i+1, std::any_cast<std::string>(params.at(i)));
	sql::ResultSet *res=prepStmt->executeQuery();
	if(isSelect(query))
		manageResult(query, resVec, res);
	delete prepStmt;
	delete res;
}

void MySQLClient::manageResult(const char* query, std::vector<std::map<std::string, std::string>> &resVec, sql::ResultSet *resSet)
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
		resVec.push_back(row);
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
		std::cout<<"Close connection failed!"<<std::endl;
}

MySQLClient::~MySQLClient()
{
	if(this->con!=nullptr)
		delete this->con;
}
