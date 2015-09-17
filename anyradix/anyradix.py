import random

class AnyRadix:
    """
        defaults to hexadecimal, which is 16-based, and case insensitive without any padding tokens
        currently only positive intergers are supported
    """

    def __init__(self, token_string = '0123456789abcdef', case_sensitive = False, padding_token = None, radix = None):
        if not case_sensitive:
            token_string = token_string.lower()
        self.caseSensitive = case_sensitive

        if None == radix:
            radix = len(token_string)
        elif radix != len(token_string):
            raise AssertionError('unable to determine radix ({} vs. {})'.format(radix, len(token_string)))

        self.radix = radix

        self.token = token_string
        self.tokenValue = {}
        for i in range(0, len(self.token)):
            self.tokenValue[self.token[i]] = i

        if padding_token:
            self.paddingToken = padding_token
            for i in range(0, len(padding_token)):
                self.tokenValue[padding_token[i]] = 0
        else:
            self.paddingToken = self.token[0]

    def encode(self, value, pad_to = 0, uppercase = False):
        if 0 > value:
            raise AssertionError('only positive intergers supported')

        r = []

        while value > 0:
            remainder = value % self.radix
            value = int(value / self.radix)
            r.append(self.token[remainder])

        while len(r) < pad_to:
            r.append(self.paddingToken[random.randrange(0, len(self.paddingToken))])

        if uppercase and not self.caseSensitive:
            r.upper()

        r.reverse()
        return ''.join(r)

    def decode(self, code):
        v = 0
        length = len(str(code))

        for i in range(0, length):
            if code[i] not in self.tokenValue:
                raise Exception('invalid token {0} at offset {1}'.format(code[i], i))

            v += self.tokenValue[code[i]] * self.radix ** (length - 1 - i)

        return v

    def __str__(self):
        return 'radix {}: {}'.format(self.radix, self.token)
