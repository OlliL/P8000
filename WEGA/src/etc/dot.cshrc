# .cshrc
#  Csh startup command file for wega super user .....
#	features:
#	-	sets path to find most admin type commands (/etc esp.)
#	-	sets history list for easy command correction
#	-	sets mail
#	-	sets prompt to identify user as super and should be careful
#			not to destroy anything
#	-	sets up convenient, commonly used alias's
#

# Set path for csh and sh
set path=(. /bin /usr/bin /etc)
setenv PATH ":/bin:/usr/bin:/etc:"

# Set want shell variables
set prompt = "#\! "
set mail=/usr/spool/mail/wega
set history = 18

# Setup the aliases for cd(1C) equivalences goto and go back
alias gt 'set gt=`/bin/pwd`;cd \!^'
alias gb 'set gb=`/bin/pwd`;cd $gt;set gt=$gb;/bin/pwd'

# Other alias's
alias H history
alias ll ls -lisa

sync
