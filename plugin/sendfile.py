#!/usr/bin/env Python

import sys
import ftplib
import datetime
from ftplib import FTP

# Usage:
# sendfile.py "filename" "ftppath" "host" "ip"

def printf(string):
    print(datetime.datetime.strftime(datetime.datetime.now(), '%Y-%m-%d %H:%M:%S') + " : " + string);

if __name__ == '__main__':
    print("");
    printf("FTP File Sender\n")
    try:
        filename = sys.argv[1]
        path = sys.argv[2]
        host = sys.argv[3]
        port = sys.argv[4]
        destfile = sys.argv[5]

        ftp = FTP()
        printf("Connecting to " + host + ":" + port);
        ftp.connect(host, int(port), timeout=3);
        printf("Connected");

        printf("Opening " + filename);
        file = open(sys.argv[1], "rb");
        printf("Success");

    except IOError as e:
        printf("/!\ An error occured. /!\ ");

    printf("Moving to: ftp:/" + path);
    try:
        ftp.cwd(path);
    except IOError:
        try:
            ftp.mkd(path);
            ftp.cwd(path);
        except Exception:
            pass

    try:        
        printf("Sending file");
        ftp.storbinary('STOR '+ destfile, file);
        printf("Done")

        file.close();

        ftp.quit();
        printf("Disconnected");

    except Exception:
        printf("/!\ An error occured. /!\ ");
