      SUBROUTINE CALFUN (N,X,F)
c      F=0.0D0
      REAL*8 X(4)
      REAL*8 F
      F = TESEO_P_FUNC_SUM_DISTANCE_BEZIER(X)
      RETURN
      END

