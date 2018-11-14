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
        #for filename in ['test_dir/1.txt']:
            #if not os.path.isdir(filename):
        with open('test_dir/1.txt', 'r') as one, open('test_dir/2.txt', 'r') as two:
            time.sleep(1)

if __name__ == '__main__':
    main()
