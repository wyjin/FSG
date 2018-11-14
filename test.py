import os, sys, time

def main():
    if len(sys.argv) != 3:
        print('Usage: test.py [r/s] <directory/to/access>')
        sys.exit(1)
    mode = sys.argv[1]
    if mode not in ['s', 'r']:
        print("Mode must be one of s or r")
        sys.exit(1)
    path = sys.argv[2]
    if not os.path.isdir(path):
        print("Only test on directories")
        sys.exit(1)

    if mode == 'r':
        while True:
            for filename in sorted(os.listdir(path)):
                if not os.path.isdir(filename):
                    with open(path + '/' + filename, 'r') as openedfile: 
                        print(filename)
    else:
        for filename in sorted(os.listdir(path)):
            if not os.path.isdir(filename):
                open(path + '/' + filename, 'r')  # memory problem but who cares?
                print(filename)
        time.sleep(1000)

if __name__ == '__main__':
    main()
