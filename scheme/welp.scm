(define DBG 0)

(define (DBG_OUT x)
  (cond
    ( (= DBG 0) (println x) )
    (else)
  )
)

(define (nwlne)
  (display "\n")
)
(define (println x) 
  (display x)
  (nwlne)
)

(define (>= x y)
  (or (> x y) (= x y)))

(define (<= x y )
  (or (< x y) (= x y )))

(define (== x y)
  (cond
      ((= x y) #t)
      (else #f)
  )
)
(define (!= x y )
  (cond 
    ((= x y) #f)
    (else #t)
  )
)

(DBG_OUT (<= 1 1))

(define (rdepth xtimes counter)
  (cond
    ((= xtimes counter) (println "done"))
    (else (rdepth xtimes (+ counter 1)) (println counter ))
  )
)
(define (abs x)
  (cond ((> x 0) x)
        ((= x 0) 0)
        ((< x 0) (- x))))

(define (rpow number original times)
  (cond
    ((<  times 0) (/ 1 (rpow-facade number (abs times))) ) ; handle negative exponent
    ((=  times 0) (+ 1 0) ) ; handle x^0 case
    ((=  times 1) (+ number 0) ) ; End of recursion - prevent off-by-one-error
    (else (rpow (+ number number) original (- times 1)))
  )
)
;  1      2      3           4            5            6              7
; 2+2 => 4+4 => 8+8(16) => 16+16(32) => 32+32(64) => 64+64(128) => 128+128 => 
(define (rpow-facade number exponent)
  (
    rpow number number exponent
  )
)

(DBG_OUT (rpow-facade 2 -5))
;(DBG_OUT (rfactor 2 2 6 0))
