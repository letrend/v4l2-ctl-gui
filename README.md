# v4l2-ctl-gui

A simple Ncurses interface to v4l2-ctl. 

### Select a video device
<img src="https://github.com/qwazwsx/v4l2-ctl-gui/assets/20545489/740f74e3-3eb5-4590-8ed9-2b4933912f54" width=500>

### Modify video driver settings
<img src="https://github.com/qwazwsx/v4l2-ctl-gui/assets/20545489/aa309c8a-fa7f-4828-bbd4-9dc201b90513" width=500>


### Install

```
# Download
git clone https://github.com/letrend/v4l2-ctl-gui
cd v4l2-ctl-gui/
# Compile
cmake .
make
# Move binary to /bin
sudo cp v4l2_ctl_gui /bin/v4l2-ctl-gui
# Execute
v4l2-ctl-gui
```
