import ftplib

def ftpput(fnam, server, user, pw):
  ftp = ftplib.FTP(server, user=user, passwd=pw)
  ftp.cwd('html')
  f = open(fnam,'rb')
  ftp.storbinary('STOR '+fnam,f)
  f.close()

def ftpget(fnam, server, user, pw):
 ftp = ftplib.FTP(server,user=user, passwd=pw)
 ftp.cwd('html') 
 f = open(fnam,'wb')
 ftp.retrbinary('RETR '+fnam,f.write)
 f.close()

fnam = 'plan.php'
server = 'alfa3026.alfahosting-server.de'
user = 'web1452'
pw = 'VcpSs4Fl'

ftpput('faq.html', server, user, pw)
