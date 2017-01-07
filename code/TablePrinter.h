#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <math.h>

#ifdef __APPLE__
#include <cmath>
#endif

class CTablePrinter
{

public:

   CTablePrinter(std::ofstream &outFile, std::string strPrintOutPath);
   ~CTablePrinter();

   std::ofstream *outFile;
   std::string strPrintOutPath;

   std::vector<std::string> MyTitles; // Data fields
   std::vector<std::string> MyValues; // Values entered by the user
   std::vector<std::string> MyTotals; // Total associated with each data field for the month entered by the user

   int iNumOfFields;                  // Number of fields (currently harcoded inside the constructor)
   int iNumOfRows;                    // Number of days logged in the month entered by the user

   std::string sDate;                 // Date in the format xx/yy/zzzz
   int iDateSpace;                    // Length of the sDate string

   double dFinalTotal;                // Sumation of all the totals - all the money spent in the month entered by the user

   bool bBoldColor;                   // Used to swap the colors of the bars on the graph
   bool bPrintTotals;                 // Used to print the table containing all the totals after the main table

   int i;
   int j;
   int iBlankSpaceLength;
   std::vector<int> vecMaxValCharSize;
   std::vector<int> vecFieldLengths;
   std::string sCol, sVal;
   int iColLen, iValLen;

   void CalculateSpaceOccupiedByEachField();
   void CalculateTotals(int iInputDay, int iInputMonth, int iInputYear);
   void PrintTopBar();
   void PrintMiddleBar();
   void PrintBottomBar();
   void PrintFields();
   void PrintValues(int iRowOffset);
   bool PrintTotalsAndGraph();
   void PrintFieldsAndCommands(std::vector<std::string> MyFields, std::vector<std::string> MyShortFields);
   void PrintDates();
   void MadeBy();

   std::string HorizontalBar_Thick;
   std::string HorizontalBar_ConnectorDown;
   std::string HorizontalBar_ConnectorUp;

   std::string VerticalBar_Thick;
   std::string VerticalBar_ConnectorRight;
   std::string VerticalBar_ConnectorLeft;

   std::string TopLeftCorner;
   std::string TopRightCorner;
   std::string BottomLeftCorner;
   std::string BottomRightCorner;

   std::string HorizontalBar_Thin;
   std::string VerticalBar_Thin;
   std::string Intersection;

   std::string Cube;

private:

};

// Constructor: Reset the counters and parse the user input.
CTablePrinter::CTablePrinter(std::ofstream &outFile, std::string strPrintOutPath)
{
   
   this->outFile = &outFile;
   this->strPrintOutPath = strPrintOutPath;

   iNumOfRows = 0;
   bBoldColor = false;
   bPrintTotals = false;

   HorizontalBar_Thick = "\xE2\x95\x90"; //  ═
   HorizontalBar_ConnectorDown = "\xE2\x95\xA4"; //  ╤
   HorizontalBar_ConnectorUp = "\xE2\x95\xA7"; //  ╧

   VerticalBar_Thick = "\xE2\x95\x91"; //  ║
   VerticalBar_ConnectorRight = "\xE2\x95\x9F"; //  ╟
   VerticalBar_ConnectorLeft = "\xE2\x95\xA2"; //  ╢

   TopLeftCorner = "\xE2\x95\x94"; //  ╔
   TopRightCorner = "\xE2\x95\x97"; //  ╗
   BottomLeftCorner = "\xE2\x95\x9A"; //  ╚
   BottomRightCorner = "\xE2\x95\x9D"; //  ╝

   HorizontalBar_Thin = "\xE2\x94\x80"; //  ─
   VerticalBar_Thin = "\x7C"; //  |
   Intersection = "\xE2\x94\xBC"; //  ┼

   Cube = "\xE2\x96\xA0"; //  ■
}

// Destructor
CTablePrinter::~CTablePrinter()
{

}

void CTablePrinter::CalculateSpaceOccupiedByEachField()
{

   iDateSpace = 12;

   // Calculate which of the entries has the most characters
   int iCurrentValLen;

   for (i = 0; i < iNumOfFields; i++)
   {
      vecMaxValCharSize.push_back(MyValues.at(i).length());
   }

   for (i = 0; i < iNumOfFields; i++)
   {
      for (j = 0; j < iNumOfRows; j++)
      {
         iCurrentValLen = MyValues.at(i + (j * iNumOfFields)).length();
         vecMaxValCharSize.at(i) = std::max(vecMaxValCharSize.at(i), iCurrentValLen);
      }
   }

   // Begin by calculating the space occupied by each field
   for (i = 0; i < iNumOfFields; i++)
   {
      sCol = MyTitles.at(i);
      iColLen = sCol.length();
      iValLen = vecMaxValCharSize.at(i);

      if (iColLen < iValLen) // Size of the field must be equal to 12 characters
      {
         iBlankSpaceLength = iValLen - iColLen;
         if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
         vecFieldLengths.push_back(iBlankSpaceLength + iColLen + 2);
      }
      else
      {
         vecFieldLengths.push_back(iColLen + 2);
      }
   }

}

void CTablePrinter::CalculateTotals(int iInputDay, int iInputMonth, int iInputYear)
{
   double dTempTotal;
   std::stringstream ssTempTotal;

   dFinalTotal = 0;

   for (j = 3; j < iNumOfFields; j++)
   {
      dTempTotal = 0.0;
      ssTempTotal.str("");
      ssTempTotal.clear();
      for (i = j; i < (iNumOfFields * iNumOfRows); i += iNumOfFields)
      {
         dTempTotal += std::stod(MyValues.at(i));
      }

      dFinalTotal += dTempTotal;

      ssTempTotal << std::setprecision(12) << dTempTotal; // Billion dollar precision

      double intpart;
      if (modf(dTempTotal, &intpart) == 0)
      {
         MyTotals.push_back(ssTempTotal.str() + ".0");
      }
      else
      {
         MyTotals.push_back(ssTempTotal.str());
      }

   }

   MyValues.clear();
   vecMaxValCharSize.clear();
   vecFieldLengths.clear();
   iNumOfRows = 1;

   std::stringstream ssTime;

   ssTime << iInputDay;
   MyValues.push_back(ssTime.str());
   ssTime.str("");
   ssTime.clear();
   ssTime << iInputMonth;
   MyValues.push_back(ssTime.str());
   ssTime.str("");
   ssTime.clear();
   ssTime << iInputYear;
   MyValues.push_back(ssTime.str());

   for (j = 3; j < iNumOfFields; j++)
   {
      MyValues.push_back(MyTotals.at(j - 3));
   }

   bPrintTotals = true;

}

void CTablePrinter::PrintTopBar()
{
   // Print top bar
   for (i = 0; i < iNumOfFields; i++)
   {

      if (i == 0 || i == 1)
      {
         // Do nothing
      }
      else if (i == 2) // First
      {
         if (!bPrintTotals)
         {
            *outFile << "console.log(chalk.gray.bold('   " << TopLeftCorner; // ╔

            for (j = 0; j < iDateSpace; j++)
            {
               *outFile << HorizontalBar_Thick; // ═
            }
         }
         else
         {
            *outFile << "console.log(chalk.gray.bold('    ";

            for (j = 0; j < iDateSpace; j++)
            {
               *outFile << " ";
            }
         }
      }
      else if ((i == iNumOfFields - 1) && (i == 3)) // Special case (applies when the table only has 1 field)
      {
         if (!bPrintTotals)
         {
            *outFile << HorizontalBar_ConnectorDown; // ╤

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thick; // ═
            }
            *outFile << TopRightCorner << "'));\n"; // ╗
         }
         else
         {
            *outFile << TopLeftCorner; // ╔

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thick; // ═
            }
            *outFile << TopRightCorner << "'));\n"; // ╗
         }
      }
      else if (i == iNumOfFields - 1) // Last
      {
         *outFile << HorizontalBar_ConnectorDown; // ╤

         for (j = 0; j < vecFieldLengths.at(i); j++)
         {
            *outFile << HorizontalBar_Thick; // ═
         }
         *outFile << TopRightCorner << "'));\n"; // ╗
      }
      else // Middle
      {
         if (!bPrintTotals)
         {
            *outFile << HorizontalBar_ConnectorDown; // ╤

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thick; // ═
            }
         }
         else
         {
            if (i == 3)
            {
               *outFile << TopLeftCorner; // ╔
            }
            else
            {
               *outFile << HorizontalBar_ConnectorDown; // ╤
            }

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thick; // ═
            }
         }
      }
   }
}

void CTablePrinter::PrintMiddleBar()
{
   // Print middle bar
   for (i = 0; i < iNumOfFields; i++)
   {
      if (i == 0 || i == 1)
      {
         // Do nothing
      }
      else if (i == 2)
      {
         if (!bPrintTotals)
         {
            *outFile << "console.log(chalk.gray.bold('   " << VerticalBar_ConnectorRight;

            for (j = 0; j < iDateSpace; j++)
            {
               *outFile << HorizontalBar_Thin;
            }
         }
         else
         {
            *outFile << "console.log(chalk.gray.bold('   " << TopLeftCorner;

            for (j = 0; j < 10; j++)
            {
               *outFile << HorizontalBar_Thick;
            }
         }
      }
      else if ((i == iNumOfFields - 1) && (i == 3))
      {
         if (!bPrintTotals)
         {
            *outFile << Intersection;

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thin;
            }
            *outFile << VerticalBar_ConnectorLeft << "'));\n";
         }
         else
         {
            *outFile << TopRightCorner << " " << VerticalBar_ConnectorRight;

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thin;
            }
            *outFile << VerticalBar_ConnectorLeft << "'));\n";
         }
      }
      else if (i == iNumOfFields - 1)
      {
         *outFile << Intersection;

         for (j = 0; j < vecFieldLengths.at(i); j++)
         {
            *outFile << HorizontalBar_Thin;
         }
         *outFile << VerticalBar_ConnectorLeft << "'));\n";
      }
      else
      {
         if (!bPrintTotals)
         {
            *outFile << Intersection;

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thin;
            }
         }
         else
         {
            if (i == 3)
            {
               *outFile << TopRightCorner << " " << VerticalBar_ConnectorRight;

               for (j = 0; j < vecFieldLengths.at(i); j++)
               {
                  *outFile << HorizontalBar_Thin;
               }
            }
            else
            {
               *outFile << Intersection;

               for (j = 0; j < vecFieldLengths.at(i); j++)
               {
                  *outFile << HorizontalBar_Thin;
               }
            }
         }
      }
   }
}

void CTablePrinter::PrintBottomBar()
{
   // Print bottom bar
   for (i = 0; i < iNumOfFields; i++)
   {
      if (i == 0 || i == 1)
      {
         // Do nothing
      }
      else if (i == 2)
      {
         if (!bPrintTotals)
         {
            *outFile << "console.log(chalk.gray.bold('   " << BottomLeftCorner;
            for (j = 0; j < iDateSpace; j++)
            {
               *outFile << HorizontalBar_Thick;
            }
         }
         else
         {
            *outFile << "console.log(chalk.gray.bold('   " << BottomLeftCorner;
            for (j = 0; j < 10; j++)
            {
               *outFile << HorizontalBar_Thick;
            }
         }
      }
      else if ((i == iNumOfFields - 1) && (i == 3))
      {
         if (!bPrintTotals)
         {
            *outFile << HorizontalBar_ConnectorUp;
            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thick;
            }
            *outFile << BottomRightCorner << "'));\n";

            PrintTotalsAndGraph();
         }
         else
         {
            *outFile << BottomRightCorner << " " << BottomLeftCorner;

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thick;
            }
            *outFile << BottomRightCorner << "'));\n";

            PrintTotalsAndGraph();
         }
      }
      else if (i == iNumOfFields - 1)
      {
         *outFile << HorizontalBar_ConnectorUp;
         for (j = 0; j < vecFieldLengths.at(i); j++)
         {
            *outFile << HorizontalBar_Thick;
         }
         *outFile << BottomRightCorner << "'));\n";

         PrintTotalsAndGraph();

      }
      else
      {
         if (!bPrintTotals)
         {
            *outFile << HorizontalBar_ConnectorUp;

            for (j = 0; j < vecFieldLengths.at(i); j++)
            {
               *outFile << HorizontalBar_Thick;
            }
         }
         else
         {
            if (i == 3)
            {
               *outFile << BottomRightCorner << " " << BottomLeftCorner;

               for (j = 0; j < vecFieldLengths.at(i); j++)
               {
                  *outFile << HorizontalBar_Thick;
               }
            }
            else
            {
               *outFile << HorizontalBar_ConnectorUp;

               for (j = 0; j < vecFieldLengths.at(i); j++)
               {
                  *outFile << HorizontalBar_Thick;
               }
            }
         }
      }
   }
}

void CTablePrinter::PrintFields()
{
   // Print fields
   for (i = 0; i < iNumOfFields; i++)
   {
      sCol = MyTitles.at(i);
      if (i == 0 || i == 1)
      {
         // Do nothing
      }
      else if (i == 2) // First field
      {
         if (!bPrintTotals)
         {
            iBlankSpaceLength = iDateSpace - 4;
            if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
            std::string sBlankSpace(iBlankSpaceLength / 2, ' ');

            *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
            *outFile << "process.stdout.write(chalk.yellow.bold('" << sBlankSpace.c_str() << "Date" << sBlankSpace.c_str() << "'));\n";
         }
         else
         {
            iBlankSpaceLength = iDateSpace - 4;
            if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
            std::string sBlankSpace(iBlankSpaceLength / 2, ' ');

            *outFile << "process.stdout.write('    ');\n";
            *outFile << "process.stdout.write(chalk.yellow.bold('" << sBlankSpace.c_str() << "    " << sBlankSpace.c_str() << "'));\n";
         }
      }
      else if ((i == iNumOfFields - 1) && (i == 3)) // Special case (applies when the table only has 1 field)
      {
         if (!bPrintTotals)
         {
            iBlankSpaceLength = vecFieldLengths.at(i) - sCol.length();
            if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
            std::string sBlankSpace(iBlankSpaceLength / 2, ' ');

            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";
            *outFile << "process.stdout.write(chalk.cyan.bold('" << sBlankSpace.c_str() << MyTitles.at(i) << sBlankSpace.c_str() << "'));\n";
            *outFile << "console.log(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
         }
         else
         {
            iBlankSpaceLength = vecFieldLengths.at(i) - sCol.length();
            if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
            std::string sBlankSpace(iBlankSpaceLength / 2, ' ');

            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
            *outFile << "process.stdout.write(chalk.cyan.bold('" << sBlankSpace.c_str() << MyTitles.at(i) << sBlankSpace.c_str() << "'));\n";
            *outFile << "console.log(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
         }
      }
      else if (i == iNumOfFields - 1) // Last field
      {
         iBlankSpaceLength = vecFieldLengths.at(i) - sCol.length();
         if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
         std::string sBlankSpace(iBlankSpaceLength / 2, ' ');

         *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";
         *outFile << "process.stdout.write(chalk.cyan.bold('" << sBlankSpace.c_str() << MyTitles.at(i) << sBlankSpace.c_str() << "'));\n";
         *outFile << "console.log(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";

      }
      else // Middle field
      {
         if (!bPrintTotals)
         {
            iBlankSpaceLength = vecFieldLengths.at(i) - sCol.length();
            if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
            std::string sBlankSpace(iBlankSpaceLength / 2, ' ');

            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";
            *outFile << "process.stdout.write(chalk.cyan.bold('" << sBlankSpace.c_str() << MyTitles.at(i) << sBlankSpace.c_str() << "'));\n";
         }
         else
         {
            if (i == 3)
            {
               iBlankSpaceLength = vecFieldLengths.at(i) - sCol.length();
               if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
               std::string sBlankSpace(iBlankSpaceLength / 2, ' ');

               *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
               *outFile << "process.stdout.write(chalk.cyan.bold('" << sBlankSpace.c_str() << MyTitles.at(i) << sBlankSpace.c_str() << "'));\n";
            }
            else
            {
               iBlankSpaceLength = vecFieldLengths.at(i) - sCol.length();
               if (iBlankSpaceLength % 2 != 0) { iBlankSpaceLength++; }
               std::string sBlankSpace(iBlankSpaceLength / 2, ' ');

               *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";
               *outFile << "process.stdout.write(chalk.cyan.bold('" << sBlankSpace.c_str() << MyTitles.at(i) << sBlankSpace.c_str() << "'));\n";
            }
         }
      }
   }
}

void CTablePrinter::PrintValues(int iRowOffset)
{
   // Print values
   int iFieldLimit = 0;
   for (i = iRowOffset*iNumOfFields; iFieldLimit < iNumOfFields; i++)
   {

      if (iFieldLimit == 0 || iFieldLimit == 1)
      {
         // Do nothing
      }
      else if (iFieldLimit == 2) // First field
      {
         if (!bPrintTotals)
         {
            std::string sValue = MyValues.at(i);
            *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
            std::string sBlankSpace(iDateSpace - 10 - 1, ' ');

            if (std::stoi(MyValues.at(i - 2)) < 10) { MyValues.at(i - 2) = '0' + MyValues.at(i - 2); }
            if (std::stoi(MyValues.at(i - 1)) < 10) { MyValues.at(i - 1) = '0' + MyValues.at(i - 1); }

            if (bBoldColor) {
               *outFile << "process.stdout.write(chalk.white.bold('" << " " << MyValues.at(i - 2) << '/' << MyValues.at(i - 1) << '/' << MyValues.at(i) << sBlankSpace.c_str() << "'));\n";
               *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";
            }
            else
            {
               *outFile << "process.stdout.write(chalk.white('" << " " << MyValues.at(i - 2) << '/' << MyValues.at(i - 1) << '/' << MyValues.at(i) << sBlankSpace.c_str() << "'));\n";
               *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";
            }
         }
         else
         {
            std::string sValue = MyValues.at(i);
            *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
            std::string sBlankSpace(iDateSpace - 10 - 1, ' ');

            if (std::stoi(MyValues.at(i - 2)) < 10) { MyValues.at(i - 2) = '0' + MyValues.at(i - 2); }
            if (std::stoi(MyValues.at(i - 1)) < 10) { MyValues.at(i - 1) = '0' + MyValues.at(i - 1); }

            *outFile << "process.stdout.write(chalk.yellow.bold('" << "  " << "Totals" << "  " << "'));\n";
            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << " '));\n";
            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
         }
      }
      else if (iFieldLimit == iNumOfFields - 1) // Last field
      {
         std::string sValue = MyValues.at(i);

         if (std::stod(sValue) == 0.0)
         {
            sValue = "0  ";
            std::string sBlankSpace(vecFieldLengths.at(iFieldLimit) - sValue.length() - 1, ' ');
            *outFile << "process.stdout.write(chalk.green('" << " " << sValue.c_str() << sBlankSpace.c_str() << "'));\n";
            *outFile << "console.log(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
         }
         else
         {
            std::string sBlankSpace(vecFieldLengths.at(iFieldLimit) - sValue.length() - 1, ' ');
            *outFile << "process.stdout.write(chalk.green.bold('" << " " << sValue.c_str() << sBlankSpace.c_str() << "'));\n";
            *outFile << "console.log(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
         }
      }
      else
      {
         std::string sValue = MyValues.at(i);

         if (std::stod(sValue) == 0.0)
         {
            sValue = "0  ";
            std::string sBlankSpace(vecFieldLengths.at(iFieldLimit) - sValue.length() - 1, ' ');
            *outFile << "process.stdout.write(chalk.green('" << " " << sValue.c_str() << sBlankSpace.c_str() << "'));\n";
            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";
         }
         else
         {
            std::string sBlankSpace(vecFieldLengths.at(iFieldLimit) - sValue.length() - 1, ' ');
            *outFile << "process.stdout.write(chalk.green.bold('" << " " << sValue.c_str() << sBlankSpace.c_str() << "'));\n";
            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";
         }
      }
      iFieldLimit++;
   }

   if (bBoldColor)
   {
      bBoldColor = false;
   }
   else
   {
      bBoldColor = true;
   }

}

bool CTablePrinter::PrintTotalsAndGraph()
{
   if (bPrintTotals)
   {
      // Determine the largest total
      double dMyMaxVal, dTempMaxCandidate;
      dMyMaxVal = std::stod(MyTotals.at(0));

      for (size_t k = 1; k < MyTotals.size(); k++)
      {
         dTempMaxCandidate = std::stod(MyTotals.at(k));
         if (dTempMaxCandidate > dMyMaxVal) {
            dMyMaxVal = dTempMaxCandidate;
         }
      }

      // Determine the length of each title
      int MaxLength, TempLength;
      MaxLength = MyTitles.at(3).length();

      std::vector<int> vecTitleLenghts;
      vecTitleLenghts.push_back(MyTitles.at(3).length());

      for (int k = 4; k < iNumOfFields; k++)
      {
         TempLength = MyTitles.at(k).length();
         if (TempLength > MaxLength)
         {
            MaxLength = TempLength;
         }
         vecTitleLenghts.push_back(TempLength);
      }

      if (MaxLength < 14)
      {
         MaxLength = 14;
      }

      std::vector<std::string> vecGraphValues;
      vecGraphValues.push_back("           0.1       0.2       0.3       0.4       0.5       0.6       0.7       0.8       0.9        1");
      vecGraphValues.push_back("            1         2         3         4         5         6         7         8         9         10");
      vecGraphValues.push_back("            10        20        30        40        50        60        70        80        90        100");
      vecGraphValues.push_back("           100       200       300       400       500       600       700       800       900       1000");
      vecGraphValues.push_back("           1000      2000      3000      4000      5000      6000      7000      8000      9000      10000");
      vecGraphValues.push_back("          10000     20000     30000     40000     50000     60000     70000     80000     90000     100000");
      vecGraphValues.push_back("          100000    200000    300000    400000    500000    600000    700000    800000    900000    1000000");
      vecGraphValues.push_back("         1000000   2000000   3000000   4000000   5000000   6000000   7000000   8000000   9000000   10000000");
      vecGraphValues.push_back("         10000000  20000000  30000000  40000000  50000000  60000000  70000000  80000000  90000000  100000000");
      vecGraphValues.push_back("        100000000 200000000 300000000 400000000 500000000 600000000 700000000 800000000 900000000 1000000000");

      int BoxSize, GraphIndex;
      if (dMyMaxVal < 1) { BoxSize = 1;        GraphIndex = 0; }
      else if (dMyMaxVal < 10) { BoxSize = 1;        GraphIndex = 1; }
      else if (dMyMaxVal < 100) { BoxSize = 1;        GraphIndex = 2; }
      else if (dMyMaxVal < 1000) { BoxSize = 10;       GraphIndex = 3; }
      else if (dMyMaxVal < 10000) { BoxSize = 100;      GraphIndex = 4; }
      else if (dMyMaxVal < 100000) { BoxSize = 1000;     GraphIndex = 5; }
      else if (dMyMaxVal < 1000000) { BoxSize = 10000;    GraphIndex = 6; }
      else if (dMyMaxVal < 10000000) { BoxSize = 100000;   GraphIndex = 7; }
      else if (dMyMaxVal < 100000000) { BoxSize = 1000000;  GraphIndex = 8; }
      else if (dMyMaxVal < 1000000000) { BoxSize = 10000000; GraphIndex = 9; }
      else
      {
         // Do not print graph is the maximum value is greater than or equal to 1 billion
         return false;
      }

      double dNumOfBoxes;
      double dMyTempVal;

      bool bBoldOk = true;
      if (dMyMaxVal < 1)
      {
         for (int e = 0; e < iNumOfFields - 3; e++)
         {
            std::string sBlankSpace(MaxLength - vecTitleLenghts.at(e), ' ');

            *outFile << "console.log('');\n";
            *outFile << "process.stdout.write(chalk.cyan.bold('" << " " << sBlankSpace.c_str() << MyTitles.at(e + 3) << " '" << "));\n";
            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << " '));\n";

            dMyTempVal = std::stod(MyTotals.at(e)) * 100;
            dMyTempVal /= BoxSize;
            dMyTempVal = std::floor(dMyTempVal + 0.5) * BoxSize;

            dNumOfBoxes = dMyTempVal / BoxSize;

            for (int g = 0; g < dNumOfBoxes; g++)
            {
               if (bBoldOk)
               {
                  if (g == 0 && g == (dNumOfBoxes - 1))
                  {
                     *outFile << "process.stdout.write(chalk.green.bold('" << Cube << "'));\n";
                     bBoldOk = false;
                  }
                  else if (g == 0) // First
                  {
                     *outFile << "process.stdout.write(chalk.green.bold('" << Cube;
                  }
                  else if (g == dNumOfBoxes - 1) // Last
                  {
                     *outFile << Cube << "'));\n";
                     bBoldOk = false;
                  }
                  else
                  {
                     *outFile << Cube;
                  }
               }
               else
               {
                  if (g == 0 && g == (dNumOfBoxes - 1))
                  {
                     *outFile << "process.stdout.write(chalk.green('" << Cube << "'));\n";
                     bBoldOk = true;
                  }
                  else if (g == 0) // First
                  {
                     *outFile << "process.stdout.write(chalk.green('" << Cube;
                  }
                  else if (g == dNumOfBoxes - 1) // Last
                  {
                     *outFile << Cube << "'));\n";
                     bBoldOk = true;
                  }
                  else
                  {
                     *outFile << Cube;
                  }
               }
            }
         }
      }
      else if (dMyMaxVal < 10)
      {
         for (int e = 0; e < iNumOfFields - 3; e++)
         {
            std::string sBlankSpace(MaxLength - vecTitleLenghts.at(e), ' ');

            *outFile << "console.log('');\n";
            *outFile << "process.stdout.write(chalk.cyan.bold('" << " " << sBlankSpace.c_str() << MyTitles.at(e + 3) << " '" << "));\n";
            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << " " << "'));\n";

            dMyTempVal = std::stod(MyTotals.at(e)) * 10;
            dMyTempVal /= BoxSize;
            dMyTempVal = std::floor(dMyTempVal + 0.5) * BoxSize;

            dNumOfBoxes = dMyTempVal / BoxSize;

            for (int g = 0; g < dNumOfBoxes; g++)
            {
               if (bBoldOk)
               {
                  if (g == 0 && g == (dNumOfBoxes - 1))
                  {
                     *outFile << "process.stdout.write(chalk.green.bold('" << Cube << "'));\n";
                     bBoldOk = false;
                  }
                  else if (g == 0) // First
                  {
                     *outFile << "process.stdout.write(chalk.green.bold('" << Cube;
                  }
                  else if (g == dNumOfBoxes - 1) // Last
                  {
                     *outFile << Cube << "'));\n";
                     bBoldOk = false;
                  }
                  else
                  {
                     *outFile << Cube;
                  }
               }
               else
               {
                  if (g == 0 && g == (dNumOfBoxes - 1))
                  {
                     *outFile << "process.stdout.write(chalk.green('" << Cube << "'));\n";
                     bBoldOk = true;
                  }
                  else if (g == 0) // First
                  {
                     *outFile << "process.stdout.write(chalk.green('" << Cube;
                  }
                  else if (g == dNumOfBoxes - 1) // Last
                  {
                     *outFile << Cube << "'));\n";
                     bBoldOk = true;
                  }
                  else
                  {
                     *outFile << Cube;
                  }
               }
            }
         }
      }
      else
      {
         for (int e = 0; e < iNumOfFields - 3; e++)
         {

            std::string sBlankSpace(MaxLength - vecTitleLenghts.at(e), ' ');

            *outFile << "console.log('');\n";
            *outFile << "process.stdout.write(chalk.cyan.bold('" << " " << sBlankSpace.c_str() << MyTitles.at(e + 3) << " '" << "));\n";
            *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << " '));\n";

            dMyTempVal = std::stod(MyTotals.at(e));
            dMyTempVal /= BoxSize;
            dMyTempVal = std::floor(dMyTempVal + 0.5) * BoxSize;

            dNumOfBoxes = dMyTempVal / BoxSize;

            for (int g = 0; g < dNumOfBoxes; g++)
            {
               if (bBoldOk)
               {
                  if (g == 0 && g == (dNumOfBoxes - 1))
                  {
                     *outFile << "process.stdout.write(chalk.green.bold('" << Cube << "'));\n";
                     bBoldOk = false;
                  }
                  else if (g == 0) // First
                  {
                     *outFile << "process.stdout.write(chalk.green.bold('" << Cube;
                  }
                  else if (g == dNumOfBoxes - 1) // Last
                  {
                     *outFile << Cube << "'));\n";
                     bBoldOk = false;
                  }
                  else
                  {
                     *outFile << Cube;
                  }
               }
               else
               {
                  if (g == 0 && g == (dNumOfBoxes - 1))
                  {
                     *outFile << "process.stdout.write(chalk.green('" << Cube << "'));\n";
                     bBoldOk = true;
                  }
                  else if (g == 0) // First
                  {
                     *outFile << "process.stdout.write(chalk.green('" << Cube;
                  }
                  else if (g == dNumOfBoxes - 1) // Last
                  {
                     *outFile << Cube << "'));\n";
                     bBoldOk = true;
                  }
                  else
                  {
                     *outFile << Cube;
                  }
               }
            }
         }
      }

      std::string sBlankSpace(MaxLength + 2, ' ');

      *outFile << "console.log('');\n";

      *outFile << "console.log(chalk.gray.bold('" << sBlankSpace.c_str() << BottomLeftCorner << HorizontalBar_Thick << HorizontalBar_Thick;
      for (int i = 1; i <= 10; i++)
      {
         for (int j = 1; j <= 10; j++)
         {
            if (j <= 9)
            {
               *outFile << HorizontalBar_Thick;
            }
            else
            {
               *outFile << HorizontalBar_ConnectorUp;
            }
         }
      }
      *outFile << "'));\n";

      *outFile << "console.log('" << sBlankSpace << vecGraphValues.at(GraphIndex) << "');\n";

      std::stringstream ssTemporary;
      ssTemporary << std::setprecision(12) << dFinalTotal;

      *outFile << "console.log(chalk.gray.bold('   " << TopLeftCorner;
      for (int i = 1; i <= 10; i++)
      {
         *outFile << HorizontalBar_Thick;
      }
      *outFile << TopRightCorner << "'));\n";

      *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
      *outFile << "process.stdout.write(chalk.yellow.bold('  Global  '));\n";
      *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
      *outFile << "console.log(chalk.green.bold('   " << ssTemporary.str() << "'));\n";

      *outFile << "console.log(chalk.gray.bold('   " << BottomLeftCorner;
      for (int i = 1; i <= 10; i++)
      {
         *outFile << HorizontalBar_Thick;
      }
      *outFile << BottomRightCorner << "'));\n";

   }

   // Return true if everything went well.
   return true;
}

void CTablePrinter::PrintFieldsAndCommands(std::vector<std::string> MyFields, std::vector<std::string> MyShortFields)
{
   int iPrevLen = 0;
   int iCurrLen = 0;
   int iMaxLenField = 10;

   for (size_t i = 0; i < MyFields.size(); i++)
   {
      iCurrLen = MyFields.at(i).length();

      if (iCurrLen > iMaxLenField)
      {
         iMaxLenField = iCurrLen;
      }

      iPrevLen = iCurrLen;
   }

   iPrevLen = 0;
   iCurrLen = 0;
   int iMaxLenCommand = 5;

   for (size_t i = 0; i < MyShortFields.size(); i++)
   {
      iCurrLen = MyShortFields.at(i).length();

      if (iCurrLen > iMaxLenCommand)
      {
         iMaxLenCommand = iCurrLen;
      }

      iPrevLen = iCurrLen;
   }

   bool bBold = true;

   *outFile << "const chalk = require('chalk');\n";

   *outFile << "console.log(chalk.gray.bold('   " << TopLeftCorner; // ╔
   for (j = 0; j < iMaxLenField + 2; j++)
   {
      *outFile << HorizontalBar_Thick; // ═
   }
   *outFile << HorizontalBar_ConnectorDown;
   for (j = 0; j < iMaxLenCommand + 2; j++)
   {
      *outFile << HorizontalBar_Thick; // ═
   }
   *outFile << TopRightCorner << "'));\n"; // ╗

   int iFieldSetter;
   int iCommandSetter;

   for (size_t i = 0; i < MyFields.size(); i++)
   {
      if (i == 0)
      {
         if (iMaxLenField - 10 == 0)
         {
            iFieldSetter = 1;
         }
         else
         {
            iFieldSetter = iMaxLenField - 10 + 1;
         }

         if (iMaxLenCommand - 5 == 0)
         {
            iCommandSetter = 1;
         }
         else
         {
            iCommandSetter = iMaxLenCommand - 5 + 1;
         }

         std::string sBlankSpace1(iFieldSetter, ' ');
         std::string sBlankSpace2(iCommandSetter, ' ');

         *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
         *outFile << "process.stdout.write(chalk.cyan.bold(' Categories" << sBlankSpace1 << "'));\n";
         *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";

         *outFile << "process.stdout.write(chalk.cyan.bold(' Codes" << sBlankSpace2 << "'));\n";
         *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";
         *outFile << "console.log("");\n";

         *outFile << "console.log(chalk.gray.bold('   " << VerticalBar_ConnectorRight;
         for (j = 0; j < iMaxLenField + 2; j++)
         {
            *outFile << HorizontalBar_Thin; // ═
         }
         *outFile << Intersection;
         for (j = 0; j < iMaxLenCommand + 2; j++)
         {
            *outFile << HorizontalBar_Thin; // ═
         }
         *outFile << VerticalBar_ConnectorLeft << "'));\n";

      }

      iFieldSetter = iMaxLenField - MyFields.at(i).length() + 1;
      iCommandSetter = iMaxLenCommand - MyShortFields.at(i).length() + 1;

      std::string sBlankSpace1(iFieldSetter, ' ');
      std::string sBlankSpace2(iCommandSetter, ' ');

      *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";

      if (bBold)
      {
         *outFile << "process.stdout.write(chalk.green.bold(' " << MyFields.at(i) << sBlankSpace1 << "'));\n";
      }
      else
      {
         *outFile << "process.stdout.write(chalk.green(' " << MyFields.at(i) << sBlankSpace1 << "'));\n";
      }

      *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thin << "'));\n";

      if (bBold)
      {
         *outFile << "process.stdout.write(chalk.green.bold(' " << MyShortFields.at(i) << sBlankSpace2 << "'));\n";
         bBold = false;
      }
      else
      {
         *outFile << "process.stdout.write(chalk.green(' " << MyShortFields.at(i) << sBlankSpace2 << "'));\n";
         bBold = true;
      }

      *outFile << "process.stdout.write(chalk.gray.bold('" << VerticalBar_Thick << "'));\n";

      if (i != MyFields.size() - 1)
      {
         *outFile << "console.log("");\n";
         *outFile << "console.log(chalk.gray.bold('   " << VerticalBar_ConnectorRight;
         for (j = 0; j < iMaxLenField + 2; j++)
         {
            *outFile << HorizontalBar_Thin; // ═
         }
         *outFile << Intersection;
         for (j = 0; j < iMaxLenCommand + 2; j++)
         {
            *outFile << HorizontalBar_Thin; // ═
         }
         *outFile << VerticalBar_ConnectorLeft << "'));\n";
      }
   }

   *outFile << "console.log("");\n";
   *outFile << "console.log(chalk.gray.bold('   " << BottomLeftCorner;
   for (j = 0; j < iMaxLenField + 2; j++)
   {
      *outFile << HorizontalBar_Thick; // ═
   }
   *outFile << HorizontalBar_ConnectorUp;
   for (j = 0; j < iMaxLenCommand + 2; j++)
   {
      *outFile << HorizontalBar_Thick; // ═
   }
   *outFile << BottomRightCorner << "'));\n";

   (*outFile).close();
   std::string strNodeCommand = "node " + strPrintOutPath;
   system(strNodeCommand.c_str());

   std::cout << '\n';

}

void CTablePrinter::PrintDates()
{

   bool bBold = true;

   *outFile << "const chalk = require('chalk');\n";

   *outFile << "console.log(chalk.gray.bold('   " << TopLeftCorner;
   for (j = 0; j < 9; j++)
   {
      *outFile << HorizontalBar_Thick; // ═
   }
   *outFile << TopRightCorner << "'));\n";

   for (size_t i = 0; i < MyValues.size() - 1; i += 2)
   {
      if (i == 0)
      {
         *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
         *outFile << "process.stdout.write(chalk.cyan.bold('  Dates '));\n";
         *outFile << "process.stdout.write(chalk.gray.bold(' " << VerticalBar_Thick << "'));\n";

         *outFile << "console.log("");\n";
         *outFile << "console.log(chalk.gray.bold('   " << VerticalBar_ConnectorRight;
         for (j = 0; j < 9; j++)
         {
            *outFile << HorizontalBar_Thin; // ═
         }
         *outFile << VerticalBar_ConnectorLeft << "'));\n";

      }

      if (std::stoi(MyValues.at(i)) < 10)
      {
         if (bBold)
         {
            *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
            *outFile << "process.stdout.write(chalk.green.bold(' 0" << MyValues.at(i) << '/' << MyValues.at(i + 1) << "'));\n";
            *outFile << "process.stdout.write(chalk.gray.bold(' " << VerticalBar_Thick << "'));\n";
         }
         else
         {
            *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
            *outFile << "process.stdout.write(chalk.green(' 0" << MyValues.at(i) << '/' << MyValues.at(i + 1) << "'));\n";
            *outFile << "process.stdout.write(chalk.gray.bold(' " << VerticalBar_Thick << "'));\n";
         }
      }
      else
      {
         if (bBold)
         {
            *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
            *outFile << "process.stdout.write(chalk.green.bold(' " << MyValues.at(i) << '/' << MyValues.at(i + 1) << "'));\n";
            *outFile << "process.stdout.write(chalk.gray.bold(' " << VerticalBar_Thick << "'));\n";
         }
         else
         {
            *outFile << "process.stdout.write(chalk.gray.bold('   " << VerticalBar_Thick << "'));\n";
            *outFile << "process.stdout.write(chalk.green(' " << MyValues.at(i) << '/' << MyValues.at(i + 1) << "'));\n";
            *outFile << "process.stdout.write(chalk.gray.bold(' " << VerticalBar_Thick << "'));\n";
         }
      }

      if (i != MyValues.size() - 2)
      {
         *outFile << "console.log("");\n";
         *outFile << "console.log(chalk.gray.bold('   " << VerticalBar_ConnectorRight;
         for (j = 0; j < 9; j++)
         {
            *outFile << HorizontalBar_Thin; // ═
         }
         *outFile << VerticalBar_ConnectorLeft << "'));\n";
      }

      if (bBold)
      {
         bBold = false;
      }
      else
      {
         bBold = true;
      }

   }

   *outFile << "console.log("");\n";
   *outFile << "console.log(chalk.gray.bold('   " << BottomLeftCorner;
   for (j = 0; j < 9; j++)
   {
      *outFile << HorizontalBar_Thick; // ═
   }
   *outFile << BottomRightCorner << "'));\n";

   (*outFile).close();
   std::string strNodeCommand = "node " + strPrintOutPath;
   system(strNodeCommand.c_str());

   std::cout << '\n';
}

void CTablePrinter::MadeBy()
{
   *outFile << "const chalk = require('chalk');\n";

   if ((*outFile).is_open())
   {
      *outFile << "console.log(chalk.yellow.bold('   " << Cube << "'));\n";

      *outFile << "process.stdout.write(chalk.blue.bold('   " << Cube << "'));\n";

      *outFile << "console.log(chalk.white('   Written with pride by Diaggio Enrico Mangianetti Bellafonte.'));\n";

      *outFile << "process.stdout.write(chalk.red.bold('   " << Cube << "'));\n";

      *outFile << "console.log("");\n";
   }

   std::cout << '\n';

   (*outFile).close();
   std::string strNodeCommand = "node " + strPrintOutPath;
   system(strNodeCommand.c_str());

   std::cout << '\n';
}
