#define RLU(x) RLUSTAR(x)
C **********************************************************************

      SUBROUTINE LYSPLI(KPART,KFLIN,KFLCH,KFLSP)

      IMPLICIT NONE

C...IN CASE OF A HADRON REMNANT WHICH IS MORE COMPLICATED THAN JUST A
C...QUARK OR A DIQUARK, SPLIT IT INTO TWO (PARTONS OR HADRON + PARTON).
*
* to avoid variable conflictions, a second keep element is necessary
* with the same common block name (see LPTOU2)
*
      COMMON /LEPTOU/ CUT(14),LST(40),PARL(30),
     &                X,Y,W2,Q2,U
      REAL CUT,PARL,X,Y,W2,Q2,U
      INTEGER LST
      SAVE /LEPTOU/

      
      INTEGER KPART,KFLIN,KFLCH,KFLSP,IFLIN,KSIGN,IFL,IDUM
      REAL RLU,R,RLUSTAR
      
      IFLIN=KFLIN
      KSIGN=ISIGN(1,KPART)
      IFL=KFLIN*KSIGN
      KFLCH=0
      IDUM=0

      IF(LST(14).EQ.0) THEN
C...If baryon production from remnant excluded, remnant is antiflavour
        KFLSP=-KFLIN
        IF(KFLIN.EQ.21) KFLSP=21
        RETURN
      ENDIF

      IF(IABS(KPART).EQ.211) THEN
C...DECOMPOSE PI+ (PI-).
        IF(IFL.EQ.2) THEN
C...VALENCE U (UBAR) REMOVED.
          KFLSP=-1*KSIGN
        ELSEIF(IFL.EQ.-1) THEN
C...VALENCE D (DBAR) REMOVED.
          KFLSP=2*KSIGN
        ELSEIF(KFLIN.EQ.21) THEN
C...GLUON REMOVED.
          R=2.*RLU(0)
          IF(R.LT.1.) THEN
            KFLCH=2*KSIGN
            KFLSP=-1*KSIGN
          ELSE
            KFLCH=-1*KSIGN
            KFLSP=2*KSIGN
          ENDIF
        ELSEIF((IFL.GE.1.AND.IFL.LE.8).AND.IFL.NE.2) THEN
C...SEA QUARK (ANTIQUARK) REMOVED.
          CALL LUKFDI(-IFLIN,2*KSIGN,IDUM,KFLCH)
          KFLSP=-1*KSIGN
        ELSEIF((IFL.GE.-8.AND.IFL.LE.-1).AND.IFL.NE.-1) THEN
C...SEA ANTIQUARK (QUARK) REMOVED.
          CALL LUKFDI(-IFLIN,-1*KSIGN,IDUM,KFLCH)
          KFLSP=2*KSIGN
        ENDIF

      ELSEIF(IABS(KPART).EQ.2212) THEN
C...DECOMPOSE PROTON (ANTIPROTON).
        IF(IFL.EQ.2) THEN
C...VALENCE U (UBAR) REMOVED.
          R=4.*RLU(0)
          IF(R.LT.3.) THEN
            KFLSP=2101*KSIGN
          ELSE
            KFLSP=2103*KSIGN
          ENDIF
        ELSEIF(IFL.EQ.1) THEN
C...VALENCE D (DBAR) REMOVED.
          KFLSP=2203*KSIGN
        ELSEIF(KFLIN.EQ.21) THEN
C...GLUON REMOVED.
          R=6.*RLU(0)
          IF(R.LT.3.) THEN
            KFLCH=2*KSIGN
            KFLSP=2101*KSIGN
          ELSEIF(R.LT.4.) THEN
            KFLCH=2*KSIGN
            KFLSP=2103*KSIGN
          ELSE
            KFLCH=1*KSIGN
            KFLSP=2203*KSIGN
          ENDIF
        ELSEIF(IFL.GT.2) THEN
C...SEA QUARK (ANTIQUARK) REMOVED.
          R=6*RLU(0)
          IF(R.LT.3.) THEN
            CALL LUKFDI(-IFLIN,2*KSIGN,IDUM,KFLCH)
            KFLSP=2101*KSIGN
          ELSEIF(R.LT.4.) THEN
            CALL LUKFDI(-IFLIN,2*KSIGN,IDUM,KFLCH)
            KFLSP=2103*KSIGN
          ELSE
            CALL LUKFDI(-IFLIN,1*KSIGN,IDUM,KFLCH)
            KFLSP=2203*KSIGN
          ENDIF
        ELSEIF(IFL.LT.0) THEN
C...SEA ANTIQUARK (QUARK) REMOVED.
  100     R=6*RLU(0)
          IF(R.LT.3.) THEN
            CALL LUKFDI(2101*KSIGN,-IFLIN,IDUM,KFLCH)
            KFLSP=2*KSIGN
          ELSEIF(R.LT.4.) THEN
            CALL LUKFDI(2103*KSIGN,-IFLIN,IDUM,KFLCH)
            KFLSP=2*KSIGN
          ELSE
            CALL LUKFDI(2203*KSIGN,-IFLIN,IDUM,KFLCH)
            KFLSP=1*KSIGN
          ENDIF
          IF(KFLCH.EQ.0) GOTO 100
        ENDIF

      ELSEIF(IABS(KPART).EQ.2112) THEN
C...DECOMPOSE NEUTRON (ANTINEUTRON).
        IF(IFL.EQ.2) THEN
C...VALENCE U (UBAR) REMOVED.
          KFLSP=1103*KSIGN
        ELSEIF(IFL.EQ.1) THEN
C...VALENCE D (DBAR) REMOVED.
          R=4.*RLU(0)
          IF(R.LT.3.) THEN
            KFLSP=2101*KSIGN
          ELSE
            KFLSP=2103*KSIGN
          ENDIF
        ELSEIF(KFLIN.EQ.21) THEN
C...GLUON REMOVED.
          R=6.*RLU(0)
          IF(R.LT.2.) THEN
            KFLCH=2*KSIGN
            KFLSP=1103*KSIGN
          ELSEIF(R.LT.5.) THEN
            KFLCH=1*KSIGN
            KFLSP=2101*KSIGN
          ELSE
            KFLCH=1*KSIGN
            KFLSP=2103*KSIGN
          ENDIF
        ELSEIF(IFL.GT.2) THEN
C...SEA QUARK (ANTIQUARK) REMOVED.
          R=6*RLU(0)
          IF(R.LT.2.) THEN
            CALL LUKFDI(-IFLIN,2*KSIGN,IDUM,KFLCH)
            KFLSP=1103*KSIGN
          ELSEIF(R.LT.5.) THEN
            CALL LUKFDI(-IFLIN,1*KSIGN,IDUM,KFLCH)
            KFLSP=2101*KSIGN
          ELSE
            CALL LUKFDI(-IFLIN,1*KSIGN,IDUM,KFLCH)
            KFLSP=2103*KSIGN
          ENDIF
        ELSEIF(IFL.LT.0) THEN
C...SEA ANTIQUARK (QUARK) REMOVED.
  110     R=6*RLU(0)
          IF(R.LT.2.) THEN
            CALL LUKFDI(1103*KSIGN,-IFLIN,IDUM,KFLCH)
            KFLSP=2*KSIGN
          ELSEIF(R.LT.5.) THEN
            CALL LUKFDI(2101*KSIGN,-IFLIN,IDUM,KFLCH)
            KFLSP=1*KSIGN
          ELSE
            CALL LUKFDI(2103*KSIGN,-IFLIN,IDUM,KFLCH)
            KFLSP=1*KSIGN
          ENDIF
          IF(KFLCH.EQ.0) GOTO 110
        ENDIF
      ENDIF

      RETURN
      END
