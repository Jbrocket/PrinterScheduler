# Printer Scheduler Overview

This is a project I made to familiarize myself with threads, mutex, and some job scheduling algorithms. Everything besides gcode/, src/printersim.c, and gfx_library was created by me

### Dependencies not included:
This is a command line program that requires XQuartz to be installed in order to display the simulated printing from the gfx library: https://www.xquartz.org/

## How to run it

First run 
### make
on the command line to generate the necessary executables and then run printer scheduler as such:
### ./src/printsched <number of printers wanted>

 
  
From there there are commands available for you to use to run the printers and display information about the different jobs that have been submitted. You can also pipe test.txt scripts into the program
### ./test/test1.txt | ./src/printsched <number of printers wanted>

  
  
To submit a job run:
### submit <./gcode/file.gcode>
Then a XQuartz window will open up and display the print job that is current running.
  
  
  
To display all current jobs running, as well as additional information such as average response time and average turnaround time run:
### list
This will show information on whether a job is scheduled, currently printing, or completed. It also displays the jobs job_id and the file it was assigned to print.

  
  
To wait for a specific job_id to finish before continuing wiht the program run:
### wait <job_id>
  
  
  
To remove a job_id from the queue entirely, run:
### remove <job_id>

  
  
To choose the algorithm type which you want to run, run:
### algo <fifo|sjf|balanced>
FIFO is first in first out algorithm where every job is run in order of being submitted. The SJF algorithm is shortest job first algorithm, it looks at the sizes of every job and chooses the shortest one to run first (this can starve larger jobs). Balanced is a mix of both to ensure that larger files aren't starved entirely.

  
  
To make a job a priority job (it gets executed next) run:
### hurry <job_id>

  
  
And, lastly, to quit out of the program, run:
### quit

  
  
To get rid of all objects and executables, run:
### make clean
