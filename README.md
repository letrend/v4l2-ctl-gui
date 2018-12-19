# v4l2-ctl-gui
simple ncurse interface to v4l2-ctl. lets you choose the device. 
then the available video driver options are displayed and can be edited.

## Usage

### Ubuntu
    $ sudo apt-get install libncurses5-dev
    $ sudo apt install cmake-curses-gui

Go to downloaded directory and type

    $ ccmake CMakeLists.txt
    
then press [c], [c] and the [g]

    $ make

After that, `v4l2-ctl-gui` executable should be generated. Run that with:

    $ ./v4l2-ctl-gui
