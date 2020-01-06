<p align="center">
  <img src="https://github.com/diegomacario/Poor-Fox/blob/master/readme_images/poor_fox_ascii_logo.png"/>
</p>

# Poor Fox

A beautiful command-line application designed to help you keep track of your expenses.

<p align="center">
  <img src="https://github.com/diegomacario/Poor-Fox/blob/master/readme_images/introductory_gif.gif"/>
</p>

## Features 

- Create your own expense categories.
- Log your expenses using the 3-letter codes generated by the application.
- Visualize the total you spend on each category through monthly reports.
- Add new expense categories to an existing database.

## Technical details

- Supported on macOS, Linux and Windows.
- Data is stored locally using [SQLite](https://www.sqlite.org/index.html).
- Terminal string styling is done using [Chalk](https://github.com/chalk/chalk).

## Installation

### Requirements 

Have Node.js, npm and Chalk installed in your system. 

- To install Node.js and npm in __macOS__ you can follow these [instructions](http://blog.teamtreehouse.com/install-node-js-npm-mac).
- To install Node.js and npm in __Linux__ you can execute the following commands:
```sh
 $ sudo apt-get update
 $ sudo apt-get install nodejs
 $ sudo apt-get install npm
```
- To install Node.js and npm in __Windows__ you can follow these [instructions](http://blog.teamtreehouse.com/install-node-js-npm-windows).

To install Chalk you can then execute the following command:
```sh
 $ npm install --save chalk
```
 
### macOS

To install the application globally, execute the command below.
 ```sh
 $ curl -s -L https://github.com/diegomacario/Poor-Fox/raw/master/installer/unix_installer.sh | sudo bash -s macos
 ```
This command downloads the binary _pfox_macOS_ included in release [1.0.0](https://github.com/diegomacario/Poor-Fox/releases) and places it in your _/usr/local/bin/_ folder.

### Linux

To install the application globally, execute the command below.
 ```sh
 $ curl -s -L https://github.com/diegomacario/Poor-Fox/raw/master/installer/unix_installer.sh | sudo bash -s linux
 ```
This command downloads the binary _pfox_linux_ included in release [1.0.0](https://github.com/diegomacario/Poor-Fox/releases) and places it in your _/usr/local/bin/_ folder. If you do not have Curl installed, execute the following command first:
 ```sh
 $ sudo apt-get install curl
 ```

### Windows

To install the application globally, download the executable named _pfox_windows.exe_ included in release [1.0.0](https://github.com/diegomacario/Poor-Fox/releases), rename it as _pfox.exe_ and place it in a folder that's on the PATH environment variable.

## Usage

__Note__: The application features clear error messages that will guide you when you make mistakes.

__1.__ Create your own expense categories:
 ```sh
 $ pfox new -Groceries -Restaurants -Movies -Shows -Clothes -Books -Photography -Climbing
 ```
 - The application automatically generates codes that you can use to log expenses into each category. 
 - Codes normally consist of the first 3 letters of each category.
 - Don't worry if 2 or more categories start with the same 3 letters (e.g. -Transportation -Travelling), the application will always generate unique codes.  

__2.__ Log the money you spent on a particular day by specifying its date in the format _day/month/year_:
 ```sh
 $ pfox log -d=1/12/2016 -gro=64.32 -boo=36.22 -mov=12.25
 
 $ pfox log -d=16/12/2016 -mov=12.25 -cli=21.34
 ```
 <p align="center">
  <img src="https://github.com/diegomacario/Poor-Fox/blob/master/readme_images/expense_report.png"/>
</p>
 
 - The application automatically displays an expense report for the month you specified.
    - The table at the top displays the expenses that have been logged into each category separated by day.
    - The second table and the bar graph display the total spent in each category.
    - The global value at the bottom is the sum of all the totals.
 - If you do not specify a date, today's date is selected by default.
 - You can log expenses using the same date more than once. The expenses you enter will be added to the existing ones.
 - You can log negative values to make corrections when you enter incorrect amounts.
 - Dates are sorted automatically.
 
__3.__ Add new expense categories to an existing database:
 ```sh
 $ pfox add -Skydiving -Dancing
 ```
  - Useful for when you pick up new hobbies and activities!
  
__4.__ Display the expense report of a particular month by specifying its date in the format _month/year_:
 ```sh
 $ pfox tab -d=12/2016
 ```

__5.__ Display the months in which you have logged expenses:
 ```sh
 $ pfox dat
 ```

__6.__ Display the expense categories included in your database and their codes:
 ```sh
 $ pfox cat
 ```
 
__7.__ Delete your database so that you can create a new one:
 ```sh
 $ pfox reset
 ```
 
 __8.__ See examples of how to use the application:
 ```sh
 $ pfox help
 ```
 
  __9.__ See what version of the application you are using:
 ```sh
 $ pfox version
 ```

## Why should you use it?

Just like this application, your bank generates monthly reports of your expenses. The big difference between the two is that your bank generates those reports without any effort from you, while this application requires you to enter your expenses manually. That might seem like a lot of work, but I consider it to be a benefit because it will help you be more conscious about your spending, which is something that a report that you look at once a month will never do. 

Log your expenses for a couple of months and you'll find all the categories in which you can improve. You will be surprised! Then use the money you save to learn or experience something new.

<p align="center">
  <img src="https://github.com/diegomacario/Poor-Fox/blob/master/readme_images/captain_crocodile.png"/>
  <em>You planning all the fun things you are going to do.</em>
</p>

## Why is it called Poor Fox?

<p align="center">
  <img src="https://github.com/diegomacario/Poor-Fox/blob/master/readme_images/fantastic_mr_fox.PNG"/>
</p>
<p align="center">
  <strong><em>Mr. Fox</strong>: I don't want to live in a hole anymore. It makes me feel poor.</em><br />
  <strong><em>Mrs. Fox</strong>: We are poor -- but we're happy.</em>
</p>

If you haven't watched _Fantastic Mr. Fox_ yet, make time in your schedule to watch it with your loved ones. You will feel elated!
