# Examples from http://matt.might.net/articles/programming-with-continuations--exceptions-backtracking-search-threads-generators-coroutines/

; right-now : -> moment
(define right-now
  (lambda ()
    (call/cc
     (lambda (cc)
       (cc cc)))))

; go-when : moment -> ...
(define go-when
  (lambda (then)
    (then then)))

; An infinite loop:
((lambda (the-beginning)
  (newline)
  (go-when the-beginning))
 (right-now))

;----------------------------------

