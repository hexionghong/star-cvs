      SUBROUTINE  MAKHVY
      COMMON/HVYIN/NHVY,KHVY(200,6),PHVY(200,5)
      REAL PHVY
      INTEGER NHVY, KHVY
      SAVE /HVYIN/
      COMMON/HVYJT1/NHVJ1,KHVJ1(100,6),PHVJ1(100,5)
      REAL PHVJ1
      INTEGER NHVJ1, KHVJ1
      SAVE /HVYJT1/
      COMMON/HVYJT2/NHVJ2,KHVJ2(100,6),PHVJ2(100,5)
      REAL PHVJ2
      INTEGER NHVJ2, KHVJ2
      SAVE /HVYJT2/
      REAL PXX, PYY, PHI(100)
      INTEGER ISAV, I, J
       PI=3.141593
       WRITE(6,501) NHVY
 501   FORMAT(' NHVY ',I10)
       ISAV=1
       FLOWMAX=0.0
       PHIDEL=.02*PI
       PRESPHI=-PHIDEL*0.5
       DO 2 I=1,100
       PRESPHI=PHIDEL+PRESPHI      
       PHI(I)=PRESPHI
       FLOW=0.0
       COSPHI=COS(PHI(I))
       SINPHI=SIN(PHI(I))
       DO 3 J=1,NHVY
       PXX=COSPHI*PHVY(J,1)+SINPHI*PHVY(J,2)
       FLOW=ABS(PXX)+FLOW
3      CONTINUE
       IF(FLOW.GT.FLOWMAX) THEN
           ISAV=I
           FLOWMAX=FLOW
       ENDIF
2      CONTINUE         
       WRITE(6,777) FLOW, ISAV, PHI(ISAV)
777    FORMAT(' FLOW ',F10.5,' ISAV ',I10,' PHI ',F10.5)   
       COSPHI=COS(PHI(ISAV))
       SINPHI=SIN(PHI(ISAV))
       NHVJ1=0
       NHVJ2=0
       DO 4 I=1,NHVY
       PXX=COSPHI*PHVY(I,1)+SINPHI*PHVY(I,2)
       IF(PXX.GT.0.0) THEN
         NHVJ1=NHVJ1+1
         KHVJ1(NHVJ1,1)=KHVY(I,1)
         KHVJ1(NHVJ1,2)=KHVY(I,2)
         KHVJ1(NHVJ1,3)=KHVY(I,3)
         KHVJ1(NHVJ1,4)=KHVY(I,4)
         KHVJ1(NHVJ1,5)=KHVY(I,5)
         KHVJ1(NHVJ1,6)=KHVY(I,6)
         PHVJ1(NHVJ1,1)=PHVY(I,5)
         PHVJ1(NHVJ1,2)=PHVY(I,2)
         PHVJ1(NHVJ1,3)=PHVY(I,3)
         PHVJ1(NHVJ1,4)=PHVY(I,4)
         PHVJ1(NHVJ1,5)=PHVY(I,5)
       ELSE
         NHVJ2=NHVJ2+1
         KHVJ2(NHVJ2,1)=KHVY(I,1)
         KHVJ2(NHVJ2,2)=KHVY(I,2)
         KHVJ2(NHVJ2,3)=KHVY(I,3)
         KHVJ2(NHVJ2,4)=KHVY(I,4)
         KHVJ2(NHVJ2,5)=KHVY(I,5)
         KHVJ2(NHVJ2,6)=KHVY(I,6)
         PHVJ2(NHVJ2,1)=PHVY(I,5)
         PHVJ2(NHVJ2,2)=PHVY(I,2)
         PHVJ2(NHVJ2,3)=PHVY(I,3)
         PHVJ2(NHVJ2,4)=PHVY(I,4)
         PHVJ2(NHVJ2,5)=PHVY(I,5)
       ENDIF
4      CONTINUE    
      RETURN
      END






