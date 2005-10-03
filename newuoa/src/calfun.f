      SUBROUTINE CALFUN (N,X,F)
c
      REAL*8 X(4)
      REAL*8 F
c     calling the c function myfun
      F=CMYFUN(X)
      RETURN
      END

