#include "CmdLineParser.h"
#include "TablePrinter.h"
#include "sqlite3.h"
#include <sstream>
#include <fstream>
#include <iomanip>

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#endif

void EnterDate(std::string sDate);
void EnterDataField(std::string sCmd, double dInputValue, std::string sDate);
static int callback(void *NotUsed, int argc, char * argv[], char **azColName);

void CreateNewDatabase(CParser CommandLineParser);
void AddFieldToDatabase(CParser CommandLineParser);
void PrintFields(CParser CommandLineParser);
bool PrintDates();
bool PrintSpecificTable(std::ofstream &outFile, CParser CommandLineParser);

void DisplayExpenses(std::ofstream &outFile, int iInputDay, int iInputMonth, int iInputYear);

void UpdateExistingRow(CParser CommandLineParser, std::vector<std::string> vecLongFields, std::vector<std::string> vecShortFields, int iInputDay, int iInputMonth, int iInputYear);
void CreateNewRow(CParser CommandLineParser, std::vector<std::string> vecLongFields, std::vector<std::string> vecShortFields);

bool EnterExpenses(std::ofstream &outFile, CParser CommandLineParser);
void ResetDatabase();

bool NewConditionsOk(CParser oMyParser);
bool AddConditionsOk(CParser oMyParser);
bool PrintFieldsConditionsOk(CParser oMyParser);
bool PrintDatesConditionsOk(CParser oMyParser);
bool PrintSpecificConditionsOk(CParser oMyParser);
bool ByConditionsOk(CParser oMyParser);
bool ResetConditionsOk(CParser oMyParser);
bool HelpConditionsOk(CParser oMyParser);
void PrintHelp();
bool VersionConditionsOk(CParser oMyParser);

sqlite3 *Database;
char *zErrMsg = 0;
int  iStatus;
char *SQL_Command;
const char* data = "Callback function called";

CTablePrinter *FinanceTable;
std::string strHomeDirectoryPath = "";

std::string strPrintOutPath = "pfox_console_printout.js";
std::string strDatabasePath = "pfox_expenses.db";
std::string strTableCategoriesPath = "pfox_categories.txt";

const std::string sVersion = "1.0.0";

int main(int argc, char * argv[])
{

#ifndef _WIN32
   const char *homedir;

   if ((homedir = getenv("HOME")) == NULL) {
     homedir = getpwuid(getuid())->pw_dir;
   }

   strHomeDirectoryPath = std::string(homedir) + "/";
   strPrintOutPath =  strHomeDirectoryPath + ".pfox_console_printout.js";
   strDatabasePath = strHomeDirectoryPath + ".pfox_expenses.db";
   strTableCategoriesPath = strHomeDirectoryPath + ".pfox_categories.txt";
#endif

   std::ofstream outFile(strPrintOutPath);

   FinanceTable = new CTablePrinter(outFile, strPrintOutPath);

   // Parse the commands entered by the user
   CParser oMyParser(argc, argv, strHomeDirectoryPath, strTableCategoriesPath);

   // Check if the user entered any instructions
   if (!oMyParser.CheckInstructions())
   {
      return 1;
   }

   if (oMyParser.bNew) // ----------------------------------------------------------------------------------------------------------------------------
   {
      if (!NewConditionsOk(oMyParser))
      {
         return 1;
      }
      else
      {
         CreateNewDatabase(oMyParser);
         return 0;
      }
   }
   else if (oMyParser.bAdd) // -----------------------------------------------------------------------------------------------------------------------
   {
      if (!AddConditionsOk(oMyParser))
      {
         return 1;
      }
      else
      {
         AddFieldToDatabase(oMyParser);
         return 0;
      }
   }
   else if (oMyParser.bPrintFields)   // -------------------------------------------------------------------------------------------------------------
   {
      if (!PrintFieldsConditionsOk(oMyParser))
      {
         return 1;
      }
      else
      {
         PrintFields(oMyParser);
         return 0;
      }
   }
   else if (oMyParser.bPrintDates)    // -------------------------------------------------------------------------------------------------------------
   {
      if (!PrintDatesConditionsOk(oMyParser))
      {
         return 1;
      }
      else if (!PrintDates())
      {
         return 1;
      }
      else
      {
         return 0;
      }
   }
   else if (oMyParser.bPrintSpecific) // -------------------------------------------------------------------------------------------------------------
   {
      if (!PrintSpecificConditionsOk(oMyParser))
      {
         return 1;
      }
      else if (!PrintSpecificTable(outFile, oMyParser))
      {
         return 1;
      }
      else
      {
         return 0;
      }
   }
   else if (oMyParser.bMadeBy) // --------------------------------------------------------------------------------------------------------------------
   {
      if (!ByConditionsOk(oMyParser))
      {
         return 1;
      }
      else
      {
         FinanceTable->MadeBy();
         return 0;
      }
   }
   else if (oMyParser.bReset) // ---------------------------------------------------------------------------------------------------------------------
   {
      if(!ResetConditionsOk(oMyParser))
      {
         return 1;
      }
      else
      {
         ResetDatabase();
         return 0;
      }
   }
   else if (oMyParser.bLog) // -----------------------------------------------------------------------------------------------------------------------
   {
      if (!EnterExpenses(outFile, oMyParser))
      {
         return 1;
      }
      return 0;
   }
   else if (oMyParser.bHelp) // ----------------------------------------------------------------------------------------------------------------------
   {
      if (!HelpConditionsOk(oMyParser))
      {
         return 1;
      }
      else
      {
         PrintHelp();
         return 0;
      }
   }
   else if (oMyParser.bVersion) // --------------------------------------------------------------------------------------------------------------------
   {
       if (!VersionConditionsOk(oMyParser))
       {
           return 1;
       }
       else
       {
           std::cout << '\n';
           std::cout << "   You are using version " << sVersion << " of Poor Fox.\n";
           std::cout << '\n';
           return 0;
       }
   }
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   FinanceTable->iNumOfRows++;

   for (int i = 0; i < argc; i++) {
      FinanceTable->MyTitles.push_back(azColName[i]);
      FinanceTable->MyValues.push_back(argv[i]);
   }

   return 0;
}

void EnterDate(std::string sDate)
{
   int iSlashIndices[2];
   int iSlashCtr = 0;

   iSlashIndices[0] = sDate.find("/");
   iSlashIndices[1] = sDate.find("/", iSlashIndices[0] + 1);

   int iDay = std::stoi(sDate.substr(0, 2));
   int iMonth = std::stoi(sDate.substr(iSlashIndices[0] + 1, 2));
   int iYear = std::stoi(sDate.substr(iSlashIndices[1] + 1, 4));

   std::stringstream oss;
   oss << "INSERT INTO Expenses (" << "Day, Month, Year" << ") VALUES (" << iDay << "," << iMonth << "," << iYear << "); ";

   iStatus = sqlite3_exec(Database, oss.str().c_str(), callback, 0, &zErrMsg);
}

void EnterDataField(std::string sCmd, double dInputValue, std::string sDate)
{
   int iSlashIndices[2];
   int iSlashCtr = 0;

   iSlashIndices[0] = sDate.find("/");
   iSlashIndices[1] = sDate.find("/", iSlashIndices[0] + 1);

   int iDay = std::stoi(sDate.substr(0, 2));
   int iMonth = std::stoi(sDate.substr(iSlashIndices[0] + 1, 2));
   int iYear = std::stoi(sDate.substr(iSlashIndices[1] + 1, 4));

   std::stringstream ssInputVal;
   ssInputVal << std::setprecision(12) << dInputValue; // Billion dollar precision

   std::stringstream ss;
   ss << "UPDATE Expenses SET " << sCmd << "=" << ssInputVal.str() << " WHERE Day=" << iDay << " AND Month=" << iMonth << " AND Year=" << iYear;

   iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, 0, &zErrMsg);
}

void CreateNewDatabase(CParser CommandLineParser)
{
   std::vector<std::string> MyFields;

   // Begin by writing the data fields entered by the user to a text file, so that they are remembered
   CommandLineParser.WriteFieldsToFile();

   //Create a database that contains the data fields entered by the user and open it
   iStatus = sqlite3_open(strDatabasePath.c_str(), &Database);

   // Compose the SQL command that is used to create the table
   MyFields = CommandLineParser.ReadFieldsFromFile();

   // Define the number of fields
   FinanceTable->iNumOfFields = MyFields.size() + 3;

   std::stringstream ss;
   ss << "CREATE TABLE Expenses (";
   ss << "Day INTEGER,";
   ss << " Month INTEGER,";
   ss << " Year INTEGER, ";
   for (size_t i = 0; i < MyFields.size() - 1; i++)
   {
      ss << MyFields.at(i) << " REAL, ";
   }
   ss << MyFields.at(MyFields.size() - 1) << " REAL);";

   // Create the table
   iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, 0, &zErrMsg);

   // Let the user know everything went well
   std::vector<std::string> MyShortFields = CommandLineParser.GetShortDataFields();

   std::cout << '\n';
   std::cout << "   Database created successfully! \n \n";
   std::cout << "   The categories currently included in the database and their corresponding codes are: \n \n";

   FinanceTable->PrintFieldsAndCommands(MyFields, MyShortFields);
}

void AddFieldToDatabase(CParser CommandLineParser)
{
   std::vector<std::string> MyFields;

   // Begin by writing the new fields entered by the user to a text file, so that they are remembered
   CommandLineParser.WriteFieldsToFile();

   // Open the existing database
   iStatus = sqlite3_open(strDatabasePath.c_str(), &Database);

   // Add the new fields to the database
   MyFields = CommandLineParser.GetNewFields();

   std::stringstream ss;
   ss << "ALTER TABLE Expenses ADD COLUMN ";
   for (size_t i = 0; i < MyFields.size(); i++)
   {
      ss.str(std::string());
      ss << "ALTER TABLE Expenses ADD COLUMN " << MyFields.at(i) << " REAL";
      iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, 0, &zErrMsg);
   }

   // We need to set all entries to zero before returning
   for (size_t i = 0; i < MyFields.size(); i++)
   {
      ss.str(std::string());
      ss << "Update Expenses SET " << MyFields.at(i) << " = 0";
      iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, 0, &zErrMsg);
   }

   // Let the user know everything went well
   MyFields = CommandLineParser.ReadFieldsFromFile();
   std::vector<std::string> MyShortFields = CommandLineParser.GetShortDataFields();

   std::cout << '\n';
   std::cout << "   New category added successfully! \n \n";
   std::cout << "   The categories currently included in the database and their corresponding codes are: \n \n";

   FinanceTable->PrintFieldsAndCommands(MyFields, MyShortFields);
}

void PrintFields(CParser CommandLineParser)
{
   std::vector<std::string> MyFields;

   // Read the fields that have been entered by the user so far
   MyFields = CommandLineParser.ReadFieldsFromFile();
   std::vector<std::string> MyShortFields = CommandLineParser.GetShortDataFields();

   std::cout << '\n';
   std::cout << "   The categories currently included in the database and their corresponding codes are: \n \n";

   FinanceTable->PrintFieldsAndCommands(MyFields, MyShortFields);
}

bool PrintDates()
{
   // Open the existing database
   iStatus = sqlite3_open(strDatabasePath.c_str(), &Database);

   std::stringstream ss;
   ss << "SELECT DISTINCT Month, Year FROM EXPENSES ORDER BY Year ASC, Month ASC";
   iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, 0, &zErrMsg);

   if (FinanceTable->MyValues.size() == 0)
   {
      std::cout << '\n';
      std::cout << "   You have not logged any expenses. You must log expenses using the 'log' command at least once before using the 'dat' command. \n";
      std::cout << '\n';
      return false;
   }

   std::cout << '\n';
   std::cout << "   Expenses have been logged in the following months: \n \n";

   FinanceTable->PrintDates();

   return true;

}

bool PrintSpecificTable(std::ofstream &outFile, CParser CommandLineParser)
{
   std::vector<std::string> MyFields;

   // Open the existing database
   iStatus = sqlite3_open(strDatabasePath.c_str(), &Database);

   // Read the fields so that they can be compared with the ones entered by the user
   MyFields = CommandLineParser.ReadFieldsFromFile();

   // Define the number of fields
   FinanceTable->iNumOfFields = MyFields.size() + 3;

   // Get the month/year pair that the user wants to print
   std::string sDate = CommandLineParser.GetDate();

   // Separate the date
   int iSlashIndex;
   iSlashIndex = sDate.find("/");
   int iInputMonth = std::stoi(sDate.substr(0, 2));
   int iInputYear = std::stoi(sDate.substr(iSlashIndex + 1, 4));

   // Get the database values that correspond to the month and year entered by the user and close the database
   std::stringstream ss;
   ss << "SELECT * from Expenses" << " WHERE Month=" << iInputMonth << " AND Year=" << iInputYear << " ORDER BY Day ASC";

   iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, (void*)data, &zErrMsg);

   sqlite3_close(Database);

   if (FinanceTable->MyValues.size() == 0)
   {
      std::cout << '\n';
      std::cout << "   You did not log any expenses in that month.\n\n";
      std::cout << "   Use the 'dat' command to see the dates that are valid. \n";
      std::cout << '\n';
      return false;
   }

   DisplayExpenses(outFile, 0, iInputMonth, iInputYear);

   return true;
}

void DisplayExpenses(std::ofstream &outFile, int iInputDay, int iInputMonth, int iInputYear)
{
   // Get the database values that correspond to the month and year entered by the user and close the database
   std::stringstream ss;
   ss << "SELECT * from Expenses" << " WHERE Month=" << iInputMonth << " AND Year=" << iInputYear << " ORDER BY Day ASC";

   iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, (void*)data, &zErrMsg);

   sqlite3_close(Database);

   outFile << "const chalk = require('chalk');\n";

   // Print the main table
   // This table contains the expenses that have been recorded in the month/year entered by the user
   FinanceTable->CalculateSpaceOccupiedByEachField();
   if (outFile.is_open()) {
      FinanceTable->PrintTopBar();
      FinanceTable->PrintFields();
      FinanceTable->PrintMiddleBar();

      for (int i = 0; i < FinanceTable->iNumOfRows; i++)
      {
         FinanceTable->PrintValues(i);
         if (i != FinanceTable->iNumOfRows - 1)
         {
            FinanceTable->PrintMiddleBar();
         }
      }

      FinanceTable->PrintBottomBar();
   }

   // Print the table of totals and the expense graph
   FinanceTable->CalculateTotals(iInputDay, iInputMonth, iInputYear);

   FinanceTable->CalculateSpaceOccupiedByEachField();
   if (outFile.is_open()) {
      FinanceTable->PrintTopBar();
      FinanceTable->PrintFields();
      FinanceTable->PrintMiddleBar();

      for (int i = 0; i < FinanceTable->iNumOfRows; i++)
      {
         FinanceTable->PrintValues(i);
         if (i != FinanceTable->iNumOfRows - 1)
         {
            FinanceTable->PrintMiddleBar();
         }
      }

      FinanceTable->PrintBottomBar();
   }

   std::cout << '\n';

   outFile.close();
   std::string strNodeCommand = "node " + strPrintOutPath;
   system(strNodeCommand.c_str());

   std::cout << '\n';
}

void UpdateExistingRow(CParser CommandLineParser, std::vector<std::string> vecLongFields, std::vector<std::string> vecShortFields, int iInputDay, int iInputMonth, int iInputYear)
{
   std::stringstream ss;
   ss << "SELECT ";
   for (size_t i = 0; i < vecLongFields.size() - 1; i++)
   {
      ss << vecLongFields.at(i) << ", ";
   }

   ss << vecLongFields.at(vecLongFields.size() - 1);
   ss << " from Expenses" << " WHERE Day=" << iInputDay << " AND Month=" << iInputMonth << " AND Year=" << iInputYear;

   iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, (void*)data, &zErrMsg);

   for (size_t j = 0; j < FinanceTable->MyTitles.size(); j++)
   {
      std::stringstream ss;
      double dNewVal = CommandLineParser.GetDataFieldValue(vecShortFields.at(j)) + std::stod(FinanceTable->MyValues.at(j));

      std::stringstream ssInputVal;
      ssInputVal << std::setprecision(12) << dNewVal; // Billion dollar precision

      ss << "UPDATE Expenses SET " << FinanceTable->MyTitles.at(j) << "=" << ssInputVal.str() << " WHERE Day=" << iInputDay << " AND Month=" << iInputMonth << " AND Year=" << iInputYear;

      iStatus = sqlite3_exec(Database, ss.str().c_str(), callback, 0, &zErrMsg);
   }
}

void CreateNewRow(CParser CommandLineParser, std::vector<std::string> vecLongFields, std::vector<std::string> vecShortFields)
{
   EnterDate(FinanceTable->sDate);

   double dInputValue;
   std::wcout.precision(12);

   for (size_t i = 0; i < vecLongFields.size(); i++)
   {
      if (CommandLineParser.DataFieldExists(vecShortFields.at(i)))
      {
         dInputValue = CommandLineParser.GetDataFieldValue(vecShortFields.at(i));
         EnterDataField(vecLongFields.at(i), dInputValue, FinanceTable->sDate);
      }
      else
      {
         EnterDataField(vecLongFields.at(i), 0, FinanceTable->sDate);
      }
   }
}

bool EnterExpenses(std::ofstream &outFile, CParser CommandLineParser)
{
   if (CommandLineParser.CheckIfFileExists(strDatabasePath))
   {
      std::cout << '\n';
      std::cout << "   A database does not exist. Before using the 'log' command you must create a database using the 'new' command. \n";
      std::cout << '\n';
      return false;
   }

   if (!CommandLineParser.CheckIfUserEnteredValidFields())
   {
      return false;
   }

   if (!CommandLineParser.CheckEachFieldHasEqualSign())
   {
      std::cout << '\n';
      std::cout << "   You entered a category that is not followed by '='. Please ensure all the categories you enter are in the format '-code=quantity'. \n";
      std::cout << '\n';

      return false;
   }

   if (!CommandLineParser.CheckEachFieldHasQuantity())
   {
      std::cout << '\n';
      std::cout << "   You entered a category without a quantity to log. Please ensure all the categories you enter are in the format '-code=quantity'. \n";
      std::cout << '\n';

      return false;
   }

   if (!CommandLineParser.CheckQuantitiesAreNumbers())
   {
      std::cout << '\n';
      std::cout << "   You entered a category followed by a quantity that is not a number. Please ensure that what follows each equal sign is a real number. \n";
      std::cout << '\n';

      return false;
   }

   if (!CommandLineParser.CheckFieldsAreUnique())
   {
      std::cout << '\n';
      std::cout << "   You entered categories that are repeated. Please ensure that the categories you enter are unique. \n";
      std::cout << '\n';

      return false;
   }

   if (!CommandLineParser.CheckUserEnteredProperDate(false))
   {
      std::cout << '\n';
      std::cout << "   You did not enter a valid date. Please ensure that the date is in the format '-d=day/month/year'. \n";
      std::cout << '\n';

      return false;
   }

   std::vector<std::string> MyLongFields;
   std::vector<std::string> MyShortFields;

   // Open the existing database
   iStatus = sqlite3_open(strDatabasePath.c_str(), &Database);

   // Read the long fields in order to retrieve values from the database
   MyLongFields = CommandLineParser.ReadFieldsFromFile();

   // Define the number of fields
   FinanceTable->iNumOfFields = MyLongFields.size() + 3;

   // Generate the short fields so that they can be compared with the ones entered by the user
   MyShortFields = CommandLineParser.GetShortDataFields();

   // Verify the date entered by the user
   FinanceTable->sDate = CommandLineParser.GetDate();

   int iSlashIndices[2];
   int iSlashCtr = 0;

   iSlashIndices[0] = FinanceTable->sDate.find("/");
   iSlashIndices[1] = FinanceTable->sDate.find("/", iSlashIndices[0] + 1);

   int iInputDay = std::stoi(FinanceTable->sDate.substr(0, 2));
   int iInputMonth = std::stoi(FinanceTable->sDate.substr(iSlashIndices[0] + 1, 2));
   int iInputYear = std::stoi(FinanceTable->sDate.substr(iSlashIndices[1] + 1, 4));

   // Enter new values into the database
   SQL_Command = "SELECT Day,Month,Year from Expenses";

   iStatus = sqlite3_exec(Database, SQL_Command, callback, (void*)data, &zErrMsg);

   int iTableDay, iTableMonth, iTableYear;
   bool bRepeated = false;

   for (size_t i = 0; i < FinanceTable->MyValues.size(); i += 3)
   {
      iTableDay = std::stoi(FinanceTable->MyValues.at(i));
      iTableMonth = std::stoi(FinanceTable->MyValues.at(i + 1));
      iTableYear = std::stoi(FinanceTable->MyValues.at(i + 2));

      // If the date entered by the user is already inside of the database, do not create a new row and simply add the new values to the old ones
      if (iInputDay == iTableDay && iInputMonth == iTableMonth && iInputYear == iTableYear)
      {
         bRepeated = true;

         FinanceTable->MyTitles.clear();
         FinanceTable->MyValues.clear();
         FinanceTable->iNumOfRows = 0;

         UpdateExistingRow(CommandLineParser, MyLongFields, MyShortFields, iInputDay, iInputMonth, iInputYear);

         break;
      }
   }

   FinanceTable->MyTitles.clear();
   FinanceTable->MyValues.clear();
   FinanceTable->iNumOfRows = 0;

   // If the date entered by the user is new, create a new row and enter the new values into the database
   if (!bRepeated)
   {
      CreateNewRow(CommandLineParser, MyLongFields, MyShortFields);
   }

   DisplayExpenses(outFile, iInputDay, iInputMonth, iInputYear);

   return true;
}

void ResetDatabase()
{

#ifdef _WIN32
   std::string strRemoveDatabaseCommandWindows = "DEL " + strDatabasePath;
   std::string strRemoveCategoriesCommandWindows = "DEL " + strTableCategoriesPath;
   system(strRemoveDatabaseCommandWindows.c_str());
   system(strRemoveCategoriesCommandWindows.c_str());
#else
   std::string strRemoveDatabaseCommandUnix = "rm " + strDatabasePath;
   std::string strRemoveCategoriesCommandUnix = "rm " + strTableCategoriesPath;
   system(strRemoveCategoriesCommandUnix.c_str());
   system(strRemoveDatabaseCommandUnix.c_str());
#endif

   std::cout << '\n';
   std::cout << "   Database deleted successfully!\n\n";
   std::cout << "   Use the 'new' command to create a new one.\n";
   std::cout << '\n';
}

bool NewConditionsOk(CParser oMyParser)
{
   if (!oMyParser.CheckIfFileExists(strDatabasePath))
   {
      std::cout << '\n';
      std::cout << "   A database already exists. Use the 'reset' command before trying to create a new one.\n\n";
      std::cout << "   If you would like to add categories to an existing database, use the 'add' command.\n";
      std::cout << '\n';
      return false;
   }
   if (!oMyParser.CheckCreationFieldsAreValid())
   {
      return false;
   }
   if (!oMyParser.CheckCreationFieldsAreUnique())
   {
      return false;
   }
   return true;
}

bool AddConditionsOk(CParser oMyParser)
{
   if (oMyParser.CheckIfFileExists(strDatabasePath))
   {
      std::cout << '\n';
      std::cout << "   A database does not exist. Before using the 'add' command you must create a database using the 'new' command.\n";
      std::cout << '\n';
      return false;
   }
   if (!oMyParser.CheckCreationFieldsAreValid())
   {
      return false;
   }
   if (!oMyParser.CheckFieldsBeingAddedAreUnique())
   {
      return false;
   }
   return true;
}

bool PrintFieldsConditionsOk(CParser oMyParser)
{
   if (oMyParser.CheckIfFileExists(strDatabasePath))
   {
      std::cout << '\n';
      std::cout << "   A database does not exist. Before using the 'cat' command you must create a database using the 'new' command.\n";
      std::cout << '\n';
      return false;
   }
   if (!oMyParser.CheckUserOnlyEnteredXFields(0))
   {
      std::cout << '\n';
      std::cout << "   You entered unnecessary data.\n\n";
      std::cout << "   You do not need to enter categories or dates when using the 'cat' command.\n";
      std::cout << '\n';
      return false;
   }
   return true;
}

bool PrintDatesConditionsOk(CParser oMyParser)
{
   if (oMyParser.CheckIfFileExists(strDatabasePath))
   {
      std::cout << '\n';
      std::cout << "   A database does not exist. Before using the 'dat' command you must create a database using the 'new' command.\n";
      std::cout << '\n';
      return false;
   }
   if (!oMyParser.CheckUserOnlyEnteredXFields(0))
   {
      std::cout << '\n';
      std::cout << "   You entered unnecessary data.\n\n";
      std::cout << "   You do not need to enter categories or dates when using the 'dat' command.\n";
      std::cout << '\n';
      return false;
   }
   return true;
}

bool PrintSpecificConditionsOk(CParser oMyParser)
{
   if (oMyParser.CheckIfFileExists(strDatabasePath))
   {
      std::cout << '\n';
      std::cout << "   A database does not exist. Before using the 'tab' command you must create a database using the 'new' command.\n";
      std::cout << '\n';
      return false;
   }
   if (!oMyParser.CheckUserEnteredProperDate(true))
   {
      std::cout << '\n';
      std::cout << "   You did not enter a valid date. Please ensure that the date is in the format '-d=month/year'.\n\n";
      std::cout << "   Use the 'dat' command to see the dates that are valid.\n";
      std::cout << '\n';
      return false;
   }
   if (!oMyParser.CheckUserOnlyEnteredXFields(1))
   {
      std::cout << '\n';
      std::cout << "   You entered unnecessary data.\n\n";
      std::cout << "   You only need to specify the date in the format '-d=month/year' when using the 'tab' command.\n\n";
      std::cout << "   Use the 'dat' command to see the dates that are valid.\n";
      std::cout << '\n';
      return false;
   }
   return true;
}

bool ByConditionsOk(CParser oMyParser)
{
   if (!oMyParser.CheckUserOnlyEnteredXFields(0))
   {
      std::cout << '\n';
      std::cout << "   You entered unnecessary data.\n\n";
      std::cout << "   You do not need to enter categories or dates when using the 'by' command.\n";
      std::cout << '\n';
      return false;
   }
   return true;
}

bool ResetConditionsOk(CParser oMyParser)
{
   if (oMyParser.CheckIfFileExists(strDatabasePath))
   {
      std::cout << '\n';
      std::cout << "   A database does not exist. Before using the 'reset' command you must create a database using the 'new' command.\n";
      std::cout << '\n';
      return false;
   }
   if (!oMyParser.CheckUserOnlyEnteredXFields(0))
   {
      std::cout << '\n';
      std::cout << "   You entered unnecessary data.\n\n";
      std::cout << "   You do not need to enter categories or dates when using the 'reset' command.\n";
      std::cout << '\n';
      return false;
   }
   return true;
}

bool HelpConditionsOk(CParser oMyParser)
{
   if (!oMyParser.CheckUserOnlyEnteredXFields(0))
   {
      std::cout << '\n';
      std::cout << "   You entered unnecessary data.\n\n";
      std::cout << "   You do not need to enter categories or dates when using the 'help' command.\n";
      std::cout << '\n';
      return false;
   }
   return true;
}

void PrintHelp()
{
   std::cout << '\n';
   std::cout << "   Usage:\n\n";

   std::cout << "      pfox [options]\n\n";

   std::cout << "   Options:\n\n";

   std::cout << "      -------------------------------- *** To create a new database ***\n\n";
   std::cout << "      new -<category>                    + Categories cannot contain any numbers or symbols, match existing commands or be repeated.\n";
   std::cout << "                                         + The first letter of each category must be uppercase.\n\n\n";

   std::cout << "      -------------------------------- *** To log expenses ***\n\n";
   std::cout << "      log -d=<date> -<code>=<quantity>   + The date must be specified in the format 'day/month/year' (e.g. 19/7/2016).\n";
   std::cout << "                                         + The date must be valid (e.g. February 29 is only valid on leap years).\n";
   std::cout << "                                         + Quantities must be real numbers.\n";
   std::cout << "                                         + Numbers that contain more than 12 digits are rounded.\n\n\n";

   std::cout << "      -------------------------------- *** To add a new category to an existing database ***\n\n";
   std::cout << "      add -<category>                    + Categories cannot contain any numbers or symbols, match existing commands or be repeated.\n";
   std::cout << "                                         + The first letter of each category must be uppercase.\n\n\n";

   std::cout << "      -------------------------------- *** To see the expense report of a particular month ***\n\n";
   std::cout << "      tab -d=<date>                      + The date must be specified in the format 'month/year' (e.g. 7/2016).\n";
   std::cout << "                                         + Expenses must have been logged at least once on the date specified.\n\n\n";

   std::cout << "      -------------------------------- *** To see the months in which expenses have been logged ***\n\n";
   std::cout << "      dat                                + Expenses must have been logged at least once.\n\n\n";

   std::cout << "      -------------------------------- *** To see the categories included in the database ***\n\n";
   std::cout << "      cat                                + A database must have been created.\n\n\n";

   std::cout << "      -------------------------------- *** To delete the database ***\n\n";
   std::cout << "      reset                              + A database must have been created.\n\n\n";

   std::cout << "      -------------------------------- *** To learn how to use the application ***\n\n";
   std::cout << "      help\n\n";

   std::cout << "   Examples:\n\n";

   std::cout << "      pfox new -Groceries -Restaurants -Books -Clothes -Climbing   // Create DB with 4 categories\n";
   std::cout << "      pfox log -d=1/12/2016 -res=16.25 -clo=24.36                  // Went to a restaurant and also bought a shirt\n";
   std::cout << "      pfox log -d=1/12/2016 -clo=-24.36 -cli=32.48                 // Returned the shirt and went climbing\n";
   std::cout << "      pfox log -d=23/12/2016 -boo=42.23                            // Bought a book\n";
   std::cout << "      pfox tab -d=12/2016                                          // Display the expenses of December\n";
   std::cout << "      pfox add -Skiing                                             // Picked up a new hobbie\n";
   std::cout << "      pfox log -d=28/12/2016 -ski=75                               // Went skiing\n";
   std::cout << "      pfox reset                                                   // Delete database\n\n";
}

bool VersionConditionsOk(CParser oMyParser)
{
    if (!oMyParser.CheckUserOnlyEnteredXFields(0))
    {
        std::cout << '\n';
        std::cout << "   You entered unnecessary data.\n\n";
        std::cout << "   You do not need to enter categories or dates when using the 'version' command.\n";
        std::cout << '\n';
        return false;
    }
    return true;
}
