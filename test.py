import os, sys, time

def main():
    if len(sys.argv) != 2:
        print('Usage: test.py <directory/to/access>')
        sys.exit(1)
    path = sys.argv[1]
    if not os.path.isdir(path):
        print("Only test on directories")
        sys.exit(1)

    while True:
        for filename in sorted(os.listdir(path)):
            if not os.path.isdir(filename):
                with open(path + '/' + filename, 'r') as openedfile:
                    time.sleep(1)
                    pass

if __name__ == '__main__':
    main()
