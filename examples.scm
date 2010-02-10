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

(define factorial
  (lambda (n)
    (if (= n 0)
	1
	(* n (factorial (- n 1))))))

(define factorial
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

(define factorial
  (Y (lambda (fact) 
       (lambda (n) 
	 (if (= n 0)
	     1
	     (* n (fact (- n 1))))))))
