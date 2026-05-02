(defn fib [n]
  (if (< n 2)
    n
    (+ (fib (- n 1)) (fib (- n 2)))))

(print "Fibonacci of 35 is: " (fib 35))