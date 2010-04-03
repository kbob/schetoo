#!/bin/sh

tmp_expected="`mktemp --tmpdir`"
tmp_actual="`mktemp --tmpdir`"
trap 'rm -f "$tmp_expected" "$tmp_actual"' 0

cat > "$tmp_expected" <<\EOF
7
(0 2 4 6)
3
8
5040
5040
5040
8
9
-3
54
no-negatives
245
-3
no-negatives
#\a
(3 . 2)
EOF

./scheme > "$tmp_actual" <<\EOF
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

(define call-negative-max
  (lambda (proc list)
    (if (null? list)
	'no-negatives
        ((lambda (n)
           (if (< n 0)
               (proc n))
           (if (> n max)
               (set! max n))
           (call-negative-max proc
                              (cdr list)))
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
	'no-negatives
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

((lambda (a)
   (cons
    ((lambda (a)
       a)
     3)
    a))
 2)

EOF

case "$#$1" in

    0)	cmp "$tmp_actual" "$tmp_expected" && echo "   17 simple tests passed"
	;;

    1-v) cmp -s "$tmp_actual" "$tmp_expected" || \
	     xxdiff --title1 Actual --title2 Expected \
                    "$tmp_actual" "$tmp_expected"
	;;
esac


