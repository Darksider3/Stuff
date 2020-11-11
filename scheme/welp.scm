#lang scheme

(define DBG 0)

(define (DBG_OUT x)
  (cond
    ( (= DBG 0) (println x) )
    (else #f)
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

(define (rdepth xtimes counter)
  (cond
    ((= xtimes (- counter 1)) (println "done"))
    (else (rdepth xtimes (+ counter 1)) (println counter ))
  )
)
(define (abs x)
  (cond ((> x 0) x)
        ((= x 0) 0)
        ((< x 0) (- x))))

; Tail-Recursion: Every step we note the multiplier(current * origin) to implement
; a PowerOf function
(define (RecursivePowDownCount number original times)
  (cond
    ((<  times 0) (/ 1 (rpow number (abs times))) ) ; handle negative exponent
    ((=  times 0) (+ 1 0) ) ; handle x^0 case
    ((=  times 1) (+ number)) ; End of recursion - prevent off-by-one-error
    ((= number 1) (+ 1))
    (else (RecursivePowDownCount (* number original) original (- times 1)))
  )
)

(define (rpow number exponent)
  (RecursivePowDownCount number number exponent)
)

(define (pot x)
  (
    rpow x 2
  )
)

(display "6^123      => ")
(DBG_OUT (rpow 6 123))
(display "2^6        => ")
(DBG_OUT (rpow 2 6))
(display "x^2(10^2)  => ")
(DBG_OUT (pot 10))
(display "2^-2       => ")
(DBG_OUT (rpow 2 -2))
(display "1^1        => ")
(DBG_OUT (rpow 1 1))
(display "2^0        => ")
(DBG_OUT (rpow 2 0))

;(DBG_OUT (rfactor 2 2 6 0))
