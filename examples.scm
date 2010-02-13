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

(map double '(0 1 2 3))
(count 3)
(count 5)
(factorial1 7)
(factorial2 7)
(factorial3 7)
