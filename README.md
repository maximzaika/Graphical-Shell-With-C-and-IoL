# Graphical Shell With IoL Console (Based on C Programming)

## Video Demonstration

[YouTube Video Demo](https://youtu.be/QPfs_uCqKdE)

## Brief Description

Very rudimentary command-line interpreter running in a ioL console with 
graphical shell.

## What Is An IoL?

An ioL program is fundamentally just an ordinary console-style program
like any other that can be written for operating environment. Instead of
plain-text characters, ioL outputs marked-up content intended to be rendered
into a graphical user interface on the screen. To learn more, refer to [ioL's 
documentation](http://doc.iol.science/). 

## Advanced Description

Displays current directory, its files, and the type of each file (colored 
icons), two buttons, and error handing window (whenever error
occurs). It accepts LMB click (used for file selection) and RMB click (on 
non-directory files, where the option such as copy, cut, and delete appears 
at the bottom of the application).

## How To Run

- Ensure that ioL is installed on your Linux machine. Unfortunately,  ioL is 
  not available for public use yet. It is Monash University's property and
  might be available for public use once it is stable and updated.
- Open terminal in the directory where `./task3shell.c` is located.
- Run the following command:
  - `gcc task3shell.c -o exec;iol -- ./exec`

## Type of Buttons & Their Purpose

| Button           | Key Bind    | Purpose     |
|  ---             |    ---      |   ---       |
| Orange           | LMB         | Indicates that the file is a directory. It updates current directory to the directory selected and then re-renders its files. If no access granted, the appropriate error is displayed in the error box (onError). |
| Green            | LMB or RMB  | Indicates that the file is an executable non-directory file. LMB - executes the file by running the program in a new 'xterm' terminal window. RMB - activates options such as copy, cut and paste at the bottom of the application. |
| Blue             | LMB or RMB  | Indicates that the file is an ordinary non-directory file (for example a text file). LMB -  displays the contents of specified text file in a new 'xterm' terminal window using 'less' utility. RMB - activates options such as copy, cut and paste at the bottom of the application. |
| Copy             | LMB         | Stores the copy of the file in the memory. Menu stays open until Paste Button is clicked. Doesn't remove the file after Paste Button is clicked. | 
| Cut              | LMB         | Similar to Copy, but removes the file after Paste Button is clicked. |
| Paste            | LMB         | If copy or cut is clicked, the file gets pasted, and if cut is clicked, then original file gets deleted. If the file exists at the directory path, then it gets renames to "copy of <filename>". At the end, this button closes the copy, cut, and paste menu. | 
| Back             | LMB         | ioL console sends the command to the system to go one folder back (return to the previous folder). |
| Parent Directory | LMB         | ioL console sends the command to the system, which indicates that the system needs to return to the main folder from where the program was executed. |
| Close            | LMB         | ioL console sends the commands to the system, which indicates the termination of the program. |

## Assumptions

| Assumption         | Description |
| ---                | ---         |
| Installed Software | ioL must be installed |
| Current Directory  | By default, the current directory is the same as the location of the task3shell file. |
| Copy or Cut File   | selected file for copying or cutting is not a directory; otherwise, the menu remains closed. |
| Paste              | Disappears if cut or copy buttons are not pressed. |


## Extra Features

| Feature    | Description |
| --- | --- |
| Orange, Green & Blue Buttons | Displays error message whenever access to the directory or a file is not granted |
| Orange & Blue Buttons | Allows the user to open many files concurrently. Shell doesn't freeze upon doing so. |
| Blue Button | To prevent shell from freezing, maximum file size allowed for selection must be not more than 500kb. More than that will display an error. |
| File Type   | File type can be differentiated via the icons colors: Orange, Blue, and Green. |
| Option Menu Event Handler | RMB clicking on some of the files displays additional menu that allows copying, cutting, and pasting files.
| Paste Button Permissions | Files keep their original permission after pasting. |

## Limitations

| Limitation | Description |
| --- | --- |
| Orange, Green & Blue Buttons | Shell is unable to access files that do not have granted permission |
| Blue Button | Files larger than 500kb cannot be opened. |
| Sort by File Type | Feature is not introduced. |
| Copy & Cut Buttons | Do not support copying and cutting on directories. RMB on directories has no actions. |

