# $Id: group_aliases.csh,v 1.13 2003/04/18 02:14:41 jeromel Exp $
# $Log: group_aliases.csh,v $
# Revision 1.13  2003/04/18 02:14:41  jeromel
# remove starver as an alias
#
# Revision 1.12  2001/04/09 15:24:19  jeromel
# Small modif for auto-build tree (not complete)
#
# Revision 1.11  2000/02/17 00:40:33  fisyak
# Add starsetup alias
#
# Revision 1.10  2000/02/16 14:21:11  fisyak
# Move objy root parasoft sniff g4 initialization to setup
#
# Revision 1.9  1999/11/04 15:38:21  fisyak
# Add ..dev
#
# Revision 1.8  1999/04/24 12:59:06  fisyak
# Add SILENT option to Makefile
#
# Revision 1.7  1998/12/01 01:55:57  fisyak
# Merge with NT
#
# Revision 1.6  1998/07/27 20:24:16  fisyak
# remove frozen
#
# Aliases to switch between different STAR Library levels
alias starold    'source ${GROUP_DIR}/.starold'
alias starpro    'source ${GROUP_DIR}/.starpro'
alias starnew    'source ${GROUP_DIR}/.starnew'
alias stardev    'source ${GROUP_DIR}/.stardev'
alias staradev   'source ${GROUP_DIR}/.staradev'
alias star.dev   'source ${GROUP_DIR}/star.dev'
alias star..dev  'source ${GROUP_DIR}/star..dev'
#alias starver    'source ${GROUP_DIR}/.starver'
alias setup      'source ${GROUP_DIR}/setup'
alias starsetup  'source ${GROUP_DIR}/setup'
if ($?SILENT == 1) then
  alias makes      "gmake --silent -f $STAR/mgr/MakePam.mk"
  alias makel      "gmake --silent -f $STAR/mgr/Makeloop.mk"
else
  alias makes      "gmake -f $STAR/mgr/MakePam.mk"
  alias makel      "gmake -f $STAR/mgr/Makeloop.mk"
endif
# last line
