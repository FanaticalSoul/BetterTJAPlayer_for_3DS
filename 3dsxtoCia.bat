cxitool TJAPlayer_for_3DS.3dsx TJAPlayer_for_3DS.cxi --banner resources/banner.bin
makerom -f cia -o TJAPlayer_for_3DS.cia -target t -i TJAPlayer_for_3DS.cxi:0:0
rm TJAPlayer_for_3DS.cxi
pause