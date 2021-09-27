/*Task Number: Task 3
  Unit code:   FIT2100 - Operating Systems
  Description: Upon execution, the IOL UI and the main programs get executed,
               which provides a user friendly UI for the user, and allows the
               system to push the files and its type to the IOL UI.

               System features:
                System reads the commands from IOL UI to perform various
                manipulations. As well as differentiates the type of a file.
                List of commands:
                   d:<directory> -> changes current working directory to
                                    <directory>, and re-prints the file of
                                    the new directory location.
                   x:<filename>  -> executes specified <filename> ny running
                                    the program in a new 'xterm' terminal
                                    window. Shell doesn't wait for this
                                    command to be fully executed, user is
                                    allowed to control the shell while this
                                    command is running
                   v:<filename>  -> displays the contents of text file
                                    <filename> in a new 'xterm' terminal
                                    window using 'less' utility.
                   o:<filename>  -> launches the options: copy, cut, and paste
                                    at the bottom of the program. Whenever the
                                    option is not selected, and another file is
                                    clicked, the option menu must disapear.
                   c:            -> peforms copying manipulation whenever copy
                                    button is clicked
                   u:            -> performs cut manipulation whenever cut button
                                    is clicked
                   p:            -> performs paste manipulation whenever paste
                                    button is clicked
                   q:            -> quits the shell.

               IOL UI features:
                The location of the current directory is displayed for the user,
                and gets updated whenever the user switches the directory. Files
                of the current directory are displayed, and can be manipulated
                by the user via left & right mouse clicks. User can select
                the directory, or any other file with the left click to open it,
                or right click on a specific non-directory file to perform
                actions like copy, cut & paste.

                The type of the file can be differentiated via the colors of
                the icons:
                   Orange: directory
                      - IOL UI sends the command d:<directory> to the system
                   Green: executable file
                      - IOL UI sends the command x:<filenName> to the system.
                   Blue: ordinary file
                      - IOL UI sends the command v:<filename> to the system.

                The user is also provided with the following buttons:
                   Back:     the IOL UI sends the command d:.. to the system,
                             which indicates that system needs to go 1 folder back.
                   Parent
                   Directory:the IOL UI send the command d:<main_directory>
                             to the system, which indicates that the system
                             needs to return to the main folder from where
                             the program has been executed.
                   Close:    the IOL UI send the command q:

                Error handling:
                   no access: the program needs to output the error for the user
                             stating the user has no access. Affects commands
                             d: & x:
                   file size > 500kb: whenever user is trying to open the file
                                     that has the size more than 500kb, the
                                     program needs to output the error. Affects
                                     command v: only.

                Right Mouse Button (RMB) Actions:
                   If non-directory file has been selected, IOL UI sends
                   o:<filename> command to the system. Also notifies the user what
                   file is selected at the top of the application.

                   If copy is clicked, IOL UI sends c: command to the system
                   to copy the data of the selected non-directory file.

                   If cut is clicked, IOL UI sends u: command to the system to
                   perform copy, paste & delete the original file manipulations.

                   If paste is clicked, IOL sends p: command to the system to
                   paste the file to the selected directory. If c: command
                   has been launched previously then simply paste, if
                   u: command has been launched previously then paste the file &
                   delete the original file.
  Author: Maxim Zaika ()
  Language: C
  Start date: 16/08/2018 1:00PM
  Last modified: 23/08/2018 19:00PM (updated xterm settings)
  How to execute: run the following command in your Terminal:
                  gcc task3shell.c -o exec;iol -- ./exec */

#include <stdio.h>
#include <dirent.h> //format of directory entries
#include <unistd.h> //for access to check the executable or not
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> //used for checking the file size using stat
#include <sys/types.h> //used for checking the file size using stat
#include <sys/file.h>
#include <time.h>

/* Reference:
     - Since it is an additional feature, the idea has been borrowed from the
       following URL (Construidor, 2013):
       https://stackoverflow.com/questions/20238042/is-there-a-c-function-to-get-permissions-of-a-file
   Pre:
     - in case of this code, it gets called by function paste(); however
       can be utilised in other cases
     - file exists
   Post:
     - returns the chmod binary values ex 777, 666 etc.
     - used for pasting a file with exactly the same permissions as the original
       file
   Returns: permissions*/
int getChmod(const char *file_path){
    struct stat permission;

    if (stat(file_path, &permission) == -1) {
        return -1;
    }

    return (permission.st_mode & S_IRUSR)|(permission.st_mode & S_IWUSR)|(permission.st_mode & S_IXUSR)| //gets owner permissions
           (permission.st_mode & S_IRGRP)|(permission.st_mode & S_IWGRP)|(permission.st_mode & S_IXGRP)| //gets group permissions
           (permission.st_mode & S_IROTH)|(permission.st_mode & S_IWOTH)|(permission.st_mode & S_IXOTH); //gets other permissions
}

/* Pre:
     - this function must be called whenever the error happens
     - the file is selected
   Post:
     - resets the error window after displaying it for 1 second
   Returns: 0*/
int errorCleanDelay(int min_msec, int max_msec) {
  clock_t old_msec = clock();
  int i=0;
  do {
    clock_t diff = clock() - old_msec;
    min_msec = diff * 1000 / CLOCKS_PER_SEC;
    i++;
  } while ( min_msec < max_msec );
  printf("<error.clear> <error.push border=0,{px}>\n");
  return 0;
}

/* Pre:
     - this function is called by getData() & validateFileType() functions
     - file type is identified
   Post:
     - sends the item to user's window in user friendly format
     - whenever directory is selected, then copy, cut and paste is not needed for
       the right click, otherwise it is needed for other files
   Returns: 0*/
int pushFilesToIOL(char row, char *filename, char *filetype) {
  char hash[11], color[7], command[3];
  if (strcmp(filetype,"executable") == 0) {
    char hash[11] = "</#x1F5D4>";
    char color[7] = "green";
    char command[3] = "x:";
  } else if (strcmp(filetype,"directory") == 0) {
    char hash[11] = "</#x1F4C1>";
    char color[7] = "orange";
    char command[3] = "d:";
  } else {
    char hash[10] = "</#x1F5B9>";
    char color[7] = "blue";
    char command[3] = "v:";
  }

  printf("<row.items%d.push\n"
         "      <td <abutton:button size=15,{px} margin-right=50,{px} margin-top=30,{px} border=0 color={%s} {<span size=35,{px} {%s}> <span color={white} {%s}>}\n"
         "                  onmousein=<border 1>\n"
         "                  onmouseout=<border 0>\n"
         "                  onclick=<putln {%s%s}>",row,color,hash,filename,command,filename);

  if (strcmp(command,"d:") == 0) { // if file is a directory then don't open the options
    printf("                  oncontextclick=<option.clear>");
  } else { // if file is not a directory then open the options
    printf("                  oncontextclick=<putln {o:%s}>,"
           "                  <error.clear>,<error.push border=3,{px} width=437,{px} color={red} {%s selected}>"
      , filename,filename);
  }

  printf("          >\n"
         "      >\n"
         ">\n");
}

/* Pre:
     - this function must be called by the function called getData()
        whenever file is not a directory
     - the file is selected
   Post:
     - verifies whether the file is executable or ordinary (file).
   Returns: 0*/
int validateFileType(int row, char *filename) {
  (access(filename,X_OK)) ? (pushFilesToIOL(row,filename,"file")) : (pushFilesToIOL(row,filename,"executable"));
  return 0;
}

/* Pre:
     - the file name is selected
     - called by the command x:<filename>
   Post:
     - verifies whether the user has a permission to access the file
     - opens the (.c) file in xterm termnial (separate window), executes the file
       that user has selected, and keeps the original Terminal functional
     - xterm removes the executable file after running it
   Returns: 0*/
int x_file(char *filename) {
  if (access(filename,R_OK) != 0) {
    printf("<error.clear> <error.push border=3,{px} width=437,{px} color={red} {Error: Cannot open file. Permission denied.}>\n");
    errorCleanDelay(0, 1000);
  } else {
    char buf[1024];
    snprintf(buf,sizeof(buf),"xterm -xrm 'XTerm.vt100.allowTitleOps: false' -T '%s' -hold -e ./'%s' &",filename,filename);
    system(buf);
  }
  return 0;
}

/* Pre:
     - the file name is selected
     - called by the command v:<filename>
   Post:
     - verifies whether the (.txt) file size is less than 500kb, if more than prompts error
     - opens the .txt file in xterm terminal using the less utility. The original terminal
       remains functional while xterm is running.
   Returns: 0*/
int v_file(char *filename) {
  struct stat st;
  stat(filename,&st);
  if (access(filename,R_OK) != 0) { //checks whether user has a permission to access the file
    printf("<error.clear> <error.push border=3,{px} width=437,{px} color={red} {Error: Cannot open file. Permission denied.}>\n");
    errorCleanDelay(0, 1000);
  } else {
    if (st.st_size > 512000) { //1kb = 1024byte
      printf("<error.clear> <error.push border=3,{px} width=437,{px} color={red} {Error: Cannot open file. File size is more than 500kb.}>\n");
      errorCleanDelay(0, 1000);
    } else {
      char buf[1024];
      snprintf(buf,sizeof(buf),"xterm -xrm 'XTerm.vt100.allowTitleOps: false' -T '%s' -hold -e less '%s' &",filename,filename);
      system(buf);
    }
  }
  return 0;
}

/* Pre:
     - the filename is selected
     - called at the beginning of the program, and by the command d:<directory>
   Post:
     - verifies whether user has a permission to access the directory
     - sets the PWD environment variable
     - prints all the files within the directory & the type of the file by
       calling the function validateFileType()
   Returns: new_items_num*/
int getData(char *filename, int items_num) {
  DIR *directory; //DIR represents a directory stream
  struct dirent *dir;

  directory = opendir(filename);


  int i=0, new_items_num=0; //i counts the files, new_items_num indicates row number
  if (directory == NULL) { //replaces error when no access to directory
    printf("<error.clear> <error.push border=3,{px} width=437,{px} color={red} {Error: Cannot open directory. Permission denied.}>\n");
    errorCleanDelay(0, 1000);
    new_items_num=items_num;
  } else {

    for (int i = 0; i<items_num; i++) { //deletes the items on the rows. as well as rows
      if (i % 2 == 0) { printf("<row.items%d.clear>\n",i); } //since 2 items per row, so need to always skip 2 items to clear 1 row
    }

    chdir(filename); getcwd(filename,FILENAME_MAX);//make a pathname a starting point, and get a name of the directory
    printf("<directory.clear> <directory.push width=634,{px} {Current Directory: <span color={orange} size=25,{px} {</#x1F4C2>}> %s}>\n",filename); //updates path with new directory

    while ((dir = readdir(directory)) != NULL) { //reads directory
      if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0) { //ignores folders like . or .. ,and gets the name of the file
        if (i % 2 == 0) { //whenever row is 0, 2, 4 then creates a row. 2 items per row is max
          printf("<row.push <items%d:tr bold=false>>\n",i); // row that holds 2 items
          new_items_num=i;
        }

        if (dir->d_type == DT_DIR) { //detects directories only
          pushFilesToIOL(new_items_num,dir->d_name,"directory");
        } else if (dir->d_type == DT_REG){ //detects regular files
          validateFileType(new_items_num,dir->d_name);
        }

        i++;
      }
    }
    new_items_num++;
    closedir(directory);
  }

  return new_items_num; //returns the number of items, in order when folder changes, to clear old rows
}

/*Pre:
   - the executable of this .c file needs to be launched
  Post:
     - sends data to IOL to show the UI
   Returns: 0*/
int runUI(char *directory) {
  printf("<program.onexit.push <putln {q:}>>"                            // Exit button handler
         "<box border = 1,{px} height=590,{px} width=650,{px}\n"         // [START] main box
         "  <table\n"                                                    // [START]Create table for parent & error
         "    <tr bold=true\n"
         "      <td <button width=50,{px} {</#x1F519>} onclick=<putln {d:..}>>>\n"
         "      <td <parent:button width=140,{px} {parent directory} onclick=<putln {d:%s}>>>\n"
         "      <td <error:box border=0,{px}>>\n"
         "    >\n"
         "  >\n"                                                         // {End} table for parent & error

         "  <table\n"                                                    // [START]Creates a table to hold a current directory path
         "    <tr bold=true\n"
         "      <td <directory:box width=633,{px} size=15,{px} bold=false {Current Directory: <span color={orange} size=25,{px} {</#x1F4C2>}> %s}>>\n"
         "    >\n"
         "  >\n"                                                         // {End} table for the current directory path

         " <box height=450,{px} width=634,{px} margin-left=5,{px} border=0,{px} scroll=true\n" // [START] box for table with items
         "  <row:table scroll=true>\n"                                   // [START-END]Creates a table to hold files
         " >\n"                                                          // {END} box for table with items

         " <option:box height=45,{px} width=634,{px} margin-left=0,{px} border=0,{px}>\n" // [START] box for table with items

         ">\n"                                                           // {END} main box
  ,directory,directory);
  return 0;
}

/*Pre:
   - any file has been right clicked
  Post:
     - sends data to IOL to show buttons like copy, cut ^ paste
   Returns: 0*/
int launchOptions(void) {
  printf(
          "<option.push"
          "  <table\n"                                                    // [START]Create table for parent & error
          "    <tr bold=true\n"
          "      <td {Available Options: }<copy:button width=50,{px} {copy} onclick=<putln {c:}>"
          "      >>\n"
          "      <td <cut:button width=50,{px} {cut} onclick=<putln {u:}>"
          "      >>\n"
          "      <td <paste:button width=55,{px} {paste} onclick=<putln {p:}>,<error.clear>,<error.push border=0,{px}>,<option.clear>"
          "      >>\n"
          "    >"
          "  >"
          ">");
  return 0;
}

/*Pre:
   - copy button is clicked (command :c received)
  Post:
     - saves the path of where the copy button has been clicked
     - opens the file that needs to be copied
   Returns: infile*/
int copy(char *original_directory, char file_name[FILENAME_MAX], int infile) {
  char file1_path[FILENAME_MAX];
  char temp_directory[FILENAME_MAX];
  getcwd(temp_directory,FILENAME_MAX); //gets the location of where copy button is clicked
  snprintf(file1_path,sizeof temp_directory, "%s/%s",temp_directory,file_name); //creates a full pathname include the file name
  strncpy(original_directory,file1_path,FILENAME_MAX); //stores the path of the original directory

  if ((infile = open(file1_path,O_RDONLY)) < 0) {
    perror(file1_path);
  }

  return infile;
}

/*Pre:
   - paste button is clicked (command :c received)
  Post:
     - gets the path of the current folder
     - renames the file to "copy of <filename>" if file exists
     - creates the file with the same name if the file does not exist
     - copies the data of original file to the destination
     - closes file that has been opened by copy button and as well paste buttons
     - refrehes the IOL UI with the new file
     - notified the user of successful instruction with the green window
   Returns: items_num*/
int paste(char *original_directory, char file2_path[FILENAME_MAX], char copyFile[FILENAME_MAX], int infile, int outfile, int items_num, int n, char condition[2]) {
  char buffer[FILENAME_MAX];
  if ((strcmp(condition,"u\0") == 0) || (strcmp(condition,"c\0") == 0)) { //whenever cut is called (u)
    char pasteHere[FILENAME_MAX];
    getcwd(pasteHere,FILENAME_MAX); //gets the location of the current directory
    snprintf(file2_path,sizeof pasteHere, "%s/%s",pasteHere,copyFile); //creates a full pathname include the file name
    char tempName[FILENAME_MAX];

    int chmod = getChmod(original_directory); //gets the permission of the original file, to use it for a copied file

    if (access(file2_path,F_OK) != -1) { // if file exists then need to add "copy of <filename>"
      snprintf(file2_path,sizeof pasteHere, "%s/copy of %s",pasteHere,copyFile);
      snprintf(tempName,FILENAME_MAX, "copy of %s",copyFile);
      strncpy(copyFile, tempName, FILENAME_MAX); //renames the file to give a proper notification to the user
    }

    if ((outfile = open(file2_path,O_WRONLY | O_APPEND | O_CREAT,chmod)) < 0) {
      perror(file2_path);
    }

    while((n=read(infile,buffer,sizeof(*buffer))) > 0 ) { //copies the data from infile to buffer
      write(outfile,buffer,n); //and then writes to outfile from buffer
    }

    close(infile);
    close(outfile);

    items_num = getData(pasteHere,items_num); //refreshes users ui page with the new file

    if ((strcmp(condition,"u\0") == 0)) { //whenever cut is called (u)
      remove(original_directory);
    }

    printf("<error.clear>,<error.push border=3,{px} width=437,{px} color={green} {Copy of file '%s' created.}>",copyFile);
    errorCleanDelay(0, 2000);

    return items_num;
  }
}

/* Pre:
     - executable of this .c file needs to be executed
   Post:
     - prints all the files within the directory.
     - allows the user to input commands such as d: , x: , v: , q:
     - stops the execution of the program once q: is detected
   Returns: 0 */
int main(int argc, char *argv[]){
  int n=0, infile=0, outfile=0; //infile = file for copy; outfile = destination file
  char currentDirectory[FILENAME_MAX]; //filename max comes from stdio.h
  char command[2]; //stores user's command (d: c: x: v: etc.)
  char condition[2]; //stores condition copy or cut

  getcwd(currentDirectory,FILENAME_MAX); //gets the location of the current directory
  runUI(currentDirectory); //executes the IOL ui

  int items_num = -1; //stores the number of items (files) in the directory
  items_num = getData(currentDirectory,items_num);

  char original_directory[FILENAME_MAX]; //the original directory of the copying file
  char copyFile[FILENAME_MAX]; //file that needs to be copied/cut
  char copyFilePath[FILENAME_MAX]; //temporary stores the path of the copying file

  do {
    char user_inp[FILENAME_MAX];
    scanf("%[^\n]%*c", user_inp); //reads until the end of the string is reached

    int cmdlen = strlen(user_inp), i=0, x=0; //i is for the loop. x is for the position of path
    char command[3], currentDirectory[cmdlen-2];

    for (i=0;i<cmdlen+1;i++) { //separate command from the file name
      if (i < 2) { //if 2 first characters then make it a command
        command[i] = user_inp[i];
      } else { //otherwise make it a filename/directory
        (i == cmdlen) ? (currentDirectory[x] = 0) : (currentDirectory[x] = user_inp[i]);
        x++;
      }
      (i == 2) && (command[i] = 0);
    }

    /* decides what to do with the commands */
    if (strcmp(command,"d:") == 0) {
      if (strcmp(condition,"o\0") == 0) { //if options are opened, but no button has been clicked
        printf("<error.clear> <error.push border=0,{px}> <option.clear>"); //then hide the option selector upon changing the folder
        items_num = getData(currentDirectory,items_num);
        strcpy(condition,"d");
      } else { //opens are closed, then simply change the folder
        items_num = getData(currentDirectory,items_num);
      }
    } else if (strcmp(command,"x:") == 0) {
      if (strcmp(condition,"o\0") == 0) { //if options are opened, but no button has been clicked
        printf("<error.clear> <error.push border=0,{px}> <option.clear>"); //then hide the option selector upon changing the folder
        x_file(currentDirectory);
        strcpy(condition,"x");
      } else { //opens are closed, then simply change the folder
        x_file(currentDirectory);
      }
    } else if (strcmp(command,"v:") == 0) {
      if (strcmp(condition,"o\0") == 0) { //if options are opened, but no button has been clicked
        printf("<error.clear> <error.push border=0,{px}> <option.clear>"); //then hide the option selector upon changing the folder
        v_file(currentDirectory);
        strcpy(condition,"v");
      } else { //opens are closed, then simply change the folder
        v_file(currentDirectory);
      }
    } else if (strcmp(command,"o:") == 0) { //enables options
      launchOptions();
      strcpy(copyFile,currentDirectory); //creates a copy of a fileName --copyFile
      strcpy(condition,"o");
    } else if ((strcmp(command,"c:") == 0)) { //copies selected file
      infile = copy(original_directory,copyFile,infile);
      strcpy(condition,"c");
    } else if ((strcmp(command,"p:") == 0)) {
      paste(original_directory,copyFilePath,copyFile,infile,outfile,items_num,n,condition);
      strcpy(condition,"p");
    } else if ((strcmp(command,"u:") == 0)) {
      infile = copy(original_directory,copyFile,infile);
      strcpy(condition,"u");
    } else { //if q
      break;
    }

  } while (strcmp(command,"q:") != 0);

  return 0;
}
