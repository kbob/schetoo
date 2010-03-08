(define map
  (lambda (proc items)
    (if (null? items)
        '()
        (cons (proc (car items))
              (map proc (cdr items))))))

(define double
  (lambda (x)
    (* 2 x)))

(define count
  ((lambda (total)
     (lambda (increment)
       (set! total (+ total increment))
       total))
   0))

(define factorial1
  (lambda (n)
    (if (= n 0)
        1
        (* n (factorial1 (- n 1))))))

(define factorial2
  (lambda (n)
    (define iter
      (lambda (product counter max-count)
        (if (> counter max-count)
            product
            (iter (* counter product)
                  (+ counter 1)
                  max-count))))
    (iter 1 1 n)))

(define Y
  (lambda (f)
    ((lambda (x) (f (lambda (y) ((x x) y))))
     (lambda (x) (f (lambda (y) ((x x) y)))))))

(define factorial3
  (Y (lambda (fact) 
       (lambda (n) 
         (if (= n 0)
             1
             (* n (fact (- n 1))))))))

(define plus3 '())

(+ 3 (call/cc
      (lambda (exit)
        (set! plus3 exit)
        4)))

(map double '(0 1 2 3))
(count 3)
(count 5)
(factorial1 7)
(factorial2 7)
(factorial3 7)
(plus3 5)
(* 2 (plus3 6))

(define caar
  (lambda (pair)
    (car (car pair))))

(define cdar
  (lambda (pair)
    (cdr (car pair))))

(define negative?
  (lambda (z)
    (< z 0)))

(define call-negative-max
  (lambda (proc list)
    (if (null? list)
	#f
        ((lambda (n)
           (if (< n 0)
               (proc n))
           (if (> n max)
               (set! max n)
           (call-negative-max proc
                              (cdr list))))
         (car list)))))

(define max -999)
(call-with-current-continuation
  (lambda (exit)
    (call-negative-max exit '(54 0 37 -3 245 19))))
max

(define max -999)
(call-with-current-continuation
  (lambda (exit)
    (call-negative-max exit '(54 0 37 245 19))))
max

;-------------------------------------------
(define call-negative
  (lambda (proc list)
    (if (null? list)
        #f
        ((lambda (n)
           (if (< n 0)
               (proc n))
           (call-negative proc
                          (cdr list)))
         (car list)))))

(call-with-current-continuation
  (lambda (exit)
    (call-negative exit '(54 0 37 -3 245 19))))

(call-with-current-continuation
  (lambda (exit)
    (call-negative exit '(54 0 37 245 19))))

((lambda (a)
   ((lambda (b)
      a)
    #\b))
 #\a)
