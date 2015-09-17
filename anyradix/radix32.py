import random
from anyradix import AnyRadix

_radix32 = AnyRadix('0123456789abcdefghjkmnpqrstuvwxy', padding_token = 'iloz', radix = 32)

def encode(value, pad_to = 0, uppercase = False):
    return _radix32.encode(value, pad_to, uppercase)

def decode(code):
    return _radix32.decode(code)

if '__main__' == __name__:
    l = []
    for i in [random.randrange(47, 99) for x in range(0, 20)]:
        l.append(encode(i, pad_to = random.randrange(4, 8)))
        print(i, '=', l[-1])

    for x in l:
        print(x, ' -> ', decode(x))
