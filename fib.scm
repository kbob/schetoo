(define fib #f)
(set! fib
  (lambda (n)
    (if (< n 2)
	1
	(+ (fib (- n 1))
	   (fib (- n 2))))))

(fib 30)
