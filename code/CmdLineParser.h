#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

#include <sys/types.h> 
#include <sys/stat.h>

#include <time.h>

class CParser
{

public:

   CParser(int argc, char * argv[], std::string strDataFieldPath, std::string strTableCategoriesPath);
   ~CParser();

   std::string strTableCategoriesPath;

   bool CheckInstructions();
   void WriteFieldsToFile();
   std::vector<std::string> ReadFieldsFromFile();
   std::vector<std::string> GetNewFields();
   bool DataFieldExists(std::string sCmd);
   double GetDataFieldValue(std::string sCmd);
   std::string GetDate();
   std::vector<std::string> GetShortDataFields();
   bool CheckEachFieldHasEqualSign();
   bool CheckEachFieldHasQuantity();
   bool CheckQuantitiesAreNumbers();
   bool CheckUserEnteredProperDate(bool bPrintSpecificTable);
   bool CheckCreationFieldsAreValid();
   bool CheckIfFileExists(std::string filePath  );
   bool CheckIfUserEnteredValidFields();
   bool CheckCreationFieldsAreUnique();
   bool CheckFieldsBeingAddedAreUnique();
   bool CheckFieldsAreUnique();
   bool CheckUserOnlyEnteredXFields(int iNumOfFields);

   bool bNew, bAdd;
   bool bPrintFields;
   bool bPrintDates;
   bool bPrintSpecific;
   bool bLog;
   bool bMadeBy;
   bool bReset;
   bool bHelp;
   bool bVersion;

   std::string strHomeDirectoryPath;

private:

   void parse(int argc, char * argv[]);

   int m_iNumInstructions;
   int m_iNumDataFields;

   typedef std::vector<std::string> vectorOfString;
   typedef vectorOfString::iterator vectorOfStringIT;

   vectorOfString m_vecInstructions;
   vectorOfString m_vecDataFields;
   vectorOfString m_vecLongDataFields;
   vectorOfString m_vecShortDataFields;

};

// Constructor: Resets the counters and parses the user input
CParser::CParser(int argc, char * argv[], std::string strHomeDirectoryPath, std::string strTableCategoriesPath)
{
   this->strTableCategoriesPath = strTableCategoriesPath;
   m_iNumInstructions = 0;
   m_iNumDataFields = 0;

   bNew = false;
   bAdd = false;
   bPrintFields = false;
   bPrintDates = false;
   bPrintSpecific = false;
   bLog = false;
   bMadeBy = false;
   bReset = false;
   bHelp = false;
   bVersion = false;

   strHomeDirectoryPath = strHomeDirectoryPath;

   parse(argc, argv);
}

// Destructor
CParser::~CParser()
{
   // Do nothing
}

// Parser: Separates the instructions from the data fields
void CParser::parse(int argc, char * argv[])
{
   for (int i = 1; i < argc; i++)                            // Skip the name of the .exe file by starting at 1
   {
      std::string sUserInput = argv[i];

      if (sUserInput[0] != '/' && sUserInput[0] != '-')      // Instructions do not start with '/' or '-'
      {
         m_iNumInstructions++;
         m_vecInstructions.push_back(sUserInput);
      }
      else                                                  // Data fields start with '/' or '-'
      {
         m_iNumDataFields++;
         m_vecDataFields.push_back(sUserInput.substr(1));   // Remove '/' or '-'
      }
   }
}

// Checks if the user entered any instructions
bool CParser::CheckInstructions()
{
   vectorOfStringIT it = m_vecInstructions.begin();

   for (; it != m_vecInstructions.end(); it++)
   {
      if ((*it).compare("new") == 0)
      {
         bNew = true;
      }
      else if ((*it).compare("add") == 0)
      {
         bAdd = true;
      }
      else if ((*it).compare("cat") == 0)
      {
         bPrintFields = true;
      }
      else if ((*it).compare("dat") == 0)
      {
         bPrintDates = true;
      }
      else if ((*it).compare("tab") == 0)
      {
         bPrintSpecific = true;
      }
      else if ((*it).compare("log") == 0)
      {
         bLog = true;
      }
      else if ((*it).compare("reset") == 0)
      {
         bReset = true;
      }
      else if ((*it).compare("help") == 0)
      {
         bHelp = true;
      }
      else if ((*it).compare("by") == 0)
      {
         bMadeBy = true;
      }
      else if ((*it).compare("version") == 0)
      {
         bVersion = true;
      }
   }

   if (!bNew && !bAdd && !bPrintFields && !bPrintDates && !bPrintSpecific && !bLog && !bMadeBy && !bReset && !bHelp && !bVersion)
   {
      // The user did not enter a command or she/he did not enter a valid one, so we should terminate the execution

      std::cout << '\n';
      std::cout << "   You did not enter a valid command. In order to see which commands are valid use the 'help' command.\n";
      std::cout << '\n';

      return false;
   }

   if (m_iNumInstructions > 1)
   {
      // The user entered more than one instruction
      // Instructions should be entered independently, so we should terminate the execution

      std::cout << '\n';
      std::cout << "   You can only enter 1 command at a time.\n\n";
      std::cout << "   If you only entered 1 command, check that the date or the categories you entered have a '-' in front of them.\n";
      std::cout << '\n';

      return false;
   }

   return true;
}

void CParser::WriteFieldsToFile()
{
   std::ofstream DataFieldsFile(strTableCategoriesPath, std::ofstream::app);

   if (DataFieldsFile.is_open())
   {
      vectorOfStringIT it = m_vecDataFields.begin();
      std::string sCurrentField;

      for (; it != m_vecDataFields.end(); it++)
      {
         sCurrentField = *it;
         DataFieldsFile << sCurrentField << '\n';
      }
      DataFieldsFile.close();
   }
}

std::vector<std::string> CParser::ReadFieldsFromFile()
{
   std::vector<std::string> Fields;

   std::string line;
   std::ifstream DataFieldsFile(strTableCategoriesPath);
   if (DataFieldsFile.is_open())
   {
      while (std::getline(DataFieldsFile, line))
      {
         Fields.push_back(line);
      }
      DataFieldsFile.close();
   }

   return Fields;
}

std::vector<std::string> CParser::GetNewFields()
{
   return m_vecDataFields;
}

// Checks if a given data field exists
bool CParser::DataFieldExists(std::string sCmd)
{
   vectorOfStringIT it = m_vecDataFields.begin();

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');

      if ((*it).substr(0, iIndex).compare(sCmd) == 0)   // If the data field entered by the user exists, the database is updated with the value entered by the user
      {
         return true;
      }
   }
   return false;
}

// Returns the date entered by the user as a string in the format xx/yy/zzzz
std::string CParser::GetDate()
{
   vectorOfStringIT it = m_vecDataFields.begin();
   time_t time_raw;
   char buf[12];

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');

      if ((*it).substr(0, iIndex).compare("d") == 0)
      {
         return (*it).substr(iIndex + 1);
      }
   }
   time(&time_raw);
   strftime(buf, 12, "%d/%m/%Y", localtime(&time_raw));
   return buf;
}

// Extracts the value associated with a data field
double CParser::GetDataFieldValue(std::string sCmd)
{
   vectorOfStringIT it = m_vecDataFields.begin();

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');

      if ((*it).substr(0, iIndex).compare(sCmd) == 0)
      {
         return std::stod((*it).substr(iIndex + 1));
      }
   }
   return 0;
}

std::vector<std::string> CParser::GetShortDataFields()
{
   vectorOfString vecLongExpenseFields;
   vectorOfString vecShortExpenseFields;

   vecLongExpenseFields = ReadFieldsFromFile();

   std::string sCurrentField;
   std::string sCurrentShortFieldVersion;
   int iExtraLetterCount;
   for (size_t i = 0; i < vecLongExpenseFields.size(); i++)
   {
      sCurrentField = vecLongExpenseFields.at(i);

      // First field
      // Its initial short version is final since there are no other short fields to compare it to.
      if (i == 0)
      {
         if (sCurrentField.length() < 3)
         {
            vecShortExpenseFields.push_back(sCurrentField);              // Long name is less than 3 characters = take the entire field.
         }
         else
         {
            vecShortExpenseFields.push_back(sCurrentField.substr(0, 3)); // Long name is more than 3 characters = take the first 3 characters.
         }
      }
      // All the other fields
      else
      {
         if (sCurrentField.length() < 3)
         {
            vecShortExpenseFields.push_back(sCurrentField);         // It is impossible to have fields of 1 or 2 characters that are repeated.
            // For this reason they are included without comparing them to the other short fields.
         }
         else
         {
            iExtraLetterCount = 0;
            sCurrentShortFieldVersion = sCurrentField.substr(0, 3);

            // Comparison loop
            for (size_t j = 0; j < vecShortExpenseFields.size(); j++)
            {
               if (sCurrentShortFieldVersion == vecShortExpenseFields.at(j))
               {
                  // Do I have an extra letter?
                  if (sCurrentField.length() > sCurrentShortFieldVersion.length())
                  {
                     // Yes
                     // Add existing letter at the end
                     iExtraLetterCount++;
                     sCurrentShortFieldVersion = sCurrentField.substr(0, 3 + iExtraLetterCount);
                  }
                  else
                  {
                     // No
                     // Add '+' at the end
                     sCurrentShortFieldVersion = sCurrentShortFieldVersion + '+';
                     break;
                  }
               }
            }

            vecShortExpenseFields.push_back(sCurrentShortFieldVersion);

         }
      }
   }

   // Force the short fields to be lowercase
   vectorOfStringIT it = vecShortExpenseFields.begin();
   std::string sTempField;
   vectorOfString vecLowercaseShortExpenseFields;

   for (; it != vecShortExpenseFields.end(); it++)
   {
      sTempField = *it;
      std::transform(sTempField.begin(), sTempField.end(), sTempField.begin(), tolower);
      vecLowercaseShortExpenseFields.push_back(sTempField);
   }

   return vecLowercaseShortExpenseFields;
}

bool CParser::CheckEachFieldHasEqualSign()
{
   vectorOfStringIT it = m_vecDataFields.begin();

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');

      if (iIndex == std::string::npos)
      {
         return false;
      }
   }

   return true;
}

bool CParser::CheckEachFieldHasQuantity()
{
   vectorOfStringIT it = m_vecDataFields.begin();

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');
      if (iIndex + 2 > (*it).length())
      {
         return false;
      }
   }

   return true;
}

bool CParser::CheckQuantitiesAreNumbers()
{
   vectorOfStringIT it = m_vecDataFields.begin();
   std::string::size_type sz;

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');
      if ((*it).substr(0, iIndex).compare("d") != 0)
      {
         try
         {
            std::stod((*it).substr(iIndex + 1), &sz);
         }
         catch (const std::invalid_argument&)
         {
            return false;
         }

         if ((*it).substr(iIndex + 1).length() != sz)
         {
            return false;
         }
      }
   }

   return true;
}

bool CParser::CheckUserEnteredProperDate(bool bPrintSpecificTable)
{
   vectorOfStringIT it = m_vecDataFields.begin();
   bool bDateAvailable = false;
   std::string sDate;

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');

      if ((*it).substr(0, iIndex).compare("d") == 0)
      {
         bDateAvailable = true;
         sDate = (*it).substr(iIndex + 1);
      }
   }

   if (!bDateAvailable && bLog)
   {
      /*
       * Return true here if no date option was entered on the commandline,
       * since we will use today's date later on
       */
      return true;
   }

   if (bPrintSpecificTable)
   {
      sDate = "01/" + sDate;
   }

   int iSlashIndices[2];

   iSlashIndices[0] = sDate.find("/");

   if (iSlashIndices[0] == std::string::npos || (iSlashIndices[0] != 1 && iSlashIndices[0] != 2))
   {
      return false;
   }

   iSlashIndices[1] = sDate.find("/", iSlashIndices[0] + 1);

   if (iSlashIndices[1] == std::string::npos || (iSlashIndices[1] != 3 && iSlashIndices[1] != 4 && iSlashIndices[1] != 5))
   {
      return false;
   }

   int iInputDay, iInputMonth, iInputYear;
   std::string::size_type sz;

   try
   {
      iInputDay = std::stoi(sDate.substr(0, iSlashIndices[0]), &sz);
   }
   catch (const std::invalid_argument&)
   {
      return false;
   }
   if (sDate.substr(0, iSlashIndices[0]).length() != sz)
   {
      return false;
   }

   try
   {
      iInputMonth = std::stoi(sDate.substr(iSlashIndices[0] + 1, iSlashIndices[1] - iSlashIndices[0] - 1), &sz);
      if (iInputMonth < 1 || iInputMonth > 12)
      {
         return false;
      }
   }
   catch (const std::invalid_argument&)
   {
      return false;
   }
   if (sDate.substr(iSlashIndices[0] + 1, iSlashIndices[1] - iSlashIndices[0] - 1).length() != sz)
   {
      return false;
   }

   try
   {
      iInputYear = std::stoi(sDate.substr(iSlashIndices[1] + 1, std::string::npos), &sz);
      if (iInputYear < 1000 || iInputYear >9999)
      {
         return false;
      }
   }
   catch (const std::invalid_argument&)
   {
      return false;
   }
   if (sDate.substr(iSlashIndices[1] + 1, std::string::npos).length() != sz)
   {
      return false;
   }

   int DaysInEachMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   if ((!(iInputYear % 4) && (iInputYear % 100) || !(iInputYear % 400)))
   {
      DaysInEachMonth[1] = 29;
   }

   if (iInputDay > DaysInEachMonth[iInputMonth - 1] || iInputDay == 0)
   {
      return false;
   }

   return true;

}

bool CParser::CheckCreationFieldsAreValid()
{
   vectorOfStringIT it = m_vecDataFields.begin();

   if (m_iNumDataFields == 0)
   {
      std::cout << '\n';
      std::cout << "   You did not enter any categories. Please specify each category as '-Category' and separate each one by one or more spaces.\n";
      std::cout << '\n';
      return false;
   }

   for (; it != m_vecDataFields.end(); it++)
   {
      if ((std::string::npos != (*it).find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")))
      {
         std::cout << '\n';
         std::cout << "   Categories cannot contain numbers or symbols.\n";
         std::cout << '\n';
         return false;
      }
   }
   return true;
}

bool CParser::CheckIfFileExists(std::string filePath)
{
   struct stat buffer;

   if ((stat(filePath.c_str(), &buffer) == 0))
   {
      return false;
   }

   return true;
}

bool CParser::CheckIfUserEnteredValidFields()
{
   vectorOfStringIT it = m_vecDataFields.begin();

   int iNumOfCategories = 0;

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');

      if ((*it).substr(0, iIndex).compare("d") != 0)
      {
         iNumOfCategories++;
      }
   }

   if (iNumOfCategories == 0)
   {
      std::cout << '\n';
      std::cout << "   You did not enter any categories to log. Please enter categories in the format '-code=quantity'.\n";
      std::cout << '\n';

      return false;
   }

   vectorOfString vecShortFields = GetShortDataFields();
   vectorOfString vecInputShortFields;

   it = m_vecDataFields.begin();
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');

      if ((*it).substr(0, iIndex).compare("d") != 0)
      {
         vecInputShortFields.push_back((*it).substr(0, iIndex));
      }
   }

   bool bFieldIsValid;
   bool bInvalidFieldPresent = false;
   for (size_t i = 0; i < vecInputShortFields.size(); i++)
   {
      bFieldIsValid = false;
      for (size_t j = 0; j < vecShortFields.size(); j++)
      {
         if ((vecInputShortFields.at(i) == vecShortFields.at(j)))
         {
            bFieldIsValid = true;
            break;
         }
      }

      if (!bFieldIsValid)
      {
         bInvalidFieldPresent = true;
         break;
      }
   }

   if (bInvalidFieldPresent)
   {
      std::cout << '\n';
      std::cout << "   You entered invalid categories to log. Use the 'cat' command to see the categories that are valid and their codes.\n";
      std::cout << '\n';
      return false;
   }
   else
   {
      return true;
   }
}

bool CParser::CheckCreationFieldsAreUnique()
{
   vectorOfString vecCategories = m_vecDataFields;
   std::string sTempString;

   vecCategories.push_back("new");
   vecCategories.push_back("add");
   vecCategories.push_back("log");
   vecCategories.push_back("dat");
   vecCategories.push_back("cat");
   vecCategories.push_back("tab");
   vecCategories.push_back("reset");
   vecCategories.push_back("by");
   vecCategories.push_back("d");
   vecCategories.push_back("help");
   vecCategories.push_back("version");

   for (size_t i = 0; i < vecCategories.size(); i++)
   {
      sTempString = vecCategories.at(i);
      std::transform(sTempString.begin(), sTempString.end(), sTempString.begin(), tolower);
      vecCategories.at(i) = sTempString;
   }

   for (size_t i = 0; i < vecCategories.size(); i++)
   {
      for (size_t j = 0; j < vecCategories.size(); j++)
      {
         if ((vecCategories.at(i) == vecCategories.at(j)) && (i != j))
         {
            std::cout << '\n';
            std::cout << "   You entered categories that are repeated or that match existing commands. Please ensure that the categories you enter are unique.\n";
            std::cout << '\n';
            return false;
         }
      }
   }
   return true;
}

bool CParser::CheckFieldsBeingAddedAreUnique()
{
   vectorOfString vecNewCategories = m_vecDataFields;
   vectorOfString vecCategories = ReadFieldsFromFile();
   std::string sTempString;

   vecCategories.push_back("new");
   vecCategories.push_back("add");
   vecCategories.push_back("log");
   vecCategories.push_back("dat");
   vecCategories.push_back("cat");
   vecCategories.push_back("tab");
   vecCategories.push_back("reset");
   vecCategories.push_back("by");
   vecCategories.push_back("d");
   vecCategories.push_back("help");
   vecCategories.push_back("version");

   for (size_t i = 0; i < vecNewCategories.size(); i++)
   {
      vecCategories.push_back(vecNewCategories.at(i));
   }

   for (size_t i = 0; i < vecCategories.size(); i++)
   {
      sTempString = vecCategories.at(i);
      std::transform(sTempString.begin(), sTempString.end(), sTempString.begin(), tolower);
      vecCategories.at(i) = sTempString;
   }

   for (size_t i = 0; i < vecCategories.size(); i++)
   {
      for (size_t j = 0; j < vecCategories.size(); j++)
      {
         if ((vecCategories.at(i) == vecCategories.at(j)) && (i != j))
         {
            std::cout << '\n';
            std::cout << "   You entered categories that are repeated or that match existing commands. Please ensure that the categories you enter are unique.\n\n";
            std::cout << "   You can see the categories that are already in the database using the 'cat' command.\n";
            std::cout << '\n';
            return false;
         }
      }
   }
   return true;
}

bool CParser::CheckFieldsAreUnique()
{
   vectorOfStringIT it = m_vecDataFields.begin();
   vectorOfString vecCategories;

   size_t iIndex;
   for (; it != m_vecDataFields.end(); it++)
   {
      iIndex = (*it).find('=');
      vecCategories.push_back((*it).substr(0, iIndex));
   }

   for (size_t i = 0; i < vecCategories.size(); i++)
   {
      for (size_t j = 0; j < vecCategories.size(); j++)
      {
         if ((vecCategories.at(i) == vecCategories.at(j)) && (i != j))
         {
            return false;
         }
      }
   }
   return true;
}

bool CParser::CheckUserOnlyEnteredXFields(int iNumOfFields)
{
   if (m_iNumDataFields != iNumOfFields)
   {
      return false;
   }
   return true;
}
