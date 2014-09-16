#!/usr/bin/python2.7

import sys

def main():
    if len(sys.argv) != 3:
        print "Usage: %s html_file dest_h_file"
    else:
        f = open(sys.argv[1], 'rb').readlines()
        head = 'const static char * html[] = {'
        f = ['"' + x.replace('"', '\\"').strip() + '"' for x in f]
        open(sys.argv[2], 'wb').write(head + ',\n'.join(f) + '};')

if __name__ == "__main__":
    main()
        
