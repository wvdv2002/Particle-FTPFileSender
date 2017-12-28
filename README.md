# FTPFileSender

A Particle library, FTPFileSender, to send compressed files over FTP to a ftp server.
Using this library you can call a function on a particle device and the Particle will compress and then send a file to the designated FTP server.

## Getting started
Add some files to an SDcard and connect this SDcard to your Particle device. Setup the right pins and SD card configuration in the example.
Set the ip address, username and password of the server in the FTPFileSender constructor.
Run the example by compiling and flashing the files to a particle device.
Setup a server that works as required by the [particleftpclient](https://github.com/jychuah/ParticleFtpClient) library. I use ProFTPD as the ftp server.
See the [examples](examples) folder for more details.

## Documentation
After sending a file, it can be decompressed as described in the [compressFile](https://github.com/wvdv2002/Particle-compressFile) library. 

### Setting up a server tips.
In my work environment the Electrons are all in a seperate network with a VPN connection with the server. 
The server has a set IP Address, some DNS lookup might be necessary if your server has no set IP address.
There might be some challenges in setting up a safe passive FTP server without a seperate VPN.
if using proFTPD make sure to at least set some limits in the config file and don't forget to open some passive ports as well as the main ftp port.
I added a user particle-ftp that can only access the folder /particle-ftp on the server. It cannot start a terminal session, only upload files over FTP.
In the [linux](examples/linux) folder in examples is an example script on how to unheatshrink a file automatically after receiving it.


## Contributing
There is no option to send files uncompressed for now. This might be a useful addition.
There is no option to download files from the same ftp server.
A nice update would be to let the user instantiate the particleftpclient and add a reference in the ftpfilesender constructor, so the ftp client can be used for more things than only file sending.
Another nice update would be to be able to upload a file with the complete file structure on the sd card.

## LICENSE
Copyright 2017 wvdv2002

Licensed under the ISC license
