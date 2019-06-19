import sys

def update():
    global a,i,j,w,z,s
    i = (i + w) % 256
    j = s[(j + s[i]) % 256]
    s[i], s[j] = s[j], s[i]

def output():
    global a,i,j,w,z,s
    x1 = (i + s[z]) % 256
    y2 = (j + s[x1]) % 256
    z = s[y2]
    return z

def whip():
    global a,i,j,w,z,s
    for v in range(256):
        update()
    w = (w + 2) % 256

def shuffle():
    global a,i,j,w,z,s
    whip()
    a = 0

def absorb_nibble(x):
    global a,i,j,w,z,s
    if a == 240:
        shuffle()
    y = (240 + x) % 256
    s[a], s[y] = s[y], s[a]
    a = (a + 1) % 256

def absorb_byte(b):
    global a,i,j,w,z,s
    absorb_nibble(b % 16)
    absorb_nibble(b / 16)

def drip():
    global a,i,j,w,z,s
    if a > 0:
        shuffle()
    update()
    return output()

def squeeze(out, outlen):
    global a,i,j,w,z,s
    if a > 0:
        shuffle()
    for v in range(outlen):
        out.append(drip())
  
f = open(sys.argv[1], 'r')
msg = f.read()
s = []
   
for v in range(256):
   s.append(v)

a = 0
i = 0
j = 0
w = 1
z = 0

for c in msg:
  absorb_byte(ord(c))

def num2hextxt(x):
  res = ''
  h__ = ['0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f']
  while x > 0:
    res = h__[x % 16] + res
    x /= 16
  return res

def bin2num(x):
  res = 0
  for c in x:
    res = (res<<8) ^ ord(c)
  return res


out = []
squeeze(out, 32)
otx = ''
for n in out:
  if len(num2hextxt(n)) > 1 and n < 16: 
    otx += num2hextxt(n)[1]
  else:
    otx += num2hextxt(n)
    
print "SPRITZ: ", otx