C***********************************************************************

      FUNCTION IDDHEP(IDISA,P)

      DIMENSION P(4)
      DIMENSION ITRAN(221)
      DIMENSION JTRAN(40,10)
      DATA ITRAN/22,12,11,14,13,16,15,111,113,221,223,0,333,441,443
     1,551,553,211,213,311,313,-431,-433,541,543,3122,4232,321,323
     2,-411,-413,531,533,0,4132,4122,5232,-421,-423,511,513,3*0,5132
     3,5122,0,521,523,2224,2112,2114,3322,3324,0,1114,3312,3314,2212
     4,2214,3212,0,4322,4324,3112,0,4312,4314,3222,0,4212,4214,5322
     5,5324,4112,4114,5312,5314,4222,4224,5212,5214,3334,0,5112,5114
     6,2*0,5222,5224,0,4332,4334,8*0,5332,5334,4422,4424,4412,4414
     7,4432,4434,4444,5242,5142,5342,5422,5424,5412,5414,5432,5434
     8,5442,5444,91*0,5554,5522,5524,5512,5514,5532,5534,5542,5544/
      DATA JTRAN/20*10211,4*20213,2*20211,2*215,4*30213,2*10215
     1,6*40213,14*10220,4*10111,331,2*10221,2*10223,2*20113
     2,225,20111,115,30223,30113,50223,2*60223,10115,6*40113
     3,20*-10211,4*-20213,2*-20211,2*-215,4*-30213,2*-10215,6*-40213
     4,32*32224,2*2222,2*12224,2*12222,2*2228,28*12212,2*2124,22212
     5,32214,2122,32212,12214,42212,32124,12122,2218,12128,28*12112
     6,2*1214,22112,32114,1212,32112,12114,42112,31214,11212,2118
     7,11218,32*31114,2*1112,2*11114,2*11112,2*1118,29*3224,2*13222
     8,2*13224,2*3226,2*13226,2*23224,3228,28*3214,13122,3124,13212
     9,23122,13214,33122,3216,43122,13216,23124,23214,3218,29*3114
     1,2*13112,2*13114,2*3116,2*13116,2*23114,3118/
        IOFF = 0
        ISIGN = 1
        IAISA = IABS(IDISA)
        IF(IAISA.NE.IDISA) ISIGN = -1
        IF(IAISA.GT.9999) GO TO 500
        IF(IAISA.LT.99) THEN
          IOFF = -9
          GO TO 1000
        ENDIF
        IF(IAISA.LT.199) THEN
          IOFF = -102
          GO TO 1000
        ENDIF
        IF(IAISA.LT.299) THEN
          IOFF = -210
          GO TO 1000
        ENDIF
        IF(IAISA.LT.399) THEN
          IOFF = -318
          GO TO 1000
        ENDIF
        IF(IAISA.LT.499) THEN
          IOFF = -426
          GO TO 1000
        ENDIF
        IF(IAISA.LT.599) THEN
          IOFF = -534
          GO TO 1000
        ENDIF
        IF(IAISA.LT.1199) THEN
          IOFF = -1061
          GO TO 1000
        ENDIF
        IF(IAISA.LT.1299) THEN
          IOFF = -1169
          GO TO 1000
        ENDIF
        IF(IAISA.LT.1399) THEN
          IOFF = -1277
          GO TO 1000
        ENDIF
        IF(IAISA.LT.1999) THEN
          IOFF = -1336
          GO TO 1000
        ENDIF
        IF(IAISA.LT.2199) THEN
          IOFF = -2104
          GO TO 1000
        ENDIF
        IF(IAISA.LT.2299) THEN
          IOFF = -2165
          GO TO 1000
        ENDIF
        IF(IAISA.LT.2399) THEN
          IOFF = -2273
          GO TO 1000
        ENDIF
        IF(IAISA.LT.2999) THEN
          IOFF = -2334
          GO TO 1000
        ENDIF
        IF(IAISA.LT.3199) THEN
          IOFF = -3113
          GO TO 1000
        ENDIF
        IF(IAISA.LT.3299) THEN
          IOFF = -3205
          GO TO 1000
        ENDIF
        IF(IAISA.LT.3399) THEN
          IOFF = -3248
          GO TO 1000
        ENDIF
        IF(IAISA.LT.3999) THEN
          IOFF = -3332
          GO TO 1000
        ENDIF
        IF(IAISA.LT.4199) THEN
          IOFF = -4039
          GO TO 1000
        ENDIF
        IF(IAISA.LT.4299) THEN
          IOFF = -4138
          GO TO 1000
        ENDIF
        IF(IAISA.LT.4399) THEN
          IOFF = -4237
          GO TO 1000
        ENDIF
        IF(IAISA.LT.4449) THEN
          IOFF = -4331
          GO TO 1000
        ENDIF
        IF(IAISA.LT.4999) THEN
          IOFF = -4330
          GO TO 1000
        ENDIF
        IF(IAISA.LT.9999) THEN
          IOFF = -5338
          GO TO 1000
        ENDIF
500     EMS=SQRT(P(4)**2-P(1)**2-P(2)**2-P(3)**2)
        I=IFIX(20.*EMS)
        IF(I.GT.40) I=40
        IF(IAISA.LT.19999) THEN
            IDDHEP = JTRAN(I,1)
            RETURN
        ENDIF
        IF(IAISA.LT.29999) THEN
            IDDHEP = JTRAN(I,2)
            RETURN
        ENDIF
        IF(IAISA.LT.39999) THEN
            IDDHEP = JTRAN(I,3)
            RETURN
        ENDIF
        IF(IAISA.LT.49999) THEN
            IDDHEP = ISIGN*JTRAN(I,4)
            RETURN
        ENDIF
        IF(IAISA.LT.59999) THEN
            IDDHEP = ISIGN*JTRAN(I,5)
            RETURN
        ENDIF
        IF(IAISA.LT.69999) THEN
            IDDHEP = ISIGN*JTRAN(I,6)
            RETURN
        ENDIF
        IF(IAISA.LT.79999) THEN
            IDDHEP = ISIGN*JTRAN(I,7)
            RETURN
        ENDIF
        IF(IAISA.LT.89999) THEN
            IDDHEP = ISIGN*JTRAN(I,8)
            RETURN
        ENDIF
        IF(IAISA.LT.99999) THEN
            IDDHEP = ISIGN*JTRAN(I,9)
            RETURN
        ENDIF
        IF(IAISA.LT.109999) THEN
            IDDHEP = ISIGN*JTRAN(I,10)
            RETURN
        ENDIF
1000    IDDHEP = ITRAN(IAISA+IOFF)*ISIGN
        RETURN
        END

