#!/usr/bin/python3
import sys

# fn unpack number factorial
def rsa():
    try:
        in_file = sys.argv[1]
        with open(in_file) as f:
            for number in f:
                number = int(number)
                if number % 2 == 0:
                        print("{}={}*{}".format(number, number // 2, 2))
                        continue
                i = 3
                while i < number // 2:
                    if number % i == 0:
                        print("{}={}*{}".format(number, number // i, i))
                        break
                    i = i + 2
                if i == (number // 2) + 1:
                    print("{}={}*{}".format(number, number, 1))
    except (IndexError):
        pass

rsa()
