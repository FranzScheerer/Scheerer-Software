import sys

def update_spritz():
    global a_spritz,i_spritz,j_spritz,w_spritz,s_spritz
    i_spritz = (i_spritz + w_spritz) % 256
    j_spritz = s_spritz[(j_spritz + s_spritz[i_spritz]) % 256]
    s_spritz[i_spritz], s_spritz[j_spritz] = s_spritz[j_spritz], s_spritz[i_spritz]

def output_spritz():
    global a_spritz,i_spritz,j_spritz,w_spritz,s_spritz
    update_spritz()
    return s_spritz[j_spritz]

def shuffle_spritz():
    global a_spritz,i_spritz,j_spritz,w_spritz,s_spritz
    for v in range(256):
        update_spritz()    
    w_spritz = (w_spritz + 2) % 256
    a_spritz = 0

def absorb_nibble_spritz(x):
    global a_spritz,i_spritz,j_spritz,w_spritz,s_spritz
    if a_spritz == 240:
        shuffle_spritz()
    s_spritz[a_spritz], s_spritz[240 + x] = s_spritz[240 + x], s_spritz[a_spritz]
    a_spritz = a_spritz + 1

def absorb_byte_spritz(b):
    absorb_nibble_spritz(b % 16)
    absorb_nibble_spritz(b / 16)

def squeeze_spritz(out, outlen):
    global a_spritz,i_spritz,j_spritz,w_spritz,s_spritz
    if a_spritz != 0:
        shuffle_spritz()
    for v in range(outlen):
        out.append(output_spritz())
  
f = open(sys.argv[1], 'r')
msg = f.read()
#msg = 'franz'
s_spritz = []   
a_spritz = i_spritz = j_spritz = 0
w_spritz = 1
for v in range(256):
   s_spritz.append(v)

for c in msg:
  absorb_byte_spritz(ord(c))

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

def h800(x):
  global a_spritz, i_spritz, j_spritz, w_spritz, s_spritz
  out = []
  for c in x:
    absorb_byte_spritz(ord(c))
  squeeze_spritz(out, 100)
  res = 0
  for n in out:
    res = (res << 8) + n
  return res
  
out = []
squeeze_spritz(out, 32)
otx = ''
for n in out:
  if len(num2hextxt(n)) > 1 and n < 16: 
    otx += num2hextxt(n)[1]
  else:
    otx += num2hextxt(n)
    
print "SPRITZ: ", otx
print "h800: ", h800('franz')