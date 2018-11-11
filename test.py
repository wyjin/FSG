import os, sys, time

def main():
    if len(sys.argv) != 3:
        print('Usage: test.py [r/s] <directory/to/access>')
        sys.exit(1)
    mode = argv[1]
    if mode not in ['s', 'r']:
        print("Mode must be one of s or r")
        sys.exit(1)
    path = argv[2]
    if not os.path.isdir(path):
        print("Only test on directories")
        sys.exit(1)
    for filename in sorted(os.listdir(path)):
        if not os.path.isdir(filename):
            with open(path + '/' + filename, 'r') as openfile:
                print(filename)
                if mode == 's':
                    time.sleep(5)
                else:
                    time.sleep(500)

if __name__ == '__main__':
    main()