## Downloading assignments

You can either download the zipped assignment straight to your computer or clone it from GitHub using the command 

    $ git clone ???

(where `???` is the *https* link on the GitHub repository page). The advantage of using `git` is that you'll be all set to use version control to automatically backup your code, which we highly recommend. If you haven't used `git` before, there are many quick introductions to it on the web, such as [this one](http://rogerdudler.github.io/git-guide/).

## Academic honesty

Please do not post code to a public GitHub repository, even after the class is finished, since these assignments will be reused both here and at other universities in the future.

The assignments are to be completed individually. You are welcome to discuss the various parts of the assignments with your classmates, but you must implement the algorithms yourself -- you should never look at anyone else's code.

## Build system

We will be using [CMake](http://www.cmake.org/) to build the assignments. If you don't have CMake (version >= 2.8) on your personal computer, you can install it using apt-get on Linux or Macports/Homebrew on OS X. Alternatively, you can [download](https://cmake.org/download/) it directly from the CMake website.

To build the code, start in the folder that GitHub made or that was created when you unzipped the download. Run 

    mkdir build; cd build

to create a build directory and enter it, then 

    cmake ..

to have CMake generate the appropriate Makefiles for your system, then

    make 

to make the executable, which will be deposited in the build directory.

## Using the instructional machines

All these steps should run with no issues on the fourteen Soda 349 instructional machines. You can access these machines by `ssh`-ing in, e.g.

    ssh cs184-??@s349-?.cs.berkeley.edu

where *cs184-??* is your WebAcct name and *s349-?* is the ?-th computer in 349. Unfortunately, OpenGL won't play nice with *ssh*, so you will only be able to compile your code over *ssh*, not run it. You will have to physically travel to Soda 349 to verify that your code runs correctly.

If you develop on your personal computer and then would like to copy the code over to test it, you can use the *scp* comment from the base project directory (on your own computer) as follows:

    scp -r . cs184-??@s349-?.cs.berkeley.edu:~/destination_folder

After inputting your WebAcct password, all your files should be copied over as when using the norm *cp* command. Note that when you move code between computers, you will probably have to recompile since the binaries are rarely cross-compatible. 

If you modify code while working on the instructional machines, you can reverse the *scp* process to transfer the files back to your own computer by running this command

 scp -r cs184-??@s349-?.cs.berkeley.edu:~/hw1_folder ~/destination_folder

from your personal computer.

## Personal machines: build FAQ

* We have tested the CMake/make build process on a couple OS X and Linux machines. However, different versions of operating systems and various tools mean that building could still be a headache.
*  If you are having problems on OS X, you should first try upgrading to the latest version of Xcode and installing command line tools by running the command

        xcode-select --install
* If you are using Windows, you're welcome to try to modify the CMake files to make the code build, as long as your code also still compiles on the s349-? computers. 
* If the CMake file complains that you are missing packages, do your best to install what you need. StackOverflow and internet forums are your friend: pasting error messages into Google usually yields many other people with the same exact problem. If this fails, then make a Piazza post.



## Code submission

Log in on one of the instructional machines. Make sure your code compiles. Though it will probably behave the same as on your machine, you should stop by Soda 349 and test whether it runs as expected. (We will be holding you accountable for this, since there are too many students for us to debug your code while grading it.) 

To submit your code, navigate to the base directory of the project. Zip up your code into a file `hw1.zip` with the command

    zip -r hw1.zip .

Then run

    submit hw1

You should see the following output:

    Looking for files to turn in....
    Submitting hw1.zip.
    The files you have submitted are:
        ./hw1.zip 
    Is this correct? [yes/no] yes
    Copying submission of assignment hw1....
    Submission complete.

If you do not, then your assignment has not been submitted.
