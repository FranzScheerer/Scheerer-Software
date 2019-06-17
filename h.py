
def update():
    global a,i,j,k,w,z,s
    i = (i + w) % 256
    y = (j + s[i]) % 256
    j = (k + s[y]) % 256
    k = (i + k + s[j]) % 256
    s[i], s[j] = s[j], s[i]

def output():
    global a,i,j,k,w,z,s
    y1 = (z + k) % 256
    x1 = (i + s[y1]) % 256
    y2 = (j + s[x1]) % 256
    z = s[y2]
    return z

def whip():
    global a,i,j,k,w,z,s
    for v in range(512):
        update()
    w = (w + 2) % 256

def shuffle():
    global a,i,j,k,w,z,s
    whip()
    a = 0

def absorb_nibble(x):
    global a,i,j,k,w,z,s
    if a == 128:
        shuffle()
    y = (128 + x) % 256
    s[a], s[y] = s[y], s[a]
    a = (a + 1) % 256

def absorb_byte(b):
    global a,i,j,k,w,z,s
    absorb_nibble(b % 16)
    absorb_nibble(b / 16)

def drip():
    global a,i,j,k,w,z,s
    if a > 0:
        shuffle()
    update()
    return output()

def squeeze(out, outlen):
    global a,i,j,k,w,z,s
    if a > 0:
        shuffle()
    for v in range(outlen):
        out.append(drip())
  
f = open('h0.c','r')
msg = f.read()
print msg
s = []
   
for v in range(256):
   s.append(v)

a = 0
i = 0
j = 0
k = 0
w = 1
z = 0

for c in msg:
  absorb_byte(ord(c))
out = []
squeeze(out, 32)
print out