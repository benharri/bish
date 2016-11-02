If you use more than 3 lines of code from the internet, cite them in a comment.
It's cause for an 'f' otherwise!!!!


THIS IS OUT OF 20 POINTS!!!!  No one can get more than 20 points.

If you work in pairs, then I'll give each person 2/3 of the total points.  So
if a team gets 30 (and each person gets 15), I'll give each student 20.


If you work in threes, then I'll give each person 1/2 of the total points.  So
if a team gets 40 (and each person gets 13.3333), I'll give each student 20.  


The Shell Assignment (total 42 points)
+ 1 ~~Can run an executable~~
    + `/bin/ls`
+ 1 ~~You search the path for the executable~~
    + `ls`
+ 1 Can do file input redirection "<"
    + `ls > fred`
+ 1 Can do file output redirection ">"
    + `ls < fred`
+ 2 Can do command piping "|"
    + `ls | wc`
+ 1 Can do lots of pipes
    + `ls | grep fred | wc`
+ 1 Can do at least one combination of these things
    + `ls | wc > fred`
+ 4 Can do any combination of three of <, >, and | 
    + `cat < filename | sort > sortedFile.txt`
+ 2 Can set enviornment variables
    + `PATH=:/bin:/sbin:/usr/sbin:/usr/bin`
+ 2 Expands enviornment variables on the command line
    + `ls $HOME`
+ 2 Does filename expansion "glob" (Hint:  Use the built in glob.)
    + `ls a*b`
+ 1 ~~Knows how to change directory~~
    + `cd /fred`
+ 1 Bang last command
    + `!l` runs ls
+ 1 Bang # command
    + `!4` runs 4th command from history
+ 1 Queue commands
    + `make ; make install`
+ 1 Can have lots of semicolons
    + `ls; sleep 3; rm fred`
+ 2 Change Prompt
    + `PS1="what is you command?"`
+ 3 Can run commands in the background.
    + `processImage &`
+ 1 Concatenate commands with &.  Only runs next command if the previous 
      comand returned success.
    + `cd /home/rappleto & rm fred.txt`
+ 1 Catch Keyboard interrupt
    + `ctrl + c` = back to prompt
+ 1 Replace "~" with the home directory
    + `rm ~/junkfile`
+ 1 ~~Control-L clears the screen~~
    + `ctrl-l` = clear screen
+ 3 When they misspell a command, offer a suggestion
    + (user) `lss`
    + (shell) Did you mean `ls`?
+ 2 Can run commands from a file
    + `. scriptFile.txt`
+ 2 ~~Tab Completion~~ and Arrow History
+ 1 Saves and reloads history to a file
+ 2 Automatically runs a file called .myshell when it starts
+ 2 Only runs execuatables from an approved list
+ -2  Commands cannot have arguments (i.e. ls -l does not work).
    


Some cases to consider
+ A person tries to run a non-executable.
+ A person tries to read or write to a non-existant file.
+ A person tries to read or write to a file they do not have permissions on.
+ A person tries a non-sensical command. (i.e. `ls > foo > fee < doo < dee`)
+ A person tries to pipe to a non-command (i.e. `ls | /tmp/foo.txt`)
+ A person tries to pipe from a non-command (i.e. `/tmp/foo | ls`)
+ A person tries to expand a non-variable (i.e. `ls $DOES_NOT_EXIST`)
+ A regular expression matches no files.
+ A regular expression matches one file.
+ A regular expression matches lots of files.
+ A person tries to change directory to something that does not exist.
        
These programs are due Friday before finals week.  It will take a totally excellent excuse to move this at all.