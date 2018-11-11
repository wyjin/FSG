import os, sys, time

def main(argv):
    if len(argv) != 1:
        print('Usage: file_access.py <directory/to/access>')
        sys.exit(1)
    path = argv[0]
    for filename in sorted(os.listdir(path)):
        if not os.path.isdir(filename):
            with open(path + '/' + filename, 'r') as openfile:
                print(filename)
                time.sleep(500)

if __name__ == '__main__':
    main(sys.argv[1:])
